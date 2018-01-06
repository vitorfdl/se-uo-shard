include "include/utils";
include "include/facings";

function UseHabs()
   var habs := GetConfigStringArray(npccfgfile, "EspecialAttack");
   if (!habs || !habs[1])
      return;
   endif

   foreach hab in habs
      sleepms(2);
      hab := SplitWords(hab);
      var hab_name  := hab[1];

      if (GetCooldown(me, hab_name))
         sleepms(2);
         continue;
      endif

      // if ("Healer" in custombehaviors && hab_name != "curar" && RandomInt(10) < 2)
      //    continue;
      // endif

      var hab_cooldown := cint(hab[2]);
      var hab_chance  := cint(hab[3]);
      var hab_dice     := hab[4];
      var hab_target   := hab[5];
      var hab_special  := hab[6];

      if (hab_chance != 100 && Randomint(100)+1 > hab_chance )
         SetCooldown(me, hab_name, 5);
         continue;
      endif

      if (hab_dice["no_roll"])
      elseif (hab_dice["d"])
         hab_dice := RandomDiceRoll(hab_dice);
      else
         hab_dice := cint(hab_dice);
      endif

      PlaySpecialHab(hab_name, hab_cooldown, hab_dice, hab_target, hab_special);
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
         if (hab_name == "curar")
            if (AP_GetVital(ally, HITS) == AP_GetVitalMaximumValue(ally, HITS))
               continue;
            endif
         endif
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
      "summon": res := SummonCreature(cint(hab_dice), hab_special); break;
      "agarrar": res :=  NPC_Agarrar(hab_dice, hab_target, hab_special); break;
      // "cançãoSedutora": CancaoSedutora(hab_dice); break;
      "furtividade": res := NPC_Sneak(); break;
      "metamorfose": res := Metamorfose(hab_special); break;
      "curar": res := Healing(hab_target, hab_dice); break;
      "roubar": res := Roubar(hab_target); break;
      // "pelede_de_arvore": res := PeleDeArvore(hab_dice); break;
      "rajada_acida": res := RajadaAcida(hab_dice, hab_target, hab_special); break;
      "teia": res := SpitWeb(hab_special, hab_target, hab_special); break;
      "teleport": res := teleport(hab_target, hab_special); break;
      default:
         Print("{} npc hab not exist".format(hab_name));
         SetCooldown(me, hab_name, 150);
         break;
   endcase

   if (hab_cooldown && res)
      SetCooldown(me, hab_name, CInt(hab_cooldown));
   else
      SetCooldown(me, hab_name, 5);
   endif
endfunction

function summonCreature(qty, creatures)
   playCastAnimation();
   creatures := SplitWords(creatures);
   for i := 1 to qty
      var rad := randomPlace(me, 5);
      var summoned := CreateNpcFromTemplate(":ghaia:{}".format(creatures.randomentry()), rad.x, rad.y, me.z, 0, me.realm);
      ghaiaattack(summoned, me.opponent);
      SetObjProperty(summoned, "nocorpse", 1);
      TS_StartTimer(summoned, "summon", 60);
   endfor

   return 1;
endfunction

function NPC_Agarrar(dice, targ, difficulty)
   if (Distance(me, targ) > 2)
      return 0;
   endif

   PrintTexT(me, "*tenta agarrar {}*".format(targ.name));
   if (rollResistDice(targ, DEXTERITY) <= difficulty)
      printText(targ, "*escapou de {}*".format(me.name));
      return 1;
   endif

   TS_StartTimer(targ, "paralysis", 3);
   TS_StartTimer(me, "paralysis", 3);
   MoveObjectToLocation(me, targ.x, targ.y, targ.z, targ.realm, MOVEOBJECT_FORCELOCATION);
   for i := 1 to 3
      if (!me)
         return;
      endif
      sleep(1);
      DamageFLS(targ, RandomDiceRoll("1d4"), DMG_BLUDGE, me);
   endfor
   return 1;
endfunction

function Metamorfose(hab_special)
   if ("MetamorfoseClose" in custombehaviors)
      if (Distance(me, me.opponent) > 4)
         return 0;
      endif
   endif

   playCastAnimation();
   var monsters := SplitWords(hab_special);
   var choose_monster := monsters.randomentry();
   PrintText(me, "*metamorfose*");
   me.frozen := 1;
   PlaySoundEffect(me, 0x656);
   PlayStationaryEffect(me.x, me.y, me.z, GFX_SMOKE, 10, 10, 0);
   SetObjProperty(me, "metamorfose", choose_monster);
   SetObjProperty(me, "oldgraphic", me.graphic);
   
   sleep(1);
   var meta_config := NPC_GetNPCConfig(":ghaia:{}".format(choose_monster));
   me.graphic := meta_config.objtype;
   me.frozen := 0;
   TS_StartTimer(me, "metamorfose", 30);
   return 1;
endfunction

function Healing(targ, heal)
   if (!targ.isA(POLCLASS_MOBILE))
      return 0;
   endif
   playCastAnimation();
   PrintText(me, "*magia estranha*");
   PlaysoundEffect(me, 1137);
   PlayObjectCenteredEffect( targ, 0x373a, 7, 0x10 );
   HealDamage(targ, heal);
   return 1;
endfunction


function NPC_Sneak()
   PrintText(me, "*esconde*");
   me.hidden := 1;
   me.stealthsteps := 15;
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

function playCastAnimation()
   if ("CastAnimation" in custombehaviors)
      me.frozen := 1;
      for i := 1 to (RandomDiceRoll("1d4"))
         PerformAction(me, ANIM_CAST_DIR);
         sleepms(650);
      endfor
      me.frozen := 0;
   endif
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

   var web := array{3811, 3812, 3813, 3814}.randomentry();

   PlayMovingEffect(me, targ, web, 10);

   me.frozen := 0;
   TS_StartTimer(targ, "teia", 5);
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
