include "include/utils";
include "include/facings";

function UseHabs()
   var habs := GetConfigStringArray(npccfgfile, "EspecialAttack");
   if (!habs || !habs[1])
      return;
   endif

   foreach hab in habs
      hab := SplitWords(hab);
      var hab_name     := hab[1];
      if (GetCooldown(me, hab_name))
         return;
      endif
      var hab_cooldown := cint(hab[2]);
      var hab_dice     := hab[3];
      var hab_target   := hab[4];
      var hab_special   := hab[5];

      if (hab_dice["no_roll"])
      elseif (hab_dice["d"])
         hab_dice := RandomDiceRoll(hab_dice);
      else
         hab_dice := cint(hab_dice);
      endif

      PlaySpecialHab(hab_name, hab_cooldown, hab_dice, hab_target, hab_special);
      sleepms(2);
   endforeach
endfunction

function PlaySpecialHab(hab_name, hab_cooldown := 0, hab_dice := 0, hab_target := "Enemy", hab_special := 0)
   if (hab_target == "Enemy")
      hab_target := me.opponent;
   elseif (hab_target == "RandomEnemy")
      foreach enemy in ListMobilesNearLocation(me.x, me.y, me.z, 5);
         if (CanFight(me, enemy))
            hab_target := enemy;
            break;
         endif
      endforeach
   elseif (hab_target == "Ally")
      foreach ally in ListMobilesNearLocation(me.x, me.y, me.z, 8);
         if (GetObjProperty(ally, "spawnpoint") == GetObjProperty(me, "spawnpoint"))
            hab_target := ally;
            break;
         endif
      endforeach
   elseif (hab_target == "AllyUnderAttack")
   elseif (hab_target == "Self")
      hab_target := me;
   endif

   var res;
   case (hab_name)
      "summon": res := SummonCreature(hab_dice, hab_special); break;
      "agarrar": res :=  NPC_Agarrar(hab_dice, hab_target); break;
      // "cançãoSedutora": CancaoSedutora(hab_dice); break;
      "furtividade": res := NPC_Sneak(); break;
      "roubar": res := Roubar(hab_target); break;
      // "pelede_de_arvore": res := PeleDeArvore(hab_dice); break;
      "rajada_acida": res := RajadaAcida(hab_dice, hab_target, hab_special); break;
      "teia": res := SpitWeb(hab_dice, hab_target, hab_special); break;
      "teleport": res := teleport(hab_target, hab_special); break;
      default:
         Print("{} npc hab not exist".format(hab_name));
         break;
   endcase

   if (hab_cooldown && res)
      SetCooldown(me, hab_name, CInt(hab_cooldown));
   else
      SetCooldown(me, hab_name, 4);
   endif
endfunction

function summonCreature(qty, creatures)
   creatures := SplitWords(creatures);
   for i := 1 to qty
      var rad := GetPointsInRadius(me.x, me.y, 5);
      var summoned := CreateNpcFromTemplate(":ghaia:{}".format(creatures.randomentry()), rad.x, rad.y, rad.z, 0, me.realm);
      TS_StartTimer(summoned, "summon", 60);
   endfor

   return 1;
endfunction

function NPC_Agarrar(difficulty, targ)
   PrintTexT(me, "*tenta agarrar {}*".format(targ.name));
   TS_StartTimer(targ, "agarrar", -1);

   return 1;
endfunction

function NPC_Sneak()
   PrintText(me, "*esconde*");
   me.hidden := 1;
   me.stealthsteps := 8;
   return 1;
endfunction

function Roubar(targ)
   if (!IsBehind(targ, me.x, me.y) || Distance(me, targ) > 1)
      return 0;
   endif
   
   return 1;
endfunction

function RajadaAcida(dmg, targ, difficulty)
   if (!targ || Distance(me, targ) < 3  || !CheckLineOfSight(me, targ))
      return;
   endif

   PrintText(me, "*cospe ácido*");
   var phase_coords := GetCoordsInLine(me.x, me.y, targ.x, targ.y);
   foreach coord in phase_coords
      var acid := CreateItemAtLocation(coord.x, coord.y, me.z, 0xA370);
      SetObjProperty(acid, "AcidDice", dmg);
      SetObjProperty(acid, "AcidDiff", difficulty);
      sleepms(50);
   endforeach

   if (rollResistDice(targ, DEXTERITY) <= difficulty)
      DamageFLS(targ, dmg, DMG_POISON, me);
   else
      DamageFLS(targ, dmg/2, DMG_POISON, me);
   endif

   return 1;
endfunction

// function throwItem(targ)
//    if ( dist < 3 || dist > 15 )
//       return 0;
//    elseif ( !CheckLineOfSight(npc, opponent) )
//       return 0;
//    elseif ( CInt( GetCooldown(npc, "throws" )) != 0 )
//       return 0;
//    endif

//    npc.frozen := 1;
//    sleepms(50);

//    if (throwinfo.anim)
//       PerformAction(npc, throwinfo.anim);
//    else
//       PerformAction(npc, 4);
//    endif

//    sleep(1);

//    PlayMovingEffect(npc, opponent, throwinfo.item, 15);
//    npc.frozen := 0;

//    var item := CreateItemAtLocation(opponent.x, opponent.y, opponent.z, throwinfo.item, 1);
//    item.movable := 1;


//    var chance := GetAttribute(npc, "Dexterity") - GetAttribute(opponent, "Dexterity") / 3;

//    if (RandomInt(100) > chance) // Errou!

//       if (throwinfo.misssound)
//          PlaySoundEffect(npc, throwinfo.misssound);
//       else
//          PlaySoundEffect(npc, 0x137);
//       endif
//    else
//       DamageFLS(opponent, RandomDiceRoll(throwinfo.damage), DMG_PHYSICAL, npc);
//       PlaySoundEffect(npc, throwinfo.hitdamage);
//    endif

//    SetCooldown(npc, "throws", throwinfo.delay);


//    return 1;
// endfunction


function Firebreath(npc, opponent, byref dist)
   if ( dist < 5 || dist > 15 )
      return 0;
   elseif ( CInt( GetCooldown(npc, "breath" )) != 0 )
      return 0;
   elseif ( !CheckLineOfSight(npc, opponent) )
      return 0;
   endif

   TurnTowardLocation( opponent.x, opponent.y );
   //AI_Turn(npc, opponent, NETURN_TOWARD);
   npc.frozen := 1;
   PerformAction(npc, 12);
   PlaySoundEffect(npc, 0x16B);

   sleep(2);

   PlayMovingEffect(npc, opponent, GFX_LARGE_FIREBALL, 10, 10, 1);
   var coord_list := GetCoordsInLine(npc.x, npc.y, opponent.x, opponent.y);

   foreach coord in coord_list
      if ( _coord_iter < 4 ) // Dont make one where its standing
         continue;
      endif

      var field := CreateItemAtLocation(coord.x, coord.y, npc.z, "FireField_NS", 1);
      field.SetDuration(0);
      sleepms(105);
   endforeach

   foreach coord in GetCoordsInSquare(opponent.x, opponent.y, CInt(npccfgfile.BreathRadius))
      if ( RandomInt(2) != 1 ) // Don't place a fire field
         continue;
      endif

      case( RandomInt(3) )
         default:
            PlayStationaryEffect(coord.x, coord.y, npc.z, GFX_EXPLODE_1, 10, 10, 0);
            break;
         1:
            PlayStationaryEffect(coord.x, coord.y, npc.z, GFX_EXPLODE_2, 10, 10, 0);
            break;
         2:
            PlayStationaryEffect(coord.x, coord.y, npc.z, GFX_EXPLODE_3, 10, 10, 0);
            break;
      endcase

      var field;
      if ( RandomInt(2) )
         field := CreateItemAtLocation(coord.x, coord.y, opponent.z, "FireFieldNS", 1);
      else
         field := CreateItemAtLocation(coord.x, coord.y, opponent.z, "FireFieldEW", 1);
      endif
      field.SetDuration(20);
      sleepms(5);
   endforeach
   
   PlaySoundEffect(opponent, 0x208);
   var chars := ListMobilesNearLocation(opponent.x, opponent.y, opponent.z, CInt(npccfgfile.BreathRadius), npc.realm);

   foreach mobile in chars
      DamageFLS(mobile, RandomDiceRoll(CInt(npccfgfile.BreathDamage)), DMG_FIRE, npc);
   endforeach
   npc.frozen := 0;

   SetCooldown(npc, "breath", CInt(npccfgfile.BreathDelay));
   return 1;
endfunction

function SpitWeb(difficulty, targ, range)
   range := cint(range);
   if (!targ || Distance(me, targ) > range )
      return 0;
   elseif ( !CheckLineOfSight(me, targ) )
      return 0;
   elseif ( targ.frozen )
      return 0;
   elseif (GetEquipmentByLayer(targ, 0x19))
      return 0;
   endif

   me.frozen := 1;
   sleepms(50);

   var web;
   case( RandomInt(4) )
      0: web := 3811; break;
      1: web := 3812; break;
      2: web := 3813; break;
      3: web := 3814; break;
   endcase

   PlayMovingEffect(me, targ, web, 10);

   me.frozen := 0;
   TS_StartTimer(targ, "paralysis", 5);

   var preso1 := CreateItemAtLocation(targ.x, targ.y, targ.z+1, 4317, 1);
   var preso2 := CreateItemAtLocation(targ.x, targ.y, targ.z+1, 4314, 1);
   sleepms(5);

   //AI_Move(me, targ, NEMOVE_TOWARD, NEMOVE_RUN, WAKEUP, dist);
   RunTowardLocation( targ.x, targ.y );
   DestroyItem(preso1);
   DestroyItem(preso2);
   sleepms(50);
   SetCooldown(me, "web", 15);

   return 1;
endfunction

function teleport(targ, effect := 0x3728)
    var phase_coords := GetCoordsInLine(me.x, me.y, targ.x, targ.y);
    phase_coords.Erase(phase_coords.Size()); 
    TurnObjectToward(me, targ.x, targ.y);
    PlaySoundEffect(me, 0x1FF);

    foreach coord in phase_coords
        PlayStationaryEffect(me.x, me.y, me.z, effect, 5, 5, 0);
        //PlayStationaryEffectHuefx( npc.x, npc.y, npc.z, CInt(npccfgfile.teleportEffect), 5, 5,  0, CInt(npccfgfile.teleportColor), CInt(npccfgfile.teleportAlpha));
        var z := GetWorldHeight(coord.x, coord.y, me.realm);
        MoveObjectToLocation(me, coord.x, coord.y, z, me.realm, MOVEOBJECT_NORMAL);
        sleepms(20);
    endforeach
  
    return 1;
endfunction