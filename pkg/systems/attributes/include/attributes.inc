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

include ":attributes:regen";
include ":attributes:settings";
include ":attributes:skills";
include ":attributes:stats";
include ":attributes:vitals";
include ":attributes:attributeConstants";
include ":attributes:advanceCheck";
include ":datafile:datafile";
include ":charactercreation:habilidades";
include "include/arrays";
include ":ghaia:ghaiaUtil";
include ":charactercreation:characters";
include ":timedscripts:timedScripts";

const BASE_DIFFICULTY := 19;
const GET_MODIFIER := 1;

// params.critical -> dice >= value
// params.failure -> dice <= value
// params.get_critical -> returns {dice, critical}
function rollAttrDice(mobile, attribute, penalty := 0, params := struct{})
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

	// Aplica penalidades comums
	if (GetObjProperty(mobile, "ArmorPenalty"))
		penalty -= 1;
	endif

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
      param_return.two_dice := ReturnHighest(dices);
	elseif (penalty > 0)
		var dices := array{ RandomDiceRoll("1d20"), dice};
		dice := ReturnHighest(dices);
      param_return.two_dice := ReturnLowest(dices);
	endif

	// Grava info de critico.
	if (dice >= params.critical)
		param_return.critical := 1;
	elseif (dice <= params.failure)
		param_return.critical := -1;
	else
		param_return.critical := 0;
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

function GetSkillBonus(mobile, skills := 0)
   var bn := 1;
	if (skills)
		if (typeof(skills) != "Array") 
			skills := array{skills};
		endif
      if (!skills[1])
         return 0;
      endif

      bn := HaveSkill(mobile, skills, GET_MODIFIER);
		if (!bn) 
         return 0; 
      endif
	endif

   var proficiency := 0;

   if (mobile.npctemplate);
      var level := 1;
      var npcelem := NPC_GetNPCConfig(mobile.npctemplate);
      level       := GetConfigInt(npcelem, "Difficulty");

      if (!level || level <= 4)
         proficiency := 2;
      elseif (level <= 8)
         proficiency := 3;
      elseif (level <= 12)
         proficiency := 4;
      elseif (level <= 16)
         proficiency := 5;
      elseif (level <= 20)
         proficiency := 6; // Players stop here
      elseif (level <= 24)
         proficiency := 7;
      elseif (level <= 28)
         proficiency := 8;
      elseif (level <= 30)
         proficiency := 9; // Npc's can get to +9
      endif
   else
      if (bn >= 1)
         bn -= 1; //fix for +0
      endif

      proficiency := bn;
   endif

	return proficiency;
endfunction

function rollResistDice(mobile, type)
	var resist_list := array{};

	if (mobile.npctemplate);
		var npcelem := NPC_GetNPCConfig(mobile.npctemplate);
		resist_list := GetConfigStringArray(npcelem, "Resistencia");
		if (GetObjProperty(mobile, "resists"))
			resist_list += GetObjProperty(mobile, "resists");
		endif
	else
		var chardata := GetObjProperty(mobile, "chardata");
		resist_list  := chardata.resist_list;
	endif

	var proficiency_bonus := 0;
	if (type in resist_list)
		proficiency_bonus := GetSkillBonus(mobile);
	endif

	var dice := RandomDiceRoll("1d20") + proficiency_bonus;

	return dice;
endfunction

function HaveSkill(mobile, skill, get_mod := 1)
	if (typeof(skill) != "Array")
		skill := array{skill};
	endif

   var modifier := 0;

	foreach name in skill
      var res := AP_GetTrueSkill(mobile, name);
		if (res > 0)
			return cint(res);
		endif

      if (get_mod)
         modifier := GetSkillModifier(name);
      endif
      sleepms(2);
	endforeach

	return modifier; //n tem skill
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

function GetSkillModifier(name)
	var cfg := AP_GetAttributesCfgFile();
	var elem := FindConfigElem(cfg, name);
	if (elem == error)
		return "elem not found: " + GetConfigStringKeys( cfg );
	endif

	var type := GetConfigString(elem, "SkillType");
	if (type == error)
		return "type error";
	endif

   var modifier := -3;
   case (type)
      "S":
         modifier := -3;
         break;
      "I":
         modifier := -4;
         break;
      "C":
         modifier := -5;
         break;
   endcase

	return modifier;
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

function Wait(mobile, segundos)
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
		segundos := segundos - 1;
	endwhile

	EraseObjProperty(mobile, "#IsWaiting");

	return 1;
endfunction
 
function GetHP(mobile)
	var chardata := GetObjProperty(mobile, "chardata");
	var hp := 0;
	if (chardata != error && chardata.hitpoints)
		hp := chardata.hitpoints;
	endif
   
	if (hp < 1) 
		hp := 1;
	endif

	hp := (5 + hp + Cint(GetObjProperty(mobile, "hitsmod")) + Cint(GetObjProperty(mobile, "#hitsmod"))) * 100;

	return cint(hp);
endfunction

function CalcSuccessPercent(AC, MOD)
   var result := (cdbl(20) - (cdbl(AC) - cdbl(MOD))) /cdbl(20);
   return result * 100;
endfunction

function LearnSkill(who, skill_name)
   var mystat := AP_GetTrueSkill(who, skill_name);
   var attr := ReadConfigFile(":attributes:attributes");
   AP_SetTrueSkill(who, skill_name, mystat+1);

   var levels := GetConfigStringArray(attr[skill_name], "Level");
   foreach giv in levels
      sleepms(2);
      giv := SplitWords(giv);
      if (cint(giv[1]) != mystat+1)
         continue;
      endif

      if (giv[2] == "feat")
         var new_feat := StrReplace(giv[3], "_", " ");
         LearnFeat(who, new_feat);
      endif
   endforeach

   if (skill_name["Rúnica"])
      LearnFeat(who, "Linguagem Rúnica", 0, 0);
   endif
   
   var gives := GetConfigStringArray(attr[skill_name], "Give");
   foreach giv in gives
      giv := SplitWords(giv);
      if (giv[1] == "mana")
         var chardata := GetObjProperty(who, "chardata");
         chardata.manapoints += cint(giv[2]);
         SetObjProperty(who, "chardata", chardata);
      elseif (giv[1] == "hits")
         var chardata := GetObjProperty(who, "chardata");
         chardata.hitpoints  += cint(giv[2]);
         SetObjProperty(who, "chardata", chardata);
      elseif (giv[1] == "stam")
         var chardata := GetObjProperty(who, "chardata");
         chardata.stampoints += cint(giv[2]);
         SetObjProperty(who, "chardata", chardata);
      endif
      sleepms(2);
   endforeach

   if (attr[skill_name].achievement)
      var achiev := SplitWords(attr[skill_name].achievement, " ");
      var nextp := cint(achiev[2]) * mystat+1;
      if (who.GetAchievement(achiev[1]) < nextp)
         who.SetAchievement(achiev[1], nextp);
      endif
   endif
   RecalcVitals(who);
endfunction