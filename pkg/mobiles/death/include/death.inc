/*
 * $Id: death.inc 664 2005-10-27 03:36:13Z muaddiblsd $
 *
 */


use uo;
use os;
use util;

include ":ghaia:ghaiaUtil";
include ":attributes:attributes";
include ":timeutils:time";

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
			death_sound := CInt(sounds[RandomInt(sounds.Size())+1]);
		endif
	endif
	if ( !death_sound || object.corpsetype == 401 )
		// No sound selected yet.
		if ( object.graphic == 402 || object.corpsetype == 400 )
			 // Male ghost / corpse
			var sounds := array{347, 348, 349, 350};
			death_sound := sounds[RandomInt(sounds.Size())+1];
		elseif ( (GetObjProperty(object, "female") == 1) || object.graphic == 401 ||  object.graphic == 403 || object.corpsetype == 401 )
			 // Female ghost / corpse
			var sounds := array{337, 338, 339, 340};
			death_sound := sounds[RandomInt(sounds.Size())+1];
		endif
	endif

	if ( death_sound )
		PlaySoundEffect(object, death_sound);
	endif
	
	return death_sound;
endfunction

/*check the deathpoints. if its 0 the player is dead and return 0 else return 1 */

function CheckDeathPoints(who)

	if (!who.npctemplate)
		//if (who.cmdlevel < 1)
			
			var deathpoints := RegenerateDeathPoints(who);
			
			deathpoints += 1;

			if( deathpoints >= 4 )
				SendSysMessage(who, "Voce ja desmaiou muitas vezes. Seus Stats foram penalizados.");
				AP_ModifyStatMod(who, DEXTERITY, -10);
				AP_ModifyStatMod(who, STRENGTH, -10);
				AP_ModifyStatMod(who, INTELLIGENCE, -10);
			endif

			SetObjProperty(who, "deathpoints", deathpoints);
			SetObjProperty(who, "LastFaint", polcore().systime);
			RecalcVitals(who);
			return 1;

		//endif
	endif

	return 0;

endfunction

function GetNPCDeathpointsDmg(template)

	var npcelem := NPC_GetNPCConfig(template);

	var DeathPoints := npcelem.DeathPoints;

	if (!DeathPoints)
		return 0;
	endif

	return DeathPoints;

endfunction

function RegenerateDeathPoints(who)

	var deathpoints := GetObjProperty(who, "deathpoints");
	if (deathpoints == error)
		deathpoints := 0;
	else
		var ratio := CInt( (GetTime() - GetObjProperty(who, "LastFaint"))/SEC_HOUR );
		if(  ratio > 0 )

			//if( deathpoints > 3 )
			//	RegenDpStats(who, ratio);
			//endif

			var deathPointsToHeal = deathpoints - 3; //calcula quanto de stat tem q regenerar
			if (deathPointsToHeal > 0)
				RegenDpStats(who, deathPointsToHeal);
			endif


			if (deathpoints > 0)
				deathpoints -= ratio;
			endif
			if( deathpoints < 0 )
				deathpoints := 0;
			endif

			SetObjProperty(who, "LastFaint", GetTime());
		endif
	endif

	SetObjProperty(who, "deathpoints", deathpoints);

	return deathpoints;
	
endfunction

function RegenDpStats(who, ratio)
	var bonus := ratio*10;

	AP_ModifyStatMod(who, INTELLIGENCE, bonus);
	AP_ModifyStatMod(who, DEXTERITY, bonus);
	AP_ModifyStatMod(who, STRENGTH, bonus);


	/*if( bonus > CInt(AP_GetStatMod(who, DEXTERITY)*(-1)) )
		bonus := CInt(AP_GetStatMod(who, DEXTERITY)*(-1));
	endif
	AP_ModifyStatMod(who, DEXTERITY, bonus);

	bonus := ratio*10;
	if( bonus > CInt(AP_GetStatMod(who, STRENGTH)*(-1)) )
		bonus := CInt(AP_GetStatMod(who, STRENGTH)*(-1));
	endif
	AP_ModifyStatMod(who, STRENGTH, bonus);

	bonus := ratio*10;
	if( bonus > CInt(AP_GetStatMod(who, INTELLIGENCE)*(-1)) )
		bonus := CInt(AP_GetStatMod(who, INTELLIGENCE)*(-1));
	endif
	AP_ModifyStatMod(who, INTELLIGENCE, bonus);*/
endfunction


function KataWakeUp(ghost, corpse)
	if (!ghost.dead)
		return 1;
	endif
	var serialC := ghost.serial;
	//TS_StartTimer(ghost, "mortedelay", 10);
	
	if (!corpse)
		corpse := cint(SystemFindObjectBySerial(GetObjProperty(ghost, "corpo")));
	endif
	
	setcooldown(ghost, "deathtime", 15);
	if ( !MoveObjectToLocation(ghost, corpse.x, corpse.y, corpse.z, corpse.realm, MOVEOBJECT_FORCELOCATION+MOVEITEM_IGNOREMOVABLE) )
		SendSysMessage(ghost, "Ocorreu algum problema ao retornar seu personagem para o corpo. Solicite ajude da Staff");
	endif

	var realname := GetObjProperty(ghost, "realname");
	SetName(ghost, realname);	
	EraseObjProperty(ghost, "realname");
	EraseObjProperty(ghost, "corpo");
	
	Resurrect(ghost, RESURRECT_FORCELOCATION);
	var ressrobe := GetEquipmentByLayer(ghost, 0x16);
	DestroyItem(ressrobe);

	foreach item in EnumerateItemsInContainer(corpse)
		if (item.container == corpse)
			MoveObjectToLocation(item, ghost.x, ghost.y, ghost.z, ghost.realm);
			MoveItemToContainer(item, ghost.backpack);
			if ( CInt(GetObjProperty(item, "#EquippedOn")) == serialC )
				if ( !EquipItem(ghost, item) )
					EraseObjProperty(item, "#EquippedOn");
				endif
			endif
		endif
	endforeach

	if (TemHabilidade(ghost, "Instinto de Sobrevivência") )
		SetVital(ghost, "hits", CInt(GetVitalMaximumValue(ghost, "hits")*0.9));
	else
//		printtextabove(ghost, " " + GetHP(ghost));
		HealDamage(ghost, GetHP(ghost)+1);
	endif
	SetVital(ghost, "stamina", CInt(GetVitalMaximumValue(ghost, "stamina")*0.5));
	SetVital(ghost, "mana", CInt(GetVitalMaximumValue(ghost, "mana")*0.5));
	RecalcVitals(ghost);

	DestroyItem(corpse);

	if (GetObjProperty(ghost, "corda"))
		PrintText(ghost, "*amarrado*");
		SendSysMessage(ghost, "Para tentar se soltar utilize o comando .soltar");
		var item;
		item := GetEquipmentByLayer(ghost, LAYER_HAND1);
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
	
	if (TS_GetCooldown(ghost, "metamorfosedruida"))
		TS_LowerDuration(ghost, "metamorfosedruida", -1);
		TS_LowerDuration(ghost, "velocidade", -1);
	endif
	
endfunction
