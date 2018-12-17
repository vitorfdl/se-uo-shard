
include ":timeUtils:time";


function GetScriptCooldown(who, script_name)
   if (!getCooldown(who, script_name))
      setCooldown(who, script_name, 1);
      return 0;
   endif

   SendSysMessageEx(who, "Prevenção de Flood. Aguarde +1 segundo.", SSM_FAIL);
   return 1;
endfunction

//se tiver passado o tempo retorna 0, se não retorna o tempo que falta
function getCooldown(who, hability, no_temp := 0)
   if (!hability["#"] && !no_temp)
	   hability := "#{}".format(hability);
   endif

	var now := polcore().systime;
	var future := Cint(GetObjProperty(who, hability));
	if (now >= future)
		EraseObjProperty(who, hability);
		return 0;
	else
		return (future-now);
	endif
endfunction
	
//verifica se ta em cooldown ainda, se tiber retorna o tempo que falta, se não tiver seta e retorna 0
function setCooldown(who, hability, time, no_temp := 0)
   if (!hability["#"] && !no_temp)
	   hability := "#{}".format(hability);
   endif

	var cooldown := GetCoolDown(who, hability);
	
	if (cint(cooldown))
		return cooldown;
	else
		SetObjProperty(who, hability, AddTimeTo(polcore().systime, time, "seconds") );
		return 0;
	endif
	
endfunction
