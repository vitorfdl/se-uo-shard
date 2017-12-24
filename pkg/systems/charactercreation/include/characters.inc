use uo;	
use cfgfile;
include ":attributes:attributes";
include ":charactercreation:feats";


//Traduz o nome da skill
function translateAttribute(attr)
	attr["Wisdom"]       := "Sabedoria";
	attr["Strength"]     := "Força";
	attr["Intelligence"] := "Inteligência";
	attr["Constitution"] := "Constituição";
	attr["Dexterity"]    := "Destreza";

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
