
use uo;
include ":ghaia:ghaiaUtil";
include ":combat:weaponInfo";
include "include/newperformaction";
include "include/client";
include ":attributes:attributes";

const IS_REMOVE := 1;

function GetDamageType(weapon)
	var attack_type := DMG_PHYSICAL;
	if (weapon.objtype)
		var weapon_type := GetConfigString(item_cfg[weapon.objtype], "DamageType");
		if (weapon_type)
			attack_type := weapon_type;
		endif
	elseif (weapon.DamageType)
		attack_type := weapon.DamageType;
	endif

	return attack_type;
endfunction

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

function GetItemCFGInfo(mobile, wpn)
	if ( mobile.IsA(POLCLASS_NPC) )
		var to_return := GetNpcRandomAttack(mobile);
      if (wpn.objtype != 0x1F020)
         var wpn_elem := GetItemDescriptor(cint(wpn.objtype));
         wpn_elem.hitbonus := to_return.hitbonus;
         wpn_elem.dicedmg := to_return.dicedmg;
         return wpn_elem;
      endif
      return to_return;
	endif
   
	var item_cfg2	:= ReadConfigFile(":*:itemdesc");
	return item_cfg2[wpn.objtype];
endfunction

function GetNPCattack(mobile)
	
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
	var wpn_a_elem2 := GetItemCFGInfo(attacker, attacker.weapon);
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

function PlayMissSound(attacker, wpn_elem)
	if ( wpn_elem.MissSound )
		PlaySoundEffect(attacker, wpn_elem.MissSound);
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
	// var anim := weapon_elem.Anim;
	// if (anim)
	// 	PerformAction(attacker, anim, 7);
	// 	return;
	// endif
//	PerformAction (attacker, 0x09, 7);
	case (cint(anim))
		9: anim := 4;
		10: anim := 5;
		11: anim := 3;
		12: anim := 6;
		13: anim := 7;
		14: anim := 8;
		18: anim := 1;
		19: anim := 2;
		26: anim := 7;
		default: anim := anim;
	endcase
	// PerformAction(attacker, anim, 5);
	NewPerformAction(attacker, 0, anim, RandomInt(4));
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

function SetArmorHitscript(who, hitscript, remove := 0);
   var hitscripts := GetObjProperty(who, "ArmorHitScripts");
   if (!hitscripts)
      hitscripts := array{};
   endif

   if (!remove)
      if (!FindInArray(hitscripts, hitscript))
         hitscripts.append(hitscript);
      endif
   else
      hitscripts := RemoveFromArray(hitscripts, hitscript);
   endif

   if (hitscripts.size() < 1)
      return EraseObjProperty(who, "ArmorHitScripts");
   endif
   SetObjProperty(who, "ArmorHitScripts", hitscripts);
endfunction

function SetHitscript(who, hitscript, remove := 0);
   var hitscripts := GetObjProperty(who, "HitScripts");
   if (!hitscripts)
      hitscripts := array{};
   endif

   if (!remove)
      if (!FindInArray(hitscripts, hitscript))
         hitscripts.append(hitscript);
      endif
   else
      hitscripts := RemoveFromArray(hitscripts, hitscript);
   endif

   if (hitscripts.size() < 1)
      return EraseObjProperty(who, "HitScripts");
   endif
   SetObjProperty(who, "HitScripts", hitscripts);
endfunction

function SetCriticalHitscript(who, hitscript, remove := 0);
   var criticalhitscript := GetObjProperty(who, "CriticalHitScripts");
   if (!criticalhitscript)
      criticalhitscript := array{};
   endif

   if (!remove)
      if (!FindInArray(criticalhitscript, hitscript))
         criticalhitscript.append(hitscript);
      endif
   else
      criticalhitscript := RemoveFromArray(criticalhitscript, hitscript);
   endif

   if (criticalhitscript.size() < 1)
      return EraseObjProperty(who, "CriticalHitScripts");
   endif
   SetObjProperty(who, "CriticalHitScripts", criticalhitscript);
endfunction

function CheckifIsAlly(who, mobiles)
   var party := who.party.members;
   var hostiles := ListHostiles(who);
   if (typeof(mobiles) != "Array")
      if (mobiles in hostiles)
         return 0;
      elseif (mobiles.npctemplate)
         if (mobiles.master && (mobiles.master != who && mobiles.master in party))
            return 0;
         elseif(mobiles.alignment != 2 || who.murderer)
            return 0;
         endif
      elseif (who.murderer)
         return 0;
      endif

      return 1;
   endif

   var newarray := array{};
   foreach mobile in mobiles
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

function CheckifIsNotEnemy(who, mobiles)
   var spotserial := GetObjProperty(me, "spawnspot");
   var hostiles := ListHostiles(who);
   var party := who.party.members;
   var newarray := array{};
   foreach mobile in mobiles
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

function CheckForAmmo(who, bow, qty := 1)
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