/*
 * $Id: titles.inc 891 2005-11-04 06:29:43Z muaddiblsd $
 *
 */

use uo;
use os;
use cfgfile;

include ":attributes:attributes";

/*
 * AP_GetLevelTitle(skill_level)
 *
 * Purpose
 * Gets the title for a particlar skill level.
 *
 * Parameters
 * skill_level:	Skill level of a skill. (e.g. 80% = 80)
 *
 * Return value
 * Returns a string on success.
 * Returns an error on failure.
 *
 */
function AP_GetLevelTitle(skill_level)
	if (skill_level >= 105)
		return "Lendário";
	elseif (skill_level >= 95)
		return "Grão Mestre";
	elseif (skill_level >= 75)
		return "Mestre";
	elseif (skill_level >= 55)
		return "Hábil";
	elseif (skill_level >= 45)
		return "Aprendiz";
	elseif (skill_level >= 20)
		return "Amador";
	else
		return "Inábil";
	endif
endfunction

/*
 * AP_GetSkillTitle(skill_name)
 * 
 * Purpose
 * Finds the title name of a skill.
 *
 * Parameters
 * skill_name:	Name of a skill from attributes.cfg
 *
 * Return value
 * Returns a string on success.
 * Returns an error on failure.
 *
 */
function AP_GetSkillTitle(skill_name)
	var elem_info := AP_GetAttributeCfgElem(skill_name);
	if ( !elem_info )
		return elem_info;
	endif
	
	if ( elem_info.Title) 
		return elem_info.Title;
	else
		return error{"errortext":="Attribute does not have a 'Title' entry."};
	endif
endfunction