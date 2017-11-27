

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
		SendSysMessage(mage, "Voce nao possui energia arcana suficiente em seu corpo para manter esta magia!");
		return 0;
	else
		SendSysMessage(mage, "Voce manteve a magia.");
		SendSysMessage(mobile, "O mago ainda mantem a magia em voce.");
		if (magia == "bless")
			TS_StartTimer(mobile, "bless", CINT(AP_GetSkill(mage, ARCANISMO)*2)+30, mage);
			return 1;
		elseif (magia == "curse")
			TS_StartTimer(mobile, "curse", CINT(AP_GetSkill(mage, ARCANISMO)*2)+30, mage);
			return 1;
		elseif (magia == "escudomagico")
			TS_StartTimer(mobile, "MagicResist", CINT(AP_GetSkill(mage, ARCANISMO)*2)+30, mage);
			return 1;
		elseif (magia == "encantoprotecao")
			TS_StartTimer(mobile, "PhysicalResist", CINT(AP_GetSkill(mage, ARCANISMO)*2)+30, mage);
			return 1;
		endif
	endif
endfunction

function CheckDelay(who, spell_elem)
	SendSysMessage(who, "Voce esta se concentrando para invocar a magia.");

	//var script := start_script("movementCheck", {who, GetPid()});
	//
	var delay := spell_elem.Delay;
	//MS_SpellDebug(who, "Cast delay ->"+delay);
	sleepms(delay);
	//
	//script.kill();
	if ( Events_Waiting() > 0 )
		SendSysMessage(who, "Sua concentracao foi quebrada.");
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
			SendSysMessage(who, "Voce nao possui "+GetObjTypeDesc(objtype, (amount>1)));
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

function FailSpell(who)
	// Simple function for handling effects when a caster
	// fails the casting.
	EraseObjProperty(who, "#medding");
	EraseObjProperty(who, "#Casting");
	if (!GetObjProperty(who, "#magiasilenciosa"));
		PlaySoundEffect(who, SFX_SPELL_FAIL);
		PlayObjectCenteredEffect(who, GFX_FIZZLE, 5, 50);
	endif
	EraseObjProperty(who, "#magiasilenciosa");
	return 1;
endfunction

function TS_ConsumeMana(mobile, qty) 
	if (!AP_ConsumeVital(mobile, MANA, qty))
		var hit_qty := qty - GetVital(mobile, MANA);
		AP_ConsumeVital(mobile, MANA, GetVital(mobile, MANA));
		ApplyDamageEX(mobile, hit_qty * 2, "FORCED");
		if (mobile.dead)
			return 0;
		endif
	endif
	// TODO: diseage for mana
	return 1;
endfunction

