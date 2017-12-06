function CreateHabObject(who, habilidade)

	var habcfg := ReadConfigFile(":charactercreation:config/habilidades");
	var elem := FindConfigElem( habcfg, habilidade );

	var hab_objects := GetConfigStringArray( elem, "createobject" );
	foreach item in hab_objects
		var novoitem := CreateItemInBackpack(who, cint(item), 1); 
		SendSysMessage(who, "Você recebeu " + novoitem.desc);
	endforeach

endfunction

function TemRequisito(who, habilidade, msg := 1)

	var habcfg := ReadConfigFile(":charactercreation:config/habilidades");
	var elem := FindConfigElem( habcfg, habilidade );

	var requisitos := GetConfigStringArray(elem, "requisito");
	var erro := 1;
	var erromsg := "";
	foreach req in requisitos
		if ( !TemHabilidade(who, req) )
			erromsg := erromsg + " " + req;
		else
			erro := 0;
		endif
	endforeach

	if (erro && requisitos[1])
		if (msg == 1)
			SendSysMessageEx(who, "Você não possui o pre requisito: " + erromsg, SSM_FAIL);
		endif
		return 0;
	endif
	
	var skillLevel := GetConfigInt(elem, "skillLevel");
	var skillname := GetConfigInt(elem, "skillname");
	if (skillLevel > AP_GetSkill(who,GetConfigString(elem, "skill")))
		if (msg == 1)
			SendSysMessageEx(who, "Você não possui " + skillLevel +  " na pericia " + GetTranslatedName(skillname), SSM_FAIL);
		endif
		return 0;
	endif
	return 1;
endfunction

function retornaRequisitos( habilidade)
	var habcfg := ReadConfigFile(":charactercreation:config/habilidades");
	var elem := FindConfigElem( habcfg, habilidade );

	var requisitos := GetConfigStringArray(elem, "requisito");

	return requisitos;
endfunction

function TemHabilidade(who, habilidade)
	// var habs := GetObjProperty(who, "myhabs");

	// foreach hab in habs
	// 	if (hab == habilidade)
	// 		return 1;
	// 	endif

	// endforeach

	return 0;
endfunction

function IsBloodied(mobile)
	// if (AP_GetVital(mobile, HITS) < CInt(AP_GetVitalMaximumValue(mobile, HITS)*0.3))
	// 	return 0;
	// endif
	/*elseif ( AP_GetVital(mobile, HITS) < GetHP(mobile) )
		return 1;
	endif*/

	return 0;

endfunction