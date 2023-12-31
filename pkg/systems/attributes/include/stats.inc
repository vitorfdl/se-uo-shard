//$Id: stats.inc 848 2005-11-02 20:54:39Z austin $

/*===============================================================
* Current Version
* STATS.INC - v1.0
* Updated 9/24/2005 6:52PM
*
* -- Revision v1.0 --
* Austin:
*  Created include file
===============================================================*/

use uo;
use math;

enum ATTR_CONST
	RETURN_BONUS := 1,
	RETURN_MOD := 1, // same
	RETURN_TRUE := 0,
endenum

function GetBonusAttr(number)
	var modifier := Floor((number - 10) / 2);
	return cint(modifier);
endfunction

/*
 * AP_GetStat(mobile, stat_name)
 *
 * Purpose
 * Retrieves the base value of the stat plus its mod value.
 *
 * Parameters
 * mobile:		Mobile reference to retrieve the points from.
 * stat_name:	Name of the stat to retrieve the points for.
 *
 * Return value
 * Returns an integer.
 *
 */
function AP_GetStat(who, stat_name, bonus := 0)
	var result := AP_GetTrueStat(who, stat_name)+AP_GetStatMod(who, stat_name);
	if (bonus)
		result := GetBonusAttr(result);
	endif

	return cint(result);
endfunction

/*
 * AP_GetTrueStat(mobile, stat_name)
 *
 * Purpose
 * Retrieves the base value of the stat and turns it into a usable number.
 *
 * Parameters
 * mobile:		Mobile reference to retrieve the points from.
 * stat_name:	Name of the stat to retrieve the points for.
 *
 * Return value
 * Returns a double.
 *
 */
function AP_GetTrueStat(who, stat_name)
	return CDbl(GetAttributeBaseValue(who, stat_name)) / 10.0;
endfunction

/*
 * AP_GetStatMod(mobile, stat_name)
 *
 * Purpose
 * Retrieves the base value of the stat mod and turns it into a usable number.
 *
 * Parameters
 * mobile:		Mobile reference to retrieve the points from.
 * stat_name:	Name of the stat to retrieve the modifier points for.
 *
 * Return value
 * Returns a double.
 *
 */
function AP_GetStatMod(who, stat_name)
	return CDbl(GetAttributeTemporaryMod(who, stat_name)) / 10.0;
endfunction

/*
 * AP_SetTrueStat(mobile, stat_name, points)
 *
 * Purpose
 * Sets the value of the stat.
 * Example: AP_SetTrueStat(mobile, "strength", 44.5)
 *
 * Parameters
 * mobile:		Mobile reference to set the points on.
 * stat_name:	Name of the stat to set the points on.
 * points:		Number of points to set the stat at.
 *
 * Return value
 * Returns a double.
 *
 */
function AP_SetTrueStat(mobile, stat_name, points)
	SetAttributeBaseValue(mobile, stat_name, CInt(CDbl(points) * 10.0));
	RecalcVitals(mobile);
endfunction

/*
 * AP_SetStatMod(mobile, stat_name, points)
 *
 * Purpose
 * Sets the modifier value for the stat.
 * Example: 	AP_SetStatMod(mobile, "strength", -15.3)
 *
 * Parameters
 * mobile:		Mobile reference to set the modifier points on.
 * stat_name:	Name of the stat to set the modifier points on.
 * points:		Number of points to set the modifier stat at.
 * 
 * Return value
 * Returns a double.
 *
 */
function AP_SetStatMod(mobile, stat_name, points)
	SetAttributeTemporaryMod(mobile, stat_name, CInt(CDbl(points) * 10.0));
	RecalcVitals(mobile);
endfunction

/*
 * AP_ModifyStatMod(mobile, stat_name, points)
 *
 * Purpose
 * Modifies the modifier value for the stat.
 * Example: 	AP_SetStatMod(mobile, "strength", -15.3)
 *
 * Parameters
 * mobile:	Mobile reference to adjust the modifier points on.
 * stat_name:	Name of the stat to adjust the modifier points on.
 * points:	Number of points to adjust the modifier by.
 *
 * Return value
 * Returns the previous value
 *
 */
function AP_ModifyStatMod(mobile, stat_name, points)
	var orig_mod := AP_GetStatMod(mobile, stat_name);
	var new_mod := CDbl(points)+Cdbl(orig_mod);
	
	AP_SetStatMod(mobile, stat_name, new_mod);
	
	return orig_mod;
endfunction

/*
 * AP_GetStatCap(stat_name)
 *
 * Purpose
 * Gets the number a stat caps at.
 *
 * Parameters
 * stat_name: Name of the stat to check
 *
 * Return value
 * Returns a number or error
 *
 */
function AP_GetStatCap(skill_name)
	var cfg_elem := AP_GetSettingsCfgElem("Stats");
	
	// Check if there is a cap for that particular skill.
	var cap_val := GetConfigReal(cfg_elem, skill_name);
		
	return cap_val;
endfunction

/*
 * AP_CheckStatCap(mobile, stat_name)
 *
 * Purpose
 * Checks if a stat is greater than or equal to its maximum value in settings.cfg
 * If no stat_name is passed, it will check the totals.
 *
 * Parameters
 * mobile:		Mobile to check the stats on
 * stat_name:	Optional parameter - name of the stat to check.
 *
 * Return value
 * Returns 1 if the stat is >= to the cap
 *
 */
function AP_CheckStatCap(mobile, stat_name:="")
	var cfg_elem := AP_GetSettingsCfgElem("Stats");
	
	if ( stat_name )
		var cap := AP_GetStatCap(stat_name);
		return ( AP_GetTrueStat(mobile, stat_name) >= cap );
	else
		var total := 0.0;
		foreach attribute_name in ( AP_ListAttributesByType("Stat") )
			total := total + AP_GetTrueStat(mobile, attribute_name);
			sleepms(2);
		endforeach
		
		return ( total >= GetConfigReal(cfg_elem, "TotalCap") );
	endif
endfunction
