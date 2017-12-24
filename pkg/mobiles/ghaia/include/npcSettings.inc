/*===============================================================
* Current Version
* NPCSETTINGS.INC - v1.0
* Updated 9/8/2005 5:21AM
*
* -- Revision v1.0 --
* Austin:
*  Created include file
===============================================================*/
use uo;
use os;
use cfgfile;

include ":ghaia:ghaiaUtil";

function InitializeSettings()
	
	g_settings:= NPC_LoadTemplateSettings(me);
	g_settings:= NPC_GetDefaultSettings(g_settings);
	g_scripts := NPC_LoadScriptSettings(me);
		
	return 1;
endfunction

/*
 * NPC_LoadTemplateSettings(source)
 *
 * Purpose
 * Loads script settings for a template from the appropriate npcdesc.cfg.
 * Uses AISetting lines.
 *
 * Parameters
 * source:	Name of the npc template to load settings for.
 *		If an NPC reference is used it will also load custom settings set on it.
 *
 * Return value
 * Returns a dictionary
 *
 */
function NPC_LoadTemplateSettings(source)
	//
	// * Load config defaults first
	//
	var npc_elem := NPC_GetNPCConfig(source);

	var setting_list := GetConfigStringDictionary(npc_elem, "ghaiaSettings");
	
	var settings := dictionary;
	foreach setting_name in (setting_list.keys())
		settings[setting_name] := UnPack(setting_list[setting_name]);
		
		sleepms(2);
	endforeach

	//
	// * If the source is an npc, check it for custom settings.
	//
	if ( source.IsA(POLCLASS_NPC) )
		var cstm_settings := GetObjProperty(source, "ghaiaSettings");
		foreach key in (cstm_settings.keys())
			if ( cstm_settings[key] == error )
				settings.Erase(key);
			else
				settings[key] := cstm_settings[key];
			endif
				
			sleepms(2);
		endforeach
	endif
	
	return settings;
endfunction

/*
 * NPC_LoadScriptSettings(source)
 *
 * Purpose
 * Loads brain AI script settings for an NPC. Gives .script and .flag values for them.
 * Uses AIScript and AIScriptFlag entries.
 *
 * Parameters
 * source:	Name of the npc template to load settings for.
 *		Can also be an npc reference - if custom settings should be loaded off it.
 *
 * Return value
 * Returns a dictionary
 *
 */
function NPC_LoadScriptSettings(source)
	//
	// * Load config defaults first
	//
	var npc_elem := NPC_GetNPCConfig(source);
	
	var script_list := GetConfigStringDictionary(npc_elem, "ghaiaScript");

	//printtextabove(npc, "npc_elem " + npc_elem + " source " + source);

	var script_settings := dictionary;
	foreach script_type in (script_list.keys())
		script_settings[script_type] := script_list[script_type];
		sleepms(2);
	endforeach

	//
	// * If the source is an npc, check it for custom script settings.
	//
	if ( source.IsA(POLCLASS_NPC) )
		var cstm_scripts := GetObjProperty(source, "ghaiaScript");
		foreach key in (cstm_scripts.keys())
			script_settings[key] := cstm_scripts[key];
			sleepms(2);
		endforeach
	endif
	
	return script_settings;
endfunction

/*
 * NPC_ErrorSysLog(errmsg)
 *
 * Purpose
 * Logs an error message.
 *
 * Parameters
 * errmsg:	Error message
 *
 * Return value
 * Always returns 1
 *
 */
function NPC_ErrorSysLog(mobile, errmsg)
	Set_Critical(1);
	SysLog("NPC Template: " + mobile.npctemplate);
	SysLog("NPC Name    : " + mobile.name);
	SysLog(errmsg);
	SysLog("------------");
	Set_Critical(0);
	
	return 1;
endfunction
