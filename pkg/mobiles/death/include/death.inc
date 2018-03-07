/*
 * $Id: death.inc 664 2005-10-27 03:36:13Z muaddiblsd $
 *
 */


use uo;
use os;
use util;

include ":ghaia:ghaiaUtil";
include ":attributes:attributes";
include ":timeUtils:time";
include ":fls_core:fls_characters";

/*
 * DP_PlayDeathSound(corpse, ghost:=0)
 *
 * Purpose
 * Plays a sound effect when something dies.
 *
 * Parameters
 * object:	Either a corpse or a ghost to play the sound on.
 *
 * Return value
 * Sound number that was played
 */
function DP_PlayDeathSound(object)
	var death_sound := 0;
	
	if ( object.IsA(POLCLASS_CORPSE) )
		var template := GetObjProperty(object, "npctemplate"); // Set by core
		if ( template )
			var cfg_info := NPC_GetNPCConfig(template);
			var sounds := GetConfigStringArray(cfg_info, "DeathSound");
			death_sound := CInt(sounds.randomentry());
		endif
	endif
	
	if ( !death_sound || checkGender(object) )
		// No sound selected yet.
		if ( checkGender(object) == 1 )
			 // Male ghost / corpse
			var sounds := array{347, 348, 349, 350};
			death_sound := sounds.randomentry();
		elseif ( checkGender(object) == 2 )
			 // Female ghost / corpse
			var sounds := array{337, 338, 339, 340};
			death_sound := sounds.randomentry();
		endif
	elseif ( death_sound )
		PlaySoundEffect(object, death_sound);
	endif
	
	return death_sound;
endfunction

function KataWakeUp(ghost, corpse)
	if (!ghost.dead)
		return 1;
	endif

	var serialC := ghost.serial;
	//TS_StartTimer(ghost, "mortedelay", 10);
	
	if (!corpse)
		corpse := SystemFindObjectBySerial(GetObjProperty(ghost, "corpo"));
	endif
	
	SetCooldown(ghost, "deathtime", 15);
	if ( !MoveObjectToLocation(ghost, corpse.x, corpse.y, corpse.z, corpse.realm, MOVEOBJECT_FORCELOCATION+MOVEITEM_IGNOREMOVABLE) )
		SendSysMessage(ghost, "Ocorreu algum problema ao retornar seu personagem para o corpo. Se algo estiver errado, chame um GM.");
      var last_loc := GetObjProperty(ghost, "death_loc");
      if (last_loc)
         MoveObjectToLocation(ghost, last_loc[1], last_loc[2], last_loc[3], ghost.realm, MOVEOBJECT_FORCELOCATION+MOVEITEM_IGNOREMOVABLE);
      else
         MoveObjectToLocation(ghost, 4032, 2581, 10, ghost.realm, MOVEOBJECT_FORCELOCATION+MOVEITEM_IGNOREMOVABLE);
      endif
	endif

	// var realname := GetObjProperty(ghost, "realname");
	SetName(ghost, ghost.name);	
	EraseObjProperty(ghost, "realname");
	EraseObjProperty(ghost, "corpo");
	EraseObjProperty(ghost, "curado");
	EraseObjProperty(corpse, "curado");

	Resurrect(ghost, RESURRECT_FORCELOCATION);
	DestroyItem(GetEquipmentByLayer(ghost, 0x16));

	foreach item in EnumerateItemsInContainer(corpse)
		if (item.container == corpse)
			MoveObjectToLocation(item, ghost.x, ghost.y, ghost.z, ghost.realm);
			MoveItemToContainer(item, ghost.backpack);
			if ( CInt(GetObjProperty(item, "#EquippedOn")) == cint(serialC)) 
            if (!EquipItem(ghost, item))
				   EraseObjProperty(item, "#EquippedOn");
            endif
			endif
		endif
	endforeach

	// if (TemHabilidade(ghost, "Instinto de Sobrevivência") )
	// 	SetVital(ghost, "hits", CInt(GetVitalMaximumValue(ghost, "hits")*0.9));
	// else
//		printtextabove(ghost, " " + GetHP(ghost));
	HealDamage(ghost, Floor(GetHP(ghost)*0.4));
	// endif
	SetVital(ghost, "stamina", CInt(GetVitalMaximumValue(ghost, "stamina")*0.5));
	SetVital(ghost, "mana", CInt(GetVitalMaximumValue(ghost, "mana")*0.5));
	RecalcVitals(ghost);

	DestroyItem(corpse);

	if (GetObjProperty(ghost, "corda"))
		PrintText(ghost, "*amarrado*");
		SendSysMessage(ghost, "Para tentar se soltar utilize o comando .soltar");
		var item := GetEquipmentByLayer(ghost, LAYER_HAND1);
		if (item)
			MoveItemToContainer(item, ghost.backpack);
		endif

		item := GetEquipmentByLayer(ghost, LAYER_HAND2);
		if (item)
			MoveItemToContainer(item, ghost.backpack);
		endif

		ghost.frozen := 1;
	endif
	//SetCoolDown(ghost, "deathtime", 30);

	var save := GetObjPRoperty(ghost, "savegame");
	if (save != error)
		MoveObjectToLocation(ghost, save.x, save.y, save.z, save.realm, MOVEOBJECT_FORCELOCATION+MOVEITEM_IGNOREMOVABLE);
		AP_SetVital(ghost, STAMINA, cint(save.stamina));
		AP_SetVital(ghost, MANA, cint(save.mana));
		AP_SetVital(ghost, HITS, cint(save.hits));
		eraseobjproperty(ghost, "savegame");
	endif
	
	// if (TS_GetCooldown(ghost, "metamorfosedruida"))
		// TS_LowerDuration(ghost, "metamorfosedruida", -1);
		// TS_LowerDuration(ghost, "velocidade", -1);
	// endif
endfunction
