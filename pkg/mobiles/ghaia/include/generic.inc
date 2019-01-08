use os;
use util;
use vitals;
use npc;

include "include/say";
include "include/sysEvent";
include ":attributes:attributes";
include ":yggdrasil:boss";

function EnableMainEvents()
   DisableEvents(/*SYSEVENT_SPEECH +*/ SYSEVENT_LEFTAREA|SYSEVENT_DISENGAGED /*+ SYSEVENT_OPPONENT_MOVED*/);
   EnableEvents(SYSEVENT_ENTEREDAREA, cint(areasize));
   EnableEvents(SYSEVENT_DAMAGED, cint(areasize));
   EnableEvents(SYSEVENT_ENGAGED, cint(areasize));
   EnableEvents(EVID_ALERT_ALLIES);
   EnableEvents(EVID_HERDING);
   EnableEvents(EVID_TAUNT);
   EnableEvents(EVID_DOOBJECTIVE);
endfunction

function DisableMainEvents(type := 0)
   DisableEvents(SYSEVENT_ENGAGED|SYSEVENT_DAMAGED|SYSEVENT_ENTEREDAREA);
endfunction


function CloseIn(me, lx, ly, mode := 0 )
  var d := coordinatedistance(me.x, me.y, lx, ly); 
  case (d)
    0:       return 1;
    default: 
	    if (mode == 0)
	      WalkTowardLocation(lx, ly);
	    else
		    RunTowardLocation(lx, ly);
	    endif	
      return 0;
  endcase
endfunction

function SetPeaceMade()
   EraseObjProperty(me, "aggros");
   SetWarMode( 0 );
   SetOpponent( 0 );
   sleep(1);
   DisableCombatEvents();
   EnableMainEvents();
endfunction

function EnableCombatEvents()
    EnableEvents(SYSEVENT_ENGAGED|SYSEVENT_DAMAGED );
    DisableEvents(SYSEVENT_SPEECH);
endfunction

function DisableCombatEvents()
    DisableEvents(SYSEVENT_ENGAGED|SYSEVENT_DAMAGED );
endfunction


function CheckFlee(opponent)
    if((AP_GetVital(me, HITS) / AP_GetVitalMaximumValue(me, HITS)) <= flee_point && !isBoss(me))
        EraseObjProperty(me,"#flees");
        flee(opponent);
    endif
endfunction

function NpcGoTo(destiny, move_type, stop_if_attacker := 0)
   var flags := FP_IGNORE_MOBILES;
   if ( cint(GetObjProperty(me, "ignoredoor")) || cint(GetObjProperty(me, "opendoor")) )
      flags := flags| FP_IGNORE_DOORS;
   endif
   var path := FindPath(  me.x, me.y, me.z, destiny.x, destiny.y, destiny.z, me.realm, flags, 20); //calcula o path
   var sleepdelay := 275 - me.run_speed;//(CInt(GetDexterity(me)) * 1.5);
   foreach coord in path
      if (move_type == 1)
         WalkTowardLocation( coord.x, coord.y ); //anda na direcao
      else
         RunTowardLocation( coord.x, coord.y ); //anda na direcao
      endif
      Opendoors(me);

      if (stop_if_attacker && RandomInt(3) == 1)
         foreach mobile in ListMobilesNearLocation(me.x, me.y, me.z, 8)
            if (CanFight(me, mobile))
               return;
            endif
         endforeach
      endif
      sleepms(sleepdelay);
   endforeach
endfunction

function Flee(byref opponent, sleepdelay := 0, change_opponent := 0)
   if ("FleeStealth" in custombehaviors)
      if (!GetCooldown(me, "fleestealth"))
         PrintText(me, "*esconde*");
         sleep(1);
         me.hidden := 1;
         me.stealthsteps := 30;
         SetCooldown(me, "fleestealth", 30);
      endif
   endif
   // var speech := GetConfigStringArray( npccfgfile, "FleeSpeech" );
   // if (speech.size() > 0)
   //    if (RandomInt(100)  < 30) //sempre 30% de chance de falar
   //          SayUC((speech[RandomInt(speech.size()+1)]));
   //    endif
   // endif
   var numflees := cint(GetObjProperty(me,"#flees"));
   if(numflees > 10)
      EraseObjProperty(me, "#flees");
      RestartScript(me);
   else
      numflees := numflees + 1;
      SetObjProperty(me,"#flees", numflees);
   endif
   
   var runs := 0;
   if (!sleepdelay)
      sleepdelay := 275 - me.run_speed;
   endif
   // var chk := 0;
   var last_loc := array{opponent.x, opponent.y, opponent.z};
   while(Distance(me, opponent) < 30 && runs < 50 && CoordinateDistance(opponent.x, opponent.y, last_loc[1], last_loc[2]) <= 3)
      sleepms(sleepdelay);
      var ev := wait_for_event(0);
      //chk := process_flee_event(opponent);
      RunAwayFrom(opponent);
      runs := runs +1;

      if (change_opponent && randomint(6) <= 2)
         me.setAggro(opponent.serial, -100);
         if (ChooseHighestOpponent(me, opponent))
            break;
         endif
      endif

      if (ev.type == SYSEVENT_DAMAGED)
         if (((ev.source.npctemplate) and (CheckLineOfSight(me, ev.source))) 
               || (CoordinateDistance(me.x, me.y, ev.source.x, ev.source.y) < CoordinateDistance(me.x, me.y ,opponent.x, opponent.y)) 
               || (!CheckLineOfSight(me, opponent)) || (!opponent) || (RandomInt(3)==1))
            opponent := ev.source;
            SetOpponent(opponent);
            me.setAggro(opponent, 300);
         endif
      endif 
   endwhile

   SetObjProperty(me, "#noback", polcore().systime+15);
endfunction
    
//no sleep mode ele o waitfor event fica 30 secs e ele não fica mais fazendo lookarounds o tempo todo
function sleepmode()
   // var speech := GetConfigStringArray( npccfgfile, "SleepSpeech" );
   // if (speech.size() > 0)
   //    if (RandomInt(100)  < 30) //sempre 30% de chance de falar
   //       SayUC(UC(speech[RandomInt(speech.size()+1)]));
   //    endif
   // endif
   EraseObjProperty(me, "aggros");
   EraseObjProperty(me,"#flees");
   SetObjProperty(me,"#sleepmode", 1);
   DisableMainEvents();

   //TODO se tiver skill hiding, usar
   EnableEvents(SYSEVENT_ENTEREDAREA, 25);
   EnableEvents(SYSEVENT_ENGAGED);
   EnableEvents(SYSEVENT_DAMAGED);
   var ev;
   while (1)
      ev := os::wait_for_event(30);
      repeat
         CreateAnimalitems();
         PlayIdleSound();

         case (ev.type)
            SYSEVENT_ENGAGED:    
               // if (me.hidden)
               //   me.hidden := 0;
               // endif
               EnableMainEvents();
               return ev;
            SYSEVENT_DAMAGED:
               if (me.hidden)
                  me.hidden := 0;
               endif
               EnableMainEvents();
               return ev;
            SYSEVENT_ENTEREDAREA: 
               if (ev.source && !ev.source.npctemplate && (!ev.source.cmdlevel || me.npctemplate["merchant"]))
                  EnableMainEvents();
                  return ev;
               endif
            // EVID_HERDING:
            EVID_FLEE:
            EVID_DOOBJECTIVE:
            EVID_ALERT_ALLIES:
            SYSEVENT_SPEECH:
            // EVID_TAUNT:
            // SYSEVENT_ITEM_GIVEN:
            //  SYSEVENT_SPEECH:
               EnableMainEvents(); 
               return ev;
         endcase
      until(!(ev := os::wait_for_event(30)));
   endwhile
endfunction

function CreateAnimalitems()
   //npcs criam ovos ou outras coisas
   if ( (GetCooldown(me, "dropitem" ) == 0) && (CInt(npccfgfile.DropDelay) > 0) )
      CreateItemAtLocation(me.x, me.y, me.z, CStr(npccfgfile.DropItem), 1, me.realm);
      SetCooldown(me, "dropitem", CInt(npccfgfile.DropDelay));
   endif

   //se for carneiro
   if ( me.graphic == 223 && GetCooldown(me, "wool") == 0)
      me.graphic := 207;
   endif
endfunction

function Opendoors(me)
  foreach door in (ListItemsNearLocation(me.x, me.y, me.z, 1))
    if  ((door.isa(POLCLASS_DOOR)) and (!door.locked)) //se não ta trancada
        if ( cint(GetObjProperty(me, "ignoredoor")) == 1 && !door.IsOpen())
          PrintText(me, "*atravessa a porta*");
          MoveObjectToLocation(me, door.x, door.y, door.z, door.realm, MOVEOBJECT_FORCELOCATION);
        elseif (cint(GetObjProperty(me, "opendoor")) == 1 && !door.IsOpen())
          PrintText(me, "*abre a porta*");
          UseItem(door, me);
        endif
    endif
  endforeach
endfunction

function ChooseHighestOpponent(me, byref opponent, new_opponent := 0, force_change := 0)
   if (!opponent && new_opponent)
      opponent := new_opponent;
      SetOpponent(new_opponent);
      TurnToward(new_opponent);
      return 1;
   elseif (opponent && !CheckLineOfSight(me, opponent))
      me.setAggro(opponent.serial, -50);
   endif

   if (GetCooldown(me, "#blockaggro"))
      return 0;
   endif

   var highest_aggro := getHighestAggro(me);
   if (highest_aggro && (!opponent || highest_aggro != opponent.serial))
      highest_aggro := SystemFindObjectBySerial(highest_aggro);
      opponent := highest_aggro;
      SetOpponent(highest_aggro);
      TurnToward(highest_aggro);
      return 1;
   endif
endfunction

function getFacingString(me_face)
   case (cint(me_face))
      0:default: return "N";
      1: return "S";
      2: return "E";
      3: return "W";
      4: return "NW";
      5: return "NE";
      6: return "SW";
      7: return "SE";
   endcase
endfunction

function FatigueNPC(stam := 0)
   PrintText(me, "*salivando pela boca e ofegante*");
   var stam := AP_GetVitalMaximumValue(me, STAMINA);
   for i := 1 to 6
      HealStamina(me, cint(stam / 6));
      sleep(1);
   endfor
endfunction

function CalcPathAndMove(me, byref opponent, sleepdelay)
   var face_str := getFacingString(GetFacing(me.x, me.y, opponent.x, opponent.y));
   var stam := AP_GetVital(me, STAMINA);
   var move_type := 1;

   if (stam < 5)
      move_type := 0;
   endif

	var path := GetObjProperty(me, "path"); //procura se ja calculou o path
	var last_loc := GetObjProperty(me, "#enemy_last_loc"); //procura se ja calculou o path
   if (last_loc && path) 
      if (CoordinateDistance(opponent.x, opponent.y, last_loc[1], last_loc[2]) > 3)
         path := 0;
         EraseObjProperty(me, "path");
         EraseObjProperty(me, "#enemy_last_loc");
         last_loc := 0;
      else
         last_loc := 1;
      endif
   endif

   if (stam <= 1)
      var last_msg := GetObjProperty(me, "#fatigue");
      if (!last_msg || last_msg < ReadGameClock())
         SetObjProperty(me, "#fatigue", ReadGameClock()+15);
         PrintText(me, "*fatigado*");
      endif
      sleep(1);
   elseif (CheckLosAt(me, opponent.x, opponent.y, opponent.z) && !path && CanMove(face_str))
      EraseObjProperty(me, "path");
      EraseObjProperty(me, "#enemy_last_loc");

      if (move_type) 
         RunToward( opponent );
      else 
         WalkToward( opponent );
      endif

      var reachtry := GetObjProperty(me, "#reachtry");
      if (reachtry == error || (reachtry[1].x != opponent.x && reachtry[2].y != opponent.y))
         reachtry := array{struct{ x := opponent.x, y := opponent.y, z := opponent.z }, ReadGameClock()+5 };
         SetObjProperty(me, "#reachtry", reachtry);
      endif

      if (reachtry[2] > ReadGameClock())
         path := FindPath(  me.x, me.y, me.z, opponent.x, opponent.y, opponent.z, me.realm, 0, 15);
         if (!path)
            Flee(opponent, sleepdelay, 1);;
            sleepms(sleepdelay);
         else
            SetObjProperty(me, "path", path);
            SetObjProperty(me, "#enemy_last_loc", array{opponent.x, opponent.y, opponent.z});
         endif
      endif

      sleepms(sleepdelay);
	else
      if (path == error || last_loc ) //se não calculou
         var flags := FP_IGNORE_MOBILES;
         if ( cint(GetObjProperty(me, "ignoredoor")) || cint(GetObjProperty(me, "opendoor")) )
            flags := flags| FP_IGNORE_DOORS;
         endif
         path := FindPath(  me.x, me.y, me.z, opponent.x, opponent.y, opponent.z, me.realm, flags, 15); //calcula o path
         
         if (!path[1] || !path || path == error)
            Flee(opponent, sleepdelay, 1);
            sleepms(sleepdelay);
         else
            if (move_type) RunTowardLocation( path[1].x, path[1].y );
            else WalkTowardLocation(path[1].x, path[1].y); endif

				path.erase(1); //deleta a coordenada e vai pra proxima
            SetObjProperty(me, "path", path);
            SetObjProperty(me, "#enemy_last_loc", array{opponent.x, opponent.y, opponent.z});
         endif
		endif

      var coord := path[1]; //pega a primeira coordenada
      if (coord); 
         var coordtry := GetObjProperty(me, "#coordtry");
         if (coordtry == error || coordtry[1] != coord)
            coordtry := array{coord, ReadGameClock()+5 };
            SetObjProperty(me, "#coordtry", coordtry); 
         endif
         if (move_type) RunTowardLocation( coord.x, coord.y );
         else WalkTowardLocation(coord.x, coord.y); endif
         sleepms(sleepdelay);
         Opendoors(me); //tenta abrir possiveis portas
         if (CoordinateDistance(me.x, me.y, coord.x, coord.y) <= 1) //se chegou onde devia 
            path.erase(1); //deleta a coordenada e vai pra proxima
            if (path[1])
               SetObjProperty(me, "path", path);
            else
               EraseObjProperty(me, "path");
            endif
         else // se não consegue chegar na coord 
            if ( coordtry[2] > ReadGameClock()) //se ta tentando chegar la a mais de 5 segundos
               EraseObjProperty(me, "path"); //deleta o path atual
               me.remAggro(opponent);
               var old_opponent := opponent;
               ChooseHighestOpponent(me, opponent);
               if (old_opponent == opponent) Flee(opponent, sleepdelay, 1); endif
            endif
         endif
      else
         if (move_type) RunToward( opponent );
         else WalkToward( opponent ); endif
         sleepms(sleepdelay);
      endif
	endif
endfunction

function Speak(text, type := SAY_TEXTTYPE_DEFAULT)
   SayUC(UC(text), type);
endfunction


function MakeBlind(mobile, is_remove := 0)
   if (is_remove)
      if (!mobile.opponent)
         return;
      endif
      
      DisableEvents(SYSEVENT_ENTEREDAREA);
      EnableEvents(SYSEVENT_ENTEREDAREA, areasize);
      return;
   endif
   DisableEvents(SYSEVENT_ENTEREDAREA);
   EnableEvents(SYSEVENT_ENTEREDAREA, 3);
endfunction