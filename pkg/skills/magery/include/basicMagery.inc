

use uo;
use os;

include ":attributes:attributes";
include ":timedscripts:timedScripts";
include "include/sounds";
include "include/damage";

function ManterMagia(magia, mage, mobile)
	var mana;
	if (magia == "bless")
		mana := 30;
	elseif (magia == "curse")
		mana := 40;
	elseif (magia == "escudomagico")
		mana := 30;
	elseif (magia == "encantoprotecao")
		mana := 30;
	endif

	if (!AP_ConsumeVital(mage, MANA, mana) )
		SendSysMessageEx(mage, "Você não possui energia arcana suficiente em seu corpo para manter esta magia!");
		return 0;
	else
		SendSysMessageEx(mage, "Você manteve a magia.");
		SendSysMessageEx(mobile, "O mago ainda mantem a magia em você.");
		// if (magia == "bless")
		// 	TS_StartTimer(mobile, "bless", CINT(AP_GetSkill(mage, ARCANISMO)*2)+30, mage);
		// 	return 1;
		// elseif (magia == "curse")
		// 	TS_StartTimer(mobile, "curse", CINT(AP_GetSkill(mage, ARCANISMO)*2)+30, mage);
		// 	return 1;
		// elseif (magia == "escudomagico")
		// 	TS_StartTimer(mobile, "MagicResist", CINT(AP_GetSkill(mage, ARCANISMO)*2)+30, mage);
		// 	return 1;
		// elseif (magia == "encantoprotecao")
		// 	TS_StartTimer(mobile, "PhysicalResist", CINT(AP_GetSkill(mage, ARCANISMO)*2)+30, mage);
		// 	return 1;
		// endif
	endif
endfunction

function CheckDelay(who, spell_elem)
	SendSysMessageEx(who, "Você está se concentrando para invocar a magia.");

	//var script := start_script("movementCheck", {who, GetPid()});
	//
	var delay := spell_elem.Delay;
	//MS_SpellDebug(who, "Cast delay ->"+delay);
	sleepms(delay);
	//
	//script.kill();
	if ( Events_Waiting() > 0 )
		SendSysMessageEx(who, "Sua concentração foi quebrada.");
		return 0;
	else
		return 1;
	endif
endfunction

function EquipmentCheck(who)
	var cfg := ReadConfigFile(":combat:itemdesc");
	var equips := ListEquippedItems( who );

	foreach equip in equips
		var elem := FindConfigElem( cfg, equip.objtype );
		if (elem && elem.Pericia && !HaveSkill(who, elem.Pericia))
			return 0;
		endif
      sleepms(2);
	endforeach

	return 1;
endfunction

function CheckReagents(who, byref spell_elem, scroll)
	if ( who.npctemplate )
		// NPCs don't need reagents to cast.
		return 1;
	elseif ( scroll.isA(POLCLASS_ITEM) )
		return SubtractAmount(scroll, 1);
	elseif ( scroll == 1 )
		// Added scroll/1 check for passing 1 to override reg
		// and scroll check. IE: runebooks, wands, etc.
		return 1;
	endif

	var reagent_costs := GetConfigStringDictionary(spell_elem, "RegCost");
	var keys := reagent_costs.keys();
	var amount;
	foreach reag in keys

		amount := CInt( reagent_costs[reag] );
		if (!amount)
			amount := 1;
		endif
		var objtype := GetObjTypeByName(reag);
		var dictToAmount := dictionary; //variavel pra ser usada na funcao abaixo
		dictToAmount["objtype"] := objtype;
		var amountinpack := AmountInContainer(who.backpack, dictToAmount);
		// print("reag " + reag + " objtype " + objtype + " pack " + amountinpack);
		if (amountinpack  < amount )
			SendSysMessageEx(who, "Você não possui "+GetObjTypeDesc(objtype, (amount>1)));
			return 0;
		endif
		sleepms(2);
	endforeach

	foreach reag in ( keys )
		amount := CInt( reagent_costs[reag] );
		if (!amount)
			amount := 1;
		endif
		ConsumeSubstance(who.backpack, GetObjTypeByName(reag), amount);
		sleepms(2);
	endforeach

	return 1;
endfunction

function FailSpell(who, no_sound := 0)
	// Simple function for handling effects when a caster
	// fails the casting.
	EraseObjProperty(who, "#medding");
	EraseObjProperty(who, "#Casting");
	if (!GetObjProperty(who, "#magiasilenciosa") && !no_sound);
		PlaySoundEffect(who, SFX_SPELL_FAIL);
		PlayObjectCenteredEffect(who, GFX_FIZZLE, 5, 50);
	endif
	EraseObjProperty(who, "#magiasilenciosa");
	return 1;
endfunction

function TS_ConsumeMana(mobile, qty) 
	if (!AP_ConsumeVital(mobile, MANA, cint(qty)))
      if (mobile.Caracteristica("Clérigo de Anar"))
         return 0;
      endif
		var hit_qty := cint(qty);
      if (GetVital(mobile, MANA) > 0)
         hit_qty -= cint(GetVital(mobile, MANA));
      endif
      
		AP_ConsumeVital(mobile, MANA, cint(GetVital(mobile, MANA)));
		ApplyDamageEX(mobile, hit_qty * 2, "FORCED");
		if (mobile.dead)
			return 0;
		endif
	endif
	// TODO: diseage for mana
	return 1;
endfunction

function MageDiceTable(tabela, nivel)
   nivel -= 1;
   case (tabela)
      "A":
         case (nivel)
            0:default: return "2d8";
            1: return "2d8+1";
            2: return "2d10";
            3: return "2d10+1";
            4: return "3d6";
            5: return "3d6+1";
            6: return "3d8";
            7: return "3d8+1";
            8: return "3d10";
            9: return "3d10+1";
            10: return "3d12";
            11: return "3d12+1";
            12: return "3d12+2";
         endcase
      "B":
         case (nivel)
            0: default: return "2d4";
            1: return "2d4+1";
            2: return "2d6";
            3: return "2d6+1";
            4: return "2d8";
            5: return "2d8+1";
            6: return "2d10";
            7: return "2d10+1";
            8: return "2d12";
            9: return "2d12+1";
            10: return "2d12+2";
            11: return "2d12+3";
            12: return "2d12+4";
         endcase
      "C":default:
         case (nivel)
            0:default: return "2d6";
            1: return "2d6+1";
            2: return "2d8";
            3: return "2d8+1";
            4: return "2d10";
            5: return "2d10+1";
            6: return "3d6";
            7: return "3d6+1";
            8: return "3d8";
            9: return "3d8+1";
            10: return "3d10";
            11: return "3d10+1";
            12: return "3d12";
         endcase
   endcase

   return 0;
endfunction