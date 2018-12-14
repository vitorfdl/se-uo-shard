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
	DMG_FIRE     := "fire",
	DMG_COLD     := "cold",
	DMG_POISON   := "poison",
	DMG_ENERGY   := "energy",
endenum

function CalculateRawDamage(attacker, wpn_elem, wpn := 0)
   if (!wpn) wpn := attacker.weapon; endif

   if (!wpn_elem)
      wpn_elem := GetItemCFGInfo(attacker, wpn);
   endif

   var base_dmg;
   if (GetObjProperty(attacker, "dmgmod"))
      base_dmg := GetObjProperty(attacker, "dmgmod");
   elseif (attacker.isA(POLCLASS_NPC))
      base_dmg := wpn_elem.AttackDamage + attacker.damage_physical_mod;
   else
      base_dmg := wpn.dmg_mod;
   endif

	base_dmg += cint(GetObjProperty(wpn, "dmgbonus")) + cint(GetObjProperty(wpn, "#dmgbonus"));
	base_dmg += cint(GetObjProperty(attacker, "dmgbonus")) + cint(GetObjProperty(attacker, "#dmgbonus"));
	base_dmg -= cint(GetObjProperty(attacker, "dmgpenalty")) - cint(GetObjProperty(attacker, "#dmgpenalty")) ;

   // Só pra tornar o dano não fixo
	if (wpn_elem.dicedmg)
		var dmg       := wpn_elem.dicedmg;
      if (GetObjProperty(attacker, "dicedmg"))
         dmg := GetObjProperty(attacker, "dicedmg");
      endif
		var attribute := wpn_elem.Attribute;
		return (base_dmg + RandomDiceRoll(dmg) );
	endif

   var dmg   := base_dmg + _ifElse(wpn_elem.Damage, RandomDiceRoll(wpn_elem.Damage), 0);
   // fim

   return dmg;
endfunction

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

function ApplyDamageEX(defender, dmg, type, source := 0)
	ApplyRawDamage( defender, cint(dmg), DAMAGE_NO_SHOW+DAMAGE_USE_REPSYS);

	if (source)
		SetLastDamageInfo(defender, dmg, type, source);
		SetScriptController(source);
	endif

	return dmg;
endfunction


/* 
 * SetLastDamageInfo(mobile, amount, type, source)
 *
 * Purpose
 *
 * Parameters
 * mobile:	
 * amount:	
 * type:	
 * source:	
 *
 * Return value
 *
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

function CalculateDefenseReduction(defender, dmg_type, is_massive := 0, armor := 0)
   if (dmg_type == DMG_FORCED) return 0; endif
   
   if (defender.isA(POLCLASS_NPC))
      var npctemplate := NPC_GetNPCConfig(defender);
      var npc_resist := GetConfigReal(npctemplate, "{}Resist".format(dmg_type)) + cdbl(defender.get_member("resist_{}_mod".format(dmg_type)));
      return npc_resist / 100.0;
   endif

   if (!is_massive && !armor)
      armor := CS_GetEquipmentInArmorZone(defender, CS_GetRandomArmorZone());
      armor := CS_SelectArmored(armor, dmg_type);
   endif

   var reduction := 0;
   if (!armor) return 0; endif
   
   if (is_massive)
      reduction := cint(defender.get_member("resist_{}".format(dmg_type)) / 6);
   else
      reduction := armor.get_member("resist_{}".format(dmg_type)) + defender.get_member("resist_{}_mod".format(dmg_type));
   endif

   if (defender.shield)
      reduction += defender.shield.get_member("resist_{}".format(dmg_type));
   endif

   if (reduction > 60) reduction := 60; endif
   return (cdbl(reduction) / 100.0);
endfunction
/* 
 * GetLastDamageInfo(mobile)
 *
 * Purpose
 *
 * Parameters
 * mobile:	
 *
 * Return value
 *
 */
function GetLastDamageInfo(mobile)
	return GetObjProperty(mobile, "LastDamage");
endfunction

    
function CalculateTotalDamageBonus(who, defender)
	// if (!who.npctemplate)
	// 	if (who.weapon && who.weapon.Attribute != "Versatil")
	// 		attr_bonus := GetBonusAttr(AP_GetStat(who, who.weapon.Attribute));
	// 	else
	// 		attr_bonus := GetBonusAttr(AP_GetStat(who, STRENGTH));
	// 	endif
	// endif

	//Bonus de prop da arma    

   var other := 0;
   if (defender != who)
      //Bonus de Spells
      other := other - cint(GetObjProperty(defender, "dmgreduction"));
   
      var dmg_increase := GetObjProperty(who.weapon, "dmg_increase");
      if (dmg_increase)
         if (defender.npctemplate)
            var template := NPC_GetNPCConfig(defender.npctemplate);
            other += cint(dmg_increase[template.Category]);
            other += cint(dmg_increase[template.size]);
         endif
      endif
   endif
	// TODO: adicionar backstab

	// TODO: ADicionar talentos de dano aqui
	
	var mod := weapon + other;
    
	dmg := dmg + mod;
	if (dmg < 1)
		dmg := 1;
	endif

	return dmg;
endfunction

function DamageFLS(defender, dmg, type := DMG_FORCED, source := 0, no_display := 0)
	dmg := Cint(dmg);

   if (defender.connected && source.connected)
      if (TS_GetCooldown(defender, "attack_low_block"))
         TS_LowerDuration(defender, "dodge_block", 1, 1);
      endif
   endif
   
   if (type != DMG_FORCED)
      var reduction := cint(cdbl(dmg) * CalculateDefenseReduction(defender, type));
      dmg := dmg - reduction;
      if (defender.cmdlevel >= 5)  SendSysMessageEx(defender, "FDMG: {} - Red: {}".format(attacker, reduction)); endif
      if (attacker.cmdlevel >= 5) SendSysMessageEx(attacker, "FDMG: {} - Red: {}".format(attacker, reduction)); endif
      if (source)
         if (defender.my_template() == "orb" && defender.master != source)
            return 0;
         endif
      endif
   endif
   
   if (defender.npctemplate && source.npctemplate)
      var guild_id := GetObjProperty(defender, "guild");
      if (guild_id)
         var guild_ref := FindGuild(guild_id);
         if (!InGuildRect(guild_ref, defender))
            dmg *= 3;
         endif
      endif
   endif

   var barrier := AP_GetTrueSkill(defender, "Barrier");
   if (barrier > 0)
      var new_dmg := dmg - barrier;
      barrier -= cint(dmg);
      dmg := new_dmg;
      AP_SetTrueSkill(defender, "Barrier", barrier);
   endif

   if (dmg < 0)
      return;
   endif

	if (!no_display) DisplayDamage(defender, cint(dmg), source); endif
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
