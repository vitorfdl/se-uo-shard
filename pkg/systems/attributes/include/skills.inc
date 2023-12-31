//$Id: skills.inc 848 2005-11-02 20:54:39Z austin $

/*===============================================================
* Current Version
* SKILLS.INC - v1.0
* Updated 9/24/2005 6:52PM
*
* -- Revision v1.0 --
* Austin:
*  Created include file
===============================================================*/

include ":fls_core:fls_characters";


use uo;

/*
 * AP_GetSkill(mobile, skill_name)
 *
 * Purpose
 * Retrieves the base value of the skill plus its mod value.
 *
 * Parameters
 * mobile:	Mobile reference to retrieve the points from.
 * skill_name:	Name of the skill to retrieve the points for.
 *
 * Return value
 * Returns an integer.
 *
 */
function AP_GetSkill(who, skill_name)
	return AP_GetTrueSkill(who, skill_name)+AP_GetSkillMod(who, skill_name);
endfunction

/*
 * AP_GetTrueSkill(mobile, skill_name)
 *
 * Purpose
 * Retrieves the base value of the skill and turns it into a usable number.
 *
 * Parameters
 * mobile:	Mobile reference to retrieve the points from.
 * skill_name:	Name of the skill to retrieve the points for.
 *
 * Return value
 * Returns a double.
 *
 */
function AP_GetTrueSkill(who, skill_name)
	return CDbl(GetAttributeBaseValue(who, skill_name)) / 10.0;
endfunction

/*
 * AP_GetSkillMod(mobile, skill_name)
 *
 * Purpose
 * Retrieves the base value of the skill mod and turns it into a usable number.
 *
 * Parameters
 * mobile:	Mobile reference to retrieve the points from.
 * skill_name:	Name of the skill to retrieve the modifier points for.
 *
 * Return value
 * Returns a double.
 *
 */
function AP_GetSkillMod(who, skill_name)
	return CDbl(GetAttributeTemporaryMod(who, skill_name)) / 10.0;
endfunction

/*
 * AP_SetTrueSkill(mobile, skill_name, points)
 *
 * Purpose
 * Sets the value of the skill.
 * Example: AP_SetTrueSkill(mobile, "strength", 44.5)
 *
 * Parameters
 * mobile:	Mobile reference to set the points on.
 * skill_name:	Name of the skill to set the points on.
 * points:	Number of points to set the skill at.
 *
 * Return value
 * Returns a double.
 *
 */
function AP_SetTrueSkill(mobile, skill_name, points)
	SetAttributeBaseValue(mobile, skill_name, CInt(CDbl(points) * 10.0));
	RecalcVitals(mobile);
endfunction

/*
 * AP_SetSkillMod(mobile, skill_name, points)
 *
 * Purpose
 * Sets the modifier value for the skill.
 * Example: AP_SetSkillMod(mobile, "strength", -15.3)
 *
 * Parameters
 * mobile:	Mobile reference to set the modifier points on.
 * skill_name:	Name of the skill to set the modifier points on.
 * points:	Number of points to set the modifier skill at.
 *
 * Return value
 * Returns a double.
 *
 */
function AP_SetSkillMod(mobile, skill_name, points)
	var e := SetAttributeTemporaryMod(mobile, skill_name, CInt(CDbl(points) * 10.0));
	if (e == error)
		Print("Erro: " + e);
	endif
	RecalcVitals(mobile);
endfunction

/*
 * AP_ModifySkillMod(mobile, skill_name, points)
 *
 * Purpose
 * Modifies the modifier value for the skill.
 * Example: 	AP_SetStatMod(mobile, "alchemy", -15.3)
 *
 * Parameters
 * mobile:	Mobile reference to adjust the modifier points on.
 * stat_name:	Name of the skill to adjust the modifier points on.
 * points:	Number of points to adjust the modifier by.
 *
 * Return value
 * Returns the previous value
 *
 */
function AP_ModifySkillMod(mobile, stat_name, points)
	var orig_mod := AP_GetStatMod(mobile, stat_name);
	var new_mod := CDbl(points)+CDbl(orig_mod);
	
//	sendsysmessage(mobile, "nmod " + new_mod + " " + stat_name);
	AP_SetSkillMod(mobile, stat_name, new_mod);
	
	return orig_mod;
endfunction
