include ":attributes:attributes";

function getFeatById(feat_id)
   var featCfg := ReadConfigFile(":charactercreation:config/feats");
   var elem := FindConfigElem(featCfg, feat_id);

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
      PointCost := GetConfigInt(elem, "PointCost")
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

function GetGodFeats(god)
   var classCfg := ReadConfigFile(":charactercreation:config/deuses");
   var godelem := FindConfigElem(classCfg, god);
   if (!godelem) return; endif

   var hab_ids := GetConfigStringArray(godelem, "Habilidade");
   var feat_list := array{};
   foreach id in hab_ids
      feat_list.append(getFeatById(id));
      sleepms(2);
   endforeach

   return feat_list;
endfunction

function GetFeatList(who)
	var chardata := GetObjProperty(who, "chardata");
   var all_feats := array{};

   if (chardata.classes)
      foreach classe in (chardata.classes)
         all_feats := all_feats + classe.selected_habs;
         sleepms(2);
      endforeach
   endif

   if (chardata.subclasses)
      foreach classe in (chardata.subclasses)
         all_feats := all_feats + classe.habs;
         sleepms(2);
      endforeach
   endif

	return feat_list;
endfunction

function HaveFeat(who, feat)
	var feat_list := GetFeatList(who);

   if (typeOf(feat) != "Integer")
      return 0;
   endif

	if (feat in feat_list)
		return 1;
	endif

	return 0;
endfunction

function LearnFeat(who, feat_id)
   var feat := getFeatById(feat_id);
   if (!feat) return SysLog("Error: {} feat_id não existe!".format(feat_id)); endif

   var classCfg := ReadConfigFile(":charactercreation:config/classes");
   var elem := FindConfigElem(classCfg, feat.Class);
   
   var class_type := "classes";
   if (elem.Type != "Main") class_type := "subclasses"; endif

   var chardata := GetObjProperty(who, "chardata");

   var index := FindInArray(chardata[class_type], struct{id := feat.Class});
   if (!index) return; endif

   chardata[class_type][index].habs.append(feat_id);
   who.SetPoints(-1);
   return 1;
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
