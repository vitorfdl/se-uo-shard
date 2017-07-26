use uo;	
use cfgfile;
include ":attributes:attributes";
include ":charactercreation:feats";

function GetExpPoints(who)
	var chardata := GetObjProperty(who, "chardata");
	var exppoints := chardata["exppoints"];
	if (exppoints == error || !exppoints)
		SetExpPoints(who, 0);
		exppoints := 0;
	endif
	return exppoints;

endfunction

function SetExpPoints(who, points)
	var chardata := GetObjProperty(who, "chardata");
	chardata["exppoints"] := points;
	SetObjProperty(who, "chardata", chardata);
	return 1;
endfunction

function GetCharLevel(who)
	var chardata := GetObjProperty(who, "chardata");
	var charlevel := chardata["level"];
	if (charlevel == error || !charlevel)
		SetCharLevel(who, 0);
		charlevel := 0;
	endif
	return charlevel;

endfunction

function SetCharLevel(who, level)
	var chardata := GetObjProperty(who, "chardata");
	chardata.level := level;
	SetObjProperty(who, "chardata", chardata);
	return 1;
endfunction

function GetSkillList(who)
	var chardata   := GetObjProperty(who, "chardata");
	var skill_list := chardata.Skills;

	if (!skill_list)
		skill_list := array{};
	endif
	
	return skill_list;
endfunction

function GetAttribPoints(who)
	var chardata := GetObjProperty(who, "chardata");
	var attpoints := chardata["attpoints"];
	if (attpoints == error || !attpoints)
		SetAttribPoints(who, 0);
		attpoints := 0;
	endif
	return attpoints;
endfunction

function SetAttribPoints(who, points)
	var chardata := GetObjProperty(who, "chardata");
	chardata["attpoints"] := points;
	SetObjProperty(who, "chardata", chardata);
	return 1;
endfunction


//Traduz o nome da skill
function translateAttribute(attr)
	attr["Wisdom"] := "Sabedoria";
	attr["Strength"] := "Força";
	attr["Intelligence"] := "Inteligência";
	attr["Constitution"] := "Constituição";
	attr["Dexterity"] := "Destreza";

	return attr;
endfunction

function GetTranslatedName(skill)
	var classCfg := ReadConfigFile(":charactercreation:config/skilltraducao");
	var elem := FindConfigElem(classCfg, "Traducao");
	
	var translatedname := GetConfigString(elem, Lower(skill));
	if (translatedname == error)
		return skill;
	endif
	return translatedname;
endfunction

function GetUntranslatedName(skill, who)
	var classCfg := ReadConfigFile(":charactercreation:config/skilltraducao");
	var elem := FindConfigElem(classCfg, "Traducao");
	foreach eng_skill in (ListConfigElemProps(elem))
		var translatedname := GetConfigString(elem, lower(eng_skill));
		if (lower(skill) == lower(translatedname))
			return eng_skill;
		endif
		sleepms(5);
	endforeach
	
	return 0;
endfunction

function CheckExpUp(who)
	var level := GetCharLevel(who);
	if (level < 20)
		var maxexp := (GetCharLevel(who) + 1) * 1000;
		var exp := GetExpPoints(who);

		if (exp >= maxexp)
			exp-=maxexp;
			SetCharLevel(who, level+1);
			// SetSkillPoints(who, GetSkillPoints(who)+1);
			SetAttribPoints(who, GetAttribPoints(who)+3);
			SetExpPoints(who, exp);
		endif
	endif
endfunction
