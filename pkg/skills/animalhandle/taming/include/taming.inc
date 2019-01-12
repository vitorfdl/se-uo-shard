use uo;
include ":timedscripts:timedScripts";
include "include/arrays";
include "include/damage";

var tam_npc_cfg := ReadConfigFile(":ghaia:npcdesc");
var pet_commands := array{"Atacar", "Proteger", "Ir Até", "Seguir", "Abandonar", "Novo Dono"};

function ListTamablePetsNearLocation(x, y, z, range, realm := _DEFAULT_REALM)
	var mobs := array;
	foreach mob in ListMobilesNearLocationEx(x, y, z, range, LISTEX_FLAG_NORMAL|LISTEX_FLAG_NPC_ONLY, realm)
		if (mob.isTamable())
			mobs.append(mob);
		endif
	endforeach
	return mobs;
endfunction

/* funcao pega o custo dos pets. tbm atualiza os pets perdidos ou que foram transferidos*/
function GetNumPets(who)
	var tamed := GetObjProperty(who, "followers");
	if ( !tamed )
		return 0;
	endif

   var remove_indexes := array{};
	foreach serial in tamed
		var pet := SystemFindObjectBySerial(serial);
		if ( !pet || pet.master != who )
			remove_indexes.append(_serial_iter);
		endif
		sleepms(2);
	endforeach

   removeIndexes(tamed, remove_indexes);
	
	SetObjProperty(who, "followers", tamed);
	who.followers := tamed.size();
	return who.followers;
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
   // var pet := SystemFindObjectBySerial(myserial);
   who.followers -= 1;

   if (GetObjProperty(who, "combat_pet") == myserial)
      EraseObjProperty(who, "combat_pet");
   endif
   SetObjProperty(who, "followers", tamed);
endfunction

//adiciona o serial do pet 
function AddPet(who, serial, no_reset := 0)
	var tamed := GetObjProperty(who, "followers");
	if ( !tamed )
		tamed := array;
	endif

   if (GetNumPets(who)+1 >= GetMaxPets(who) && !who.cmdlevel) return; endif
   
   who.followers += 1;
	tamed.append(serial);
	SetObjProperty(who, "followers", tamed);

	var pet := SystemFindObjectBySerial(serial);
   EraseObjProperty(pet, "spawnspot");
   EraseObjProperty(pet, "spawnpoint");
   EraseObjProperty(pet, "anchor");
   EraseObjProperty(pet, "RouteConfig");

   pet.SetMaster(who);
         
   SetObjProperty(pet, "owner", who.serial);
   SetObjProperty(pet, "name", pet.name);
   
   if ( !GetObjProperty(pet, "script") )
      SetObjproperty(pet, "script", pet.script);
   endif
      
   pet.script := ":ghaia:follower";
   RestartScript(pet);
   
   if (!no_reset) pet.InitPetProperties(0); endif

   return 1;
endfunction

//numero maximo de pets que alguem pode ter
function GetMaxPets(who)
	return 10;
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

function PetGump(who, pet, targ := 0, no_commands := 0)
  var pet_cfg := NPC_GetNPCConfig(pet, 1);

	var gump := GFCreateGump();
	
	GFPage(gump, 0);
   GFSetId(gump, 0x124);
	GFResizePic(gump, 3, 11, 9380, 276, _ifElse(pet_cfg.TamingCombat, 213, 327));
	GFGumpPicTiled(gump, 25, 42, 232, 10, 50);
	GFTextLine(gump, 41, 58, 2212, "CRIATURA");
	GFTextLine(gump, 40, 74, 1152, "{}".format(pet.name));
	GFTextLine(gump, 40, 104, 2212, "STATUS");
	GFTextLine(gump, 40, 119, 1152, "Vida: {}".format(AP_GetVitalMaximumValue(pet, HITS)));
	GFTextLine(gump, 40, 134, 1152, "Vigor: {}".format(AP_GetVitalMaximumValue(pet, STAMINA)));

   if (!no_commands)
      GFTextLine(gump, 169, 59, 2212, "COMANDOS");
      var y := 0;
      foreach command in pet_commands
         sleepms(2);
         if (!pet_cfg.TamingCombat)
            if (command in array{"Atacar", "Proteger"}) continue; endif
            if (!targ || targ == pet)
               if (command in array{"Novo Dono", "Seguir"}) continue; endif
            endif
            if (targ == who || targ.isA(POLCLASS_NPC))
               if (command in array{"Novo Dono"}) continue; endif
            endif
         else
            if (command in array{"Novo Dono"}) continue; endif
            if (!targ || targ == pet)
               if (command in array{"Atacar", "Proteger", "Seguir"}) continue; endif
            endif
         endif

         GFTextLine(gump, 180, 77+y, 1152, "{}".format(command));
         GFAddButton(gump, 166, 83+y, 2103, 2104, GF_CLOSE_BTN, 10+_command_iter);
         y+= 17;
      endforeach
   endif

   if (!pet_cfg.TamingCombat)
      var btn := 10840;
      var text := "I.A Automática";
      if (no_commands) text := "Desabilitar {}".format(text); btn := 0x2A44;
      else text := "Habilitar {}".format(text); endif
      
      GFTextLine(gump, 64, 171, 2212, text);
      GFAddButton(gump, 84, 188, btn, btn, GF_CLOSE_BTN, 99);
      GFHTMLArea(gump, 26, 218, 228, 72, "Desabilitar a I.A automática fará o animal retornar a obedecer seus comandos. Ele não produzirá nenhum recurso e não se moverá espontaneamente.", 1, 1);
      GFGumpPicTiled(gump, 27, _ifElse(pet_cfg.TamingCombat, 179, 295), 232, 10, 50);
   endif

	var res := GFSendGump(who, gump)[0];
   return res;
endfunction

function PetSkillGump(who, targ)
   var gump := GFCreateGump();

   GFSetID(gump, GUMPID_SKILLLIST);
	GFResizePic(gump, 13, 13, 9380, 712, 510);

   GFHtmlShadowed(gump, 297+15, 45, 150, 20, "#FFFFFF", "Ficha de Domador", 4);
	GFAddButton(gump, 425+13, 49, 2224, 2224, GF_CLOSE_BTN, GF_CHANGE_PAGE_TAME);
   var pet := SystemFindObjectBySerial(GetObjProperty(targ, "combat_pet"));
   if (pet)
      var pet_properties := pet.GetPetProperties();
      var pet_cfg := NPC_GetNPCConfig(pet);
      GFHtmlShadowed(gump, 57, 90, 128, 20, "#FFFFFF", "<center>{}".format(pet.name), 4);
      GFTextLine(gump, 51, 134, 1152, "Level");
      var max_exp := (cint(pet_properties.loyalt) + 1) * 100;
      buildExpBar(gump, 106, 143, 576, cint(pet_properties.exp), max_exp);
      // GFGumpPicTiled(gump, 106, 143, 576, 9, 9750); // exp
      GFTextLine(gump, 88, 134, 1152, "{}".format(pet_properties.loyalt));

      GFTextLine(gump, 406, 68, 2212, "Resistências");
      var resists := array{DMG_SLASH, DMG_BLUDGE, DMG_PIERCE, DMG_FIRE, DMG_COLD, DMG_ENERGY, DMG_POISON};
      var x := 0;
      var y := 0;
      foreach resist in resists
         var res_var := cint(GetConfigInt(pet_cfg, "{}Resist".format(resist))) + pet.get_member("resist_{}_mod".format(resist));
         GFTextLine(gump, 317+x, 84+y, 1152, "Def. {}:".format(TranslateDamage(resist)));
         GFTextLine(gump, 317+x+80, 84+y, 1152, "{}".format(res_var));
         
         y += 18;
         if (_resist_iter % 3 == 0)
            y := 0;
            x += 142;
         endif
         sleepms(2);
      endforeach
      // GFTextLine(gump, 396, 85, 1152, "60");
      // GFTextLine(gump, 544, 88, 1152, "60");
      // GFAddButton(gump, 419, 86, 55, 248, GF_CLOSE_BTN, 0);
      // GFAddButton(gump, 419, 105, 55, 248, GF_CLOSE_BTN, 0);
      // GFAddButton(gump, 419, 124, 55, 248, GF_CLOSE_BTN, 0);
      // GFAddButton(gump, 566, 91, 55, 248, GF_CLOSE_BTN, 0);
      // GFAddButton(gump, 566, 108, 55, 248, GF_CLOSE_BTN, 0);

      GFTextLine(gump, 210, 69, 2212, "Vitalidades");
      GFTextLine(gump, 204, 84, 1152, "Vida: {}".format(AP_GetVitalMaximumValue(pet, HITS)));
      GFTextLine(gump, 198, 101, 1152, "Vigor: {}".format(AP_GetVitalMaximumValue(pet, STAMINA)));
      // GFAddButton(gump, 281, 85, 55, 248, GF_CLOSE_BTN, 0);
      // GFAddButton(gump, 281, 103, 55, 248, GF_CLOSE_BTN, 0);

      GFTextLine(gump, 598, 70, 2212, "Habilidades");
      GFGumpPic(gump, 634, 91, 0x5DD3, 1000);
      GFTooltipText(gump, "Habilidade Indisponível");
      GFGumpPic(gump, 587, 91, 0x5DD3, 1000);
      GFTooltipText(gump, "Habilidade Indisponível");
   else
      GFHtmlShadowed(gump, 245, 95, 300, 20, "#AFAFAF", "Nenhuma criatura combatente domesticada", 4);
   endif

	GFGumpPicTiled(gump, 65, 149, 607, 21, 58);
	GFGumpPic(gump, 37, 149, 57, 0);
	GFGumpPic(gump, 667, 149, 59, 0);

	GFTextLine(gump, 233, 175, 2212, "Nível de aprendizado para domesticar criaturas");
   for i := 0 to 8
	   GFGumpPicTiled(gump, 59, 221+(i * 20), 611, 1, 2701);
   endfor

   var pet_knowledge := GetObjProperty(targ, "pet_knowledge");
   if (!pet_knowledge) pet_knowledge := dictionary{}; endif

   var npcdesc := ReadConfigFile(":*:npcdesc");
   var x := 0;
   var y := 0;
   foreach key in (pet_knowledge.Keys())
      var name := FindConfigElem(npcdesc, key).Name;
      var percent := cint((pet_knowledge[key] * 100) / TamingDiff(0, npcdesc[key]));
	   GFTextLine(gump, 67+x, 202+y, 1152, "{}: {}%".format(name, _ifELse(percent >= 100, 100, percent)));
      y += 20;
      if (_key_iter % 11 == 0)
         x += 200;
         y := 0;
      endif
   endforeach
	// GFTextLine(gump, 67, 223, 1152, "Galinha: 100%");

	GFTextLine(gump, 285, 385, 2212, "Criaturas Passivas Domesticadas");
   for i := 0 to 3
	   GFGumpPicTiled(gump, 59, 421+(i * 20), 611, 1, 2701);
   endfor
   
	var tamed := GetObjProperty(who, "followers");
   x := 0;
   y := 0;
	if ( tamed && tamed.size() )
		foreach serial in tamed
         if (tamed == pet.serial) continue; endif
         var animal := SystemFindObjectBySerial(serial);
         GFAddButton(gump, 55+x, 405+y, 2103, 2104, GF_CLOSE_BTN, 600+animal.serial);
         GFTextLine(gump, 67+x, 402+y, 1152, "{}".format(animal.name));
         if (_serial_iter % 3 == 0)
            x += 200;
            y := 0;
         endif
      endforeach
	endif
   return GFSendGump(who, gump);
endfunction

function HandleTamingPage(who, res, targ)
   if (res >= 600)
      res -= 600;
      var pet := SystemFindObjectBySerial(res);
      var output := RadioGump(who, 0, 0, "O que deseja fazer?", array{"Abandonar", "Rastrear", "Cancelar"}, 0, 3);

      if (output == "Abandonar")
         SetObjProperty(pet, "last_owner", who.serial);
         ErasePet(who, res);
         Destama(pet.serial);
      elseif (output == "Rastrear")
         SendSysMessageEx(who, "Opção desativada no momento");
      endif
   endif
endfunction

function TamingDiff(pet, pet_cfg)
   if (!pet_cfg)
      pet_cfg := NPC_GetNPCConfig(pet, 1);
   endif
   
   return _ifElse(pet_cfg.TamingCombat, 50, 25);
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
