use os;
use util;
use vitals;

include "include/say";
include "include/sysEvent";
include ":attributes:attributes";
include ":yggdrasil:boss";

function hasSpecial(me)
    if (npccfgfile.Special)
        if (!GetCooldown(me, npccfgfile.Special))
            return 1;
        endif
    endif
    return 0;
endfunction


function doSpecial(me)
  //checa as condicoes
  var conds :=  GetConfigStringArray( npccfgfile, "SpecialCondition" );
  
  foreach cond in  conds
    if (cond == "enemies")
    
    endif
    
    if (cond == "closedist")
      if  (distance(me, me.opponent)  >= 2)
        return;
      endif
    endif
  endforeach

  SetCooldown(me, npccfgfile.Special, npccfgfile.SpecialCooldown);
  
  var says := GetConfigStringArray( npccfgfile, "SpecialSpeak" );
  var text := says[RandomInt(says.size())+1];
  if (says != error)
    SayUC(UC(text));
  endif
  
  if (npccfgfile.SpecialAnimation)
    var e := PerformAction(me, cint(npccfgfile.SpecialAnimation));
  endif

  var effect := SplitWords(GetconfigString(npccfgfile, "SpecialEffect" ));
  if (effect)
    if (effect[1] == "timer")
      if (effect[2] == "area")
        foreach  m in (ListMobilesNearLocation( me.x, me.y, me.z, 3, me.realm))
          if (!m.npctemplate)
          TS_StartTimer(m, effect[3], cint(effect[4]), cint(effect[5]), me);
          endif
        endforeach
      else
        TS_StartTimer(me.opponent, effect[3], cint(effect[4]), cint(effect[5]), me);
      endif
    endif
  endif

endfunction

function EnableMainEvents(type := 0)
    case (type)
        "merchant":
            DisableEvents(/*SYSEVENT_SPEECH +*/ SYSEVENT_LEFTAREA + SYSEVENT_DISENGAGED /*+ SYSEVENT_OPPONENT_MOVED*/);
            EnableEvents(SYSEVENT_DOUBLECLICKED, 20);
            EnableEvents(SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + SYSEVENT_ITEM_GIVEN, areasize);
        default:
            DisableEvents(/*SYSEVENT_SPEECH +*/ SYSEVENT_LEFTAREA + SYSEVENT_DISENGAGED /*+ SYSEVENT_OPPONENT_MOVED*/);
            EnableEvents(SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + SYSEVENT_ENTEREDAREA, areasize);
    endcase
endfunction

function DisableMainEvents(type := 0)
    case (type)
        "merchant": DisableEvents(SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + SYSEVENT_DOUBLECLICKED + SYSEVENT_ITEM_GIVEN);
        default: DisableEvents(SYSEVENT_ENGAGED + SYSEVENT_DAMAGED + SYSEVENT_ENTEREDAREA);
    endcase
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
    
function Fight(opponent, alerted := 0)
 // mystate := FIGHTING;
  if(opponent.serial == me.serial)
    SetWarMode(0);
    opponent := 0;
    return;
  endif
    
  set_priority(50);
  SetOpponent(opponent);
  DisableMainEvents();
  EnableCombatEvents();
  TurnToward(opponent);
  var loops := 0;
  var ev;
  var waittime := 0;
  
  if (!alerted) 
	if (alert_allies) 
		AlertAllies(me, opponent); 
	endif

  endif

/*   var speech := GetConfigStringArray( npccfgfile, "InitCombatSpeech" );
   if (speech.size() > 0)
	if (RandomInt(100)  < 30) //sempre 30% de chance de falar
		Say(speech[RandomInt(speech.size()+1)]);
	endif
   endif
*/

  var dist;
  while( me )

    if (me.dead)
       break;
    endif  

    if (!opponent)
      break;
    endif
  
    if (opponent.dead)
	var temnego := 0;
	foreach npc in ListMobilesInLineOfSight(me, areasize + 2)
		if( CanFight(me, npc, align) )
			temnego := 1;
			break;
		endif
		sleepms(2);
	endforeach
	if (!temnego)
		var corpo := opponent.GetCorpse();
		var end := ReadGameClock() + 10;
		while (CoordinateDistance(me.x, me.y, corpo.x, corpo.y) < areasize + 5 && end > ReadGameClock())
			RunAwayFromLocation(corpo.x, corpo.y);
			sleepms(100);
		endwhile
	endif
	break;
    endif

    if (opponent.concealed)
      break;
    endif

    if (opponent.hidden)
	break;
    endif

    dist := CoordinateDistance(me.x, me.y, opponent.x, opponent.y);
    if ( dist > 20)
      break;
    endif

 
  //se eh boss e tem eventos
  if ( GetBossStruct(me) != error && isBoss(me) )
    //printtextabove(me, "teste " + GetBossStruct(me)); //apagar
	var unique := GetBossStruct(me);
	foreach ce in (unique.combatevent)
		//printtextabove(me, " "  + ce); //apagar
		if (TS_GetCooldown(me, ce.name) == error)
			Start_Script(":ghaia:special/genericspecial" , array{me, ce});
		endif
	endforeach
  endif

  if (hasCombat())
    DoCombatSpecial(me); ////////////// SPECIALL
  endif

    if ( (dist > throwinfo.range) && (throwinfo.canthrow == 1) )
      throwItem(me, opponent, dist);
    endif
  
    if ( (dist > 6) && ( CInt(npccfgfile.teleport) ) )
      teleport(me, opponent);
    endif

    if ( (dist >  CInt(npccfgfile.BreathRange)) && CInt(npccfgfile.BreathAttack) == 1 )
      FireBreath(me, opponent, dist);
    endif

    if ( (npccfgfile.WebRange) && (dist > npccfgfile.WebRange) )
      SpitWeb(me, opponent, dist);
    endif
  
    if(!CloseDistance(opponent))
      loops := loops + 1;
      waittime := 0;
    else
      loops := 0;
      waittime := 1;
    endif
	
    checkFlee(opponent);
	
    ev := wait_for_event(waittime);
//    printtextabove(self(), "event " + ev);
	if(ev.type != SYSEVENT_SPEECH)
		case (ev.type)
			SYSEVENT_DAMAGED: 

			//se ele receber dano de alguem mais proximo que o oponente ou se receber dano de alguem e o oponente nao estiver acessivel
			if( /*(CoordinateDistance(me.x, me.y, ev.source.x, ev.source.y) < CoordinateDistance(me.x, me.y, opponent.x, opponent.y)) ||*/ (!CheckLineOfSight(me, opponent)) || (!opponent))
				if ( (opponent) && RandomInt(4) == 1 ) //se ainda existir o oponente so tem 25% de chance de mudar de alvo
					if (ev.source)
						opponent := ev.source;
						SetOpponent(opponent);
						TurnToward(opponent);
					endif
				elseif ( (!opponent) || (!CheckLineOfSight(me, opponent))) //mas se nao tiver mais o oponent entao 100% de chance de mudar de alvo
					if (ev.source)
						opponent := ev.source;
						SetOpponent(opponent);
					        TurnToward(opponent);
					endif
				endif
			endif

			SYSEVENT_ENGAGED: 
				if(ev.source)
					if(RandomInt(6) == 1)
						opponent := ev.source;
						SetOpponent(opponent);
						TurnToward(opponent);
					endif
				endif

			EVID_TAUNT:
				if(ev.source)
					opponent := ev.source;
					SetOpponent(opponent);
				        TurnToward(opponent);		
				endif
			EVID_FLEE:
				if (ev.source)
					flee(ev.source);
				endif
			EVID_PEACEMADE:
			//	sendsysmessage(ev.source, "peacemade");
				SetWarMode( 0 );
                           	SetOpponent( 0 );
                           	sleep(1);
                           	DisableCombatEvents();
                           	EnableMainEvents();
				return;
			EVID_COMBAT_MOVE:
				RunTowardLocation( ev.x, ev.y );
				if (coordinateDistance(me.x, me.y, ev.x, ev.y) <= 1)
					clear_event_queue();
				endif
		endcase
	endif
	sleepms(5);
  endwhile
  
  DisableCombatEvents();
  EnableMainEvents();
  SetWarMode(0);
  SetOpponent(0);
  sleep(1);
endfunction

function EnableCombatEvents()
  EnableEvents(SYSEVENT_ENGAGED + SYSEVENT_DAMAGED );
  DisableEvents(SYSEVENT_SPEECH);
endfunction

function DisableCombatEvents()
  DisableEvents(SYSEVENT_ENGAGED + SYSEVENT_DAMAGED );
endfunction


function CheckFlee(opponent)
  if((AP_GetVital(me, HITS) /AP_GetVitalMaximumValue(me, HITS)) <= flee_point && !isBoss(me))
	EraseObjProperty(me,"#flees");
    flee(opponent);
  endif
endfunction

function NpcGoTo(destiny, move_type)
	if((me.script == "immobile") || (me.script == "immobilespell"))
		sleep(2);
		return;
	else
		var flags := FP_IGNORE_MOBILES;
		if ( cint(GetObjProperty(me, "ignoredoor")) || cint(GetObjProperty(me, "opendoor")) )
			flags := flags| FP_IGNORE_DOORS;
		endif
		var path := FindPath(  me.x, me.y, me.z, destiny.x, destiny.y, destiny.z, me.realm, flags, 12); //calcula o path
		var sleepdelay := 275 - me.run_speed;//(CInt(GetDexterity(me)) * 1.5);
		foreach coord in path
			if (move_type == 1)
				WalkTowardLocation( coord.x, coord.y ); //anda na direcao
			else
				RunTowardLocation( coord.x, coord.y ); //anda na direcao
			endif
			sleepms(sleepdelay);
		endforeach
	endif
endfunction

function Flee(opponent)
  if((me.script == "immobile") || (me.script == "immobilespell"))
	sleep(2);
	return;
  else
   var speech := GetConfigStringArray( npccfgfile, "FleeSpeech" );
   if (speech.size() > 0)
	if (RandomInt(100)  < 30) //sempre 30% de chance de falar
		SayUC((speech[RandomInt(speech.size()+1)]));
	endif
   endif

    var numflees := GetObjProperty(me,"#flees");
	if(numflees > 10)
      RestartScript(me);
	else
	  numflees := numflees + 1;
	endif
    SetObjProperty(me,"#flees", numflees);
    var runs := 0;
    var chk := 0;
    var ev;
    while((Distance(me, opponent) < 15) && (runs < 50) && (CheckLineOfSight(me, opponent)))
      ev := wait_for_event(1);
      //chk := process_flee_event(opponent);
	  RunAwayFrom(opponent);
	  runs := runs +1;
	  case (ev.type)
        SYSEVENT_DAMAGED: if(((ev.source.npctemplate) and (CheckLineOfSight(me, ev.source))) ||(CoordinateDistance(me.x, me.y, ev.source.x, ev.source.y) < CoordinateDistance(me.x, me.y ,opponent.x, opponent.y)) || (!CheckLineOfSight(me, opponent)) || (!opponent) || (RandomInt(3)==1))
					        opponent := ev.source;
			  		        SetOpponent(opponent);
			              endif
	  endcase
	  if(chk)
	    break;
	  endif
    endwhile
  endif
endfunction
    



//no sleep mode ele o waitfor event fica 30 secs e ele nao fica mais fazendo lookarounds o tempo todo
function sleepmode()


   var speech := GetConfigStringArray( npccfgfile, "SleepSpeech" );
   if (speech.size() > 0)
	if (RandomInt(100)  < 30) //sempre 30% de chance de falar
		SayUC(UC(speech[RandomInt(speech.size()+1)]));
	endif
   endif

  EraseObjProperty(me,"#flees");
  DisableMainEvents();

  //TODO se tiver skill hiding, usar
  EnableEvents(SYSEVENT_ENTEREDAREA, areasize);
  EnableEvents(SYSEVENT_ENGAGED);
  EnableEvents(SYSEVENT_DAMAGED);
  var ev;
  while (1)
    ev := os::wait_for_event(30);
    repeat
    //npcs criam ovos ou outras coisas
    if ( (GetCooldown(me, "dropitem" ) == 0) && (CInt(npccfgfile.DropDelay) > 0) )
       CreateItemAtLocation(me.x, me.y, me.z, CStr(npccfgfile.DropItem), 1, me.realm);
       SetCooldown(me, "dropitem", CInt(npccfgfile.DropDelay));
    endif
    //se for carneiro
    if ( me.graphic == 223)
		if (GetCooldown(me, "wool") == 0)
			me.graphic := 207;
		endif
    endif
    PlayIdleSound();
    case (ev.type)
      SYSEVENT_ENGAGED:    
                             if (me.hidden)
                               me.hidden := 0;
                             endif
                             EnableMainEvents();
                             return ev;

      SYSEVENT_DAMAGED:
                             if (me.hidden)
                               me.hidden := 0;
                             endif
                             EnableMainEvents();
                             return ev;

      SYSEVENT_ENTEREDAREA: 
                          if (CanFight(me, ev.source, align))
                             if(me.hidden)
                               me.hidden := 0;
                             endif
                             EnableMainEvents();
                             return ev;
                          endif

	EVID_TAUNT:
                             EnableMainEvents();
                             return ev;
	EVID_ALERT_ALLIES:	EnableMainEvents();
                             return ev;
	EVID_HERDING:
				EnableMainEvents(); 
				return  ev;
	EVID_FLEE:
			EnableMainEvents(); 
			return ev;
	EVID_DOOBJECTIVE:
		EnableMainEvents(); 
		return ev;
    endcase
    until(!(ev := os::wait_for_event(30)));
  endwhile
endfunction


function Opendoors(me)

  foreach door in (ListItemsNearLocation(me.x, me.y, me.z, 1))
    if  ((door.isa(POLCLASS_DOOR)) and (!door.locked)) //se nao ta trancada
		if ( cint(GetObjProperty(me, "ignoredoor")) == 1)
			PrintText(me, "*atravessa a porta*");
			MoveObjectToLocation(me, door.x, door.y, door.z, door.realm, MOVEOBJECT_FORCELOCATION);
		elseif (cint(GetObjProperty(me, "opendoor")) == 1)
			PrintText(me, "*abre a porta*");
			door.Open();
		endif
	endif
  endforeach
endfunction

function ChooseAnotherOpponent(me)

	var otherTarget := ListMobilesNearLocationEx( me.x, me.y, me.z, 8, LISTEX_FLAG_NORMAL, me.realm );
	if  (otherTarget.size() > 0)
		foreach mob in otherTarget
			if(CanFight(me, mob, align)) //verifica se eh inimigo
				Fight(mob);
			endif
		endforeach
	endif
endfunction

function CalcPathAndMove(me, opponent, byref sleepdelay)
	sleepdelay += 5;
	if (!CheckLosAt(me, opponent.x, opponent.y, opponent.z) || (opponent.z != me.z && ListStaticsNearLocation(me.x, me.y, me.z, 1))) //se nao consegue ver
		var path := GetObjPRoperty(me, "path"); //procura se ja calculou o path
		if (path == error) //se nao calculou
			var flags := FP_IGNORE_MOBILES;
			if ( cint(GetObjProperty(me, "ignoredoor")) || cint(GetObjProperty(me, "opendoor")) )
				flags := flags| FP_IGNORE_DOORS;
			endif
			path := FindPath(  me.x, me.y, me.z, opponent.x, opponent.y, opponent.z, me.realm, flags, 12); //calcula o path
			
			if (!path[1] || !path || path == error)
				Flee (opponent);
				sleepms(sleepdelay);
				sleepms(500);
				ChooseAnotherOpponent(me);
			else
				SetObjProperty(me, "path", path);
			endif
			
			sleepms(sleepdelay);
		else //se calculou o path
			var coord := path[1]; //pega a primeira coordenada
			if (coord) 
				SetObjProperty(me, "#coordtry", ReadGameClock()+5); 
				RunTowardLocation( coord.x, coord.y ); //anda na direcao
				sleepms(sleepdelay);
				Opendoors(me); //tenta abrir possiveis portas
				if (CoordinateDistance(me.x, me.y, coord.x, coord.y) <= 1) //se chegou onde devia 
					path.erase(1); //deleta a coordenada e vai pra proxima
					if (path[1])
						SetObjProperty(me, "path", path);
					else
						EraseObjProperty(me, "path");
					endif
				else // se nao consegue chegar na coord 
					if ( cint(GetObjProperty(me, "#coordtry")) > ReadGameClock()) //se ta tentando chegar la a mais de 5 segundos
						EraseObjProperty(me, "path"); //deleta o path atual
						ChooseAnotherOpponent(me);
					endif
				endif
			else
				RunToward( opponent );
				sleepms(sleepdelay);
			endif
		endif
	else
		EraseObjProperty(me, "path");
		RunToward( opponent );
		sleepms(sleepdelay);
	endif
endfunction