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
	var feat_list := GetFeatList(who);
	if (feat in feat_list)
		return 1;
	endif

	return 0;
endfunction

function LearnFeat(who, feat, minus_point := 1, gives := 1)
	if (typeof(feat) != "Struct")
		feat := FeatInfo(feat, 1);
	endif

	if (typeof(feat) != "Struct")
		Print(">>>>> ERROR OCURRED, NO FEAT FOUND");
		return 0;
	endif

	var chardata := GetObjProperty(who, "chardata");
	var feat_list := chardata.Feats;
   if (!feat_list)
      feat_list := array{};
   endif
   
	if (feat.name in feat_list)
		return 0;
	endif

	feat_list.append(feat.name);
	chardata.Feats := feat_list;
	SetObjProperty(who, "chardata", chardata);
	
	if (minus_point)
		who.SetTalentPoints(-1);
	endif

   if (gives)
      foreach give_parse in (feat.give)
         give_parse := SplitWords(give_parse);
         if (give_parse[1] == "item")
            createiteminBackpack(who, give_parse[2], give_parse[3]);
         elseif (give_parse[1] == "stat")
            AP_SetTrueStat(who, give_parse[2], AP_GetTrueStat(who, give_parse[2]) + cint(give_parse[3]));
         else
            AP_SetTrueSkill(who, give_parse[2], 1);
         endif
         sleepms(2);
      endforeach
   endif

   return 1;
endfunction


function removeFeat(mobile, feat_name)
	var chardata := GetObjProperty(mobile, "chardata");
	var feat_list := chardata.Feats;

   foreach feat in feat_list
      if (lower(feat_name) == lower(feat))
         feat_list.Erase(_feat_iter);
      endif
      sleepms(2);
   endforeach

   chardata.Feats := feat_list;
   SetObjProperty(mobile, "chardata", chardata);
   return 1;
endfunction

function GetNewFeatsList(who)
	var feat_cfg := ReadConfigFile(":charactercreation:feats");
	var feat_list := array{};
	foreach feat in (GetConfigStringKeys(feat_cfg))
		sleepms(2);
		if (HaveFeat(who, feat))
			continue;
		endif

		feat := FeatInfo(feat, 1);
		if (feat.level && feat.level < who.GetLevel())
			continue;
		endif

		var pass := 1;
      if (!who.cmdlevel)
         foreach req in (feat.requirements)
            req := SplitWords(req);

            if (req[1] == "achievement")
               if(who.GetAchievement(req[2]) < cint(req[3]))
                  pass := 0;
                  break;
               endif
            elseif (req[1] == "class")
               if(!who.isClass(req[2]))
                  pass := 0;
                  break;
               endif
            elseif (req[1] == "skill")
               if(!HaveSkill(who, req[2]))
                  pass := 0;
                  break;
               endif
            elseif (req[1] == "stat")
               if(!who.GetStat(req[2]) < cint(req[3]))
                  pass := 0;
                  break;
               endif
            endif
         endforeach
      else
         pass := 1;
      endif
      if (pass)
         feat_list += feat;
      endif
   endforeach

	return feat_list;
endfunction

function FeatInfo(feat, plus := 0)
	var habcfg := ReadConfigFile(":charactercreation:feats");
	var elem   := FindConfigElem( habcfg, feat );

	if (!elem)
		return feat;
	endif
   
	var feat_info := struct{
		"name" := feat,
		"desc" :=   GetConfigString( elem, "Description" ),
		"script" := GetConfigString( elem, "Script" ),

		"mana" := GetConfigInt( elem, "Mana" ),
		"stam" := GetConfigInt( elem, "Stam" ),

		"cooldown" := GetConfigInt( elem, "Cooldown" ),

		"level" := GetConfigInt( elem, "Level" )
	};

	if (plus)
		feat_info["requirements"] := GetConfigStringArray( elem, "Require" );
		feat_info["give"] := GetConfigStringArray( elem, "Give" );
	endif

	return feat_info;
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
