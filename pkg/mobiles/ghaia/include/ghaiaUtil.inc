include "include/sysEvent";
include ":taming:taming";
include "include/say";

function ghaiaRessurect(targ, notame := 0)
	var npc := CreateNpcFromTemplate( GetObjProperty(targ, "npctemplate"), targ.x, targ.y, targ.z, 0, targ.realm);
	npc.script := ":ghaia:follower";
	RestartScript(npc);
	CopyProps(targ, npc);
	var ownerserial := GetObjProperty(targ, "owner");
	var owner := SystemFindObjectBySerial(ownerserial);
	if (!owner)
		destama(npc.serial);
	else
		if ( (GetNumPets(owner) + GetPetCost(npc)) > GetMaxPets(owner) )
			SendSysMessageEx(owner, "Você ja tem muitos animais.", SSM_FAIL);
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
	ev.type     := EVID_DOOBJECTIVE;
	ev.destiny  := destiny;
	ev.move_type:= move_type;
	// ev.move_direction:=
	
	sendEvent( npc, ev);
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
	if (template.isA(POLCLASS_MOBILE))
		template := struct{ "package" := template.package(), "my_template" := template.my_template()};
	else
		var splited := SplitWords(template, ":");
		template := struct{ "package" := splited[2], "my_template" := splited[3]};
	endif

	var npc_cfg := ReadConfigFile(":"+template.package+":npcdesc");
	if ( npc_cfg.errortext )
		return error{"errortext":="Error::NPC_GetNPCConfig() - Could not open :"+template.package+":npcdesc ->"+npc_cfg.errortext};
	endif

	var cfg_elem := npc_cfg[template.my_template];
	if ( cfg_elem.errortext )
		return error{"errortext":="Error::NPC_GetNPCConfig() - Could not find elem ["+template.my_template+"] ->"+cfg_elem.errortext};
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
	ev.+type     := EVID_SPEAK;
	ev.+text     := text;
	ev.+texttype := text_type;
	ev.+doevent  := do_event;
	ev.+source   := npc;
	ev.+process  := GetProcess(GetPid());

	return SendEvent(npc, ev);
endfunction

function GetNpcAttacks(npctemplate)
	npctemplate := NPC_GetNPCConfig(npctemplate);
	var attacks := GetConfigStringArray(npctemplate, "Attack");
	if (!attacks || !attacks[1])
		return array{};
	endif

	var result := array{};
	foreach attack in attacks
		attack := SplitWords(attack, " ");

		var attack_s := struct{
			"name"    := attack[1],
			"dicedmg"    := attack[2],
			"hitbonus"   := cint(attack[3]),
			"DamageType" := attack[4],
			"AttackAnim" := cint(attack[5]),
			"Attribute"  := npctemplate.AttackAttribute,
			"MaxRange"   := npctemplate.MaxRange,
			"MinRange"   := npctemplate.MinRange
		};
		result.append(attack_s);
		sleepms(2);
	endforeach

	return result;
endfunction

function GetNpcRandomAttack(mobile)
	var attacks := GetObjProperty(mobile, "customAttacks");
	if (!attacks)
		attacks := GetNpcAttacks(mobile.npctemplate);
	endif
	
	if (!attacks || !attacks[1])
		return 0;
	endif

	var result := attacks.randomEntry();

	return result;
endfunction
