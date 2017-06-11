/* $Id$
 * 
 * Purpose
 * Handles damages and resistances to damage types
 *
 */

use uo;
use os;
use math;
use cfgfile;

include ":tn:cooldown";
include ":ghaia:ghaiaUtil";
include ":attributes:attributeConstants";
include ":attributes:attributes";
include ":charactercreation:habilidades";
include "include/say";
include ":blood:blood";
include ":fls_core:packets";
include ":tn:combat";

/*
 * http://uo.stratics.com/content/guides/resistance.shtml
 * These could be attributes, but not every shard will want to
 * follow OSI. This gives more freedom to make custom resistances
 * and remove existing ones more easily.
 */
enum DAMAGE_TYPES
	DMG_FORCED  := -1, // Ignores resistance
	DMG_PHYSICAL:= "Physical",
	DMG_FIRE    := "Fire",
	DMG_COLD    := "Cold",
	DMG_POISON  := "Poison",
	DMG_ENERGY  := "Energy"
endenum


function HealFLS(targ, amt)

	if (!GetObjProperty(targ, "nocure"))
		HealDamage(targ, amt);
	else
		SendSysMessageEx(targ, "Voce nao pode ser curado.", SSM_FAIL);

	endif

endfunction

function ApplyDamageEX(defender, dmg, type, source := 0)
	ApplyRawDamage( defender, dmg );

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
	if (!who.npctemplate)
		if (who.weapon)
			attr_bonus := GetBonusAttr(AP_GetStat(who, who.weapon.Attribute));
		else
			attr_bonus := GetBonusAttr(STRENGTH);
		endif
	endif
/*	if (temHabilidade(who, "Ataques Precisos"))
		stat := DEXTERITY;
	endif
	
	if (temHabilidade(who, "Taticas Arcanas"))
		stat := INTELLIGENCE;
	endif*/

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
		dmg := Floor(dmg/2);
	elseif (type != DMG_FORCED && checkResist(defender, type) < 0)
		dmg := Floor(dmg*2);
	elseif (type != DMG_FORCED && checkImmunity(defender, type))
		dmg := 0;
		PrivateSystemReport(defender, "*Imune*", defender);
		PrivateSystemReport(defender, "*Imune*", attacker);
		return;
	endif

	DisplayDamage(defender, dmg, source);
	ApplyRawDamage(defender, dmg, DAMAGE_NO_SHOW+DAMAGE_USE_REPSYS);
	
	if (isBloodied(defender))
		RecalcVitals(defender);
		MakeBleed(defender);
	endif

	if (source)
		SetLastDamageInfo(defender, dmg, type, source);
		SetScriptController(source);
	endif

	return dmg;

endfunction

function checkResist(defender, damage_type)
	var resist_list := array{};
	var vulnerability_list := array{}
	if (!defender.npctemplate)
		var chardata := GetObjProperty(defender, "chardata_resist");
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
		vulnerability_list := GetConfigStringArray(npcelem, "Vulnerabilidade");
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
		var chardata := GetObjProperty(defender, "chardata_resist");
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
	endif

	if (damage_type in resist_list)
		return 1;
	endif

	return 0;
endfunction