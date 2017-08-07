
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
	var day_config := readconfigfile(":nature:config/daynight");
	day_config := day_config["settings"];
	var darkness := cint(day_config.darkness);

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

function maolivre(mobile, qtd, checkmount := 1)
	var rightHand := GetEquipmentByLayer(mobile, 0x01);
	var leftHand  := GetEquipmentByLayer(mobile, 0x02);

	if (qtd == 1)
		if (rightHand && leftHand)
			SendSysMessageEx(mobile, "Voce precisa de uma mao livre.", SSM_INFO);
	    		return 0;
		endif
	elseif (qtd == 2)
		if (rightHand || leftHand)
			SendSysMessageEx(mobile, "Voce precisa das duas maos livres.", SSM_INFO);
	    		return 0;
		endif
	endif

	if (checkmount)
		var mounted := GetEquipmentByLayer(mobile, 0x19);

		if (mounted)
			SendSysMessageEx(mobile, "Voce nao pode fazer isto montado.", SSM_INFO);
			return 0;
		endif
	endif

	return 1;
endfunction

function GetSkillNameByID(input)
	var cfg_file := ReadConfigFile(":attributes:attributes");
	var entries := GetConfigStringKeys(cfg_file);
	foreach entry in entries
		var elem := FindConfigElem(cfg_file, entry);
		if (cint(elem.SkillId) == input)
			return entry;
		endif
	endforeach

	return "";

endfunction