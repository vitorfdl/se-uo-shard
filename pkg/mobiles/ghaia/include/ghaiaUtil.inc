include "include/sysEvent";
include ":taming:taming";
include "include/say";

function ghaiaRessurect(targ, notame := 0)
	var npc := CreateNpcFromTemplate( GetObjProperty(targ, "npctemplate"), targ.x, targ.y, targ.z, 0, targ.realm);
	npc.script := ":ghaia:tamed";
	RestartScript(npc);
	CopyProps(targ, npc);
	var ownerserial := GetObjProperty(targ, "owner");
	var owner := SystemFindObjectBySerial(ownerserial);
	if (!owner)
		destama(npc.serial);
	else
		if ( (GetNumPets(owner) + GetPetCost(npc)) > GetMaxPets(owner) )
			SendSysMessageEx(owner, "Voce ja tem muitos animais.", SSM_FAIL);
			destama(npc.serial);
		else
			AddPet(owner, npc.serial);
		endif
	endif
	DestroyItem(targ);
	SetCooldown(npc, "resurrect", 180);
	var skills := GetObjProperty(npc, "petskills");
	foreach skill in (skills.keys())
		AP_SetTrueSkill(npc, skill, skills[skill]);
	endforeach
endfunction

function ghaiaAttack(who, npc)
	var event 		:= 	struct;
	event.type		:=	EVID_TAUNT;
	event.source	:=	who;

	SendEvent(npc, event);
endfunction

function ghaiaPeaceMade(npc)
	var ev 		:= struct;
	ev.type 	:= EVID_PEACEMADE; // ajetra isso
	ev.source 	:= who;
	
	sendEvent( npc, ev);
endfunction

function CopyProps(origin, destiny)
	var propnames := GetObjPropertyNames(origin);
	foreach prop in propnames
		SetObjPRoperty(destiny, prop, GetObjProperty(origin, prop));
	endforeach
endfunction

function ghaiaObjective(npc, destiny, move_type)
	var ev      := struct;
	ev.type     := EVID_DOOBJECTIVE; // ajetra isso
	ev.destiny  := destiny;
	ev.move_type:= move_type;
	// ev.move_direction:=
	
	sendEvent( npc, ev);
endfunction


/*
 * NPC_ParseTemplateName(template_name)
 *
 * Purpose
 * Finds the appropriate npcdesc.cfg to load.
 * This enables npcs to be placed in packages and not just the ::npcdesc file.
 * ReadConfigFile(":*:npcdesc") does not work in this case because packaged npcs
 * have the package name at the begining of their .npctemplate member. Reading the
 * specific config file - helps to avoid possible elem name collisions.
 *
 * Parameters
 * template_name:	Name of the npc template to parse
 *
 * Return value
 * Returns struct .package .template
 *
 */
function NPC_ParseTemplateName(template_name)
	if ( template_name.IsA(POLCLASS_NPC) )
		template_name := template_name.npctemplate;
	endif

	//Find positions of ":" characters
	var colon_a := Find(template_name, ":", 1);
	var colon_b := Find(template_name, ":", 2);

	var parsed := struct{"package", "template"};

	if ( !colon_a || !colon_b )
		//Not a complete package name - use default npcdesc.cfg
		parsed.package := "";
		parsed.template := template_name;
	else
		parsed.package := template_name[colon_a+1, colon_b-2];
		parsed.template := template_name[colon_b+1, Len(template_name)];
	endif

	return parsed;
endfunction

/*
 * NPC_GetNPCConfig(template)
 *
 * Purpose
 * Retrieves a config elem for an npc template.
 *
 * Parameters
 * info:	Template info from NPC_ParseTemplateName()
 *
 * Return value
 * Config elem reference
 *
 */
function NPC_GetNPCConfig(template)
	if ( !template.package )
		template := NPC_ParseTemplateName(template);
	endif

	var npc_cfg := ReadConfigFile(":"+template.package+":npcdesc");
	if ( npc_cfg.errortext )
		return error{"errortext":="Error::NPC_GetNPCConfig() - Could not open :"+template.package+":npcdesc ->"+npc_cfg.errortext};
	endif

	var cfg_elem := npc_cfg[template.template];
	if ( cfg_elem.errortext )
		return error{"errortext":="Error::NPC_GetNPCConfig() - Could not find elem ["+template.template+"] ->"+cfg_elem.errortext};
	endif

	return cfg_elem;
endfunction

/*
 * NPC_GetAllTemplateNames()
 *
 * Purpose
 * Retrieves a list of all npc templates in the shard's npcdesc.cfg files
 *
 * Parameters
 *
 * Return value
 * Returns an array
 *
 */
function NPC_GetAllTemplateNames()
	var elem_names := array{};
	elem_names := elem_names + GetConfigStringKeys(ReadConfigFile("::npcdesc"));
	foreach package in (Packages())
		if ( package.npcdesc )
			var pkg_name := ":"+package.name+":";
			foreach template_name in (GetConfigStringKeys(ReadConfigFile(pkg_name+"npcdesc")))
				elem_names.Append(pkg_name+template_name);
				sleepms(2);
			endforeach
		endif
		sleepms(2);
	endforeach

	return elem_names;
endfunction
/*
 * NPC_DefaultSettings(settings:=0)
 *
 * Purpose
 * Checks the settings dictionary to make sure required values are set.
 * If a dictionary is passed, it will check it for required parameters.
 * If they are not present, they will be set.
 *
 * Parameters
 * settings:	Optional parameter
 *		Dictionary read in from NPC_GetDefaultSettings()
 *
 * Return value
 * Returns a dictionary
 *
 */
function NPC_GetDefaultSettings(settings:=0)
	if ( Lower(TypeOf(settings)) != "dictionary" )
		settings := dictionary;
	endif

	if ( CInt(settings.exists("CycleWait")) <= 0 )
		settings["CycleWait"] := 10;
	endif
	
	if ( !settings.Exists("NoSleep") )
		if ( CInt(settings.exists("SleepWait")) <= 0 )
			settings["SleepWait"] := 1024;
		endif
		if ( CInt(settings.exists("IdleTicks")) <= 0 )
			settings["IdleTicks"] := 5;
		endif
	endif

	return settings;
endfunction

function AI_Speak(npc, text, text_type:=SPEAK_TEXTTYPE_DEFAULT, do_event:=SPEAK_DOEVENT_DISABLE)
	var ev      := struct;
	ev.+type    := SYSEVENT_SPEECH;
	ev.+text    := text;
	ev.+texttype:= text_type;
	ev.+doevent := do_event;
	ev.+process := GetProcess(GetPid());

	return SendEvent(npc, ev);
endfunction

function GetNpcRandomAttack(mobile)
	var npctemplate := NPC_GetNPCConfig(mobile.npctemplate);
	var attack     := GetConfigStringArray(npctemplate, "Attack");
	if (!attack || !attack[1])
		return 0;
	endif

	attack := SplitWords(attack.randomEntry(), " ");

	var result := struct{
		"dicedmg" := attack[2],
		"hitbonus" := cint(attack[3]),
		"DamageType" := attack[4],
		"AttackAnim" := cint(attack[5]),
		"Attribute" := npctemplate.AttackAttribute,
		"MaxRange" := npctemplate.MaxRange,
		"MinRange" := npctemplate.MinRange
	};
	return result;
endfunction