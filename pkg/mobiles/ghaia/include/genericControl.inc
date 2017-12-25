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
   var loops := 0;
	while(me)
		sleepms(15);
      if (npccfgfile.Category == "moriquendi")
         if (GetGlobalProperty("dia") == "dia" && !IsDungeon(me.x, me.y))
            ApplyRawDamage(me, 200);
            ApplyRawDamage(me, 200);
         endif
      endif

		//se tem objetivo, anda na direcao dele, se não tem so wander
		if (GetObjProperty(me, "objloc") != error)
			wanders := 0;
			DoObjective();
		elseif (!LookForFood())
			MoveAround(wanders);
		endif
		
		wanders := wanders +1;
		if (wanders > 20 && RandomInt(2) == 1)
         if (ListMobilesNearLocationEx(me.x, me.y, me.z, 20, LISTEX_FLAG_NORMAL|LISTEX_FLAG_HIDDEN|LISTEX_FLAG_PLAYERS_ONLY).size() > 0)
			   wanders := 0;
         endif
      endif

		if (wanders >= 30)
         if (npccfgfile.CustomBehavior == "camuflar" && RandomInt(8) == 1)
            me.hidden := 1;
            me.stealthsteps := 5;
         endif
			wanders := 0;
			lookAround();
         loops := 0;
			ev := sleepmode();
		else
			ev := os::wait_for_event(10);
		endif
		
		resolveEvent(ev, wanders);

		lookAround();
      loops += 1;
      if (loops >= 150)
         print(me.serial+" "+me.name);
      endif
	endwhile
endfunction


function resolveEvent(ev, byref wanders)
	if (!ev) return; endif;

	case(ev.type)
		EVID_DOOBJECTIVE:
			if (ev.destiny)
				wanders := 0; NpcGoTo(ev.destiny, ev.move_type);
			endif
		SYSEVENT_ENTEREDAREA:
			if (CheckLineOfSight(me, ev.source) && CanFight(me, ev.source))
				// wanders := 0;
            // if (((me.listAggro()).keys()).size() < 1)
            //    me.setAggro(ev.source.serial, 20);
            // else
            //    me.setAggro(ev.source.serial, 15);
            // endif
            var mobs := ListMobilesNearLocation(ev.source.x, ev.source.y, ev.source.z, 5);
            foreach mob in mobs
               if (mob == ev.source)
                  me.setAggro(mob.serial, mob.getToughness()+12);
               elseif (CanFight(me, mob))
                  me.setAggro(mob.serial, mob.getToughness()+10);
               endif
               sleepms(2);
            endforeach
				Fight(ev.source);
			endif
		SYSEVENT_ENGAGED:
		SYSEVENT_DAMAGED:
         me.setAggro(ev.source.serial, cint(ev.damage/2)+1+cint(ev.source.getToughness()));
			wanders := 0; Fight(ev.source);
		EVID_ALERT_ALLIES: 
         me.setAggro(ev.opponent, 5);
			wanders := 0; Fight(ev.opponent, 1);
		EVID_HERDING:
			herd(ev);
		EVID_TAUNT:
			if(ev.source)
            me.setAggro(ev.source, 15);
				wanders := 0; Fight(ev.opponent, 1);
			endif
		EVID_FLEE:
			if (ev.source)
				flee(ev.source);
			endif
	endcase
endfunction
