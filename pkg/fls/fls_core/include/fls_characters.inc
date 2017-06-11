
use uo;
use cfgfile;

include ":ghaia:ghaiaUtil";
include ":attributes:attributes";
include ":fls_core:fls_chargumps_ex";

function checkGender(object)
	var gender := 0;
	if (object.graphic)
		case (object.graphic)
			400:
			605: // elf
				gender := 1;
			401:
			606: // elf
				gender := 2;
		endcase
	elseif (object.corpsetype)
		case (object.graphic)
			400:
			605: // elf
			403:
			607: // elf
				gender := 1;
			401:
			606: // elf
			404:
			608: // elf
				gender := 2;
		endcase
	elseif (object.gender)
		if (gender == 0)
			gender := 1;
		else
			gender := 2;
		endif
	endif

	return gender;
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
	else
		var newlight := abs(darkness - light);
		player.setlightlevel(newlight, 9000);
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
