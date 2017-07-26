function ReqHabilidade(habilidade)

	var habcfg := ReadConfigFile(":charactercreation:config/habilidades");
	var elem := FindConfigElem( habcfg, habilidade );

	var req := GetConfigStringArray( elem, "requisito" );

	return req;

endfunction

function CreateHabObject(who, habilidade)

	var habcfg := ReadConfigFile(":charactercreation:config/habilidades");
	var elem := FindConfigElem( habcfg, habilidade );

	var hab_objects := GetConfigStringArray( elem, "createobject" );
	foreach item in hab_objects
		var novoitem := CreateItemInBackpack(who, cint(item), 1); 
		SendSysMessage(who, "Voce recebeu " + novoitem.desc);
	endforeach

endfunction

function UsoHabilidade(nome)
	var habcfg := ReadConfigFile(":charactercreation:config/habilidades");
	var elem := FindConfigElem( habcfg, nome );
	
	var uso := lower(GetConfigString(elem, "uso"));
	if (uso == "ativa")
		var delay := GetConfigInt(elem, "delay");
		if (delay)
			nome := nome + " ["+delay+" s]";
		endif
		var stam := GetConfigInt(elem, "stam");
		if (!stam)
			stam := GetConfigInt(elem, "mana");
			if (stam)
				nome := nome + " [" + stam + " mana]";
			endif
		else
			nome := nome + " [" + stam + " stam]";
		endif
	elseif (uso == "passiva")
		nome := nome + " [Passiva]";
	elseif (uso == "tempo")
		nome := nome + " [Ativa]";
		var delay := GetConfigInt(elem, "delay");
		nome := nome + " ["+delay+" s]";
	endif
	
	return nome;
	
endfunction

function NivelHabilidade(habilidade)
	var habcfg := ReadConfigFile(":charactercreation:config/habilidades");
	var elem := FindConfigElem( habcfg, habilidade );

	var nivel := GetConfigInt( elem, "skillLevel" );

	return nivel;
endfunction

function NomeHabilidade(habilidade)

	var habcfg := ReadConfigFile(":charactercreation:config/habilidades");
	var elem := FindConfigElem( habcfg, habilidade );

	var nome := GetConfigString( elem, "nome" );

	return nome;

endfunction

function DescHabilidade(habilidade)

	var habcfg := ReadConfigFile(":charactercreation:config/habilidades");
	var elem := FindConfigElem( habcfg, habilidade );

	var desc := GetConfigString( elem, "desc" );
	var ativavel := GetConfigString( elem, "uso" );
	var DamageType := GetConfigString( elem, "DamageType" );
	
	if (DamageType)
		desc := desc + "<br>Categoria de Arma: " + DamageType;
	endif
	
	if (ativavel == "Ativa")
		desc := desc + "<br><br>Para usar habilidades ativas use '.shab habilidade', lembre-se de não usar acentos";
	endif
	

	return desc;

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
			SendSysMessageEx(who, "Voce nao possui o pre requisito: " + erromsg, SSM_FAIL);
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
	var habs := GetObjProperty(who, "myhabs");

	foreach hab in habs
		if (hab == habilidade)
			return 1;
		endif

	endforeach

	return 0;
endfunction

function IsBloodied(mobile)

	//if (mobile.npctemplate)
		if (AP_GetVital(mobile, HITS) < CInt(AP_GetVitalMaximumValue(mobile, HITS)*0.3))
			return 1;
		endif
	/*elseif ( AP_GetVital(mobile, HITS) < GetHP(mobile) )
		return 1;
	endif*/

	return 0;

endfunction

function HabilidadesPorSkilleNivel(who, skill, lvl)
	var possiveis := array;

	var habcfg := ReadConfigFile(":charactercreation:config/habilidades");
	var keys := getConfigStringKeys( habcfg );

	foreach key in keys
		var elem := FindConfigElem( habcfg, key );
		var d := GetConfigString( elem, "skill" );
		var f := GetConfigInt( elem, "skillLevel" );
		
		if ( cint(f) == lvl && lower(d) == lower(skill))
			possiveis.append(key);
		endif
		sleepms(5);
	endforeach

	return possiveis;

endfunction