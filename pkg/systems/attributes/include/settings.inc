//$Id: settings.inc 264 2005-09-27 21:55:42Z austin $

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
 * AP_GetSettingsCfgFile(engine_name)
 *
 * Purpose
 * Reads in :attributes:configs/settings.cfg
 *
 * Parameters
 *
 * Return value
 * A config file reference.
 *
 */
function AP_GetSettingsCfgFile()
	var cfg := ReadConfigFile(":attributes:config/settings");

	if ( cfg.errortext )
		SysLog("Error::AP_GetSettingsCfgFile() - Unable to open [:attributes:config/settings.cfg] ->"+cfg.errortext);
	endif

	return cfg;
endfunction

/*
 * AP_GetSettingsCfgElem(elem_name, cfg_file)
 *
 * Purpose
 * Retrieves an elem from a config file. 
 *
 * Parameters
 * elem_name:	A string matching the elem name to be retrieved.
 * cfg_file:	Optional parameter - reference to a config already read in by AP_GetSettingsCfgFile()
 *
 * Return value
 * A config file elem reference.
 *
 */
function AP_GetSettingsCfgElem(elem_name, byref cfg_file:=0)
	if ( !cfg_file )
		cfg_file := AP_GetSettingsCfgFile();
	endif
	
	var elem := cfg_file[elem_name];

	if ( elem.errortext )
		SysLog("Error::AP_GetSettingsCfgElem() - Unable to find elem ["+elem_name+"] ->"+elem.errortext);
	endif

	return elem;
endfunction

