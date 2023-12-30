/* $Id$
 * 
 * Purpose
 * Handles damages and resistances to damage types
 *
 */

use uo;
use os;
use math;
use guilds;
use cfgfile;

include ":tn:cooldown";
include ":ghaia:ghaiaUtil";
include ":attributes:attributeConstants";
include ":attributes:attributes";

include "include/say";
include ":blood:blood";
include ":faccao:faccao";
include ":combat:combat";
include ":charactercreation:proficiencies";
include ":magery:tnmagery";
include ":fls_core:packets";

include ":combat:armorZones";

/*
 * http://uo.stratics.com/content/guides/resistance.shtml
 * These could be attributes, but not every shard will want to
 * follow OSI. This gives more freedom to make custom resistances
 * and remove existing ones more easily.
 */
enum DAMAGE_TYPES
	DMG_FORCED   := -1, // Ignores resistance
	DMG_PHYSICAL := "physical",
	DMG_FIRE     := "Fogo",
	DMG_COLD     := "Gelo",
	DMG_POISON   := "Veneno",
	DMG_ENERGY   := "Energia",
	DMG_BLUDGE   := "Concussão",
	DMG_SLASH    := "Corte",
	DMG_PIERCE := "Perfuração",
endenum

/**
 * CalculateRawDamage
 *
 * @param {object} attacker - The character dealing the damage.
 * @param {object} wpn_elem - The weapon element used for the attack.
 * @param {object} wpn - The weapon used for the attack. (Optional)
 *
 * @returns {number} The calculated raw damage.
 */
function CalculateRawDamage(attacker, wpn_elem, wpn := 0)
   wpn := wpn ? wpn : attacker.weapon;
   wpn_elem := wpn_elem ? wpn_elem : GetWeaponCFGInfo(attacker, wpn);

   var base_dmg := GetObjProperty(attacker, "dmgmod");
   if (!base_dmg)
      base_dmg := attacker.isA(POLCLASS_NPC) ? wpn_elem.AttackDamage + attacker.damage_physical_mod : wpn.dmg_mod;
   endif

	base_dmg += cint(GetObjProperty(wpn, "dmgbonus")) + cint(GetObjProperty(wpn, "#dmgbonus"));
	base_dmg += cint(GetObjProperty(attacker, "dmgbonus")) + cint(GetObjProperty(attacker, "#dmgbonus"));
	base_dmg -= cint(GetObjProperty(attacker, "dmgpenalty")) - cint(GetObjProperty(attacker, "#dmgpenalty"));

   // Dice Damage
	if (wpn_elem.dicedmg)
		var dmg := wpn_elem.dicedmg;
      if (GetObjProperty(attacker, "dicedmg"))
         dmg := GetObjProperty(attacker, "dicedmg");
      endif

		var attribute := wpn_elem.Attribute;

		return (base_dmg + RandomDiceRoll(dmg) );
	endif

   var dmg := base_dmg + (wpn_elem.Damage ? RandomDiceRoll(wpn_elem.Damage) : 0);

   // Percent Damage
   if (wpn_elem.DamagePercent)
      var dmgp := cdbl(wpn_elem.DamagePercent);
      if (wpn_elem.WeaponType && !attacker.cmdlevel && !HaveProficiency(attacker, wpn_elem.WeaponType))
         dmgp -= 20;
      endif
      dmg := cint(cdbl(dmg) * (dmgp / 100.0 ));
   endif

   return dmg;
endfunction

/**
 * HealFLS
 *
 * @param {object} targ - The target to be healed.
 * @param {number} amt - The amount of healing to be applied.
 * @param {number} show_heal - Whether to show the healing effect (default is 1).
 */
function HealFLS(targ, amt, show_heal := 1)
   amt := cint(amt);
   if (amt <= 0)
      return;
	elseif (GetObjProperty(targ, "noheal"))
		SendSysMessageEx(targ, "Você não pode ser curado.", SSM_FAIL);
		return;
	endif

   var redu_heal := GetObjProperty(targ, "heal_reduce");
   if (redu_heal)
      amt := cint(cdbl(amt) - (cdbl(amt) * redu_heal));
   endif

   if (targ.connected)
      if (77 in GetGodFeats(targ.GetGod(), 1)) // cura acelerada
         amt += (amt * 0.5);
      endif
   endif

   var start_vital := AP_GetVital(targ, HITS);
	HealDamage(targ, amt);
   var amt_healed := AP_GetVital(targ, HITS) - start_vital;
	if (!show_heal || amt_healed <= 0)	
		return;
	endif

	SendSystemReport(targ, amt_healed, SYSTEM_HEAL, targ);
	if (show_heal.isA(POLCLASS_MOBILE) && show_heal != targ)
		SendSystemReport(targ, amt_healed, SYSTEM_HEAL, show_heal);
      var mobs := ListMobilesNearLocationEx(targ.x, targ.y, targ.z, 8, LISTEX_FLAG_NORMAL|LISTEX_FLAG_NPC_ONLY);
	endif
endfunction

/**
 * SetLastDamageInfo
 *
 * @param {object} mobile - The mobile object that received the damage.
 * @param {number} amount - The amount of damage received.
 * @param {string} type - The type of damage received.
 * @param {object} source - The source of the damage.
 * @returns {object} - The last damage information.
 */
function SetLastDamageInfo(mobile, amount, type, source)
	var last_damage := struct;
	last_damage.+serial	:= source.serial;
	last_damage.+time	:= polcore().systime;
	last_damage.+amount	:= amount;
	last_damage.+type	:= type;
	
	if ( last_damage.acctname )
		last_damage.+acctname := source.acctname;
	endif
	if ( source.name )
		last_damage.+name := source.name;
	else
		last_damage.+name := source.desc;
	endif
	
	SetObjProperty(mobile, "LastDamage", last_damage);
	
	return last_damage;
endfunction


/*
 * Function: fixResistLevel
 * 
 * Description:
 * This function fixes the resistance level within a specified range.
 * Only used by CalculateDefenseReduction.
 *
 * Parameters:
 * n - The resistance level to be fixed.
 *
 * Returns:
 * The function returns the fixed resistance level within the specified range.
 */
function fixResistLevel(n)
   if (n < -1) 
      n := -1; 
   elseif (n > 4) 
      n := 4; 
   endif

   return cdbl(n * 20);
endfunction


/*
 * Function: CalculateDefenseReduction
 * 
 * Description:
 * This function calculates the defense reduction based on the defender, damage type, damage amount, and armor.
 *
 * Parameters:
 * defender - The character that is defending.
 * dmgs_type - The type of damage being dealt.
 * dmg - The amount of damage being dealt.
 * armor - The armor of the defender. Default value is 0.
 *
 * Returns:
 * The function returns the final damage after reduction.
 */
function CalculateDefenseReduction(defender, dmgs_type, dmg, armor := 0)
   if (dmgs_type == DMG_FORCED) return 0; endif

   if (typeof(dmgs_type) != "Struct")
      var percent := struct{};
      percent[dmgs_type] := 100;

      dmgs_type := percent;
   endif

   var npctemplate;
   if (defender.isA(POLCLASS_NPC))
      npctemplate := NPC_GetNPCConfig(defender);
      armor := defender;
   elseif (!armor)
      armor := CS_GetEquipmentInArmorZone(defender, CS_GetRandomArmorZone());
      armor := CS_SelectArmored(armor, dmgs_type);
   endif

   if (!armor) 
      return dmg;
   endif
   
   var reduction := 0;
   var resists := _ifOr(GetObjProperty(armor, "ArmorResists"), struct{});
   var armorcfg := GetWeaponCFGInfo(defender, armor);
   var fdmg := 0;

   foreach type in (dmgs_type.keys())
      dmgs_type[type] := cdbl(dmg) * (cdbl(dmgs_type[type]) / 100);
      var reduction := 0;

      if (npctemplate)
         var star := cint(resists[type]) + GetConfigInt(npctemplate, $"{type}Resist");
         reduction := fixResistLevel(star) / 100;
      else
         var star := GetConfigInt(armorcfg, $"{type}Resist") + cint(resists[type]);
         reduction := fixResistLevel(star) / 100;
      endif

      if (defender.shield) 
         reduction += 0.05;
      endif

      fdmg += cint(dmgs_type[type] - (dmgs_type[type] * reduction)); 
      sleepms(2);
   endforeach

   return fdmg;
endfunction

/**
 * Calculate the reduction in damage based on the defender's armor and resistances.
 *
 * @param {object} defender - The defender object.
 * @param {string} dmgs_type - The type of damage being dealt.
 * @param {number} dmg - The amount of damage before reduction.
 * @param {number} armor - The defender's armor value.
 * @returns {number} - The final damage after reduction.
 */
function DamageFLS(defender, dmg, type := DMG_FORCED, source := 0, no_display := 0)
	dmg := Cint(dmg);

   // Lower dodge block cooldow
   if (defender.connected && source.connected && TS_GetCooldown(defender, "attack_low_block"))
      TS_LowerDuration(defender, "dodge_block", 1, 1);
   endif

   if (type != DMG_FORCED && source && defender.my_template() != "orb" || defender.master == source)
      dmg := CalculateDefenseReduction(defender, type, dmg);
   endif

   if (defender.npctemplate && source.npctemplate && GetObjProperty(defender, "guild"))
      var guild_ref := FindGuild(GetObjProperty(defender, "guild"));
      if (guild_ref && !InGuildRect(guild_ref, defender))
         dmg *= 3;
      endif
   endif

   // Barrier Handling
   var barrier := AP_GetTrueSkill(defender, "Barrier");
   if (barrier > 0)
      dmg -= barrier;
      AP_SetTrueSkill(defender, "Barrier", barrier - cint(dmg));
   endif

   if (dmg < 0)
      return; // Stop if damage is 0 or less
   endif

	if (!no_display) 
      DisplayDamage(defender, cint(dmg), source); 
   endif

	ApplyRawDamage(defender, cint(dmg), DAMAGE_NO_SHOW+DAMAGE_USE_REPSYS);
	
	if (source)
		SetLastDamageInfo(defender, dmg, type, source);
		SetScriptController(source);
	endif

	return dmg;
endfunction

// function checkResist(defender, damage_type)
// 	var resist_list        := array{};
// 	var vulnerability_list := array{};
// 	if (!defender.npctemplate)
// 		var chardata := GetObjProperty(defender, "resist_list");
// 		if (chardata)
// 			foreach resist in (chardata.Keys())
// 				if (chardata[resist] != 0)
// 					resist_list += resist;
// 				endif
// 			endforeach
// 		endif
// 	else
// 		var npcelem := NPC_GetNPCConfig(defender);
// 		resist_list := GetConfigStringArray(npcelem, "Resistencia");
// 		if (GetObjProperty(defender, "resists"))
// 			resist_list += GetObjProperty(defender, "resists");
// 		endif
// 		vulnerability_list := GetConfigStringArray(npcelem, "Vulnerabilidade");
// 		if (GetObjProperty(defender, "vulnerabilities"))
// 			vulnerability_list += GetObjProperty(defender, "vulnerabilities");
// 		endif
// 	endif

// 	if (damage_type in resist_list)
// 		return 1;
// 	endif

// 	if (damage_type in vulnerability_list)
// 		return -1;
// 	endif

// 	return 0;
// endfunction
