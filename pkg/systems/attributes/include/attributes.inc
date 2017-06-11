//$Id: attributes.inc 826 2005-11-02 09:23:08Z austin $

/*===============================================================
* Current Version
* ATTRIBUTES.INC - v1.0
* Updated 9/24/2005 6:52PM
*
* -- Revision v1.0 --
* Austin:
*  Created include file
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
include ":attributes:skillCheck";
include ":attributes:advanceCheck";
include ":datafile:datafile";
include ":charactercreation:habilidades";
include "include/arrays";
include ":ghaia:ghaiaUtil";

function rollAttrDice(mobile, attribute, penalty := 0, critical := 20)
	var dice := RandomDiceRoll("1d20");

	if (penalty < 0 || GetObjProperty(mobile, "ArmorPenalty") >= 1)
		var dices := array{ RandomDiceRoll("1d20"), dice};
		RemoveHighest(dices);
		dice := dices[1];
	elseif (penalty > 0)
		var dices := array{ RandomDiceRoll("1d20"), dice};
		RemoveLowest(dices);
		dice := dices[1];
	endif

	// print("{}: DiceResult: {}".format(mobile.name, dice));

	if (attribute && attribute != 0)
		var mod := GetBonusAttr(AP_GetTrueStat(mobile, attribute));;
		// print("{}: dice mod: {}".format(mobile.name, mod));
		dice += mod;
	endif
	
	if (dice < 1)
		dice := 1;
	endif

	return dice;
endfunction

function getProfiencyBonus(mobile)
	var proficiency := 0;
	var level := 1;
	if (mobile.npctemplate);
		var npcelem := NPC_GetNPCConfig(mobile.npctemplate);
		level       := GetConfigInt(npcelem, "Difficulty");
	else
		var chardata := GetObjProperty(mobile, "chardata");
		level        := cint(chardata.level);
	endif

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

	return proficiency;
endfunction

function rollResistDice(mobile, type)
	var resist_list := array{};

	if (mobile.npctemplate);
		var npcelem := NPC_GetNPCConfig(mobile.npctemplate);
		resist_list  := GetConfigStringArray(npcelem, "Resistência");
	else
		var chardata := GetObjProperty(mobile, "chardata");
		resist_list   := chardata.resist_list;
	endif

	var proficiency_bonus := 0;
	if (type in resist_list)
		proficiency_bonus := getProfiencyBonus(mobile);
	endif

	var dice := RandomDiceRoll("1d20") + proficiency_bonus;

	return dice;
endfunction

function GetBonusAttr(number)
	var modifier := Floor((number - 10) / 2);
	return modifier;
endfunction

function GetTranslatedAttribute(number)
	var translated := GetBonusAttr(number);
	case(cstr(translated))
		"-5":"-4":
			translated := "Ameba";
		"-3":"-2":
			translated := "Baixissimo";
		"-1":"0":
			translated := "Baixo";
		"1":"2":
			translated := "Normal";
		"3":"4":
			translated := "Médio";
		"5":"6":
			translated := "Alto";
		"7":"8":
			translated := "Muito Alto";
		"9":"10":
			translated := "Heróico";
	endcase

	return translated;
endfunction

function HaveSkill(mobile, skill)
	var skill_list := array{};

	if (mobile.npctemplate);
		var npcelem := NPC_GetNPCConfig(mobile.npctemplate);
		skill_list  := GetConfigStringArray(npcelem, "Pericia");
	else
		var chardata := GetObjProperty(mobile, "chardata");
		skill_list   := chardata.Skills;
	endif

	if (!skill_list)
		return 0; //n tem skill
	elseif (skill in skill_list)
		return 1; //Tem Skill
	endif

	return 0; //n tem skill
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
	return data_elem.GetProp(type);
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

function maolivre(mobile, qtd, checkmount := 1)
	var rightHand := GetEquipmentByLayer(mobile, 0x01);
	var leftHand := GetEquipmentByLayer(mobile, 0x02);

	if (qtd == 1)
		if (rightHand && leftHand)
			SendSysMessage(mobile, "Voce precisa de uma mao livre.");
	    		return 0;
		endif
	elseif (qtd == 2)
		if (rightHand || leftHand)
			SendSysMessage(mobile, "Voce precisa das duas maos livres.");
	    		return 0;
		endif
	endif

	if (checkmount)
		var mounted := GetEquipmentByLayer(mobile, 0x19);

		if (mounted)
			SendSysMessage(mobile, "Voce nao pode fazer isto montado.");
			return 0;
		endif
	endif

	return 1;
endfunction

function GetHP(mobile)
	var cons     := CInt(AP_GetTrueStat(mobile, "Constitution"));
	var attr_mod := GetBonusAttr(cons);
	var chardata := GetObjProperty(mobile, "chardata");
	var hp := 0;
	if (chardata != error && chardata.hitpoints)
		hp := chardata.hitpoints;
	endif

	hp += cint(attr_mod + Cint(GetObjProperty(mobile, "hitsmod")) + Cint(GetObjProperty(mobile, "#hitsmod"))) * 100;
	if (hp < 1) 
		hp := 1;
	endif
                      
	return hp;
endfunction