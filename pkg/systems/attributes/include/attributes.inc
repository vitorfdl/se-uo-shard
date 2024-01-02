//$Id: attributes.inc 826 2005-11-02 09:23:08Z austin $

/*===============================================================
* Current Version
* ATTRIBUTES.INC - v1.0
* Updated 9/24/2005 6:52PM
*
* -- Revision v1.0 --
* Austin:
*  Created include file
*
* Functions:
* rollAttrDice(mobile, attribute, penalty := 0, critical := 20) - Rolagem de atributo
* GetSkillBonus(mobile, skill := 0) - Bonus de profiencia pelo nivel, se passar skill já checa se vai ter
* rollResistDice(mobile, type) - Rolagem de resistência
* GetBonusAttr(number) - Convert numero em bonus 12 -> 1
* HaveSkill(mobile, skill) - Checa se tem skill
* (attr...) 
* GetHP(mobile)
===============================================================*/

use uo;
use os;
use cfgfile;
use vitals;
use attributes;
use math;

include ":attributes:regen";
include ":attributes:settings";
include ":attributes:skills";
include ":attributes:stats";
include ":attributes:vitals";
include ":attributes:attributeConstants";
include ":attributes:advanceCheck";
include ":datafile:datafile";

include "include/arrays";
include ":ghaia:ghaiaUtil";
include ":charactercreation:characters";
include ":timedscripts:timedScripts";

const BASE_DIFFICULTY := 19;
const GET_MODIFIER := 1;

enum DICE_PENALTY
	DICE_PENALTY_NONE := 0,
	DICE_PENALTY_LIGHT := 1,
	DICE_PENALTY_MEDIUM := 2,
	DICE_PENALTY_HEAVY := 3,
	DICE_PENALTY_EXHAUSTED := 4,
endenum


enum STAT_BEHAVIOR
	SS_OVERRIDE := 0x0,
	SS_ADD := 0x1,
	SS_SUBTRACT := 0x2,
endenum


// params.critical -> dice >= value
// params.failure -> dice <= value
// params.get_critical -> returns {dice, critical}
function rollAttrDice(mobile, attribute := "", penalty := DICE_PENALTY_NONE, params := struct{})
	// Seta parametro de critico
	if (!params.critical)
		params.critical := 20;
	endif
	if (!params.failure)
		params.failure := 1;
	endif

	// Rolagem de dados
	var dice := RandomDiceRoll("1d20");
	var param_return := struct{};

	if (!mobile.npctemplate && !mobile.master)
		if (AP_GetVitalMaximumValue(mobile, "DP")-30 > AP_GetVital(mobile, "DP"))
			penalty -= 2;
		endif

		var hunger := AP_GetVital(mobile, "Hunger") / 60;
		if (!mobile.npctemplate && hunger >= 5)
			penalty -= 2;
		endif
	endif

	// Rola o dado com penalidade, se for o caso.
	if (penalty < 0 || GetObjProperty(mobile, "ArmorPenalty") >= 1)
		var dices := array{ RandomDiceRoll("1d20"), dice};
		dice := ReturnLowest(dices);
      param_return.second_dice := ReturnHighest(dices);
	elseif (penalty > 0)
		var dices := array{ RandomDiceRoll("1d20"), dice};
		dice := ReturnHighest(dices);
      param_return.second_dice := ReturnLowest(dices);
	endif

	// Grava info de critico.
	if (dice >= params.is_critical)
		param_return.is_critical := 1;
	elseif (dice <= params.failure)
		param_return.is_critical := -1;
	else
		param_return.is_critical := 0;
	endif

	// Adiciona bonus de atributo
	if (attribute && attribute != 0)
		var mod := AP_GetStat(mobile, attribute, RETURN_BONUS);
		dice += mod;
	endif
	
   if (GetObjProperty(mobile, "#divineorientation"))
      dice += RandomDiceRoll("1d4");
      TS_LowerDuration(mobile, "divineorientation", -1);
   endif


	if (dice < 1)
		dice := 1;
	endif

	// Retorna
	if (params.get_critical)
		param_return.dice := dice;
		return param_return;
	endif

	return dice;
endfunction

function checkImmunity(a := 1, b:= 1)
   return 0;
endfunction

function GetSkillBonus(mobile, skills := 0) // TODO: Fix this
   DebugMsg(mobile, "Searching skill bonus for {}".format(skills));
   // var bn := 1;
	// if (skills)
	// 	if (typeof(skills) != "Array") 
	// 		skills := array{skills};
	// 	endif
   //    if (!skills[1])
   //       return 0;
   //    endif

      // bn := HaveSkill(mobile, skills, GET_MODIFIER);
		// if (!bn) 
         return 0; 
      // endif
	// endif

   // var proficiency := 0;

   // if (mobile.npctemplate);
   //    var level := 1;
   //    var npcelem := NPC_GetNPCConfig(mobile.npctemplate);
   //    level       := GetConfigInt(npcelem, "Difficulty");

   //    if (!level || level <= 4)
   //       proficiency := 2;
   //    elseif (level <= 8)
   //       proficiency := 3;
   //    elseif (level <= 12)
   //       proficiency := 4;
   //    elseif (level <= 16)
   //       proficiency := 5;
   //    elseif (level <= 20)
   //       proficiency := 6; // Players stop here
   //    elseif (level <= 24)
   //       proficiency := 7;
   //    elseif (level <= 28)
   //       proficiency := 8;
   //    elseif (level <= 30)
   //       proficiency := 9; // Npc's can get to +9
   //    endif
   // else
   //    if (bn >= 1)
   //       bn -= 1; //fix for +0
   //    endif

   //    proficiency := bn;
   // endif

   // DebugMsg(mobile, "Bonus is: {}".format(proficiency));
	// return proficiency;
endfunction

function getResistBonus(mobile, type)
	if (type == "Fortitude")
		var strength_modifier := AP_GetStat(mobile, "Strength", RETURN_MOD);
		var constitution_modifier := AP_GetStat(mobile, "Constitution", RETURN_MOD);
		return ceil((strength_modifier + constitution_modifier) / 2);
	elseif (type == "Reflex")
		var dexterity_modifier := AP_GetStat(mobile, "Dexterity", RETURN_MOD);
		var wisdom_modifier := AP_GetStat(mobile, "Wisdom", RETURN_MOD);
		return ceil((dexterity_modifier + wisdom_modifier) / 2);
	elseif (type == "Will")
		var intelligence_modifier := AP_GetStat(mobile, "Intelligence", RETURN_MOD);
		var charisma_modifier := AP_GetStat(mobile, "Charisma", RETURN_MOD);
		return ceil((intelligence_modifier + charisma_modifier) / 2);
	else
		print("Invalid Resist type: " + type);
		return 0;
	endif
endfunction

function rollResistDice(mobile, type)
	var proficiency_bonus := 0;
	var dice := RandomDiceRoll("1d20");

	dice += getResistBonus(mobile, type);
	return dice;
endfunction

// function HaveSkill(mobile, skill, get_mod := 1)
// 	if (typeof(skill) != "Array")
// 		skill := array{skill};
// 	endif

//    var modifier := 0;

// 	foreach name in skill
//       var res := AP_GetTrueSkill(mobile, name);
// 		if (res > 0)
// 			return cint(res);
// 		endif

//       if (get_mod)
//          modifier := GetSkillModifier(name);
//       endif
//       sleepms(2);
// 	endforeach

// 	return modifier; //n tem skill
// endfunction

function CalculatePowerPercent(power, percent := 100)
   percent := cdbl(percent);
   return cint(cdbl(power) * (percent / 100));
endfunction

function CheckBlockDodge(who, return_block := 0, return_dodge := 0, no_message := 0)
   if (return_dodge)
      if (TS_GetCooldown(who, "dodge")) 
         if (!no_message) 
			   PlaySoundEffect(who, array{569, 570, 571}.randomentry());
            PrintText(who, "*esquivou*", SSM_FAIL);
         endif
         return 1; 
      endif
   endif

   if (return_block)
      if (TS_GetCooldown(who, "block"))
         if (!no_message)
            PrintText(who, "*bloqueou*", SSM_FAIL);
         endif
         return 1; 
      endif
   endif
endfunction

function debugMSG(who, text)
	if (GetObjProperty(who, "debug"))
		SendSysMessageEx(who, text);	
	endif
endfunction

function GetSkillIdFromName(name)
	var cfg := AP_GetAttributesCfgFile();
	var elem := FindConfigElem(cfg, name);
	if (elem == error)
		return "elem not found: " + GetConfigStringKeys( cfg );
	endif
	var id := GetConfigInt(elem, "SkillId");
	if (id == error)
		return "id error";
	endif
	return id;

endfunction

function GetSkillNameFromId(id)
	var cfg := AP_GetAttributesCfgFile();
	var keys := GetConfigStringKeys( cfg );

	foreach  key in keys
		var elem := FindConfigElem(cfg, key);
		var skillid := GetConfigInt(elem, "SkillId");
		if (id == skillid )
			return key;
		endif
	endforeach

	return "";

endfunction

/* AP_GetAttributesCfgFile()
 * 
 * Purpose
 * Loads attributes.cfg
 *
 * Parameters
 *
 * Return value
 * Config file
 *
 */
function AP_GetAttributesCfgFile()
	var cfg_file := ReadConfigFile(":attributes:attributes");
	if ( cfg_file.errortext )
		SysLog("Error::AP_GetAttributesCfgFile() - Unable to open attributes.cfg ->"+cfg_file.errortext);
	endif

	return cfg_file;
endfunction

/*
 * AP_GetAttributeCfgElem(attribute_name, cfg_file)
 *
 * Purpose
 * Retrieves a config elem for an attribute in attributes.cfg
 *
 * Parameters
 * attribute_name:	Name of the attribute to get the elem of.
 * cfg_file:		Optional parameter - reference to a config already read in.
 *
 * Return value
 * Returns a config file elem.
 *
 *
 */
function AP_GetAttributeCfgElem(attribute_name, byref cfg_file:=0)
	if ( !cfg_file )
		cfg_file := AP_GetAttributesCfgFile();
	endif
	
	var cfg_elem := cfg_file[attribute_name];
	if ( cfg_elem.errortext )
		SysLog("Error::AP_GetAttributeCfgElem() - unable to find cfg elem ["+attribute_name+"] ->"+cfg_elem.errortext);
	endif
	
	return cfg_elem;
endfunction

/* AP_GetAttributesCfgFile()
 * 
 * Purpose
 * Loads the attributes datafile.
 *
 * Parameters
 *
 * Return value
 * Config file
 *
 */
function AP_GetAttributesDataFile()
	var data_file := DFOpenDataFile(":attributes:attributes", DF_CREATE);
	if ( data_file.errortext )
		SysLog("Error::AP_GetAttributesDataFile() - Unable to open attributes.txt ->"+data_file.errortext);
	endif

	return data_file;
endfunction

/*
 * AP_GetAttributeDataElem(attribute_name, cfg_file)
 *
 * Purpose
 * Retrieves a datafile elem from the attributes package.
 *
 * Parameters
 * attribute_name:	Name of the elem to retrieve.
 *
 * Return value
 * Returns a datafile elem.
 *
 */
function AP_GetAttributeDataElem(elem_name)
	var data_file := AP_GetAttributesDataFile();
	var data_elem := DFFindElement(data_file, elem_name, DF_CREATE);
	
	if ( data_elem.errortext )
		SysLog("Error::AP_GetAttributeDataElem() - unable to find elem ["+elem_name+"] ->"+data_elem.errortext);
	endif
	
	return data_elem;
endfunction

/*
 * AP_ListAttributesByType(type)
 *
 * Purpose
 * Gets the name of all attributes of a certain type.
 *
 * Parameters
 * Type:	String containing the type of attributes to list.
 *
 * Return value
 * Returns an array
 *
 */
function AP_ListAttributesByType(type:="")
	if ( !type )
		var cfg_file := ReadConfigFile(":attributes:attributes");
		return GetConfigStringKeys(cfg_file);
	endif
	
	type := Lower(type);
	var data_elem := AP_GetAttributeDataElem("Categories");
	var list := data_elem.GetProp(type);
	list := RemoveFromArray(list, array{"Parry", "Tactics", "Wrestling", "Versatil"});

	return list;
endfunction

function Wait(mobile, segundos, play_act := 0, sayabove := 0)
	if ( GetObjProperty(mobile, "#IsWaiting") > ReadGameClock() )
		return 0; // Já está esperando
	else
		EraseObjProperty(mobile, "#IsWaiting");
	endif

	var x := mobile.x;
	var y := mobile.y;
	var hp := AP_GetVital(mobile, "Hits");
	SetObjProperty(mobile, "#IsWaiting", ReadGameClock()+segundos+1);
	while (segundos)
		sleep(1);
		if (x != mobile.x || y != mobile.y ||
		    AP_GetVital(mobile, "Hits") < hp ||
		    !CInt(GetObjProperty(mobile, "#IsWaiting")) ||
		    mobile.warmode ||
		    GetEquipmentByLayer(mobile, 0x01) ||
		    GetEquipmentByLayer(mobile, 0x02)
		   )
			EraseObjProperty(mobile, "#IsWaiting");
			return 0;
		endif

      if (play_act || sayabove)
         if ( segundos % 2 == 0)
            if (play_act)
               PerformAction(mobile, play_act);
            endif

            if (sayabove)
               PrintText(mobile, sayabove);
            endif
         endif
      endif
		segundos := segundos - 1;
	endwhile

	EraseObjProperty(mobile, "#IsWaiting");

	return 1;
endfunction
 
function GetHP(mobile)
	var chardata := GetObjProperty(mobile, "chardata");
	var hp := 0;

	if (chardata != error)
		hp := cint(chardata.hits);
	endif
   
	if (hp < 1) 
		hp := 1;
	endif

	hp := (hp + Cint(GetObjProperty(mobile, "hitsmod")) + Cint(GetObjProperty(mobile, "#hitsmod"))) * 100;

	return cint(hp);
endfunction

function CalcSuccessPercent(AC, MOD)
   var result := (cdbl(20) - (cdbl(AC) - cdbl(MOD))) /cdbl(20);
   return result * 100;
endfunction