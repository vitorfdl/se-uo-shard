use uo;
use cfgfile;

include ":attributes:attributes";
include ":charactercreation:chargumps";
include ":fls_core:fls_characters";

function coverFace(item)
	case(item.graphic)
		0x1408: 0x1409: 0x1412: 
		0x1419: 0x1451: 0x1F0B: 
		0x140C: 0x140D: 0x1456: 
		0x1F0C: 0x1549: 0x154A:
		0x154B: 0x154C: 0x278E:
		0x278F: 0x1388: 0x27D9:
			return 1;
		default:
			return 0;
	endcase
endfunction

function EnumeratePhysicalItems(mobile)
	var EquipList := array;

	var SkipLayers := {11, 16, 21, 25};
	var i;

	for (i:=0; i<=25; i:=i+1)
		if (!(i in SkipLayers))
			var layer := GetEquipmentByLayer(mobile, i);
			if (layer)
				EquipList.append(layer);
			endif
		endif
	endfor

	return EquipList;
endfunction

function UnequipAll(who)
	var equips := EnumeratePhysicalItems(who);

	foreach equip in equips
		MoveItemToContainer(equip, who.backpack);
	endforeach
endfunction


function RemoveEquipmentMods(who, item)
	var itemdesc := ReadConfigFile("itemdesc");

	var penalty_list := GetConfigStringArray(itemdesc[item.objtype], "Penalty");
	foreach skill in penalty_list
		AP_ModifySkillMod(who, skill, 1);
	endforeach
   
   var item_e := itemdesc[item.objtype];
	if (item_e.ArmorType)
      if (item_e.ArmorType["Pesada"])
         AP_SetSkillMod(who, "Dodge", AP_GetSkillMod(who, "Dodge") + 1);
         who.setToughness(-3);
      elseif (item_e.ArmorType["Leve"])
         AP_SetTrueSkill(who, "FeatConsume", AP_GetTrueSkill(who, "FeatConsume") - 5);
         who.setToughness(-2);
      endif
	endif

   var specialprop := GetObjProperty(item, "setprop");
   if (specialprop)
      specialprop := Splitwords(specialprop);
      EraseObjProperty(who, specialprop[1]);
   endif
	
	var mods := GetObjProperty(item, "equip");
	if (!mods)
		mods := struct{};
	endif
   
	foreach mod in (mods.keys())
		if (mod == "manamod")
         SetObjProperty(who, "manamod", Cint(GetObjProperty(who, "manamod") - Cint( mods["manamod"] )));
      elseif (mod == "toughmod")
         who.setToughness(cint(mods["toughmod"])*(-1));
		elseif (mod == "hitsmod")
         SetObjProperty(who, "hitsmod", Cint(GetObjProperty(who, "hitsmod") - Cint( mods["hitsmod"] )));
		elseif (mod == "stammod")
         SetObjProperty(who, "stammod", Cint(GetObjProperty(who, "stammod") - Cint( mods["stammod"] )));
      elseif (mod == "dmg_reduce")
			var dmg_reduce := GetObjProperty(who, "dmg_reduce");
         if (!dmg_reduce) dmg_reduce := struct{}; endif
         foreach type in (mods[mod].keys())
            dmg_reduce[type] := cint(dmg_reduce[type]) - mods[mod][type];
            if (dmg_reduce[type] == 0) dmg_reduce.erase(type); endif
         endforeach
      elseif (mod == "regen_rate")
			var regen_rate := AP_LoadVitalRegenRateProp(who);
         foreach type in (mods[mod].keys())
            regen_rate[type] := cint(regen_rate[type]) - mods[mod][type];
            if (regen_rate[type] == 0) regen_rate.erase(type); endif
         endforeach
         AP_SaveVitalRegenRateProp(who, regen_rate);
		else
			// AP_ModifySkillMod(who, mod, (-1)*Cint(mods[mod]));
		endif
   endforeach

   if (mods.keys().size() > 0)
      RecalcVitals(who);
   endif
endfunction

function CheckLevelRequirement(who, item)
   if (who.cmdlevel >= 3) return 1; endif
   var chardata := GetObjProperty(who, "chardata");
   if (!chardata) return 1; endif

   var level := cint(chardata.level);
   var class_level := cint(chardata.classes[1].level);
   if (item.isA(POLCLASS_WEAPON))
      var power := item.dmg_mod;
      if (cint(cdbl(power) / 30.0 )+1 > level || cint(power / 50 )+1 > class_level) return 0; endif
   elseif (item.isA(POLCLASS_EQUIPMENT))
      var resists := _ifOr(GetObjProperty(armor, "ArmorResists"), struct{});
      var resist_list := array{DMG_FIRE, DMG_POISON, DMG_COLD, DMG_ENERGY, DMG_SLASH, DMG_BLUDGE, DMG_PIERCE};
      var res := 0;
      foreach resist in resist_list
         var value := cint(elem["{}Resist".format(resist)]) + cint(resists[resist]);
         if (value)
            if (value > res)
               res := value;
            endif
         endif
         sleepms(2);
      endforeach

      if (getEquipCharRequirement(res) > level) return 0; endif
      if (getEquipClassRequirement(res) > class_level) return 0; endif
   endif

   return 1;
endfunction

function getEquipCharRequirement(res)
   if (res >= 4)
      return 12;
   elseif (res >= 3)
      return 8;
   elseif (res >= 2)
      return 4;
   endif
   return 1;
endfunction


function getEquipClassRequirement(res)
   if (res >= 4)
      return 4;
   elseif (res >= 3)
      return 3;
   elseif (res >= 2)
      return 2;
   endif

   return 1;
endfunction