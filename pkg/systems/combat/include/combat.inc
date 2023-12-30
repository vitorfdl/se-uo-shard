
/**
 * This file contains functions related to combat.
 *    GetDamageType(attacker, wpn, wpn_elem)
 *    GetWeaponCFGInfo(mobile, wpn)
 *    PlayHurtSound(defender)
 *    PlayHitSounds(attacker, defender)
 *    PlayMissSound(attacker, wpn_elem)
 *    PlayAttackAnimation(attacker, weapon_elem)
 *    CheckifIsAlly(who, mobileOrMobileList)
 *    FilterMobileListByEnemies(who, mobileList)
 *    ConsumeAmmo(who, bow, qty)
 */

use uo;
include ":ghaia:ghaiaUtil";
include ":combat:weaponInfo";
include "include/newperformaction";
include "include/client";
include ":attributes:attributes";

/**
 * GetDamageType function
 * 
 * @param {Object} attacker - The attacker object
 * @param {number} wpn - The weapon id (optional if wpn_elem is present)
 * @param {number} wpn_elem - The weapon element id (optional if wpn is present)
 * 
 * @returns {Object} result_types - The result damage types
 */
function GetDamageType(attacker, wpn := 0, wpn_elem := 0)
   // Default damage type is bludgeoning
	var attack_types := struct{};
   attack_types[DMG_BLUDGE] := 100;

   wpn := wpn ? wpn : attacker.weapon;
   wpn_elem := wpn_elem ? wpn_elem : GetWeaponCFGInfo(attacker, wpn);

   var dmg_mods;

	if (typeof(wpn_elem) != "Struct")
		var weapon_type := GetConfigStringDictionary(wpn_elem, "DamageType");
		attack_types := weapon_type ? weapon_type : attack_types;
      dmg_mods := GetObjProperty(wpn, "DamageTypes");
   else
      // Usually NPC's
      var wpnDmgType := wpn_elem.DamageType;
      if (wpnDmgType)
         // Not sure why, but sometimes the damage type is an error for NPC's
         attack_types := struct{};
         attack_types[wpnDmgType] := 100;
      endif

      dmg_mods := GetObjProperty(attacker, "DamageTypes");
	endif

   if (!(typeof(attack_types) in array{"Struct", "Dictionary"}))
      // There was so many .cfg files with wrong damage types, so we need to check if it's a valid type before using it
      attack_types := struct{};
      attack_types[DMG_BLUDGE] := 100;
   endif

   // To capture modded damage types
   dmg_mods := typeof(dmg_mods) in array{"Struct", "Dictionary"} ? dmg_mods : struct{};

   var result_types := struct{};
   foreach key in (dmg_mods.keys() + attack_types.keys())
      result_types[key] := cint(dmg_mods[key] * 10) + cint(attack_types[key]);
      // sleepms(1);
   endforeach

	return result_types;
endfunction

/**
 * Get Weapon Damage function
 * @param {Object} mobile - The mobile object
 * @param {Object} wpn - The weapon object
 * @returns {Object} - Returns weapon configuration information
 */
function GetWeaponCFGInfo(mobile, wpn)
   // IF the mobile is a NPC, we need to get the weapon info from the NPC config
	if ( mobile.IsA(POLCLASS_NPC) )
		var to_return := GetNpcRandomAttack(mobile);
      if (wpn.objtype != 0x1F020) // Wrestling
         var wpn_elem := GetItemDescriptor(cint(wpn.objtype));
         wpn_elem.hitbonus     := to_return.hitbonus;
         wpn_elem.dicedmg      := to_return.dicedmg;
         wpn_elem.AttackDamage := to_return.dicedmg; // TODO: Remove this line when all NPC's are updated
         return wpn_elem;
      endif
      
      return to_return;
	endif
   
   // If the mobile is a player, we need to get the weapon info from the weapon itself
	var item_cfg2	:= ReadConfigFile(":*:itemdesc");
	return item_cfg2[wpn.objtype];
endfunction

function PlayHurtSound(defender)
	if ( defender.IsA(POLCLASS_NPC) )
      var damaged_sound := NPC_GetNPCConfig(defender).HurtSound;
		if ( damaged_sound && RandomInt(2) == 2)
			PlaySoundEffect(defender, damaged_sound);
		endif
	endif
endfunction
  
function PlayHitSounds(attacker, defender := 0)
	var wpn_a_elem2 := GetWeaponCFGInfo(attacker, attacker.weapon);
   var metamorfose := GetObjProperty(attacker, "metamorfose");


   if (metamorfose)
		var npcelem := NPC_GetNPCConfig(":ghaia:{}".format(metamorfose));
      PlaySoundEffect(attacker, npcelem.AttackSound);
	elseif ( wpn_a_elem2.HitSound )
		PlaySoundEffect(attacker, wpn_a_elem2.HitSound);
	else
		//NPC intrinsic weapon
		var npcelem := NPC_GetNPCConfig(attacker.npctemplate);
		PlaySoundEffect(attacker, npcelem.AttackSound);
	endif
	
	if (defender)
		var hurt_sound;
      var metamorfose_2 := GetObjProperty(defender, "metamorfose");
      if (metamorfose_2)
         hurt_sound := NPC_GetNPCConfig(":ghaia:{}".format(metamorfose_2)).HurtSound;
		elseif ( defender.npctemplate )
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

/**
 * This function plays the miss sound for a given attacker and weapon.
 * @param {Object} attacker - The entity performing the attack.
 * @param {Object} wpn_elem - The weapon used in the attack.
 */
function PlayMissSound(attacker, wpn_elem)
	if ( wpn_elem.MissSound )
		PlaySoundEffect(attacker, wpn_elem.MissSound);
      return;
	endif


   //NPC intrinsic weapon
   var npcelem := NPC_GetNPCConfig(attacker.npctemplate);
   if (npcelem.MissSound)
      PlaySoundEffect(attacker, npcelem.MissSound);
      return;
   endif


   var miss_sound := array{569, 570, 571}; // Generic miss sounds
   PlaySoundEffect(attacker, miss_sound.randomentry());
endfunction

/**
 * This function plays the attack animation for a given attacker and weapon.
 * @param {Object} attacker - The entity performing the attack.
 * @param {Object} weapon_elem - The weapon used in the attack.
 */
function PlayAttackAnimation(attacker, weapon_elem)
   var playerGraphics := array{400, 401, 605, 606, 666, 667, 744, 745};

	//If they're not human, use different attack animations based on their graphic
	if (!(attacker.graphic in playerGraphics))
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

	var anim := 0;

	//If the player is mounted, use special mount animations, if they're defined in
	if (GetEquipmentByLayer(attacker, LAYER_MOUNT) and (attacker.graphic != 666 and attacker.graphic != 667))
		anim := CINT(weapon_elem.MountedAnim);

		if(anim == 0x1D)
			anim := 0;
		endif
	else
	   //the weapon's itemdesc.cfg entry, or a default animation if not
		anim := weapon_elem.Anim;
	endif

	case (cint(anim)) // I have no idea why the anims are being remapped, but they are
		9: anim := 4;
		10: anim := 5;
		11: anim := 3;
		12: anim := 6;
		13: anim := 7;
		14: anim := 8;
		18: anim := 1;
		19: anim := 2;
		26: anim := 7;
	endcase

	NewPerformAction(attacker, 0, anim, RandomInt(4));
endfunction

/**
 * This function, CheckifIsAlly, is used to determine if a given set of mobiles are allies of a specific character.
 *
 * It takes two parameters: 'who', the character checking for allies, and 'mobiles', the list of mobiles to check.
 * The function first checks if the character is in a party and lists all hostiles around the character.
 * If the 'mobiles' parameter is not an array, the function checks if the mobile is hostile, has a master, or is aligned differently.
 * If the 'mobiles' parameter is an array, the function iterates through each mobile and checks if it is hostile, has a master, is in the same guild, or is aligned differently.
 * The function returns an array of allies.
 *
 * @param {Object} who - The character checking for allies
 * @param {Array} mobileOrMobileList - The list of mobiles to check
 * @returns {Array} - Returns an array of allies or 1/0 if the mobile is an ally
 */

function CheckifIsAlly(who, mobileOrMobileList)
   // TODO: Fazer duas versòes dessa funçao, uma para o who ser um mobile e outra para ser um array de mobiles
   var party := who.party.members;
   var hostiles := ListHostiles(who, 20, LH_FLAG_INCLUDE_HIDDEN);
   
   if (typeof(mobileOrMobileList) != "Array")
      if (mobileOrMobileList in hostiles)
         return 0;
      elseif (mobileOrMobileList.npctemplate)
         if (mobileOrMobileList.master && (mobileOrMobileList.master != who && mobileOrMobileList.master in party))
            return 0;
         elseif(mobileOrMobileList.alignment != 2 && (who.connected || who.alignment == 2))
            return 0;
         endif
      elseif (who.murderer)
         return 0;
      endif

      return 1;
   endif

   var newarray := array{};
   foreach mobile in mobileOrMobileList
      sleepms(2);
      if (mobile in hostiles)
          newarray.append(mobile);
      elseif (mobile.npctemplate)
         if (mobile.master == who || mobile.master in party)
            continue;
         elseif (GetObjProperty(mobile, "guildid") == who.guildid)
            continue;
         else
            if (who.npctemplate)
               if (mobile.alignment == who.alignment)
                  continue;
               endif
            elseif (mobile.alignment == 2)
               continue;
            endif
         endif
      elseif (mobile in party || mobile == who)
         continue;
      endif

      newarray.append(mobile);
   endforeach

   return newarray;
endfunction

/**
 * Filter the mobileList by mobile enemies only
 * @param {Object} who - The character checking for enemies
 * @param {Array} mobileList - The list of mobiles to check
 */
function FilterMobileListByEnemies(who, mobileList)
   var spotserial := GetObjProperty(who, "spawnspot");

   var hostiles := ListHostiles(who);
   var party := who.party.members;
   var newarray := array{};
   foreach mobile in mobileList
      sleepms(2);
      if (mobile in hostiles)
         continue;
      elseif (mobile.npctemplate)
         if (mobile.master == who || mobile.master in party)
            newarray.append(mobile);
         elseif (GetObjProperty(mobile, "guildid") == who.guildid)
            newarray.append(mobile);
         elseif ((spotserial && GetObjProperty(mobile, "spawnspot") == spotserial))
            newarray.append(mobile);
         else
            if (who.npctemplate)
               if (mobile.alignment == who.alignment)
                  newarray.append(mobile);
               endif
            elseif (mobile.alignment == 2)
               newarray.append(mobile);
            endif
         endif
      elseif (mobile in party || mobile == who)
         newarray.append(mobile);
      endif
   endforeach

   return newarray;
endfunction

/**
 * Check for available ammo for the bow and consume it
 *
 * @param {Object} who - The character checking for ammo
 * @param {Object} bow - The bow being checked for ammo
 * @param {number} [qty=1] - The quantity of ammo to check for
 * @returns {Object} - The type of ammo available for the bow
 */
function ConsumeAmmo(who, bow, qty := 1)
	var aljava := GetEquipmentByLayer(who, 0x14);
	var ammo := GetObjProperty(bow, "ammo");
	if (!ammo)
		SendSysMessageEx(who, "Você não definiu o tipo de flecha que vai usar. [clique 2x no arco fora do modo agressivo e na flecha ]");
		return 0;
	elseif ( ConsumeSubstance( aljava, ammo, qty ) == error)
		SendSysMessageEx(who, "Você não definiu o tipo de flecha que vai usar ou ela acabou.");
		return 0;
	endif

   return ammo;
endfunction