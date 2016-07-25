use uo;	
use cfgfile;
include ":attributes:attributes";

function checkskillcap(who)

  foreach skill in AP_ListAttributesByType()
		if ( (skill != "Dexterity") && (skill != "Strength") && (skill != "Intelligence"))
      var skillvalue := AP_GetTrueSkill(who, skill);
			if ( skillvalue > 0 )
        if ( skillvalue > GetLocalSkillCap(who, skill) )
          AP_SetTrueSkill(who, skill, GetLocalSkillCap(who, skill));
        endif
      endif
    endif
  endforeach

endfunction

function GetLocalSkillCap(mobile, skill_name)

  var elem := AP_GetAttributeCfgElem(skill_name);
	var stat := GetConfigString(elem, "Stat");
  
  var att := AP_GetTrueStat(mobile, stat);
  return att;

endfunction


//as funções abaixo sao old. usadas no antigo sistema de classes
// nada de util
// pode deletar acho
function GetSkillCap(who, skillname)

	if (!GetObjProperty(who, "chardata"))
		return 100.0;
	endif

	var primarias := GetSkills(who, "primarias");
	var secundarias := GetSkills(who, "secundarias");
	var terciarias := GetSkills(who, "terciarias");

	foreach skill in secundarias
		if (skillname == skill)
			return 70.0;
		endif
	endforeach

	foreach skill in terciarias
		if (skillname == skill)
			return 40.0;
		endif
	endforeach

	foreach skill in primarias
		if (skillname == skill)
			return 100.0;
		endif
	endforeach

	return 0.0;

endfunction

function GetSkills(who, category)

	var grad := 0;
	if (category == "primarias")
		grad := 3;
	elseif (category == "secundarias")
		grad := 2;
	elseif (category == "terciarias")
		grad := 1;
	endif

	if (!who.npctemplate)

		var chardata := GetObjProperty(who, "chardata");
		if ( (!chardata) || (chardata == error))
			return 0;
		else
			var skills := chardata.Skills;
			var skillsName := skills.keys();
			var primarias := array;
			foreach skill in skillsName
				if (skills[skill] == grad)
					primarias.append(skill);
				endif
			endforeach
			return primarias;
		endif
	endif

	return 0;
endfunction

//Get e Set SkillPoints
function GetSkillPoints(who)
	var chardata := GetObjProperty(who, "chardata");
	var skillpoints := chardata["skillpoints"];
	if (skillpoints == error || !skillpoints)
		SetSkillPoints(who, 0);
		skillpoints := 0;
	endif
	return skillpoints;
endfunction

function SetSkillPoints(who, points)
	var chardata := GetObjProperty(who, "chardata");
	chardata["skillpoints"] := points;
	SetObjProperty(who, "chardata", chardata);
	return 1;
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
	var skill_list := array;

	foreach skill in (chardata.Skills.keys())
		if (noskill)
			if (lower(skill) == "gathering" || lower(skill) == "tactics" || lower(skill) == "research")
				continue;
			endif
		endif
		skill_list.append(skill);
	endforeach
	
	return skill_list;
endfunction

//Pega valor da skill na classe
function GetSkillClass(who, skill)
	var classCfg := ReadConfigFile(":charactercreation:config/classes");
	var elem := FindConfigElem(classCfg, getobjproperty(who, "classe"));
	if (skill in GetConfigStringArray(elem, "P"))
		return 1;
	elseif (skill in GetConfigStringArray(elem, "S"))
		return 2;
	elseif (skill in GetConfigStringArray(elem, "T"))
		return 3;
	elseif ((skill in GetConfigStringArray(elem, "Q")) || (skill in GetConfigStringArray(elem, "C")))
		return 4;
	endif
	return 0;
endfunction

//Pega os pontos gastos na skill por classe
function GetSkillValue(who, skill)
	var skillclass := GetSkillClass(who, skill);
	if (skillclass == 1 or skillclass == 4)
		return 1;
	elseif (skillclass == 2)
		return 2;
	elseif (skillclass == 3)
		return 3;
	endif
	return 0;
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



function GetSkillLevel(who, skill, number := 0)
	var chardata := GetObjProperty(who, "chardata");
	if (!chardata.Skills[skill])
		return;
	endif
	
	if (chardata.Skills[skill] == 1)
		if (number)	return 1;
		endif
		return "Amador";
	elseif (chardata.Skills[skill] == 2)
		if (number)	return 2;
		endif
		return "Aprendiz";
	elseif (chardata.Skills[skill] == 3)
		if (number)	return 4;
		endif
		return "Hábil";
	elseif (chardata.Skills[skill] == 4)
		if (number)	return 5;
		endif
		return "Mestre";
	elseif (chardata.Skills[skill] == 5)
		if (number)	return 6;
		endif
		return "Grão-Mestre";		
	endif
endfunction

function CheckExpUp (who, skill_name, skillbefore, skillafter)
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

function UpSkilLLevel(item, skill_name, times, maxlevel := 0)
	var x := 1;
	while (x <= times)
		var true_skill := AP_GetTrueSkill(item, skill_name);
		
		var skillbefore := floor(AP_GetTrueSkill(item, skill_name));
		if (maxlevel)
			if (true_skill >= 70)
				AP_SetTrueSkill(item, skill_name, skillbefore + 0.5);
			else
				AP_SetTrueSkill(item, skill_name, skillbefore+1);
			endif
		else
			AP_SetTrueSkill(item, skill_name, skillbefore+1);
		endif
		
		var skillafter := floor(AP_GetTrueSkill(item, skill_name));
		
		CheckExpUp(item, skill_name, skillbefore, skillafter);
		x+=1;
	endwhile
endfunction
