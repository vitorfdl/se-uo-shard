// function ReqHabilidade(habilidade)

// 	var habcfg := ReadConfigFile(":charactercreation:config/habilidades");
// 	var elem := FindConfigElem( habcfg, habilidade );

// 	var req := GetConfigStringArray( elem, "requisito" );

// 	return req;

// endfunction

// function UsoHabilidade(nome)
// 	var habcfg := ReadConfigFile(":charactercreation:config/habilidades");
// 	var elem := FindConfigElem( habcfg, nome );
	
// 	var uso := lower(GetConfigString(elem, "uso"));
// 	if (uso == "ativa")
// 		var delay := GetConfigInt(elem, "delay");
// 		if (delay)
// 			nome := nome + " ["+delay+" s]";
// 		endif
// 		var stam := GetConfigInt(elem, "stam");
// 		if (!stam)
// 			stam := GetConfigInt(elem, "mana");
// 			if (stam)
// 				nome := nome + " [" + stam + " mana]";
// 			endif
// 		else
// 			nome := nome + " [" + stam + " stam]";
// 		endif
// 	elseif (uso == "passiva")
// 		nome := nome + " [Passiva]";
// 	elseif (uso == "tempo")
// 		nome := nome + " [Ativa]";
// 		var delay := GetConfigInt(elem, "delay");
// 		nome := nome + " ["+delay+" s]";
// 	endif
	
// 	return nome;
	
// endfunction


function GetFeatList(who)
	var chardata := GetObjProperty(who, "chardata");
	var feat_list := chardata.Feats;

	if (!feat_list)
		feat_list := array{};
	endif
	
	return feat_list;
endfunction

function HaveFeat(who, feat)
	var feat_list = GetFeatList(who);
	if (feat in feat_list)
		return 1;
	endif

	return 0;
endfunction

function FeatName(feat)

	var habcfg := ReadConfigFile(":charactercreation:config/feats");
	var elem := FindConfigElem( habcfg, feat );

	var nome := GetConfigString( elem, "nome" );

	return nome;
endfunction

function FeatInfo(feat)

	var habcfg := ReadConfigFile(":charactercreation:config/feats");
	var elem   := FindConfigElem( habcfg, feat );

	var desc       := GetConfigString( elem, "Desc" );
	var ativavel   := GetConfigString( elem, "Use" );
	var DamageType := GetConfigString( elem, "DamageType" );
	
	if (DamageType)
		desc := desc + "<br>Categoria de Arma: " + DamageType;
	endif
	
	if (ativavel == "Ativa")
		desc := desc + "<br><br>Para usar Talentos ativos use '.shab Talento', lembre-se de não usar acentos";
	endif
	
	return desc;
endfunction

// function IsBloodied(mobile)

// 	//if (mobile.npctemplate)
// 		if (AP_GetVital(mobile, HITS) < CInt(AP_GetVitalMaximumValue(mobile, HITS)*0.3))
// 			return 1;
// 		endif
// 	/*elseif ( AP_GetVital(mobile, HITS) < GetHP(mobile) )
// 		return 1;
// 	endif*/

// 	return 0;

// endfunction
