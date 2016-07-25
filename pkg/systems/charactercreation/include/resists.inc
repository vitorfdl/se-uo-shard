include ":attributes:attributes";
include ":tn:cooldown";
include ":charactercreation:habilidades";

const FORTITUDE := "Fortitude";
const REFLEXOS := "Reflexos";
const VONTADE := "Vontade";

function GetResist(who, resist)
	var chardata := GetObjProperty(who, "chardata");
	var classe := chardata.classe;
	var vantagem := chardata.vantagemracial;
	var kit := chardata.kit;

	var valor;

	if (resist == VONTADE)
		if (GetCooldown(who, "desviarmente") > 0) // Se tiver usado habilidade Desviar Mente
			resist := REFLEXOS;
		endif
	endif

	if (resist == FORTITUDE)
		if (classe == "Guerreiro")
			valor := 30;
		else
			valor := 15;
		endif

		valor := valor + CInt(AP_GetTrueStat(who, STRENGTH)/10);

		valor := valor + CInt(GetObjProperty(who, "#FortitudeMod"));
	elseif (resist == REFLEXOS)
		if (classe == "Ladino")
			valor := 30;
		else
			valor := 15;
		endif

		valor := valor + CInt(AP_GetTrueStat(who, DEXTERITY)/10);

		valor := valor + CInt(GetObjProperty(who, "#ReflexosMod"));
	else // Vontade
		if (resist == "Sabio")
			valor := 30;
		else
			valor := 15;
		endif

		if (vantagem == "Vontade de Ferro")
			valor := valor + 20;
		endif
		
		if (temHabilidade(who, "Treinamento Vigilante"))
			valor := valor + 40;
		endif

		if ( GetCooldown(who, "VontadeIndomavel"))
			valor := valor + 100;
		endif

		valor := valor + CInt(AP_GetTrueStat(who, INTELLIGENCE)/10);

		valor := valor + CInt(GetObjProperty(who, "#VontadeMod"));
	endif

	if (kit == "Fortitude do Urso")
		if (resist == FORTITUDE)
			valor := valor + 15;
		else
			valor := valor - 7;
		endif
	elseif (kit == "Reflexos de Pantera")
		if (resist == REFLEXOS)
			valor := valor + 15;
		else
			valor := valor - 7;
		endif
	elseif (kit == "Vontade da Aguia")
		if (resist == VONTADE)
			valor := valor + 15;
		else
			valor := valor - 7;
		endif
	endif

	if (vantagem == "Resistentes")
		valor := valor + 10;
	endif

	return valor;
endfunction

function CheckResist(who, resist, dif, mod := 0)
	var dado := RandomInt(100) + 1;
	var valor := GetResist(who, resist);

	//printtextabove(who, "dado " + dado + " valor " + valor + " diff " + dif); 

	if (dado + valor + mod >= dif)
		return 1;
	else
		if ((GetObjProperty(who, "chardata")).povo == "Zharkiano" && IsBloodied(who))
			// Se for Zharkiano e tiver Bloodied ganha segunda chance
			dado := RandomInt(100) + 1;
			if (dado + valor + mod >= dif)
				SendSysMessageEx(who, "Zharkia esta do seu lado!", SSM_INFO);
				return 1;
			endif
		endif
		return 0;
	endif
endfunction
