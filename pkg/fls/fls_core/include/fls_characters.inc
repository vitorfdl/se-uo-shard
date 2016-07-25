
use uo;
use cfgfile;

include ":ghaia:ghaiaUtil";
include ":attributes:attributes";
include ":fls_core:fls_chargumps_ex";

function AwardClassSkill(who, diff, chance, classe)

	var myclass := GetObjProperty(who, "classe");
	if (classe != myclass)
		return;
	endif

	CheckSkillAdvance(who, BEGGING, diff, 100 );

endfunction

function IsInDungeon(player)

	if (player.x >= 3749 && player.y >= 0 && player.x <= 6141 && player.y <= 4000)
		return 1;
	endif

	return 0;

endfunction

function HaveReverseVision(who)

	if (GetObjProperty(who, "raca") == "Drow")
		return 1;
	elseif (cint(GetObjProperty(who, "reversevision")) == 1)
		return 1;
	else
		return 0;
	endif

endfunction

function ApllyLightLevel()
	foreach player in enumerateonlinecharacters()
		if (HaveReverseVision(player))
			ReverseLight(player);
		endif
	endforeach
endfunction

function ReverseLight(player)
	var config := readconfigfile(":nature:config/daynight");
	config := config["settings"];
	var darkness := cint(config.darkness);

	var light := cint(getglobalproperty("lightlevel"));

	if (player.x >= 3749 && player.y >= 0 && player.x <= 6141 && player.y <= 4000)
		player.setlightlevel(0, 9000); // Is in a dungeon
		print("em dungeon");
	else
		print("fora dunfeon");
		var newlight := abs(darkness - light);
		player.setlightlevel(newlight, 9000);
	endif

endfunction

function GetClass(who)
	var classe := GetObjProperty(who, "classe");
	if (!classe)
		return 0;
	else
		return classe;
	endif
endfunction

function GetRace(who)
	var raca := GetObjProperty(who, "raca");
	if (!raca)
		return 0;
	else
		return raca;
	endif
endfunction

function GetRaceCfgElem(race_name)
	var cfg := GetRaceCfg();
	return cfg[race_name];
endfunction

function GetRaceCfg()
	var cfg := ReadConfigFile(":fls_core:config/racas");
	if ( cfg.errortext )
		SysLog("Error - Unable to open [:fls_core:racas.cfg] ->"+cfg.errortext);
	endif

	return cfg;
endfunction


function GetTitleRace(racename)

	if (racename == "humano")
		return "Humano";
	elseif (racename == "orc")
		return "Uruk";
	elseif (racename == "hobbit")
		return "Hobbit";
	elseif (racename == "elfo")
		return "Quende";
	elseif (racename == "anao")
		return "Khuzd";
	elseif (racename == "drow")
		return "Ilythiiri";
	endif

endfunction

function IsPoisonImune(who, poison)

	if (who.npctemplate)
		var npcelem := NPC_GetNPCConfig(who.npctemplate);
		var imuneList := splitwords(npcelem.PoisonImune);
		print(" " + imuneList);
		foreach poisonName in imuneList
			if (poisonName == poison)
				return 1;
			elseif ( lower(poisonName) == "all")
				return 1;
			endif
		endforeach
		return 0;
	else
		var imuneList := splitwords(GetObjProperty(who, "PoisonImune"));
		foreach poisonName in imuneList
			if (poisonName == poison)
				return 1;
			elseif ( lower(poisonName) == "all")
				return 1;
			endif
		endforeach
		return 0;
	endif

endfunction

function FLS_GetSkillCap(who, skillname)

	if (!GetObjProperty(who, "chardata"))
		return 100.0;
	endif

	if (skillname == "classskill")
		return 100;
	endif
	if (skillname == "Begging")
		return 100;
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

function FLS_GetSkills(who, category)

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
					primarias.append(GetTrueSkillName(skill));
				endif
			endforeach
			return primarias;
		endif
	endif

	return 0;
endfunction

function GetSkillTitle(who, skillname)

	if (skillname == "BowCraft")
		skillname := "BowcraftFletching";
	endif

	var value := AP_GetSkill(who, skillname);

	if (value >= 100 )
		return "Grao Mestre";
	elseif (value >= 80)
		return "Mestre";
	elseif (value >= 60)
		return "Competente";
	elseif (value >= 40)
		return "Treinado";
	elseif (value >= 20)
		return "Aprendiz";
	else
		return "Inexperiente";
	endif

endfunction


function GetSkillNameByID(input)

	var cfg := AP_GetAttributesCfgFile();
	var entries := GetConfigStringKeys(cfg);

	foreach entry in entries
		var elem := FindConfigElem(cfg, entry);
		if (elem.SkillId == input)
			return entry;
		endif
	endforeach

	return "";

endfunction
