use uo;
include ":timedscripts:timedScripts";

function ListTamablePetsNearLocation(x, y, z, range, realm := _DEFAULT_REALM)
	var cfg := ReadConfigFile(":ghaia:npcdesc");

	var mobs := array;
	foreach mob in ListMobilesNearLocationEx(x, y, z, range, LISTEX_FLAG_NPC_ONLY, realm)
		if (mob.isTamable())
			mobs.append(mob);
		endif
	endforeach
	return mobs;
endfunction

//ve quantos slots custa o npc
function GetPetCost(pet)
	var cfg := ReadConfigFile(":ghaia:npcdesc");
	var npctemplate := pet.npctemplate;
	npctemplate := npctemplate[10, len(npctemplate)-9];
	cfg := cfg[npctemplate];
	if (!Cint(cfg.petslot))
		return 1;
	else
		return Cint(cfg.petslot);
	endif
endfunction

/* funcao pega o custo dos pets. tbm atualiza os pets perdidos ou que foram transferidos*/
function GetNumPets(who)
	var num_tamed := 0;
	var tamed := GetObjProperty(who, "followers");
	if ( !tamed )
		return 0;
	endif

	var cfg := ReadConfigFile(":ghaia:npcdesc");
	foreach serial in tamed
		var pet := SystemFindObjectBySerial(serial);
		if ( pet.master == who )
			var npctemplate := pet.npctemplate;
			npctemplate := npctemplate[10, len(npctemplate)-9];
			var pet_cfg := cfg[npctemplate];
			if (!Cint(pet_cfg.petslot))
				num_tamed := num_tamed+1;
			else
				num_tamed := Cint(pet_cfg.petslot);
			endif
		elseif ( !pet || pet.master !=who )
			tamed.erase(_serial_iter);
		endif
		sleepms(2);
	endforeach
	
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
			SetObjProperty(who, "followers", tamed);
			var pet := SystemFindObjectBySerial(serial);
			who.followers := who.followers - GetPetCost(pet);
			return;
		endif
		sleepms(2);
	endforeach

endfunction

//adiciona o serial do pet 
function AddPet(who, serial)
	var tamed := GetObjProperty(who, "followers");
	if ( !tamed )
		tamed := array;
	endif
	tamed.append(serial);
	SetObjProperty(who, "followers", tamed);

	var pet := SystemFindObjectBySerial(serial);
	who.followers := who.followers + GetPetCost(pet);
	pet.InitPetProperties();
endfunction

//numero maximo de pets que alguem pode ter
function GetMaxPets(who)
	var skill := AP_GetStat(who, WISDOM);
	var maxpets := 1 + skill;
	maxpets += cint( GetObjProperty(who, "extrapets") );
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
		if ( (pet.master == who)  && (Distance(pet, who) < 3))
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
function ListPetsNear(who,  range)
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