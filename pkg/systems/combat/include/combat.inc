
use uo;
include ":ghaia:ghaiaUtil";
include ":combat:weaponInfo";
include "include/newperformaction";
include "include/client";

function DistanceChecks(attacker, defender)
	var cur_range := Distance(attacker, defender);
	var wpn_a_elem2 := GetItemCFGInfo(attacker, attacker.weapon);
	if ( wpn_a_elem2.MaxRange == error )
		return 1;
	endif
	if ( cur_range > wpn_a_elem2.MaxRange )
		// CombatDbgMsg(attacker, "Oponente esta muito longe.");
		return 0;
	elseif ( cur_range < wpn_a_elem2.MinRange )
		// CombatDbgMsg(attacker, "Oponente esta muito perto.");
		return 0;
	else
		return 1;
	endif
endfunction

function GetItemCFGInfo(mobile, weapon)

	var item_cfg2	:= ReadConfigFile(":*:itemdesc");

	if ( mobile.IsA(POLCLASS_NPC) && weapon.objtype == 0x1F020 )
		return GetNpcRandomAttack(mobile);
	else
		return item_cfg2[weapon.objtype];
	endif

endfunction

function GetNPCattack(mobile)
	
endfunction
  
function PlayHitSounds(attacker, defender := 0)
	var wpn_a_elem2 := GetItemCFGInfo(attacker, attacker.weapon);

	if ( wpn_a_elem2.HitSound )
		PlaySoundEffect(attacker, wpn_a_elem2.HitSound);
	else
		//NPC intrinsic weapon
		var npcelem := NPC_GetNPCConfig(attacker.npctemplate);
		PlaySoundEffect(attacker, npcelem.AttackSound);
	endif
	
	if (defender)
		var hurt_sound;
		if ( defender.npctemplate )
			var npcelem := NPC_GetNPCConfig(defender.npctemplate);
			hurt_sound  := npcelem.HurtSound;		
		else
			var sounds;
			if ( defender.gender == 0 )
				// Male
				sounds := array(341, 342, 343, 345, 346);
			else
				// Female
				sounds := array(332, 333, 334, 335, 336);
			endif
			hurt_sound := sounds.randomentry();
		endif
		
		PlaySoundEffect(defender, hurt_sound);
	endif
endfunction

function PlayMissSound(attacker)
	if ( wpn_a_elem.MissSound )
		PlaySoundEffect(attacker, wpn_a_elem.MissSound);
	else
		//NPC intrinsic weapon
		var npcelem := NPC_GetNPCConfig(attacker.npctemplate);
		if (!npcelem.MissSound)
			var miss_sound := array{569, 570, 571};
			PlaySoundEffect(attacker, miss_sound.randomentry());
		else
			PlaySoundEffect(attacker, npcelem.MissSound);
		endif
	endif
endfunction

function PlayAttackAnimation(attacker, weapon_elem)
	//If they're not human, use different attack animations based on their graphic
	if (attacker.graphic != 400 and attacker.graphic != 401 and attacker.graphic != 605 and attacker.graphic != 606 and attacker.graphic != 666 and attacker.graphic != 667 and attacker.graphic != 744 and attacker.graphic != 745)
		case (attacker.graphic)
			CID_ALLIGATOR:
			CID_PIG:
			CID_BROWN_BEAR:
			CID_GRIZZLY_BEAR:
			CID_POLAR_BEAR:
			CID_BOAR:
			CID_BUNNY:
			CID_CAT:
			CID_DOG:
			CID_WOLF:
			CID_COW:
			CID_COW2:
			CID_BULL:
			CID_BULL2:
			CID_CHICKEN:
			CID_GOAT:
			CID_RAT:
			CID_GIANT_RAT:
			CID_PANTHER:
				PerformAction(attacker, (RandomInt (3) + 5));
			CID_EAGLE:
				PerformAction(attacker, 4);
			default:
				if (weapon_elem.AttackAnim)
					PerformAction(attacker, weapon_elem.AttackAnim);
				else
					PerformAction(attacker, (RandomInt (3) + 4));
				endif
		endcase
	
		return;
	endif
	var anim;
	//If the player is mounted, use special mount animations, if they're defined in
	//the weapon's itemdesc.cfg entry, or a default animation if not
	if (GetEquipmentByLayer (attacker, LAYER_MOUNT) and (attacker.graphic != 666 and attacker.graphic != 667))
		anim := CINT(weapon_elem.MountedAnim);
//		if (anim)
//			PerformAction (attacker, anim);
//			return;
//		endif
//		PerformAction (attacker, 0x1A);
//		return;
		if(anim == 0x1D)
			anim := 0;
		endif
	else
		anim := weapon_elem.Anim;
	endif
	
//	//Otherwise, if they're not mounted, use the animation defined in the itemdesc.cfg file
//	//or a default animation if one isn't defined
//	var anim := weapon_elem.Anim;
	// if (anim)
	// 	PerformAction(attacker, anim, 7);
	// 	return;
	// endif
//	PerformAction (attacker, 0x09, 7);
	var newanim := 0;
	case (Hex(anim))
		0x09:  newanim := 4;
		0x0A: newanim := 5;
		0x0B: newanim := 3;
		0x0C: newanim := 6;
		0x0D: newanim := 7;
		0x0E: newanim := 8;
		0x12: newanim := 1;
		0x13: newanim := 2;
		0x1A: newanim := 7;
		default: newanim := anim;
	endcase
	// OldPerformAction(attacker, anim, 5);
	NewPerformAction(attacker, 0, newanim, RandomInt(4));
	return;
endfunction
  
function GetRawDmg(attacker)
  
    if (attacker.npctemplate)
      var npcelem := NPC_GetNPCConfig(attacker.npctemplate);
      var dmg := GetConfigString(npcelem, "AttackDamage");
      return RandomDiceRoll(dmg);
    else
      var dmg := CS_GetWeaponInfo(attacker.weapon, "Damage");
      return RandomDiceRoll(dmg);
    endif
  
endfunction

function debugMSG(who, text)
	if (GetObjProperty(who, "debug"))
		SendSysMessageEx(who, text);	
	endif
endfunction
