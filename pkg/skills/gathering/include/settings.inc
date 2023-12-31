//$Id: settings.inc 891 2005-11-04 06:29:43Z muaddiblsd $

/*===============================================================
* Current Version
* SETTINGS.INC - v1.0
* Updated 9/27/2005 2:54PM
*
* -- Revision v1.0 --
* Austin:
*  Created include file
===============================================================*/

use uo;
use os;
use cfgfile;

/*
 * LJ_GetSettingsCfgFile(engine_name)
 *
 * Purpose
 * Reads in :lumberjacking:config/settings.cfg
 *
 * Parameters
 *
 * Return value
 * A config file reference.
 *
 */
function LJ_GetSettingsCfgFile()
	var cfg := ReadConfigFile(":gathering:config/settings");

	if ( cfg.errortext )
		SysLog("Error::LJ_GetSettingsCfgFile() - Unable to open [:lumberjacking:config/settings.cfg] ->"+cfg.errortext);
	endif

	return cfg;
endfunction

/*
 * LJ_GetSettingsCfgElem(elem_name, cfg_file)
 *
 * Purpose
 * Retrieves an elem from a config file. 
 *
 * Parameters
 * elem_name:	A string matching the elem name to be retrieved.
 * cfg_file:	Optional parameter - reference to a config already read in by LJ_GetSettingsCfgFile()
 *
 * Return value
 * A config file elem reference.
 *
 */
function LJ_GetSettingsCfgElem(elem_name, byref cfg_file:=0)
	if ( !cfg_file )
		cfg_file := LJ_GetSettingsCfgFile();
	endif
	
	var elem := cfg_file[elem_name];

	if ( elem.errortext )
		SysLog("Error::LJ_GetSettingsCfgElem() - Unable to find elem ["+elem_name+"] ->"+elem.errortext);
	endif

	return elem;
endfunction

