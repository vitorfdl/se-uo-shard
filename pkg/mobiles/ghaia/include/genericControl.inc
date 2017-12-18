include "objective";

function genericControl()
	var ev;
	var wanders;

	set_critical(1);
	InitializeNPC(me);
	set_critical(0);
	SetWarMode( 0 );
	ev := os::wait_for_event(0);
	wanders := 0;
	EnableMainEvents();
	Set_Event_Queue_Size(50);
	while(me)
		//se tem objetivo, anda na direcao dele, se não tem so wander
		if (GetObjProperty(me, "objloc") != error)
			wanders := 0;
			DoObjective();
		elseif (!LookForFood())
			MoveAround(wanders);
		endif
		
		wanders := wanders +1;
		if (ListMobilesNearLocationEx(me.x, me.y, me.z, 20, LISTEX_FLAG_PLAYERS_ONLY)[1])
			wanders := 0;
		elseif (wanders >= 30)
         if (npccfgfile.CustomBehavior == "camuflar" && RandomInt(4) == 1)
            me.hidden := 1;
            me.stealthsteps := 10;
         endif
			wanders := 0;
			lookAround();
			ev := sleepmode();
		else
			ev := os::wait_for_event(10);
		endif
		
		resolveEvent(ev, wanders);

		lookAround();
		sleepms(5);
	endwhile
endfunction


function resolveEvent(ev, byref wanders)
	if (!ev) return; endif;

	case(ev.type)
		SYSEVENT_ENTEREDAREA:
			if (CheckLineOfSight(me, ev.source) && CanFight(me, ev.source))
				wanders := 0;
				Fight(ev.source);
			endif
		SYSEVENT_ENGAGED:	
		SYSEVENT_DAMAGED:	
			wanders := 0; Fight(ev.source);
		EVID_ALERT_ALLIES: 
			wanders := 0; Fight(ev.opponent, 1);
		EVID_HERDING:
			herd(ev);
		EVID_TAUNT:
			if(ev.source)
				wanders := 0; Fight(ev.opponent, 1);
			endif
		EVID_FLEE:
			if (ev.source)
				flee(ev.source);
			endif
		EVID_DOOBJECTIVE:
			if (ev.destiny)
				wanders := 0; NpcGoTo(ev.destiny, ev.move_type);
			endif
	endcase
endfunction