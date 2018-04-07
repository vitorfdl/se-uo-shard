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

/*
 * http://uo.stratics.com/content/guides/resistance.shtml
 * These could be attributes, but not every shard will want to
 * follow OSI. This gives more freedom to make custom resistances
 * and remove existing ones more easily.
 */
enum DAMAGE_TYPES
	DMG_FORCED   := -1, // Ignores resistance
	DMG_PHYSICAL := "Físico",
	DMG_FIRE     := "Fogo",
	DMG_COLD     := "Gelo",
	DMG_POISON   := "Veneno",
	DMG_ENERGY   := "Eletrecidade",
	DMG_PIERCING := "Perfuração",
	DMG_SLASH    := "Corte",
	DMG_BLUDGE   := "Concussão",
endenum

function SelectAttribute(attacker, attacker_weapon)
	var attribute := STRENGTH;
	if (attacker_weapon.Attribute)
		attribute := attacker_weapon.Attribute;
		// Escolhe maior atributo para versatilidade
		if (attribute == "Versatil")
			var dex := AP_GetTrueStat(attacker, DEXTERITY);
			var str := AP_GetTrueStat(attacker, STRENGTH);
			if (str > dex)
				attribute := DEXTERITY;
			else
				attribute := STRENGTH;
			endif
		endif
	endif

	return attribute;
endfunction

function CalculateRawDamage(attacker, wpn_elem, wpn := 0)
	if (wpn_elem.dicedmg)
		var dmg       := wpn_elem.dicedmg;
      if (GetObjProperty(attacker, "dicedmg"))
         dmg := GetObjProperty(attacker, "dicedmg");
      endif
		var attribute := wpn_elem.Attribute;
		return ( RandomDiceRoll(dmg) + AP_GetStat(attacker, attribute, RETURN_BONUS) );
	endif

   if (!wpn) wpn := attacker.weapon; endif

   var attribute := SelectAttribute(attacker, attacker.weapon);
   var dmg       := wpn_elem.Damage;
   if (GetObjProperty(attacker, "dicedmg"))
      dmg := GetObjProperty(attacker, "dicedmg");
   endif
   dmg :=  RandomDiceRoll(cstr(dmg)) + AP_GetStat(attacker, attribute, RETURN_BONUS) + GetAttackerDMGBonus(attacker);

   return ( dmg + attacker.weapon.dmg_mod );
endfunction

function GetAttackerDMGBonus(attacker)
   var bonus := 0;

   // if (wpn_a_elem.WeaponType == "sword" && HaveFeat(attacker, "Mestre das Lâminas"))
   //    bonus += 1;
   // elseif (wpn_a_elem.WeaponType == "arco" && HaveFeat(attacker, "Arqueiro Experiente"))
   //    bonus += 1;
   // elseif (wpn_a_elem.WeaponType == "arco" && HaveFeat(attacker, "Arqueiro Experiente"))
   //    bonus += 1;
   // elseif (wpn_a_elem.WeaponType == "arco" && HaveFeat(attacker, "Arqueiro Experiente"))
   //    bonus += 1;
   // elseif (wpn_a_elem.WeaponType == "arco" && HaveFeat(attacker, "Arqueiro Experiente"))
   //    bonus += 1;
   // endif
      
   return bonus;
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

    
function CalculateTotalDamageBonus(dmg, who, defender)
	dmg := cint(dmg);
	var attr_bonus := 0;
	// if (!who.npctemplate)
	// 	if (who.weapon && who.weapon.Attribute != "Versatil")
	// 		attr_bonus := GetBonusAttr(AP_GetStat(who, who.weapon.Attribute));
	// 	else
	// 		attr_bonus := GetBonusAttr(AP_GetStat(who, STRENGTH));
	// 	endif
	// endif

	//Bonus de prop da arma    
	var weapon := cint(GetObjProperty(who.weapon, "dmgbonus")) + cint(GetObjProperty(who.weapon, "#dmgbonus"));

	//Bonus de prop do mobile
	var other := cint(GetObjProperty(who, "dmgbonus")) + cint(GetObjProperty(who, "#dmgbonus")) ;

	//Bonus de Spells
	other := other - cint(GetObjProperty(defender, "dmgreduction"));
  
	// TODO: adicionar backstab

	// TODO: ADicionar talentos de dano aqui
	
	var mod := attr_bonus + weapon + other;
    
	dmg := dmg + mod;
	if (dmg < 1)
		dmg := 1;
	endif

	return dmg;
endfunction

function DamageFLS(defender, dmg, type := DMG_FORCED, source := 0)
	dmg := Cint(dmg);
	
	if (type != DMG_FORCED && checkResist(defender, type) > 0)
		dmg := cint(Floor(dmg/2));
	elseif (type != DMG_FORCED && checkResist(defender, type) < 0)
		dmg := cint(Floor(dmg*2));
	elseif (type != DMG_FORCED && checkImmunity(defender, type))
		dmg := 0;
		SendSystemReport(defender, "*Imune*", SSM_FAIL);
		if (source) SendSystemReport(defender, "*Imune*", SSM_FAIL, source); endif
		return;
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

   var dmg_reduce := GetObjProperty(defender, "dmg_reduce");
   if (dmg_reduce)
      if (type in dmg_reduce.keys())
         dmg := dmg - cint((cdbl(dmg) * dmg_reduce[type]));
      endif
   endif
   
	DisplayDamage(defender, cint(dmg), source);
	ApplyRawDamage(defender, cint(dmg), DAMAGE_NO_SHOW+DAMAGE_USE_REPSYS);
	
	if (source && type != DMG_FORCED)
		SetLastDamageInfo(defender, dmg, type, source);
		SetScriptController(source);

		var cleric_healing := cint(GetObjProperty(source, "cleric_spell"));
      var chance := 75;
      if (cleric_healing == source.serial)
         chance := 40;
      endif
		if (cleric_healing && dmg >= 1 && Randomint(101) <= chance)
         var bencao := GetObjProperty(source, "#bencaodivina");
         if (bencao);
            var mobs := CheckIfIsAlly(source, ListMobilesNearLocation(source.x, source.y, source.z, 2));
            mobs.append(source);
            foreach mob in mobs
			      HealFLS(mob, Floor(dmg/2), source);
            endforeach
            EraseObjProperty(source, "#bencaodivina");
            return dmg;
         endif
		   var targ := SystemFindObjectBySerial(cleric_healing);
			if (source != targ && Distance(source, targ) > 8 && targ.connected && !targ.dead)
				targ := source;
				SetClericHeal(source, targ);
			endif

			HealFLS(targ, Floor(dmg/2), source);
		endif
	endif

	return dmg;
endfunction

function checkResist(defender, damage_type)
	var resist_list        := array{};
	var vulnerability_list := array{};
	if (!defender.npctemplate)
		var chardata := GetObjProperty(defender, "resist_list");
		if (chardata)
			foreach resist in (chardata.Keys())
				if (chardata[resist] != 0)
					resist_list += resist;
				endif
			endforeach
		endif
	else
		var npcelem := NPC_GetNPCConfig(defender);
		resist_list := GetConfigStringArray(npcelem, "Resistencia");
		if (GetObjProperty(defender, "resists"))
			resist_list += GetObjProperty(defender, "resists");
		endif
		vulnerability_list := GetConfigStringArray(npcelem, "Vulnerabilidade");
		if (GetObjProperty(defender, "vulnerabilities"))
			vulnerability_list += GetObjProperty(defender, "vulnerabilities");
		endif
	endif

	if (damage_type in resist_list)
		return 1;
	endif

	if (damage_type in vulnerability_list)
		return -1;
	endif

	return 0;
endfunction

function checkImmunity(defender, damage_type)
	var resist_list := array{};
	if (!defender.npctemplate)
		var chardata := GetObjProperty(defender, "imunity_list");
		if (chardata)
			foreach resist in (chardata.Keys())
				if (chardata[resist] != 0)
					resist_list += resist;
				endif
			endforeach
		endif
	else
		var npcelem := NPC_GetNPCConfig(defender);
		resist_list := GetConfigStringArray(npcelem, "Imunidade");
	endif

   if (GetObjProperty(defender, "#ethereal"))
      resist_list.append(DMG_BLUDGE);
      resist_list.append(DMG_PHYSICAL);
      resist_list.append(DMG_PIERCING);
      resist_list.append(DMG_SLASH);
   endif

	if (damage_type in resist_list)
		return 1;
	endif

	return 0;
endfunction
