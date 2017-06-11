use uo;	
use cfgfile;
include ":attributes:attributes";

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
	var charlevel := chardata["charlevel"];
	if (charlevel == error || !charlevel)
		SetCharLevel(who, 0);
		charlevel := 0;
	endif
	return charlevel;

endfunction

function SetCharLevel(who, points)
	var chardata := GetObjProperty(who, "chardata");
	chardata["charlevel"] := points;
	SetObjProperty(who, "chardata", chardata);
	return 1;
endfunction

function GetSkillList(who, noskill := 0)
	var chardata := GetObjProperty(who, "chardata");
	var skill_list := chardata.Skills;

	if (!skill_list)
		skill_list := array{}
	endif
	
	return skill_list;
endfunction

//Traduz o nome da skill
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

function CheckExpUp(who, skill_name, skillbefore, skillafter)
	var level := GetCharLevel(who);
	if (level < 26)
		if (skillbefore != skillafter)
			var maxexp := (GetCharLevel(who) + 5) * 30;
			var exp := GetExpPoints(who);
			exp := GetExpPoints(who) + cint(skillafter);
			
			if (exp >= maxexp)
				exp-=maxexp;
				SetCharLevel(who, level+1);
				SetSkillPoints(who, GetSkillPoints(who)+1);
				SetAttribPoints(who, GetAttribPoints(who)+3);
			endif
			
			SetExpPoints(who, exp);
		endif
	endif
endfunction
