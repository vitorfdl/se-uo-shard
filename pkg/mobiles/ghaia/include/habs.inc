

function throwItem(npc, opponent, dist)
   if ( dist < 3 || dist > 15 )
      return 0;
   elseif ( !CheckLineOfSight(npc, opponent) )
      return 0;
   elseif ( CInt( GetCooldown(npc, "throws" )) != 0 )
      return 0;
   endif

   npc.frozen := 1;
   sleepms(50);

   if (throwinfo.anim)
      PerformAction(npc, throwinfo.anim);
   else
      PerformAction(npc, 4);
   endif

   sleep(1);

   PlayMovingEffect(npc, opponent, throwinfo.item, 15);
   npc.frozen := 0;

   var item := CreateItemAtLocation(opponent.x, opponent.y, opponent.z, throwinfo.item, 1);
   item.movable := 1;


   var chance := GetAttribute(npc, "Dexterity") - GetAttribute(opponent, "Dexterity") / 3;

   if (RandomInt(100) > chance) // Errou!

      if (throwinfo.misssound)
         PlaySoundEffect(npc, throwinfo.misssound);
      else
         PlaySoundEffect(npc, 0x137);
      endif
   else
      DamageFLS(opponent, RandomDiceRoll(throwinfo.damage), DMG_PHYSICAL, npc);
      PlaySoundEffect(npc, throwinfo.hitdamage);
   endif

   SetCooldown(npc, "throws", throwinfo.delay);


   return 1;
endfunction

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

function SpitWeb(npc, opponent, byref dist)
    if ( dist < 3 || dist > 10 )
        return 0;
    elseif (Abs(npc.z - opponent.z) >= 15)
        return 0;
    elseif ( !CheckLineOfSight(npc, opponent) )
        return 0;
    elseif ( opponent.frozen )
        return 0;
    elseif (GetEquipmentByLayer(opponent, 0x19))
        return 0;
    elseif ( CInt( GetCooldown(npc, "web" )) != 0 )
        return 0;
    endif

    npc.frozen := 1;
    sleepms(50);

    var web;
    case( RandomInt(4) )
        0: web := 3811; break;
        1: web := 3812; break;
        2: web := 3813; break;
        3: web := 3814; break;
    endcase

    PlayMovingEffect(npc, opponent, web, 10);

    npc.frozen := 0;
    TS_StartTimer(opponent, "paralysis", 5);

    var preso1 := CreateItemAtLocation(opponent.x, opponent.y, opponent.z+1, 4317, 1);
    var preso2 := CreateItemAtLocation(opponent.x, opponent.y, opponent.z+1, 4314, 1);
    sleepms(5);

    //AI_Move(npc, opponent, NEMOVE_TOWARD, NEMOVE_RUN, WAKEUP, dist);
    RunTowardLocation( opponent.x, opponent.y );
    DestroyItem(preso1);
    DestroyItem(preso2);
    sleepms(50);
    SetCooldown(npc, "web", 15);

    return 1;
endfunction

function teleport(npc, opponent)
    var phase_coords := GetCoordsInLine(npc.x, npc.y, opponent.x, opponent.y);
    phase_coords.Erase(phase_coords.Size()); 
    TurnObjectToward(npc, opponent.x, opponent.y);
    PlaySoundEffect(npc, CInt(npccfgfile.teleportSound));

    foreach coord in phase_coords
        PlayStationaryEffect(npc.x, npc.y, npc.z, CInt(npccfgfile.teleportEffect), 5, 5, 0);
        //PlayStationaryEffectHuefx( npc.x, npc.y, npc.z, CInt(npccfgfile.teleportEffect), 5, 5,  0, CInt(npccfgfile.teleportColor), CInt(npccfgfile.teleportAlpha));
        var z := GetWorldHeight(coord.x, coord.y, npc.realm);
        MoveObjectToLocation(npc, coord.x, coord.y, z, npc.realm, MOVEOBJECT_NORMAL);
        sleepms(20);
    endforeach
  
    return 1;
endfunction