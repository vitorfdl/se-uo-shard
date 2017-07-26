use uo;
use cfgfile;

include ":attributes:attributes";
include ":charactercreation:chargumps";
include ":charactercreation:habilidades";
include ":fls_core:fls_characters";

function coverFace(item)
	case(item.graphic)
		0x1408: 0x1409: 0x1412: 
		0x1419: 0x1451: 0x1F0B: 
		0x140C: 0x140D: 0x1456: 
		0x1F0C: 0x1549: 0x154A:
		0x154B: 0x154C:	0x278E:
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
	
	//sneak
  	penalty := CInt(itemdesc[item.objtype].SneakPenalty);
  	AP_ModifySkillMod(who, SNEAK, penalty);

	//mechanical  apitude
  	penalty := CInt(itemdesc[item.objtype].ArmorDexPenalty);
  	AP_ModifySkillMod(who, MECHANICALAPITUDE, penalty);

	//awareness
  	penalty := CInt(itemdesc[item.objtype].AwarenessPenalty);
  	AP_ModifySkillMod(who, AWARENESS, penalty);

	//magery
  	penalty := CInt(itemdesc[item.objtype].MageryPenalty);
  	AP_ModifySkillMod(who, MAGERY, penalty);

	var mods := GetObjProperty(item, "equip");
	if (!mods)
		mods := struct{}
	endif
	foreach mod in (mods.keys())
		if (GetObjProperty(item,"magic") == 1))
			continue;
		endif
		
		if (mod == "strmod")
			AP_ModifyStatMod(who, STRENGTH, (-1)*Cint( mods["strmod"] ));
		elseif (mod == "dexmod")
			AP_ModifyStatMod(who, DEXTERITY, (-1)*Cint( mods["dexmod"] ));
		elseif (mod == "intmod")
			AP_ModifyStatMod(who, INTELLIGENCE, (-1)*Cint( mods["intmod"] ));
		elseif (mod == "conmod")
			AP_ModifyStatMod(who, CONSTITUTE, (-1)*Cint( mods["conmod"] ) );
		elseif (mod == "wismod")
			AP_ModifyStatMod(who, WISDOM, (-1)*Cint( mods["wismod"] ) );
		elseif (mod == "setprop"))
			EraseObjProperty(who, "#" + GetObjProperty(item, "propname"));
		elseif (mod == "manamod")
			SetObjProperty(who, "#manamod", Cint(GetObjProperty(who, "#manamod")) - Cint( mods["manamod"] ));
			RecalcVitals(who);
		elseif (mod == "hitsmod")
			var hitswho := Cint(GetObjProperty(who, "#hitsmod"));
			var hitsitem :=  Cint( mods["hitsmod"] );
			var final := hitswho - hitsitem;
			if (final <= 0)
				EraseObjProperty(who, "#hitsmod");
			else
				SetObjProperty(who, "#hitsmod", final );
			endif
			RecalcVitals(who);
		elseif (mod == "stammod")
			SetObjProperty(who, "#stammod", Cint(GetObjProperty(who, "#stammod")) - Cint( mods["stammod"] ));
			RecalcVitals(who);
		else
			AP_ModifySkillMod(who, mod, (-1)*Cint(mods[mod]));
		endif
endforeach

endfunction