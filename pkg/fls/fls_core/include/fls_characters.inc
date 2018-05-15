
use uo;
use cfgfile;

include "include/dungeons";
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

function ApplyLightLevel()
	foreach player in EnumerateOnlineCharacters()
		ApplyElfLightLevel(player);
      sleepms(2);
	endforeach
endfunction

function ApplyElfLightLevel(player)
	var chardata := GetObjProperty(player, "chardata");
	if (!chardata || !chardata.raca["Elfo"])
	   return;
   endif

   if (IsInDungeon(player))
		player.setlightlevel(20, 9000);
      return 1;
   endif

	var day_config := readconfigfile(":nature:config/daynight");
	day_config := day_config["settings"];
	var darkness := cint(day_config.darkness);

	var light := cint(getglobalproperty("lightlevel"));

	if (light > 20)
		player.setlightlevel(20, 9000);
	else
		player.setlightlevel(1, 0);
	endif

   return 1;
endfunction

function maolivre(mobile, qtd, checkmount := 1)
	var rightHand := GetEquipmentByLayer(mobile, 0x01);
	var leftHand  := GetEquipmentByLayer(mobile, 0x02);

	if (qtd == 1)
		if (rightHand && leftHand)
			SendSysMessageEx(mobile, "Você precisa de uma mão livre.", SSM_INFO);
	    		return 0;
		endif
	elseif (qtd == 2)
		if (rightHand || leftHand)
			SendSysMessageEx(mobile, "Você precisa das duas mãos livres.", SSM_INFO);
	    		return 0;
		endif
	endif

	if (checkmount)
		var mounted := GetEquipmentByLayer(mobile, 0x19);

		if (mounted)
			SendSysMessageEx(mobile, "Você não pode fazer isto montado.", SSM_INFO);
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

function GetPlayerNameBySerial(serial)
   var player := SystemFindObjectBySerial(serial, SYSFIND_SEARCH_OFFLINE_MOBILES);
   if (!player) return 0; endif
   var player_name := player.name;
   if (GetObjProperty(player, "realname"))
      player_name := GetObjProperty(player, "realname");
   endif
	return player_name;
endfunction

function getCost(type, nivel)
   case (type)
      "S": return 1 + ((nivel-1) * 3);
      "I": return 3 + ((nivel-1) * 3);
      "C": return 5 + ((nivel-1) * 3);
   endcase
// Simples 1 4 7 10 13 16 19 22 25 28 31 34 37
// Intermediária 3 6 9 12 15 18 21 24 27 30 33 36 39
// Complexa 5 8 11 14 17 20 23 26 29 32 35 38 41
endfunction

function getMaxUP(nhabs, bonusum)
   if (nhabs >= 13 && bonusum >= 77)
      return 13;
   elseif (nhabs >= 12 && bonusum >= 65)
      return 12;
   elseif (nhabs >= 11 && bonusum >= 54)
      return 11;
   elseif (nhabs >= 10 && bonusum >= 44)
      return 10;
   elseif (nhabs >= 9 && bonusum >= 35)
      return 9;
   elseif (nhabs >= 8 && bonusum >= 27)
      return 8;
   elseif (nhabs >= 7 && bonusum >= 20)
      return 7;
   elseif (nhabs >= 6 && bonusum >= 14)
      return 6;
   elseif (nhabs >= 5 && bonusum >= 9)
      return 5;
   elseif (nhabs >= 4 && bonusum >= 5)
      return 4;
   elseif (nhabs >= 3 && bonusum >= 2)
      return 3;
   elseif (nhabs >= 2 && bonusum >= 0)
      return 2;
   endif

   return 1;
endfunction