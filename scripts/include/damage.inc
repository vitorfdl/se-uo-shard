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


function getNewDamageList(attacker)

	var list := dictionary;

	//GAMBI PRO BUG DO POL. REMOVER QUANDO CORRIGIREM
	if (attacker.npctemplate)
		var npctemplate    := NPC_ParseTemplateName(attacker.npctemplate);
		var cfg            := ReadConfigFile(":brainai:npcdesc");
		list[DMG_PHYSICAL] := Cdbl(cfg[npctemplate.template].PhysicalDamage/100.0);
		list[DMG_FIRE]     := Cdbl(cfg[npctemplate.template].FireDamage/100.0);
		list[DMG_COLD]     := Cdbl(cfg[npctemplate.template].ColdDamage/100.0);
		list[DMG_POISON]   := Cdbl(cfg[npctemplate.template].PoisonDamage/100.0);
		list[DMG_ENERGY]   := Cdbl(cfg[npctemplate.template].EnergyDamage/100.0);
		//printtextabove(attacker, " " + list);
		return list;		

	else

		list[DMG_PHYSICAL] := Cdbl(attacker.damage_physical/100.0);
		list[DMG_FIRE]     := Cdbl(attacker.damage_fire/100.0);
		list[DMG_COLD]     := Cdbl(attacker.damage_cold/100.0);
		list[DMG_POISON]   := Cdbl(attacker.damage_poison/100.0);
		list[DMG_ENERGY]   := Cdbl(attacker.damage_energy/100.0);

		var elemental := GetObjProperty(attacker.weapon, "#elementalweapon");
		if (elemental != error)
			list[elemental.type] := list[elemental.type] + elemental.bonus;
		endif
		
		var ammo := GetObjProperty(attacker.weapon, "ammo");
		if (ammo)
			var cfg:=ReadConfigFile(":woodworking:itemdesc");
			var elem := FindConfigElem(cfg, ammo);
			var dmgtype := GetConfigStringArray(elem, "dmgtype");
			if (dmgtype[1])
				foreach dmg in dmgtype
					var physicalreduction := Cdbl( (list[DMG_PHYSICAL] * elem.dmgbonus) / 100.0 );
					if (dmg != "Physical")
						list[DMG_PHYSICAL] := Cdbl(list[DMG_PHYSICAL] - physicalreduction);
					endif
				endforeach
			endif
			
		endif

		return list;
	endif

endfunction

function CalculateTotalDamage(defender, basedamage, attacker, dmglist)

	var dmg := 0;

	foreach key in (dmgList.keys())

		dmg := dmg + CalculateDamage(defender, key, basedamage*dmglist[key]);

	endforeach

	return cint(dmg);

endfunction

function CalculateDamage(defender, type, dmg)

	var resistSpells := 0;
	var resistMod := 0;
//	if (!defender.npctemplate)
//		resistSpells := cdbl(AP_GetSkill(defender, RESISTING_SPELLS)/200.0);
//	endif

	var resist;
	
	if (defender.npctemplate)
		var npctemplate := NPC_ParseTemplateName(defender.npctemplate);
		var cfg := ReadConfigFile(":brainai:npcdesc");

		case(type)
			DMG_FIRE: 		resist := cdbl( cfg[npctemplate.template].FireResist/100.0) + cdbl( defender.resist_fire_mod/100.0);
			DMG_COLD: 		resist := cdbl( cfg[npctemplate.template].ColdResist/100.0 ) + cdbl( defender.resist_cold_mod/100.0); 
			DMG_ENERGY: 	resist := cdbl( cfg[npctemplate.template].EnergyResist/100.0 ) + cdbl( defender.resist_energy_mod/100.0);
			DMG_PHYSICAL:	resist := cdbl( cfg[npctemplate.template].PhysicalResist /100.0 ) + cdbl( defender.resist_physical_mod/100.0);
			DMG_POISON: 	resist := cdbl( cfg[npctemplate.template].PoisonResist/100.0 ) + cdbl( defender.resist_poison_mod/100.0);
		endcase
	else
		//printtextabove( defender, "player " + type);
		case(type)
			DMG_FIRE: 	resist := cdbl( defender.resist_fire/100.0); 
				  		resistMod := cdbl( defender.resist_fire_mod/100.0);
			DMG_COLD: 	resist := cdbl( defender.resist_cold/100.0 ); 
				  		resistMod := cdbl( defender.resist_cold_mod/100.0);
			DMG_ENERGY: resist := cdbl( defender.resist_energy/100.0 );
				    	resistMod := cdbl( defender.resist_energy_mod/100.0);
			DMG_PHYSICAL: 	resist := cdbl( defender.resist_physical /100.0 );
				      		resistMod := cdbl( defender.resist_physical_mod/100.0);
			DMG_POISON: resist := cdbl( defender.resist_poison/100.0 );
				    	resistMod := cdbl( defender.resist_poison_mod/100.0);
		endcase		
	endif

	if (type == DMG_PHYSICAL)
		if (defender.shield)
			if (TemHabilidade(defender, "Defensor 4"))
				resist := resist + 0.4;
			elseif (TemHabilidade(defender, "Defensor 3"))
				resist := resist + 0.3;
			elseif (TemHabilidade(defender, "Defensor 2"))
				resist := resist + 0.2;
			elseif (TemHabilidade(defender, "Defensor"))
				resist := resist + 0.1;
			endif
		endif

		/*if (TemHabilidade(defender, "Especialista em Escudos") && (defender.shield).IsA(POLCLASS_ARMOR) ) // Se tiver a hab e usar escudo
			resist := resist + (AP_GetTrueSkill(defender,PARRY)/1000); // Ganha até +10% de bonus
		endif
		
		if (isBloodied(defender) && TemHabilidade(defender, "Pele de Rocha"))
			if (resist < 0.5)
				resist := 0.5;
			endif
		endif*/
	else
		if (TemHabilidade(defender, "Protecao Elemental") && defender.shield)
			resist := resist + 0.5;
		endif
	endif
		

//	printtextabove(defender, "dano " + type + " resist " + resist + " total dmg  " + dmg + " resistido " + (dmg*resist) + " final " + (dmg - (dmg*resist)) );

	if (!defender.npctemplate)
		if (resist >= 0.8)
			resist := 0.8;
		endif
	endif
	

//	if ( (resistSpells+resistMod) > resist)
//		return (dmg - (dmg*(resistSpells+resistMod)));
//	else
		return (dmg - (dmg*resist));
//	endif

endfunction

function ApplyDamageEX(defender, dmg, type, source := 0)

	var totaldmg := cint(CalculateDamage(defender, type, dmg ));

//	printtextabove(defender, "raw dmg " + dmg + " type " + type + " final " + totaldmg);
	
	ApplyRawDamage( defender, totaldmg );

	if (source)
		SetLastDamageInfo(defender, totaldmg, type, source);
		SetScriptController(source);
	endif

	return totaldmg;

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

	//Bonus Tacitcs
	var tatics := cdbl(AP_GetSkill(who, TACTICS));
	var taticsBonus := 0;
	//if (tatics >= 100)
	//	taticsBonus := 0.06;
	//endif
	tatics := cdbl(tatics);
	tatics := tatics/100.0 + taticsBonus;

	//Bonus Anatomy    
	//var anatomy := cdbl(AP_GetSkill(who, ANATOMY)/2.0) + 5.0;
	//anatomy := anatomy/100.0;
    
	//Bonus STR
	var stat := STRENGTH;
/*	if (temHabilidade(who, "Ataques Precisos"))
		stat := DEXTERITY;
	endif
	
	if (temHabilidade(who, "Taticas Arcanas"))
		stat := INTELLIGENCE;
	endif*/
	var str := cdbl( AP_GetStat(who, stat)*0.3 );
	var strBonus := 0;
	if (AP_GetStat(who, stat) >= 100 ) //se tiver mais de 100 de str
		strBonus := 0.1;
	endif

//	printtextabove(who, " str dmg " + str/100 + " bonus str " + strBonus);
	str := str/100.0 + strBonus;
	
	//arco curto, longo, besta, besta pesada e besta de repeticao
	if ( lower(who.weapon.attribute) == "rangedcombat")
		str := 0;
	endif 

	//Bonus de prop da arma    
	var weapon := Cdbl(GetObjProperty(who.weapon, "dmgbonus")) + cdbl(GetObjProperty(who.weapon, "#dmgbonus"));

	//Bonus de prop do mobile
	var other :=Cdbl(GetObjProperty(who, "dmgbonus")) + cdbl(GetObjProperty(who, "#dmgbonus")) ;

	//Bonus de Spells
	other := other - cdbl(GetObjProperty(who, "dmgreduction"));
  
	if (GetObjProperty(who, "#backstab") == 2)
		EraseObjProperty(who, "#backstab");
		other := other + 0.2;
		if (TemHabilidade(who, "Flanquear"))
			other := other + cdbl(AP_GetSkill(who, TACTICS) / 100);
		endif
	endif    

	if (GetObjProperty(who, "twohanded") == 1)
		other := other + 0.2;
	endif
	
	if (lower(who.weapon.attribute) == "meleecombat2h")
		if (TemHabilidade(who, "Barbaro"))
			other := other + 0.3;
		endif
		if (TemHabilidade(who, "Barbaro 2"))
			other := other + 0.2;
		endif
		if (TemHabilidade(who, "Barbaro 3"))
			other := other + 0.2;
		endif
		if (TemHabilidade(who, "Barbaro 4"))
			other := other + 0.2;
		endif
	endif
	
	if (lower(who.weapon.attribute) == "meleecombat1h")
		if (TemHabilidade(who, "Combatente"))
			other := other + 0.3;
		endif
		if (TemHabilidade(who, "Combatente 2"))
			other := other + 0.2;
		endif
		if (TemHabilidade(who, "Combatente 3"))
			other := other + 0.2;
		endif
		if (TemHabilidade(who, "Combatente 4"))
			other := other + 0.2;
		endif
	endif
	
	if (lower(who.weapon.attribute) == "rangedcombat")
		if (TemHabilidade(who, "Queda Livre"))
			other := other + 0.2;
		endif
		if (TemHabilidade(who, "Queda Livre 2"))
			other := other + 0.2;
		endif
		if (TemHabilidade(who, "Queda Livre 3"))
			other := other + 0.2;
		endif
		if (TemHabilidade(who, "Queda Livre 4"))
			other := other + 0.2;
		endif
	endif
	
	if (who.shield)
		if (TemHabilidade(who, "Defensor 4"))
			other := other - 0.1;
		elseif (TemHabilidade(who, "Defensor 3"))
			other := other - 0.2;
		elseif (TemHabilidade(who, "Defensor 2"))
			other := other - 0.3;
		elseif (TemHabilidade(who, "Defensor"))
			other := other - 0.4;
		endif
	endif
	
	if (GetCooldown(who, "partiraomeio"))
		if (!(defender.serial in GetObjProperty(who, "#mobilespartiraomeio")))
			other := other + 1.0;
			PrintText(defender, "*Feriu-se seriamente com a investida*");
		endif
	endif
	
	if (GetObjProperty(who, "#SequenciaDevastadoraAcumulada"))
		other := cint(GetObjProperty(who, "#SequenciaDevastadoraAcumulada")) * 0.2;
	endif

	var mod := dmg * (tatics + str + weapon + other);
    
	//printtextabove(who,  " d " + dmg + " t " + tatics + " s " + str + " w " + weapon + " o " + other + " dt " + mod );
    
	dmg := dmg + mod;
	if (dmg < 1)
		dmg := 1;
	endif
	return dmg;
    
endfunction

function DamageFLS(defender, dmg, type := DMG_FORCED, source := 0)
	var totaldmg;
	if (type == DMG_FORCED)
		totaldmg := CInt(dmg);
	else
		totaldmg := CInt(CalculateDamage(defender, type, dmg ));
		//printtextabove(defender, "raw dmg " + dmg + " type " + type + " final " + totaldmg);
	endif

	DisplayDamage(defender, totaldmg, source);
	ApplyRawDamage(defender, totaldmg, DAMAGE_NO_SHOW+DAMAGE_USE_REPSYS);
	

	if (isBloodied(defender))
		RecalcVitals(defender);
		MakeBleed(defender);
	endif

	if (source)
		SetLastDamageInfo(defender, totaldmg, type, source);
		SetScriptController(source);
	endif

	return totaldmg;

endfunction

