/* Resumo de Funï¿½ï¿½es
 * PlayCraftWork(who, craftSound, loops:=0, delay:=2, craftAction:=0, fastcraft := 0)
 * GetRecipe(objtype, elem)
 * GetRecipeList(who, recipe_cfg, search := "")
 * MaterialSelection(who, type, chk)
 * CraftItem(who, craft_skill, tool, objtype, amt, canFix, anvil, more := 0)
 * SubtractMaterial(who, targ_materials, amountMod)
 * AddItemDescription(who, item)
**/

use os;
use uo;
use cfgfile;
include "include/say";
include "include/objtype";
include ":attributes:attributes";
include ":crafting:fls_craftingUtils";
include ":itemUtils:toolWear";
include ":gumps:gump_prompts";
include ":crafting:craft_props";
include ":crafting:excep_gump";
include ":crafting:recipes";
include ":quest:quest";

const OUT_OF_MATERIALS := 1;

var itemdesc_cfg    := ReadConfigFile(":*:itemdesc");
var resource_config := ReadConfigFile(":crafting:config/resourceList");
var resource_level := ReadConfigFile(":crafting:config/resourceLevels");
var menu_config     := ReadConfigFile(":crafting:config/craftConfig");
var requires_config := ReadConfigFile(":crafting:config/requires");

/**
 * PlayCraftWork - Plays a crafting sound effect for the specified character.
 * 
 * @param {who} - The character for whom the crafting sound effect will be played.
 * @param {craftSound} - The sound effect to be played for crafting.
 * @param {loops} - The number of times the sound effect will be played in a loop.
 * @param {delay} - The delay in seconds between each loop iteration.
 * @param {craftAction} - The crafting action to be performed.
 * @param {fastcraft} - Flag indicating fast crafting mode.
 */

function PlayCraftWork(who, craftSound, loops:=0, delay:=2, craftAction:=0, fastcraft := 0)
   if(loops)
      for i := 1 to loops
         if(craftAction)
            PerformAction( who, craftAction );
         endif
         PlaySoundEffect(who, craftSound);
         sleep(delay);
      endfor
   else
      PlaySoundEffect(who, craftSound);
      if (fastcraft)
         sleepms(200);
         return;
      endif
      sleep(delay);
   endif

   return;
endfunction

/**
 * GetRecipe - Retrieves recipe information for the specified objtype and elem.
 * 
 * @param {objtype} - The object type for which the recipe information will be retrieved.
 * @param {elem} - The element containing the recipe information.
 * @returns {struct} - The recipe information including graphic, color, name, requireFeat, difficulty, desc, colored, mark, exceptional, delay, fastcraft, more, hasLevel, materials, main_material, and requires.
 */

function GetRecipe(objtype, elem)
   var item := itemdesc_cfg[Hex(objtype)];
   var graphic := item.graphic;
   if (!graphic)
      graphic := item.Graphic;
      if (!graphic)
         graphic := objtype;
      endif
   endif

   var raw_materials := GetConfigStringDictionary(elem, "materials");
   var materials := dictionary{};
   foreach material in (raw_materials.Keys())
      var new_key := cint(material);
      if (!new_key)
         new_key := material;
      endif
      materials[new_key] := cint(raw_materials[material]);
   endforeach

   var recipe_info := struct{
      "graphic"     := cint(graphic),
      "color" := cint(item.color),
      "name"        := TruncatePlural(objtype),
      "objtype"     := objtype,
      "requireFeat"  := GetConfigInt(elem, "requireFeat"),
      "difficulty"  := GetConfigInt(elem, "difficulty"),
      "desc"  := GetConfigString(elem, "desc"),
      "colored"     := GetConfigInt(elem, "colored"),
      "mark"        := GetConfigInt(elem, "mark"),
      "exceptional" := GetConfigInt(elem, "exceptional"),
      "delay"       := GetConfigInt(elem, "delay"),
      "fastcraft"       := GetConfigInt(elem, "fastcraft"),
      "more"       := GetConfigInt(elem, "more"),
      "hasLevel"       := GetConfigInt(elem, "hasLevel"),
      "materials"  := materials,
      "main_material" := elem.main_material,
      "requires"   :=  GetConfigStringArray(elem, "requires")
   };
   return recipe_info;
endfunction

/**
 * GetRecipeList - Retrieves a list of recipes for the specified player and recipe configuration, optionally filtered by a search string.
 * 
 * @param {who} - The player for whom the recipe list will be retrieved.
 * @param {recipe_cfg} - The recipe configuration from which the recipe information will be retrieved.
 * @param {search} - Optional. A search string to filter the recipe list.
 * @returns {array} - An array of recipe information including graphic, color, name, objtype, requireFeat, difficulty, desc, colored, mark, exceptional, delay, fastcraft, more, hasLevel, materials, main_material, requires, level, and exp.
 */

function GetRecipeList(who, recipe_cfg, search := "")
	var items := GetObjProperty(who,  "recipes"); //pega a lista de recipes
	
	var recipelist := array{}; //lista temp pra veriicar se os items sao da skill do menu
	foreach recipe_id in (items.keys())
		var elem        := FindConfigElem(recipe_cfg, Hex(recipe_id));
      if (!elem) elem := FindConfigElem(recipe_cfg, cint(recipe_id)); endif
		var recipe_type := GetConfigString(elem, "Type");
		if (recipe_type == "Recipe")
         var recipe := GetRecipe(recipe_id, elem);
         var found := Lower(recipe.name).find(Lower(search));
         if (search == "" || found)
            recipe.level := items[recipe_id].level;
            recipe.exp := items[recipe_id].exp;
            recipelist.append(recipe);
         endif
      elseif (recipe_type == "group")
         var group_recipes := GetConfigIntArray(elem, "recipe");
         foreach new_recipe in (group_recipes)
		      elem := FindConfigElem(recipe_cfg, new_recipe);
            new_recipe := GetRecipe(new_recipe, elem);
            var found := Lower(new_recipe.name).find(Lower(search));
            if (search == "" || found)
               recipelist.append(new_recipe);
            endif
         endforeach
		endif
      sleepms(2);
	endforeach
	
	return recipelist;
endfunction

/**
 * MaterialSelection - Retrieves a list of recipes for the specified player and recipe configuration, optionally filtered by a search string.
 * 
 * @param {who} - The player for whom the recipe list will be retrieved.
 * @param {type} - The type of material being selected.
 * @param {chk} - The check value for the material selection.
 * @param {recipe} - The recipe being used for crafting.
 * @param {craft_skill} - The crafting skill being used for the material selection.
 */
function MaterialSelection(who, type, chk, recipe, craft_skill)
   var material;
   var levelselect := 0;
   if (!recipe.level)
    	SendSysMessageEx(who, "Escolha o "+lower(type)+" que você deseja trabalhar.");
	   material := Target(who);
   else
      var resources := GetConfigStringDictionary(FindConfigElem(resource_level, type), "Level");
      var opts := array{};
      for i := 1 to (recipe.level)
         opts.append("Level {} - {}".format(i, itemdesc_cfg[resources[cstr(i)]].desc));
      endfor
      var res := RadioGump(who, 300, 0, "Qual level você deseja fazer?", opts, 1);
      if (!res) return; endif

      material := FindObjtypeInContainer(who.backpack, resources[cstr(res)]);
      levelselect := res;
   endif

	if(!material)
		SendSysMessage(who, "Cancelado.");
		return 0;
	endif
	
	if(material.container.serial != who.backpack.serial)
		SendSysMessageEx(who, "O material não esta na sua mochila.");
		return 0;
	endif
	
	if( (!Accessible(who, material)) || (Distance(who, material) > 2)
       || (material.movable == 0) || (!ReserveItem(material)))
		SendSysMessageEx(who, "Você não pode usar isto.");
		return 0;
	endif
	
   // if (AP_GetTrueSkill(who, craft_skill) < 3)
   //    if (type == "metal")
   //       if (material.objtype != 0x6309)
   //          SendSysMessageEx(who, "Você não sabe trabalhar com isto.", SSM_FAIL);
   //          return 0;
   //       endif
   //    elseif (type == "couro")
   //       if (material.objtype != 0xee45 && material.color != 0)
   //          SendSysMessageEx(who, "Você não sabe trabalhar com isto.", SSM_FAIL);
   //          return 0;
   //       endif
   //    elseif (type == "madeira")
   //       if (material.objtype != 0x1BD7 && material.color != 0)
   //          SendSysMessageEx(who, "Você não sabe trabalhar com isto.", SSM_FAIL);
   //          return 0;
   //       endif
   //    endif
   // endif

	var materialobjtype := 0;
	if(chk)
		if(GetObjProperty(material, chk))
			materialobjtype := GetObjProperty(material, chk);
		else
			materialobjtype := material.objtype;
		endif
	else
		materialobjtype := material.objtype;
	endif
	
	// if(!canUseMaterial(who, materialobjtype, craft_skill))
	// 	SendSysMessageEx(who, "você não possui conhecimento suficiente para fazer algo com isso.");
	// 	return 0;
	// endif
	
	var rtype := lower(resource_config[materialobjtype].Type);
	var materialchk := 0;
	if(rtype == type)
		materialchk := 1;
	endif

	if(materialchk == 0)
	   SendSysMessageEx(who, "Esse não é o material certo.");
	   return 0;
	endif
	
	if((material.objtype >= UOBJ_BOLT_START) && (material.objtype <= UOBJ_BOLT_END))
		var totalcloth := material.amount * 50;
		var cloth;
		var material_color := material.color;
		if(DestroyItem(material))
			cloth := CreateItemInBackpack(who, 0x1767, totalcloth);
			if(!cloth)
				cloth := CreateItemAtLocation(who.x, who.y, who.z, 0x1767, totalcloth);
			endif
			cloth.color := material_color;
		endif
		material := cloth;
	endif

	var retarray := array{material, materialobjtype, levelselect};
	return retarray;
endfunction

/**
 * CraftItem - Crafts an item using the specified parameters.
 * 
 * @param {who} - The character who is crafting the item.
 * @param {craft_skill} - The crafting skill being used to craft the item.
 * @param {tool} - The tool being used for crafting.
 * @param {recipe} - The recipe for the item being crafted.
 * @param {amt} - The amount of the item to be crafted.
 * @param {canFix} - Flag indicating whether the item can be fixed during crafting.
 */
function CraftItem(who, craft_skill, tool, recipe, amt, canFix)
   if (recipe.requireFeat && !HaveFeatSecondary(who, cint(recipe.RequireFeat)))
      return SendSysMessageEx(who, "Este item requer a habilidade {} para ser construido".format(GetFeatById(recipe.RequireFeat).Name), SSM_FAIL);
   endif

	var craft_sound  := menu_config[craft_skill].CraftSound;
	var craft_delay  := menu_config[craft_skill].CraftDelay;
	var craft_action := menu_config[craft_skill].CraftAnim;
	var craft_loops  := _ifOr(recipe.delay, 1);

	var materials     := recipe.materials;
	var more          := cint(recipe.more);
	var main_material := recipe.main_material;
	var targ_materials   := GetBagList(who, materials.Keys());
   var material_amt := 1;
   var level_select := 0;

   if (craft_skill == "Alquimia" && recipe.level)
      var opts := array{};
      for i := 1 to (recipe.level)
         opts.append("Level {} - {}x mais recursos".format(i, i));
      endfor
      level_select := cint(RadioGump(who, 300, 0, "Qual level você deseja fazer?", opts, 1));
   endif

	var choosed_material := 0;
	var material_color   := 0;
	var material_cprops := struct{};

	foreach thing in (materials.keys())
		sleepms(5);
      if (thing in array{"metal", "madeira", "vidro", "couro", "pano", "pontaflecha", "planta"})
			var ret := MaterialSelection(who, thing, 1, recipe, craft_skill);
			if (!ret)
				SendSysMessageEx(who, "você não possui os materiais necessarios.", SSM_FAIL);
				return OUT_OF_MATERIALS;
			elseif (ret[1].amount < materials[thing] )
            SendSysMessageEx(who, "você não possui os materiais necessarios." , SSM_FAIL);
            return OUT_OF_MATERIALS;			
			elseif (thing == "pontaflecha" && cint(GetObjProperty(ret[1], "material")) != recipe.arrowtype)
            SendSysMessageEx(who, "você não pode fabricar estas flechas com a ponta escolhida." , SSM_FAIL);
            return OUT_OF_MATERIALS;
         else
            if (ret[1].objtype in array{0x0F0E, 0x1F91})
               targ_materials[ret[1].objtype] := materials[thing];
            else
               targ_materials[ret[1].objtype] := materials[thing] * material_amt;
            endif
            if (thing == main_material)
               material_color   := ret[1].color;
               choosed_material := ret[1].objtype;
               level_select := ret[3];
            endif
         endif
         continue;
		endif

      if ( targ_materials[thing] < (materials[thing] + more))
         SendSysMessageEx(who, "você não possui os materiais necessarios." , SSM_FAIL);
         return OUT_OF_MATERIALS;
      else
         targ_materials[thing] := materials[thing];
      endif
	endforeach

	var counter  := 0;
	var theitems := array{};
	var initialX := who.x;
	var initialY := who.y;
   var theitem;
   var item_list := _ifOr(recipe.pieces, array{recipe.objtype});

	while (counter < amt)
		counter := counter + 1;
		if ( (who.x != initialX) || (who.y != initialY) )
			SendSysMessageEx(who, "Você parou de se concentrar no que estava fazendo.", SSM_FAIL);
			return 1;
		endif

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
			   SendSysMessageEx(who, "você precisa estar perto de um {} para fazer isso.".format(group), SSM_FAIL);
            return 1;
         endif
         sleepms(2);
      endforeach

		var work_items := array{"Maquina de Costura", "Maquina de Tensão", "Aparato de Aquecimento"};
		foreach work_item in work_items
			if (work_item in found.Keys())
            var req_elem := FindConfigElem(requires_config, work_item);
				var machine := found[work_item];
				if (!ReserveItem(machine))
					SendSysMessageEx(who, "Outra pessoa esta usando {}.".format(work_item), SSM_FAIL);
				endif

            if (req_elem.PlayText) PrintText(machine, "{}".format(req_elem.PlayText)); endif
            if (req_elem.StartSound) PlaySoundEffect(machine, cint(req_elem.StartSound)); endif
            if (req_elem.ChangeGraphic) machine.graphic := machine.graphic + cint(req_elem.ChangeGraphic); endif
				
			endif
		endforeach

		var item := 0;
		if ("Bigorna" in found.Keys())
         var forja := found["Bigorna"];
			item := CreateItemAtLocation(forja.x, forja.y, forja.z+3, recipe.objtype, 1, forja.realm);
			item.movable := 0;
			item.color := 1914;
			PrintTextAbove(item, "*item em brasa*");
			MoveObjectToLocation(forja.x, forja.y, (forja.z+7), forja.realm);
		endif

		playCraftWork(who, craft_sound, craft_loops, craft_delay, craft_action, recipe.fastcraft);

		if (item)
			DestroyItem(item);
		endif

		foreach work_item in work_items
			if (work_item in found.Keys())
            var req_elem := FindConfigElem(requires_config, work_item);
				var machine := found[work_item];
				ReleaseItem(machine);
            if (req_elem.EndSound) PlaySoundEffect(machine, cint(req_elem.EndSound)); endif
            if (req_elem.ChangeGraphic) machine.graphic := machine.graphic - cint(req_elem.ChangeGraphic); endif
			endif
		endforeach

      var diff := _ifElse(level_select, level_select * 10, 0);
		if(Randomint(100) >= 10 || who.cmdlevel > 4)
			if (!SubtractMaterial(who, targ_materials))
				SendSysMessageEx(who, "Você não possui todos os materiais.", SSM_FAIL);
				return theitems;
			endif

         foreach item_objtype in item_list
            theitem := _CreateItemInBackpack(who, cint(item_objtype), cint(more));
            if(!theitem)
               SendSysMessageEx(who, "Você não tem espaço suficiente na sua mochila!", SSM_FAIL);
               return theitems;
            elseif (recipe.colored)
               theitem.color := material_color;
            endif

            CheckToolWear(who, tool, craft_skill);
            theitem.movable := 1;

            //passando a cor pra materiais coloridos que não tem Id proprio (como madeira)
            AddMaterialMod(recipe, theitem, choosed_material, material_color);
            if ((recipe.exceptional == 1 && HaveFeatSecondary(who, 66) && Randomint(100) <= 10) || who.cmdlevel > 3)
               SendSysMessageEx(who, "Você fez uma obra prima! Seu item tem qualidade {} e se tornará {}.".format(getQuality(theitem.quality, 1), getQuality(theitem.quality+1, 1)), SSM_INFO);
               SendSysMessageEx(who, "Para validar o objeto, você deve escolher uma joia para o item (Safira, Safira Estrela, Ruby, etc)", SSM_INFO);
               while (who.connected)
                  if (CraftExceptional(who, theitem, craft_skill) != 2)
                     break;
                  endif
                  sleepms(2);
               endwhile
            endif

            // foreach key in (material_cprops.Keys())
            //    SetObjProperty(theitem, key, material_cprops[key]);
            // endforeach

            CraftLockable(who, theitem);
            makeTinkeringItems(who, theitem);
            ExpGain(who, "minor");
            SetObjProperty(who, "#craft_last", item_objtype);
            // who.SetAchievement("craft_{}".format(lower(craft_skill)), 1);
            SendSysMessageEx(who, "Você criou o item o colocou na mochila.", SSM_INFO);
            ApplyCraftChanges(theitem, level_select);

            if(theitem.isA(POLCLASS_CONTAINER))
               SetObjProperty(theitem, "trappable", 1);
            endif

            if ((theitem.graphic != 11058) && (theitem.graphic != 3903) && (theitem.graphic != 0x0f09))
               SetObjProperty(theitem, "crafterserial", who.serial);
               SetObjProperty(theitem, "crafteracc", who.acctname);
            endif

            if (canFix)
               SetObjProperty(theitem, "canfix", 1);
            endif

            if (craft_skill == "Alquimia")
               SetObjProperty(theitem, "level", level_select);
            endif
            
            SetName(theitem, _ifOr(theitem.name, theitem.desc));
            SendQuestEvent(who, "Produzir Item", struct{ quality := cint(theitem.quality), item_objtype := theitem.objtype, level := _ifOr(level_select, 1) });
            theitems.append(theitem);
            sleepms(2);
         endforeach
		else
			SubtractMaterial(who, targ_materials, 0.5); //consome metade do material
			SendSysMessageEx(who, "você falhou ao fazer o item e perdeu material." , SSM_FAIL);
			//return 0;
		endif
		sleepms(100);
	endwhile

   foreach item in theitems
	   ReleaseItem(item);
   endforeach

	return theitems;	
endfunction

/**
 * SubtractMaterial - Subtracts materials from the player's inventory based on the provided target materials and amount modifier.
 * 
 * @param {who} - The character from whom the materials will be subtracted.
 * @param {targ_materials} - The target materials to be subtracted from the player's inventory.
 * @param {amountMod} - Optional. The amount modifier to be applied to the target materials.
 * @returns {int} - 1 if the materials were successfully subtracted, 0 otherwise.
 */

function SubtractMaterial(who, targ_materials, amountMod := 1)
   var container_items := EnumerateItemsInContainer(who.backpack);

	foreach thing in (targ_materials.keys())
      sleepms(2);
      
      if (amountMod < 1 && (cint(thing) == 3968 || (cint(thing) >= 39066 && cint(thing) <= 39069)))
         targ_materials.-thing;
         continue;
      endif

		targ_materials[thing] := cint(cdbl(targ_materials[thing])*amountMod);
      if (targ_materials[thing] == 0)
         continue;
      endif

      foreach item in container_items
         sleepms(5);
         if (cint(item.objtype) != thing)
            continue;
         endif

         if (item.amount >= targ_materials[thing] )
            var chk := SubtractAmount(item, cint(targ_materials[thing]));
            if (!chk ) return 0; endif
            targ_materials[thing] := 0;
            break;
         endif

         targ_materials[thing] -= item.amount;
         if (!SubtractAmount(item, item.amount))
            break;
         endif
      endforeach

      if (targ_materials[thing] != 0)
         return 0;
      endif
	endforeach
  
	return 1;
endfunction

/**
 * AddMaterialMod - Adds a material modifier to the specified item based on the provided recipe, principal material, and material color.
 * 
 * @param {recipe} - The recipe for the item being modified.
 * @param {theitem} - The item to which the material modifier will be added.
 * @param {principalMaterial} - The principal material to be used for the material modifier.
 * @param {materialColor} - The color of the material to be used for the material modifier.
 */
function AddMaterialMod(recipe, theitem, principalMaterial, materialColor)
	if (!principalMaterial)
		return;
	endif	

	var elem  := FindConfigElem(resource_config, principalMaterial);
	if((recipe.colored == 1))
		if(resource_config[principalMaterial].Color)
			theitem.color := resource_config[principalMaterial].Color;
		else
		
			theitem.color := materialColor;
		endif
	endif
	
	SetObjProperty(theitem, "material", principalMaterial);

   if (theitem.isA(POLCLASS_ARMOR) && theitem.tile_layer != 0x2)
      var resists := GetConfigStringDictionary(elem, "Resist");
      var item_resists := _ifOr(GetObjProperty(theitem, "ArmorResists"), struct{});
      foreach key in (resists.keys())
         var key_fix := "{}Resist".format(key);
         item_resists[key_fix] := cint(item_resists[key_fix]) + cint(resists[key]);
         if (item_resists[key_fix] > 4) 
            item_resists[key_fix] := 4;
         elseif (item_resists[key_fix] < -1)
            item_resists[key_fix] := -1;
         endif
      endforeach

      SetObjProperty(theitem, "ArmorResists", item_resists);
      // var resist_list := array{DMG_FIRE, DMG_POISON, DMG_COLD, DMG_ENERGY, DMG_SLASH, DMG_BLUDGE, DMG_PIERCE};

   endif

	// if (theitem.isA(POLCLASS_WEAPON) || theitem.isA(POLCLASS_ARMOR))
	// 	var hpmod := GetConfigInt(elem, "hpmod");
	// 	theitem.maxhp_mod += hpmod;
   //    theitem.hp := theitem.maxhp + theitem.maxhp_mod;
	// endif
endfunction

 function AddItemDescription(who, item)
   var desc := QuestionsGump(who, array{ struct{ title := "Entre com a descrição do item", lines := 1 }});
   if (desc)
	   SetObjProperty(item, "description", desc[1]);
	   SetName(item, _ifOr(item.name, item.desc));
   endif
endfunction

/**
 * CraftExceptional - Crafts an exceptional item using a special material.
 *
 * @param {who} - The character attempting to craft the exceptional item.
 * @param {theitem} - The item being crafted.
 * @param {craft_skill} - The crafting skill being used for the exceptional crafting.
 */
function CraftExceptional(who, theitem, craft_skill)
   SendSysMessageEx(who, "Selecione o material que deseja unir a este objeto.", SSM_REQUEST);
   var special_obj := Target(who);
   if (!special_obj)
      return;
   elseif (special_obj.isA(POLCLASS_CONTAINER))
      UseItem(special_obj, who);
      return;
   endif

   var craft_index := GetObjProperty(special_obj, "craftproperty");
   var custom_name :=  GetObjProperty(special_obj, "craftcustomname");;
   
   if (!craft_index)
      var exceptional_cfg := ReadConfigFile(":crafting:exceptional_cfg");
      var item := exceptional_cfg[hex(special_obj.objtype)];
      if (!item)
         SendSysMessageEx(who, "Material inválido, selecione outro.", SSM_FAIL);
         return 2;
      endif
      
      craft_index := GetConfigInt(item, "excepID");
      custom_name := GetConfigStringArray(item, "custom_name");
      // special_props := struct{
      //    "equips" := GetConfigStringArray(item, "equip"),
      //    "props" := GetConfigStringDictionary(item, "prop"),
      //    "skills" := GetConfigStringArray(item, "skill"),
      //    "level" := GetConfigInt(item, "level")
      // };
   endif

   var special_props := GetItemEvent(craft_index);
   if (!special_props)
      SendSysMessageEx(who, "Material inválido, selecione outro.", SSM_FAIL);
      return 2;
   endif

   if (!(craft_skill in special_props.skills))
      SendSysMessageEx(who, "O material não pode ser usado por {}".format(craft_skill), SSM_FAIL);
      return 2;
   endif

   // if (cint(special_props.diff) > cint(AP_GetTrueSkill(who, craft_skill)) - 4)
   //    SendSysMessageEx(who, "Seu nível em {} é muito baixo para usar este recurso".format(craft_skill), SSM_FAIL);
   //    return 2;
   // endif

	var item_type := "None";
   item_type := GetItemType(theitem);

   if (!(item_type in special_props.uso))
      SendSysMessageEx(who, "O material não pode ser usado nesse tipo de equipamento.", SSM_FAIL);
      return 2;
   endif
   
   applyEffect(theitem, special_props.effects);

   theitem.quality += 1;
   who.SetAchievement("exceptional", 1);
   SetObjProperty(theitem, "madeof", fixItemName(special_obj.desc));

   if (custom_name)
      theitem.name_suffix := custom_name;
   endif

   if ( YesNo(who, "Deseja adicionar uma descrição no item?"))
      AddItemDescription(who, theitem);
   endif
   DestroyItem(special_obj);
   return 1;
endfunction

/**
 * SmeltItem - Smelts an item using specific configuration and crafting skill.
 *
 * @param {who} - The character performing the smelting.
 * @param {item} - The item to be smelted.
 * @param {specific_cfg} - The specific configuration for smelting.
 * @param {craft_skill} - The crafting skill used for smelting.
 */

function fixItemName(name)
   for i := 1 to 9
      name := StrReplace(name, cstr(i), "");
      sleepms(2);
   endfor

   return Trim(name);
endfunction

/**
 * SmeltItem - Smelts an item using specific configuration and crafting skill.
 *
 * @param {who} - The character performing the smelting.
 * @param {item} - The item to be smelted.
 * @param {specific_cfg} - The specific configuration for smelting.
 * @param {craft_skill} - The crafting skill used for smelting.
 */

function SmeltItem(who, item, specific_cfg, craft_skill)
   if(item.movable == 0)
      SendSysMessageEx(who, "Você não pode consertar isto.");
      return;
   elseif(!maolivre(who, 1))
      SendSysMessageEx(who, "Você deve estar com as mãos livres");
      return;
   elseif((!Accessible(who, item)) or (distance(who, item) > 2))
      SendSysMessageEx(who, "Você não alcança o item.");
      return;
   endif

   var elem := FindConfigElem(specific_cfg, Hex(item.objtype));
   if (!elem) elem := FindConfigElem(specific_cfg, cint(item.objtype)); endif
   if (!elem) return SendSysMessageEx(who, "Este item não pode ser descontruído usando esta ferramenta.", SSM_FAIL); endif

   var level := _ifOr(GetObjProperty(item, "level"), 1);
	var recipe := GetRecipe(item.objtype, elem);

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
         SendSysMessageEx(who, "Você precisa estar perto de um {} para fazer isso.".format(group), SSM_FAIL);
         return 1;
      endif
      sleepms(2);
   endforeach

	var craft_sound  := menu_config[craft_skill].CraftSound;
	var craft_delay  := menu_config[craft_skill].CraftDelay;
	var craft_action := menu_config[craft_skill].CraftAnim;

   // var work_items := array{"Maquina de Costura", "Maquina de Tensão", "Maquina de Marcenaria"};

	var craft_loops  := GetConfigInt(elem, "delay")/ 2;
	if (craft_loops == error || craft_loops < 1)
      craft_loops := 1;
	endif
 	craft_loops := cint(craft_loops);

   playCraftWork(who, craft_sound, craft_loops, craft_delay, craft_action, 0);

   var resource_cfg := ReadConfigFile(":crafting:resourceLevels");
   var resource_list := GetConfigStringKeys(resource_cfg);

   var material_recover := dictionary{};
   foreach material in (recipe.materials.Keys())
      if (material in resource_list)
         var elem := FindConfigElem(resource_cfg, material);
         var level_items := GetConfigStringDictionary(elem, "Level");
         var value := cint(cdbl(recipe.materials[material]) / 2);
         if (value >= 1)
            material_recover[level_items[cstr(level)]] := value;
         endif
      elseif (material in array{0x0F0E, 0x1F91})
         material_recover[material] := recipe.materials[material];
      else
         var value := cint(cdbl(recipe.materials[material]) / 2);
         if (value >= 1)
            material_recover[material] := value;
         endif
      endif
   endforeach

   foreach material in (material_recover.Keys())
      _CreateItemInBackpack(who, cint(material), item.amount * material_recover[material]);
   endforeach

   if (RandomInt(100) <= 15)
      if (!recipe.requireFeat || HaveFeatSecondary(who, recipe.requireFeat) )
         AddRecipe(who, item.objtype);
         SendSysMessageEx(who, "Você aprendeu mais sobre este item!", SSM_INFO);
      endif
   endif

   DestroyItem(item);
endfunction