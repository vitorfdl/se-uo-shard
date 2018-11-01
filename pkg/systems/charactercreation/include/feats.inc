include ":attributes:attributes";
include "include/colormap";

const RETURN_ACTIVE_ONLY := 1;

function getFeatById(feat_id, get_class := 0)
   var featCfg := ReadConfigFile(":charactercreation:config/feats");
   var elem := FindConfigElem(featCfg, feat_id);

   if (get_class)
      var classCfg := ReadConfigFile(":charactercreation:config/classes");
      var class_elem := FindConfigElem(classCfg, elem.Class);
      if (class_elem.Type == "Main")
         get_class := "classes";
      else
         get_class := "subclasses";
      endif
   endif

   return struct{
      FeatID := GetConfigInt(elem, "FeatID"),
      Name := GetConfigString(elem, "Name"),
      Class := GetConfigString(elem, "Class"),
      Description := GetConfigString(elem, "Description"),
      Mana := GetConfigInt(elem, "Mana"),
      Vigor := GetConfigInt(elem, "Vigor"),
      Cooldown := GetConfigInt(elem, "Cooldown"),
      Icon := GetConfigInt(elem, "Icon"),
      CastTime := GetConfigInt(elem, "CastTime"),
      Script := GetConfigString(elem, "Script"),
      Targ := GetConfigString(elem, "Target"),
      Group := GetConfigInt(elem, "Group"),
      Level := GetConfigInt(elem, "Level"),
      Experience := GetConfigInt(elem, "Experience"),
      PointCost := GetConfigInt(elem, "PointCost"),
      ClassType := get_class
   };
endfunction

function GetClassFeats(class_id, elem := 0)
   if (!elem)
      var classCfg := ReadConfigFile(":charactercreation:config/classes");
      elem := FindConfigElem(classCfg, class_id);
   endif

   if (!elem) return; endif

   var hab_ids := GetConfigStringArray(elem, "Habilidade");

   var feat_list := array{};
   foreach id in hab_ids
      feat_list.append(getFeatById(id));
      sleepms(2);
   endforeach

   return feat_list;
endfunction

function GetRaceFeats(race, return_id := 0)
   var raceCfg := ReadConfigFile(":charactercreation:config/races");
   var elem := FindConfigElem(raceCfg, race);

   if (!elem) return; endif

   var hab_ids := GetConfigStringArray(elem, "Habilidade");
   if (return_id) return hab_ids; endif

   var feat_list := array{};
   foreach id in hab_ids
      feat_list.append(getFeatById(id));
      sleepms(2);
   endforeach

   return feat_list;
endfunction

function GetGodFeats(god, return_id := 0)
   var classCfg := ReadConfigFile(":charactercreation:config/deuses");
   var godelem := FindConfigElem(classCfg, god);
   if (!godelem) return; endif

   var hab_ids := GetConfigStringArray(godelem, "Habilidade");
   if (return_id) return hab_ids; endif

   var feat_list := array{};
   foreach id in hab_ids
      feat_list.append(getFeatById(id));
      sleepms(2);
   endforeach

   return feat_list;
endfunction

function GetFeatList(who, only_active := 0)
	var chardata := GetObjProperty(who, "chardata");
   var feat_list := array{};
   
   if (chardata.current_class && chardata.classes)
      var classe := FindElemInArray(chardata.classes, struct{ id := chardata.current_class});
      if (classe)
         feat_list := feat_list + classe.selected_habs;
      endif
      
      sleepms(2);
   endif

   foreach classe in (chardata.subclasses)
      feat_list := feat_list + classe.habs;
      sleepms(2);
   endforeach

   feat_list := feat_list + GetRaceFeats(chardata.raca, 1);
   feat_list := feat_list + GetGodFeats(chardata.god, 1);

	if (!feat_list)
		feat_list := array{};
	endif

   var return_list := array{};
   foreach feat in feat_list
      sleepms(2);
      feat := GetFeatByid(feat);
      if ((only_active && !feat.Script) || (chardata.level < feat.Level))
         continue;
      endif

      return_list.append(feat);
   endforeach
	
	return return_list;
endfunction

function HaveFeat(who, feat)
	var feat_list := GetFeatList(who);
	if (feat in feat_list)
		return 1;
	endif

	return 0;
endfunction

function LearnFeat(who, feat, minus_point := 0, gives := 1)
	if (typeof(feat) != "Struct")
		feat := FeatInfo(feat, 1);
	endif

	if (typeof(feat) != "Struct")
		Print(">>>>> ERROR OCURRED, NO FEAT FOUND {}".format(feat));
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

   if (feat.type == "Classe")
      var class_feats := chardata.class_feats;
      if (!class_feats || typeof(class_feats) != "Array" )
         class_feats := array{};
      endif
      class_feats.append(feat.level);
      chardata.class_feats := class_feats;
   endif

	feat_list.append(feat.name);
	chardata.Feats := feat_list;
	SetObjProperty(who, "chardata", chardata);
	
	// if (minus_point)
	// 	who.SetTalentPoints(-1);
	// endif

   if (gives)
      foreach give_parse in (feat.give)
         give_parse := SplitWords(give_parse);
         if (give_parse[1] == "item")
            createiteminBackpack(who, give_parse[2], give_parse[3]);
         elseif (give_parse[1] == "stat")
            AP_SetTrueStat(who, give_parse[2], AP_GetTrueStat(who, give_parse[2]) + cint(give_parse[3]));
         // else
         //    var oldskill := AP_GetTrueSkill(who, give_parse[2]);
         //    AP_SetTrueSkill(who, give_parse[2], oldskill+1);
         endif
         sleepms(2);
      endforeach
   endif
   return 1;
endfunction


function removeFeat(mobile, feat_name)
	var chardata := GetObjProperty(mobile, "chardata");
	var feat_list := chardata.Feats;

   foreach feat_name2 in feat_list
      if (lower(feat_name) == lower(feat_name2))
         feat_list.Erase(_feat_name2_iter);
      endif

      var feat := FeatInfo(feat_name2);
      foreach give_parse in (feat.give)
         give_parse := SplitWords(give_parse);
         if (give_parse[1] == "stat")
            AP_SetTrueStat(mobile, give_parse[2], AP_GetTrueStat(mobile, give_parse[2]) - cint(give_parse[3]));
         else
            var oldskill := AP_GetTrueSkill(mobile, give_parse[2]);
            AP_SetTrueSkill(mobile, give_parse[2], oldskill-1);
         endif
         sleepms(2);
      endforeach
      sleepms(2);
   endforeach

   chardata.Feats := feat_list;
   SetObjProperty(mobile, "chardata", chardata);
   return 1;
endfunction

function GetNewFeatsList(who, type := "normal", levels := array{})
	var feat_cfg := ReadConfigFile(":charactercreation:feats");
	var feat_list := array{};
	foreach feat in (GetConfigStringKeys(feat_cfg))
		sleepms(2);
		if (HaveFeat(who, feat))
			continue;
		endif

		feat := FeatInfo(feat, 1);
		if (feat.level && cint(feat.level) > who.GetLevel() && who.cmdlevel < 3)
			continue;
		endif

		var pass := 1;
      if (!who.cmdlevel)
         if(type == "Classe")
            if (feat.type != "Classe")
               continue;
            elseif (!(feat.level in levels))
               continue;
            endif
         elseif (feat.type)
            continue;
         endif
         foreach req in (feat.requirements)
            req := SplitWords(req);

            if (req[1] == "achievement")
               if(who.GetAchievement(req[2]) < cint(req[3]))
                  pass := 0;
                  break;
               endif
            elseif (req[1] == "skill")
               if(!HaveSkill(who, req[2]))
                  pass := 0;
                  break;
               endif
            elseif (req[1] == "stat")
               if(AP_GetStat(who, req[2]) < cint(req[3]))
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

		"level" := GetConfigInt( elem, "Level" ),
		"ph" := GetConfigInt( elem, "PH" ),
		"type" := GetConfigString( elem, "Type" )
	};

	if (plus)
		feat_info["requirements"] := GetConfigStringArray( elem, "Require" );
		feat_info["give"] := GetConfigStringArray( elem, "Give" );
	endif

	return feat_info;
endfunction

function BuildFeatToolTip(feat, return_points := 0)
   var desc := "";
   if (return_points && feat.PointCost) desc += "{}Custo: {} ponto(s)<br>".format(LIGHTGRAY, feat.PointCost); endif
   if (feat.Mana) desc += "{}Mana: {}<br>".format(BLUE, feat.Mana); endif
   if (feat.Vigor) desc += "{}Vigor: {}<br>".format(YELLOW, feat.Vigor); endif
   if (feat.Cooldown) desc += "{}Cooldown: {}<br>".format(LIGHTGRAY, feat.Cooldown); endif
   if (feat.Description) desc += "{}{}<br>".format(WHITE, feat.Description); endif
   var tooltip := "<center><basefont color=#7FFFD4><b>{}</b></basefont><br>{}".format(feat.Name, desc);

   return tooltip;
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
