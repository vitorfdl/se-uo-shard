use uo;
include ":timedscripts:timedScripts";
include "include/arrays";
include "include/damage";

function ListTamablePetsNearLocation(x, y, z, range, realm := _DEFAULT_REALM)
	var cfg := ReadConfigFile(":ghaia:npcdesc");

	var mobs := array;
	foreach mob in ListMobilesNearLocationEx(x, y, z, range, LISTEX_FLAG_NORMAL|LISTEX_FLAG_NPC_ONLY, realm)
		if (mob.isTamable())
			mobs.append(mob);
		endif
	endforeach
	return mobs;
endfunction

//ve quantos slots custa o npc
function GetPetCost(pet)
	var cfg := ReadConfigFile(":ghaia:npcdesc");
	var npctemplate := pet.my_template();
	cfg := cfg[npctemplate];
	if (cfg.petslot == error)
		return 1;
	else
		return Cint(cfg.petslot);
	endif
endfunction

/* funcao pega o custo dos pets. tbm atualiza os pets perdidos ou que foram transferidos*/
function GetNumPets(who)
	var num_tamed := 0;
   var max_followers := GetMaxPets(who);

	var tamed := GetObjProperty(who, "followers");
	if ( !tamed )
		return 0;
	endif

   var remove_indexes := array{};
	var cfg := ReadConfigFile(":ghaia:npcdesc");
	foreach serial in tamed
		var pet := SystemFindObjectBySerial(serial);
		if ( pet.master == who )
			var npctemplate := pet.my_template();
			var pet_cfg := cfg[npctemplate];

         var pet_num := 1;
			if (pet_cfg.petslot)
				pet_num := Cint(pet_cfg.petslot);
			endif

         if (num_tamed+pet_num > max_followers)
            destama(serial);
            remove_indexes.append(_serial_iter);
         else
            num_tamed += pet_num;
         endif
		elseif ( !pet || pet.master !=who )
			remove_indexes.append(_serial_iter);
		endif
		sleepms(2);
	endforeach

   removeIndexes(tamed, remove_indexes);
	
	SetObjProperty(who, "followers", tamed);
	who.followers := num_tamed;
	return num_tamed;
endfunction

//deleta o serial da lista de seriais do char (quando o npc morre ou eh transferido)
function ErasePet(who, myserial)
	var tamed := GetObjProperty(who, "followers");
	if ( !tamed )
		return;
	endif
	
	foreach serial in tamed
		if ( myserial == serial )
			MP_Unmount(who);
			tamed.erase(_serial_iter);
			var pet := SystemFindObjectBySerial(serial);
         var custom_cost := GetObjProperty(pet, "pet_cost");
         if (!custom_cost)
			   who.followers -= GetPetCost(pet);
         else
            who.followers -= custom_cost;
         endif
			break;
		endif
		sleepms(2);
	endforeach

   SetObjProperty(who, "followers", tamed);
endfunction

//adiciona o serial do pet 
function AddPet(who, serial, pet_cost := 0)
	var tamed := GetObjProperty(who, "followers");
	if ( !tamed )
		tamed := array;
	endif
   
	tamed.append(serial);
	SetObjProperty(who, "followers", tamed);

	var pet := SystemFindObjectBySerial(serial);
   if (!pet_cost)
	   who.followers := who.followers + GetPetCost(pet);
   else
      who.followers := who.followers + pet_cost;
   endif
	pet.InitPetProperties(0);
endfunction

//numero maximo de pets que alguem pode ter
function GetMaxPets(who)
	// var skill := AP_GetStat(who, WISDOM);
	var maxpets := 1 + cint( GetObjProperty(who, "extrapets") ) + cint( getProficiencyBonus(who, TRATO_ANIMAIS) / 2 );

   if (HaveFeat(who, "Treinador de Animais") || HaveFeat(who, "Cadeia de Comando") )
	   maxpets += 1;
   endif

	return maxpets;
endfunction

//desloga os pets, teleportando eles pra um lugar l
function LogoutPets(who)
	var tamed := GetObjProperty(who, "followers");
	if ( !tamed )
		return;
	endif
	
	foreach serial in tamed
		var pet := SystemFindObjectBySerial(serial);
      if (pet.my_template()["orb"])
         ApplyRawDamage(pet, 200);
      elseif ( (pet.master == who)  && (Distance(pet, who) < 6))
			pet.hidden := 1;
			var location := array;
			location.append(pet.x);
			location.append(pet.y);
			location.append(pet.z);
			MoveObjectToLocation(pet, 6066, 60, 22, pet.realm, MOVEOBJECT_FORCELOCATION);
			SetObjProperty(pet, "previous_location", location);
		endif
		sleepms(2);
	endforeach
endfunction

//pega os pets da pessoa na area
function ListPetsNear(who, range)
	var pets := array;
	var tamed := GetObjProperty(who, "followers");
	if ( !tamed )
		return pets;
	endif
	
	foreach serial in tamed
		var pet := SystemFindObjectBySerial(serial);
		if ( (pet.master == who)  && (Distance(pet, who) < range))
			pets.append(pet);
		endif
		sleepms(2);
	endforeach

	return pets;
endfunction

//traz os pets pra proximo do dono.
function LogonPets(who)
	var tamed := GetObjProperty(who, "followers");
	if ( !tamed )
		return;
	endif
	
	foreach serial in tamed
		var pet := SystemFindObjectBySerial(serial);
		if ( (pet.master == who)  )
			pet.hidden := 0;
			var location := GetObjProperty(pet, "previous_location");
			MoveObjectToLocation(pet, location[1], location[2], location[3], pet.realm, MOVEOBJECT_FORCELOCATION);
			EraseObjProperty(pet, "previous_location");
		endif
		sleepms(2);
	endforeach
endfunction

//igual a untame que tem na IA :ghaia:follower
function destama(serial)
	var me := SystemFindObjectBySerial(serial);	
	me.SetMaster(0);
	var orig_script := GetObjProperty(me, "script");
	me.script := orig_script;
	SetName(me, GetObjProperty(me, "name"));
	EraseObjProperty(me, "owner");
	EraseObjProperty(me, "name");
	EraseObjproperty(me, "script");
	EraseObjProperty(me, "happiness");
	RestartScript(me);
endfunction

// function isTamable(mob)
// 	if (!mob.npctemplate)
// 		return 0;
// 	endif

// 	var cfg := ReadConfigFile(":ghaia:npcdesc");
// 	var npctemplate := mob.npctemplate;
// 	npctemplate := npctemplate[10, len(npctemplate)-9];
// 	cfg := cfg[npctemplate];
// 	if ( !cfg.TameDifficulty )
// 		return 0;
// 	else
// 		return 1;
// 	endif
// endfunction
