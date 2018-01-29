
include ":timeUtils:time";
//se tiver passado o tempo retorna 0, se não retorna o tempo que falta
function getCooldown(who, hability)

	hability := "#{}".format(hability);

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
function setCooldown(who, hability, time)
	
	hability := "#{}".format(hability);

	var cooldown := GetCoolDown(who, hability);
	
	if (cint(cooldown))
		return cooldown;
	else
		SetObjProperty(who, hability, AddTimeTo(polcore().systime, time, "seconds") );
		return 0;
	endif
	
endfunction
