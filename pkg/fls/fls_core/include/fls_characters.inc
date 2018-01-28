
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

function HaveReverseVision(who)
	var chardata := GetObjProperty(who, "chardata");

	if (chardata.raca["Elfo"])
		return 1;
	endif
   
	return 0;
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

	if (light > 20)
		player.setlightlevel(20, 9000);
	else
		player.setlightlevel(light, 9000);
	endif
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

function SubtractMoney(container, qty := array{})
	if (qty.length() > 3) return 0;

	var money := array{0, 0, 0};
	foreach item in (EnumerateItemsInContainer(container))
		if (item.serial == 0x1) money[1] += item.amount;
		elseif (item.serial == 0x2) money[2] += item.amount;
		elseif (item.serial == 0x3) money[3] += item.amount; endif
		sleepms(2);
	endforeach

	var check := 1;
	var change_back := array{0, 0, 0};
	var i;
	for (i := 1; i <= 3; i++)
		if (qty[i] < money[i]) check := 0;
		if (!check && i != 1 && qty[i-1] < money[i-1] && money[i]+100 >= qty[i])
			qty[i-1] += 1;
			qty[i] -= 100;
			if (qty[i] < 0) 
				change_back[i] := qty[i] * -1;
				qty[i] := 0; 
			endif
			check := 1;
		endif
	endfor

	if (!check) return 0; endif

	for (i := 1; i <= 3; i++)
		var coin_type;
		if (i == 1) coin_type := 0x1;
		elseif (i == 2) coin_type := 0x2;
		elseif (i == 3) coin_type := 0x3; endif
		foreach item in (EnumerateItemsInContainer(container))
			if (item.objtype != coin_type) continue;
			if (item.amount >= qty[i])
				var chk := SubtractAmount(item, qty[i]);
				if (!chk )
					SendSysMessage(who, "chk " + chk);
					return 0;
				endif
			else
				qty[i] -= item.amount;
				if (!SubtractAmount(item, item.amount))
					return 0;
				endif
			endif
         sleepms(2);
		endforeach

		if (change_back[i])
			CreateItemInContainer(container, coin_type, change_back);
		endif
	endfor

	return 1;	
endfunction
