/**
 * Lista de Funções
 * GetBagList(who, materials := 0)
 * TruncatePlural(objtype, add_S := 0)
 * CreateItem(who, objtype, more:=0)
 * CheckObjectNear(who, item_list)
 * CraftLockable(who, theitem)
 * doRepair(who, craft_skill)
 **/

include ":keys:key";
include ":gumps:yesno";

function GetBagList(who, materials := 0)
   var items     := EnumerateItemsInContainer(who.backpack, ENUMERATE_IGNORE_LOCKED);
   var item_list := dictionary{};
   foreach item in (items)
      if (!materials || item.objtype in materials)
         item_list[item.objtype] := item.amount;
         if (resource_config[item.objtype])
            var type := resource_config[item.objtype].Type.lower();
            item_list[type] := cint(item_list[type]) + item.amount;
         endif
      endif
      sleepms(5);
   endforeach

   return item_list;
endfunction

function CreateItem(who, objtype, more:=0)
	var theitem := 0;
	var cfg           := ReadConfigFile(":crafting:stackingitens");
	var elem          := FindConfigElem(cfg, "stacks");
	var stackingitens := ListConfigElemProps( elem );
	if(cstr(objtype) in stackingitens)
		var stacknow := FindObjtypeInContainer( who.backpack, objtype );
		if (objtype == 7124 || objtype == 0x9087)
			more := more + 4;
		endif
		if ( stacknow )
			AddAmount (stacknow, 1+more);
			theitem := stacknow;
		else
			theitem := CreateItemInBackpack(who, objtype, 1+more);
		endif
	else
		theitem := CreateItemInBackpack(who, objtype, 1+more);
		//theitem.quality := 0.6;
		theitem.name := theitem.desc;
	endif
	return theitem;
endfunction

function CheckObjectNear(who, item_list)
	var object_near;
	foreach item in ListItemsNearLocation(who.x, who.y, who.z, 2)
		if(item.objtype in item_list)
			object_near := item;
			break;
		endif
		sleepms(5);
	endforeach

	return object_near;
endfunction

function CraftLockable(who, theitem)
		if (theitem.isDoor())
			SetObjProperty(theitem, "pedreiro", who.serial); 
			SetObjProperty(theitem, "criador", who.acctname);	
			SetObjProperty(theitem, "validade", CInt(ReadGameClock()+360));
			MoveObjectToLocation(theitem, who.x, who.y, who.z, who.realm, MOVEOBJECT_NORMAL);
			theitem.movable := 0;
		endif

		if (KP_IsLockable(theitem ))
			var fechadura := YesNo(who, "Voce deseja por uma fechadura nesse item?");
			if (fechadura)
				SendSysMessageEx(who, "Escolha a fechadura para por neste item.", SSM_REQUEST);
				var targ := Target(who);
				if (targ)
					if (targ.objtype == 4001)
						if (!theitem.isDoor())
							var lvl := Cint(GetObjProperty(targ, "level"));
							if (!lvl)
								lvl := 1;
							endif
							SetObjProperty(theitem, "level", lvl);
						endif
						var lockid := CInt(GetObjProperty(targ, "LockId"));
						SetObjProperty(theitem, "LockId", lockid);
						DestroyItem(targ);
						SendSysMessageEx(who, "Voce instalou a fechadura com sucesso.", SSM_INFO);
					else
						SendSysMessageEx(who, "Isto nao parece uma fechadura.", SSM_FAIL);
					endif
				endif
			endif
		endif
endfunction

function doRepair(who, craft_skill, recipe_cfg)
   SendSysMessage(who, "Escolha o item para consertar..");
   var use_on := Target(who);
   if(use_on.movable == 0)
      SendSysMessage(who, "Voce nao pode consertar isto.");
      return;
   elseif((!Accessible(who, use_on)) or (distance(who, use_on) > 2))
      SendSysMessage(who, "Voce nao alcanca o item.");
      return;
   endif

   // if(CInt(use_on.objtype) == CInt(0xe34))
   //    case(CInt(craft_skill))
   //       METALWORKING:reateRepairDeed(who, use_on, craft_skill);
   //       //    BOWCRAFT_FLETCHING:    CreateRepairDeed(who, use_on, craft_skill);
   //       //  CARPENTRY:   		 CreateRepairDeed(who, use_on, craft_skill);
   //       //TAILORING:   		 CreateRepairDeed(who, use_on, craft_skill);
   //       endcase
   //    return;
   // endif

   var elem := FindConfigElem(recipe_cfg, use_on.objtype);
   if (!elem) 
      SendSysMessage(who, "Voce nao pode consertar isso.");	
   endif

   var combatcfg := ReadConfigFile(":combat:itemdesc");
   var elem := FindConfigElem(combatcfg, what);
   var repairable := GetConfigInt(elem, "repairable");
   if (!repairable)
      SendSysMessage(who, "So pode consertar armas ou armaduras.");
      return 0;
   endif

   var recipe := GetRecipe(who, elem);

   var found := dictionary{};
   foreach group in (recipe.requires)
      var req_elem := FindConfigElem(requires_config, group);
      if (!req_elem)
         continue;
      endif

      var req_items := GetConfigIntArray(req_elem, "item");
      var req_found := CheckObjectNear(who, req_items);
      if (req_found)
         found[group] := req_found;
      else 
         SendSysMessageEx(who, "Voce precisa estar perto de um {} para fazer isso.".format(group), SSM_FAIL);
         return 1;
      endif
      sleepms(2);
   endforeach

   var work_items := array{"Maquina de Costura", "Maquina de Tensão", "Maquina de Marcenaria"};
   foreach work_item in work_items
      if (work_item in found.Keys())
         var machine := found[work_item];
         machine.graphic := machine.graphic+1;
      endif
   endforeach

   var reparos := GetObjProperty(what,"reparos");

   if (reparos == error)
      reparos:= 2 + GetBonusAttr(AP_GetTrueStat(mobile, "Intelligence"));
      SetObjProperty(what,"reparos",reparos);
   endif

   SetObjProperty(what, "broken", 0);
   SetName(what, GetObjProperty(what, "realdesc"));
  
   if(what.hp == what.maxhp)
      SendSysMessage(character, "O item nao esta danificado.");
      return;
   endif
  
   if((what.hp <= 0) || (what.quality <= 0.1) || (!what.quality))
      SendSysMessage(character, "O fragil material quebrou quando voce manipulou ele.");
      DestroyItem(what);
      return;
   endif
  
   if(what.quality <= damageamount)
      SendSysMessage(character, "O fragil material quebrou quando voce manipulou ele.");
      DestroyItem(what);
      return;
   endif

   var skill        := GetBonusAttr(AP_GetTrueStat(mobile, "Intelligence")) + getProficiencyBonus(mobile, craft_skill);
   var damageamount := (2 * (1 + skill)/3000.0;
   if (damageamount > 0)
      damageamount := damageamount * -1;
   endif
   what.quality -= damageamount;
  
   if((what.hp <= 0) || (what.quality <= 0.1) || (!what.quality))
      SendSysMessage(character, "O fragil material quebrou quando voce manipulou ele.");
      DestroyItem(what);
      return;
   endif
 
   var maxhp     := cint(what.maxhp * what.quality);
   var damaged   := maxhp - what.hp;
   var advantage := 0;
   if (recipe.objtype in GetObjProperty(mobile, recipes))
      advantage += 1;
   endif
   var dice_result := rollAttrDice(who, "Intelligence", advantage) + getProficiencyBonus(mobile, craft_skill);
   if (dice_result > recipe.difficulty - 1)
      var repaired_amount := cint((skill+2)*2);
      //SendSysMessage(character, "rep"+repaired_amount);
      //SendSysMessage(character, "hisskill: "+((randomint(hisskill) + 1))/2);
      
      if (damaged == 1)
         repaired_amount := 1;
      endif
      
      if (thehp + repaired_amount > what.maxhp)
         what.hp := what.maxhp;
         EraseObjProperty(what, "swings");
         SendSysMessage(character, "Voce consertou o item completamente.");
      else
         what.hp := thehp + repaired_amount;
         SendSysMessage(character, "Voce consertou o item.");
      endif
      SetName( what, what.desc ); //Update the Item
      
      if ((what.hp <= 0) || (what.quality <= 0.1) || (!what.quality))
         SendSysMessage(character, "O fragil material quebrou quando voce manipulou ele.");
         DestroyItem(what);
         return;
      endif
   
   else
      var damage_amount := randomint(10);
      
      if (thehp <= damage_amount)
         DestroyItem(what);
         SendSysMessage(character, "Voce falhou e destruiu o item.");
      endif
      
      thehp := thehp - damage_amount;
      what.hp := thehp;
      
      if(randomint(2) == 1)
         what.quality := what.quality - (damaged/100);
      endif
      
      if(thehp <= 0)
         DestroyItem(what);
         SendSysMessage(character, "Voce falhou e destruiu o item.");
      else
         SendSysMessage(character, "Voce falhou e danificou o item.");
      endif
      
      SetName( what, what.desc ); //Update the Item
      
      if ((what.hp <= 0) || (what.quality <= 0.1) || (!what.quality))
         SendSysMessage(character, "O fragil material quebrou quando voce manipulou ele.");
         DestroyItem(what);
         return;
      endif
   endif

   // if (GetObjProperty(what, "reparos") == error)
   //    reparos:=hisskill/2;
   //    reparos:= (reparos  -(11 - (hisskill / 11)));
   //    SetObjProperty(what,"reparos",reparos);
   // else
   //    reparos:=GetObjProperty(what, "reparos");
   //    reparos:= (reparos  -(11 - (hisskill / 11)));
   //    SetObjProperty(what,"reparos",reparos);
   // endif
endfunction

//
//  TruncatePlural()
//
//  * Returns the name for the item of the objtype passed.
//
function TruncatePlural(objtype, add_S := 0)
   var obj_type := CInt(objtype);
   if (!obj_type)
      return objtype;
   endif
   
   var config := itemdesc_cfg[obj_type];
   var name;
   if (config.desc)
      name := config.desc;
   elseif (config.name)
      name := config.name;
   else
      return error{"errortext":="Invalid Objtype : " + hex(obj_type) + "."};
   endif

   if ( !name["%"] )
         if ( add_S == 0 )
            return name;
         endif
         if ( name[len(name)] in array{"s"} )
            return name;
         elseif ( name[len(name)] in array{"h","j","x","z"} )
            return name + "es";
         endif
         return name + "s";
   endif

    // Will deal with:
    //
    //      Bread Stick%s
    //      Bread Loa%ves/f%
    //      Lump%s% of Coal
    //      Loa%ves/f% of Bread
    //
   var first  := find(name, "%", 1);
   var slash  := find(name, "/", first);
   var second := find(name, "%", first + 1);
   if ( slash && !second )
         // Force-append a % if a '/' exists!
      name := name + "%";
      second := len(name);
   elseif ( !slash )
      slash := second;
   endif

    // wwwww%xx[/yy]%[zzzzz]
    // 12345678     9 01234
    // 12345678 901 2 34567

   var s_plur := name[first+1, slash-(first+1)];
   var s_sing := "";
   if ( slash != first )
         s_sing := name[slash+1, second-(slash+1)];
   endif

   var s_first := name[1, first-1];
   var s_middle := s_plur;
   var s_last := "";

   if ( len(name) > second )
         s_last := name[second+1, len(name)-second];
   endif

   if ( add_S == 0 )
         s_middle := s_sing;
   endif

   if ( !s_first )
         s_first := "";
   endif
   if ( !s_middle )
         s_middle := "";
   endif
   if ( !s_last )
         s_last := "";
   endif
   return s_first + s_middle + s_last;

endfunction