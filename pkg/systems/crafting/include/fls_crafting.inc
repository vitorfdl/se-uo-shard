/* Resumo de Funções
 * PlayCraftWork(who, craftSound, loops:=0, delay:=2, craftAction:=0, fastcraft := 0)
 * GetRecipe(objtype, elem)
 * GetRecipeList(who, recipe_cfg, search := "")
 * MaterialSelection(who, craft_skill, type, chk)
 * CraftItem(who, craft_skill, tool, objtype, amt, canFix, anvil, more := 0)
 * SubtractMaterial(who, targ_materials, amountMod)
**/

use os;
use uo;
use cfgfile;
include "include/say";
include "include/objtype";
include ":attributes:attributes";
include ":crafting:fls_craftingUtils";

const OUT_OF_MATERIALS := 1;

var itemdesc_cfg    := ReadConfigFile(":*:itemdesc");
var resource_config := ReadConfigFile(":crafting:config/resourceList");
var menu_config     := ReadConfigFile(":crafting:config/craftConfig");
var requires_config := ReadConfigFile(":crafting:config/requires");
// This function is used to display a craft skill's action,
// sound, the loops to do it in, and the delay between loops.
// Sound is required, loops defaults to 0 if none sent, and delay
// defaults to 2 seconds if none sent, Action defaults to 0
// (none) if none sent. Who is who the info is to be sent to.
// All delays are in seconds.
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
         sleepms(500);
         return;
      endif
      sleep(delay);
   endif

   return;
endfunction

function GetRecipe(objtype, elem)
   var item := itemdesc_cfg[objtype];
   var graphic := item.graphic;
   if (!graphic)
      graphic := objtype;
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
      "name"        := TruncatePlural(objtype),
      "objtype"     := objtype,
      "difficulty"  := GetConfigInt(elem, "difficulty"),
      "colored"     := GetConfigInt(elem, "colored"),
      "mark"        := GetConfigInt(elem, "mark"),
      "exceptional" := GetConfigInt(elem, "exceptional"),
      "delay"       := GetConfigInt(elem, "delay"),
      "materials"  := materials,
      "requires"   :=  GetConfigStringArray(elem, "requires")
   };


   return recipe_info;
endfunction

function GetRecipeList(who, recipe_cfg, search := "")
	var items := GetObjProperty(who,  "recipes"); //pega a lista de recipes
	
	var recipelist := array{}; //lista temp pra veriicar se os items sao da skill do menu
	foreach recipe in (items)
		var elem        := FindConfigElem(recipe_cfg, recipe);
		var recipe_type := GetConfigString(elem, "type");
		if (recipe_type == "recipe")
         recipe := GetRecipe(recipe, elem);
         var found := Lower(recipe.name).find(Lower(search));
         if (search == "" || found)
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

// Function used to target material and check ability for crafting.
// Returns an array of ItemRef for the item and Objtype for the
// resourceList of the material, and the Points bonus the resource
// gives for using it. If chk is passed, it is used for the "cprop"
// to check the item for. This is used for example, for boards that
// was originally colored logs. The colored log objtype is stored
// in the cprop, and is used to get it's "ResourceList" reference.
function MaterialSelection(who, craft_skill, type, chk)

 	SendSysMessageEx(who, "Escolha o "+lower(type)+" que voce deseja trabalhar.");
	var material := Target(who);

	if(!material)
		SendSysMessage(who, "Cancelado.");
		return 0;
	endif
	
	if(material.container.serial != who.backpack.serial)
		SendSysMessage(who, "O material nao esta na sua mochila.");
		return 0;
	endif
	
	if( (!Accessible(who, material)) || (Distance(who, material) > 2)
       || (material.movable == 0) || (!ReserveItem(material)))
		SendSysMessage(who, "Voce nao pode usar isto.");
		return 0;
	endif
	
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
	// 	SendSysMessage(who, "Voce nao possui conhecimento suficiente para fazer algo com isso.");
	// 	return 0;
	// endif
	
	var rtype := lower(resource_config[materialobjtype].Type);
	var materialchk := 0;
	if(rtype == type)
		materialchk := 1;
	endif

	if(materialchk == 0)
	   SendSysMessage(who, "Esse nao e o material certo.");
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

	var retarray := array;
	retarray.append(material);
	retarray.append(materialobjtype);
	retarray.append(resource_config[materialobjtype].Ptmod);
	return retarray;
endfunction

function CraftItem(who, craft_skill, tool, recipe, amt, canFix, more := 0)
	var craft_sound  := menu_config[craft_skill].CraftSound;
	var craft_delay  := menu_config[craft_skill].CraftDelay;
	var craft_action := menu_config[craft_skill].CraftAnim;
   
	var craft_loops  := recipe.delay;
	if (craft_loops == error)
      craft_loops := 2;
	endif
 	craft_loops := cint(craft_loops);

	var materials     := recipe.materials;
	var main_material := recipe.main_material;
	//verifica se tem os objtypes na bag
	var targ_materials   := GetBagList(who, materials.Keys());
	var choosed_material := 0;
	var material_color   := 0;
	foreach thing in (materials.keys())
      if (thing in array{"metal", "madeira", "vidro", "couro", "pano", "arrowhead"})
			var ret := MaterialSelection(who, craft_skill, thing, 0);
			if (!ret)
				SendSysMessageEx(who, "Voce nao possui os materiais necessarios.", SSM_FAIL);
				return OUT_OF_MATERIALS;
			elseif (ret[1].amount < materials[thing] )
            SendSysMessageEx(who, "Voce nao possui os materiais necessarios." , SSM_FAIL);
            return OUT_OF_MATERIALS;			
			elseif (thing == "arrowhead" && cint(GetObjProperty(ret[1], "material")) != recipe.arrowtype)
            SendSysMessageEx(who, "Voce nao pode fabricar estas flechas com a ponta escolhida." , SSM_FAIL);
            return OUT_OF_MATERIALS;
         else
            targ_materials[ret[1].objtype] := materials[thing];
            if (thing == main_material)
               material_color   := ret[1].color;
               choosed_material := ret[1].objtype;
            endif
         endif
         continue;
		endif

      if ( targ_materials[thing] < (materials[thing] + more))
         SendSysMessageEx(who, "Voce nao possui os materiais necessarios." , SSM_FAIL);
         return OUT_OF_MATERIALS;
      else
         targ_materials[thing] := materials[thing];
      endif
		sleepms(5);
	endforeach

	var counter  := 0;
	var theitems := array{};
	var initialX := who.x;
	var initialY := who.y;
   var theitem;
	while (counter < amt)
		counter := counter + 1;
		if ( (who.x != initialX) || (who.y != initialY) )
			SendSysMessageEx(who, "Voce parou de se concentrar no que estava fazendo.", SSM_FAIL);
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
			   SendSysMessageEx(who, "Voce precisa estar perto de um {} para fazer isso.".format(group), SSM_FAIL);
            return 1;
         endif
         sleepms(2);
      endforeach

		var work_items := array{"Maquina de Costura", "Maquina de Tensão"};
		foreach work_item in work_items
			if (work_item in found.Keys())
				var machine := found[work_item];
				if (!ReserveItem(machine))
					SendSysMessageEx(who, "Outra pessoa esta usando {}.".format(work_item), SSM_FAIL);
				endif
				PrintText(machine, "*werck*");
				machine.graphic := machine.graphic-1;
			endif
		endforeach

		var item := 0;
		if ("Forja" in found.Keys())
         var forja := found["Forja"];
			item := CreateItemAtLocation(forja.x, forja.y, forja.z, recipe.objtype, 1, forja.realm);
			item.movable := 0;
			item.color := 1161;
			PrintTextAbove(item, "*item em brasa*");
			MoveObjectToLocation(forja.x, forja.y, (forja.z+7), forja.realm);
		endif


		playCraftWork(who, craft_sound, craft_loops, craft_delay, craft_action, recipe.fastcraft);

		if (item)
			DestroyItem(item);
		endif

		foreach work_item in work_items
			if (work_item in found.Keys())
				var machine := found[work_item];
				ReleaseItem(machine);
				machine.graphic := machine.graphic+1;
			endif
		endforeach

      var bonus := 0;
      if (HaveSkill(who, craft_skill))
         bonus := cint(getProfiencyBonus(who));
      endif

		var check := rollAttrDice(who, "Intelligence") + bonus;
		if(check > recipe.difficulty)
			if (!SubtractMaterial(who, targ_materials, 1+more))
				SendSysMessageEx(who, "Voce nao possui todos os materiais.", SSM_FAIL);
				return theitems;
			endif
			theitem := CreateItem(who, recipe.objtype, more);
			if(!theitem)
				SendSysMessageEx(who, "Voce nao tem espaco suficiente na sua mochila!", SSM_FAIL);
				return theitems;
			endif
			// CheckToolWear(who, tool, craft_skill); TODO: Solve This
			theitem.movable := 1;

			//passando a cor pra materiais coloridos que nao tem Id proprio (como madeira)
			// AddMaterialMod(who, theitem, choosed_material, material_color);
			// CraftExceptional(who, theitem, craft_skill);
			CraftLockable(who, theitem);

			SendSysMessageEx(who, "Voce criou o item o colocou na mochila.", SSM_INFO);
		
			// TODO: Gravar feitos para talentos
			if (theitem.isa(POLCLASS_WEAPON) || theitem.isa(POLCLASS_ARMOR))	
				SetObjProperty(theitem, "reparos", check * 4); // TODO: Check how this work
			endif

			if(theitem.isA(POLCLASS_CONTAINER))
				SetObjProperty(theitem, "trappable", 1);
			endif

			if ((theitem.graphic != 11058) && (theitem.graphic != 3903))
				SetObjProperty(theitem, "crafterserial", who.serial);
				SetObjProperty(theitem, "crafteracc", who.acctname);
			endif

			if (canFix)
				SetObjProperty(theitem, "canfix", 1);
			endif

			theitems.append( theitem);
		else
			SubtractMaterial(who, targ_materials, 0.5); //consome metade do material
			SendSysMessageEx(who, "Voce falhou ao fazer o item e perdeu material." , SSM_FAIL);
			//return 0;
		endif
		sleepms(100);
	endwhile

	ReleaseItem(theitem);
	return theitems;	

endfunction

function SubtractMaterial(who, targ_materials, amountMod)
	foreach thing in (targ_materials.keys() )
		//sendsysmessage(who, " materiais antes " + thing + " " +  targ_materials[thing] );
		targ_materials[thing] := Cint( cint(targ_materials[thing])*amountMod);
		if (targ_materials[thing] == 0 )
			targ_materials[thing] := 1;
		endif
      sleepms(2);
		//sendsysmessage(who, " materiais depois " + thing + " " +  targ_materials[thing] );
	endforeach

	//sendsysmessage(who, " targmaterial " + targ_materials.keys() );

	//no loop anterior procurou os materiais, agora consome.
	foreach thing in (targ_materials.keys())
	      foreach item in (EnumerateItemsInContainer(who.backpack))
				if (cint(item.objtype) == thing)
					if (item.amount >= targ_materials[thing] )
						//sendsysmessage(who, " " + item + " " + cint(targ_materials[thing]) + " " + thing);
						var chk := SubtractAmount(item, cint(targ_materials[thing]));
						if (!chk )
							SendSysMessage(who, "chk " + chk);
							return 0;
						endif
						targ_materials[thing] := 0;
						break;
					else
						targ_materials[thing] := targ_materials[thing] - item.amount;
						if (!SubtractAmount(item, item.amount))
							return 0;
						endif
					endif
				endif
            sleepms(2);
			endforeach
	endforeach
  
	return 1;
endfunction