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

include "include/arrays";
include ":ghaia:ghaiaUtil";
include ":charactercreation:characters";
include ":timedscripts:timedScripts";

const BASE_DIFFICULTY := 19;
const GET_MODIFIER := 1;

function rollAttrDice(a := 1, b:= 1, c :=1, d:= 1, e:= 1)
   return 0;
endfunction
function GetSkillBonus(a := 1, b:= 1)
   return 0;
endfunction
function rollResistDice(a := 1, b:= 1)
   return 0;
endfunction
function checkImmunity(a := 1, b:= 1)
   return 0;
endfunction

function CalculatePowerPercent(power, percent)
   percent := cdbl(percent);
   return cint(cdbl(power) * (percent / 100));
endfunction

function CheckBlockDodge(who, return_block := 1, return_dodge := 1, no_message := 0)
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
	if (chardata != error && chardata.hits)
		hp := chardata.hits;
	endif
   
	if (hp < 1) 
		hp := 1;
	endif

	hp := (15 + hp + Cint(GetObjProperty(mobile, "hitsmod")) + Cint(GetObjProperty(mobile, "#hitsmod"))) * 100;

	return cint(hp);
endfunction

function CalcSuccessPercent(AC, MOD)
   var result := (cdbl(20) - (cdbl(AC) - cdbl(MOD))) /cdbl(20);
   return result * 100;
endfunction