use uo;
include ":timedscripts:timedScripts";
include "include/arrays";
include "include/damage";

var tam_npc_cfg := ReadConfigFile(":ghaia:npcdesc");

function ListTamablePetsNearLocation(x, y, z, range, realm := _DEFAULT_REALM)
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
   var pet_cost := GetObjProperty(pet, "pet_cost");
   if (pet_cost) return pet_cost; endif

	var npctemplate := pet.my_template();
	tam_npc_cfg := tam_npc_cfg[npctemplate];
	if (tam_npc_cfg.petslot == error)
		return 1;
	else
		return Cint(tam_npc_cfg.petslot);
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
	foreach serial in tamed
		var pet := SystemFindObjectBySerial(serial);
		if ( pet.master == who )
         var pet_num := GetPetCost(pet);

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
   if (myserial.isA(POLCLASS_MOBILE)) myserial := myserial.serial; endif

	var tamed := GetObjProperty(who, "followers");
	if ( !tamed )
		return;
	endif

   var index := myserial in tamed;
   tamed.erase(index);
   MP_Unmount(who);
   var pet := SystemFindObjectBySerial(myserial);
   who.followers -= GetPetCost(pet);

   SetObjProperty(who, "followers", tamed);
endfunction

//adiciona o serial do pet 
function AddPet(who, serial, no_reset := 0)
	var tamed := GetObjProperty(who, "followers");
	if ( !tamed )
		tamed := array;
	endif
   
	tamed.append(serial);
	SetObjProperty(who, "followers", tamed);

	var pet := SystemFindObjectBySerial(serial);
   EraseObjProperty(pet, "spawnspot");
   EraseObjProperty(pet, "spawnpoint");
	who.followers += GetPetCost(pet);

   pet.SetMaster(who);
         
   SetObjProperty(pet, "owner", who.serial);
   SetObjProperty(pet, "name", pet.name);
   
   if ( !GetObjProperty(pet, "script") )
      SetObjproperty(pet, "script", pet.script);
   endif
      
   pet.script := ":ghaia:follower";
   RestartScript(pet);
   
   if (!no_reset) pet.InitPetProperties(0); endif
endfunction

//numero maximo de pets que alguem pode ter
function GetMaxPets(who)
   if (who.cmdlevel)
      return 300;
   endif
	// var skill := AP_GetStat(who, WISDOM);
   var skill := AP_GetTrueSkill(who, TRATO_ANIMAIS);
   var comando := AP_GetTrueSkill(who, "Magia_de_Comando");
   if (comando > skill) skill := comando; endif

	var maxpets := 1 + cint( GetObjProperty(who, "extrapets") ) + cint( skill / 2 );

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
   var pets := array{};
   var mobs := ListMobilesNearLocationEx(who.x, who.y, who.z, range, LISTEX_FLAG_NORMAL|LISTEX_FLAG_NPC_ONLY, who.realm);
	foreach pet in mobs
		if ( pet.master.serial == who.serial )
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
	var tamed := SystemFindObjectBySerial(serial);	
	tamed.SetMaster(0);
	var orig_script := GetObjProperty(tamed, "script");
	tamed.script := orig_script;
	SetName(tamed, GetObjProperty(tamed, "name"));
	EraseObjProperty(tamed, "owner");
	EraseObjProperty(tamed, "name");
	EraseObjproperty(tamed, "script");
	EraseObjProperty(tamed, "happiness");
	RestartScript(tamed);
endfunction

// function isTamable(mob)
// 	if (!mob.npctemplate)
// 		return 0;
// 	endif

// 	var tam_npc_cfg := ReadConfigFile(":ghaia:npcdesc");
// 	var npctemplate := mob.npctemplate;
// 	npctemplate := npctemplate[10, len(npctemplate)-9];
// 	cfg := cfg[npctemplate];
// 	if ( !cfg.TameDifficulty )
// 		return 0;
// 	else
// 		return 1;
// 	endif
// endfunction
