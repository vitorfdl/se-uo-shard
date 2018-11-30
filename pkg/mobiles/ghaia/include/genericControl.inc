include "objective";
include "include/dungeons";

function genericControl()
	var ev;
	var wanders;

	set_critical(1);
	InitializeNPC(me);
	set_critical(0);
	SetWarMode( 0 );
	ev := os::wait_for_event(0);
	wanders := 30;
	EnableMainEvents();
	Set_Event_Queue_Size(50);
   var loops := 0;
	while(me)
		sleepms(15);
      if (npccfgfile.Category == "moriquendi")
         if (GetGlobalProperty("dia") == "dia" && !InDungeon(me))
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
		
		wanders += 1;
		if (wanders > 20 && RandomInt(2) == 1)
         if (HasPlayerNear(me.x, me.y, me.realm))
			   wanders := 0;
            loops := 0;
         endif
      endif

		if (wanders >= 30 || me.frozen)
         if ("camuflar" in custombehaviors && RandomInt(8) == 1)
            me.hidden := 1;
            me.stealthsteps := 5;
         endif

         if ("changeskin" in custombehaviors && !me.concealed)
            me.paralyzed := 1;
            me.concealed := 1;
            var changeskin := GetObjProperty(me, "changeskin");
				var item := CreateItemAtLocation(me.x, me.y, me.z, changeskin, 1, me.realm);

            SetObjProperty(me, "skinchanged", item.serial);
         endif

			wanders := 0;
			lookAround();
         loops := 0;
			ev := sleepmode();
         EraseObjProperty(me,"#sleepmode");
		else
			ev := os::wait_for_event(10);
		endif
		
		resolveEvent(ev, wanders);
      EraseObjProperty(me, "#charge_enemy");
		lookAround();
      loops += 1;
      if (loops >= 200 && me.alignment != 2)
         SysLog("I'm stuck: " + me.serial+" "+me.name);
         SetObjProperty(me, "GuardKill", 1);	
         ApplyRawDamage(me, 500);
      endif
	endwhile
endfunction


function resolveEvent(ev, byref wanders)
	if (!ev) return; endif;

	case(ev.type)
		EVID_DOOBJECTIVE:
			if (ev.destiny)
				wanders := 0; NpcGoTo(ev.destiny, ev.move_type, ev.can_stop);
			endif
         break;
		SYSEVENT_ENTEREDAREA:
         if (!CheckLineOfSight(me, ev.source))
            break;
         elseif (!CanFight(me, ev.source) || areasize < Distance(me, ev.source))
            break;
         endif
         wanders := 0;

         if ("changeskin" in custombehaviors && me.concealed)
            var changeskin := GetObjProperty(me, "skinchanged");
            if (changeskin)
               var item := SystemFindObjectBySerial(changeskin);
               DestroyItem(item);
            endif
            me.concealed := 0;
            me.paralyzed := 0;
         endif
 
         // if (!me.listAggro().size())
         me.setAggro(ev.source.serial, ev.source.getToughness()+60);
         // endif
         // var mobs := ListMobilesNearLocation(ev.source.x, ev.source.y, ev.source.z, 5);
         // foreach mob in mobs
         //    if (mob == ev.source)
         //       me.setAggro(mob.serial, mob.getToughness()+12);
         //       // if (HaveFeat(mob, "Cadeia de Comando"))
         //       //    var followers := ListPetsNear(mob, 5);
         //       //    foreach follower in followers
         //       //       me.setAggro(follower.serial, 5);
         //       //       sleepms(2);
         //       //    endforeach
         //       // endif
         //    elseif (CanFight(me, mob))
         //       me.setAggro(mob.serial, mob.getToughness()+8);
         //    endif
         //    sleepms(2);
         // endforeach

         Fight(ev.source);
         break;
		SYSEVENT_ENGAGED:
		SYSEVENT_DAMAGED:
         me.setAggro(ev.source.serial, cint(ev.damage/2)+1+cint(ev.source.getToughness()));
			wanders := 0; Fight(ev.source);
         break;
		EVID_ALERT_ALLIES:
         if ("changeskin" in custombehaviors && me.concealed)
            break;
         endif      
         me.setAggro(ev.opponent, 40);
			wanders := 0; Fight(ev.opponent, 1);
         break;
		EVID_HERDING:
			herd(ev);
         break;
		EVID_TAUNT:
			if(ev.source)
            me.setAggro(ev.source, 80);
				wanders := 0; 
            Fight(ev.source, 1);
			endif
         break;
		EVID_FLEE:
			if (ev.source)
				flee(ev.source);
			endif
         break;
      EVID_BLINDED:
         MakeBlind(me, ev.remove);
	endcase
endfunction
