include "include/say";

function sendToSpy(source, message_txt, color := 0);
   var sender := source.getName();
	foreach player in GetGlobalProperty("GmsOnlines")
		Sleepms(5); 
		player := SystemFindObjectBySerial(Cint(player));
      var sayspy := GetObjProperty(player, "SaySpyOn");

		if ( sayspy && player.cmdlevel >= 3 && player != source && (!CheckLineOfSight(player, source) || player.enabled("losany")))
			if (sayspy == "on")
				SendSysMessageEx(player, "["+sender+"]: "+message_txt, color);
			elseif(sender in sayspy)
				SendSysMessageEx(player, "["+sender+"]: "+message_txt, color);
			endif
		endif
	endforeach
endfunction
