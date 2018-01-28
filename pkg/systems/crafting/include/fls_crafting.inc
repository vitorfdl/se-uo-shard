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
include ":tn:tngumps";

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
         sleepms(200);
         return;
      endif
      sleep(delay);
   endif

   return;
endfunction

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
      "name"        := TruncatePlural(objtype),
      "objtype"     := objtype,
      "difficulty"  := GetConfigInt(elem, "difficulty"),
      "colored"     := GetConfigInt(elem, "colored"),
      "mark"        := GetConfigInt(elem, "mark"),
      "exceptional" := GetConfigInt(elem, "exceptional"),
      "delay"       := GetConfigInt(elem, "delay"),
      "fastcraft"       := GetConfigInt(elem, "fastcraft"),
      "more"       := GetConfigInt(elem, "more"),
      "materials"  := materials,
      "main_material" := elem.main_material,
      "requires"   :=  GetConfigStringArray(elem, "requires")
   };
   return recipe_info;
endfunction

function GetRecipeList(who, recipe_cfg, search := "")
	var items := GetObjProperty(who,  "recipes"); //pega a lista de recipes
	
	var recipelist := array{}; //lista temp pra veriicar se os items sao da skill do menu
	foreach recipe in (items)
		var elem        := FindConfigElem(recipe_cfg, Hex(recipe));
      if (!elem) elem := FindConfigElem(recipe_cfg, cint(recipe)); endif
		var recipe_type := GetConfigString(elem, "Type");
		if (recipe_type == "Recipe")
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
function MaterialSelection(who, type, chk, craft_skill)
	var name;
	if (type["monsterpart"])
		name := "parte de criatura";
	elseif (type["demonbone"])
		name := "osso de demônio";
	else
		name := type;
	endif

 	SendSysMessageEx(who, "Escolha o "+lower(name)+" que você deseja trabalhar.");
	var material := Target(who);

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
	
   if (!HaveFeat(who, "Formidável {}".format(craft_skill))
      if (thing == "metal")
         if (material.objtype != 0x6309)
            SendSysMessageEx(who, "Você não sabe trabalhar com isto.");
            return 0;
         endif
      elseif (thing == "couro")
         if (material.objtype != 0xee45 && material.color != 0)
            SendSysMessageEx(who, "Você não sabe trabalhar com isto.");
            return 0;
         endif
      elseif (thing == "madeira")
         if (material.objtype != 0x1BD7 && material.color != 0)
            SendSysMessageEx(who, "Você não sabe trabalhar com isto.");
            return 0;
         endif
      endif
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

   var diff := cint(resource_config[materialobjtype].Skill);

	var retarray := array;
	retarray.append(material);
	retarray.append(materialobjtype);
	retarray.append(diff);
	return retarray;
endfunction

function CraftItem(who, craft_skill, tool, recipe, amt, canFix)
	var craft_sound  := menu_config[craft_skill].CraftSound;
	var craft_delay  := menu_config[craft_skill].CraftDelay;
	var craft_action := menu_config[craft_skill].CraftAnim;
   
	var craft_loops  := recipe.delay;
	if (craft_loops == error)
      craft_loops := 2;
	endif
 	craft_loops := cint(craft_loops);

	var materials     := recipe.materials;
	var more     := recipe.more;
	var main_material := recipe.main_material;
	//verifica se tem os objtypes na bag
	var targ_materials   := GetBagList(who, materials.Keys());
	var choosed_material := 0;
	var material_color   := 0;
   var bonus_diff := 0;
	var material_cprops := struct{};
	foreach thing in (materials.keys())
      if (thing in array{"metal", "madeira", "vidro", "couro", "pano", "pontaflecha", "demonbone"} || (typeof(thing) == "String" && thing["monsterpart"]))
			var ret := MaterialSelection(who, thing, 1, craft_skill);
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
            targ_materials[ret[1].objtype] := materials[thing];
				if (thing == "demonbone")
					material_cprops["magicslot"] := GetObjProperty(ret[1], "power");
				elseif (thing["monsterpart"])
					material_cprops["element"] := GetObjProperty(ret[1], "element");
					material_cprops["elementpower"] := GetObjProperty(ret[1], "elementpower");
					material_cprops["madeof"] := ret[1].desc;
				endif
				if (ret[3])
               bonus_diff += cint(ret[3]);
            endif
            
            if (thing == main_material)
               material_color   := ret[1].color;
               choosed_material := ret[1].objtype;
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
				var machine := found[work_item];
				ReleaseItem(machine);
				machine.graphic := machine.graphic+1;
			endif
		endforeach

		var roll := rollAttrDice(who, WISDOM, 0, struct{ "get_critical" := 1}) + getProficiencyBonus(who, craft_skill);
      var check := roll.dice;
      var diff := recipe.difficulty + bonus_diff;
		if(check > recipe.difficulty || who.cmdlevel > 4)
			if (!SubtractMaterial(who, targ_materials))
				SendSysMessageEx(who, "você não possui todos os materiais.", SSM_FAIL);
				return theitems;
			endif

			theitem := CreateItem(who, recipe.objtype, more);
			if(!theitem)
				SendSysMessageEx(who, "você não tem espaço suficiente na sua mochila!", SSM_FAIL);
				return theitems;
         elseif (recipe.colored)
            theitem.color := material_color;
			endif
			CheckToolWear(who, tool, craft_skill);
			theitem.movable := 1;

			//passando a cor pra materiais coloridos que não tem Id proprio (como madeira)
			AddMaterialMod(who, recipe, theitem, choosed_material, material_color);
         if (recipe.exceptional == 1 && HaveFeat(who, "Mestre em {}".format(craft_skill)) && roll.critical == 1)
            SendSysMessageEx(who, "Você fez uma obra prima!.", SSM_INFO);
            while (who.connected)
			      if (CraftExceptional(who, theitem, craft_skill) != 2)
                  break;
               endif
               sleepms(2);
            endwhile
         endif

			foreach key in (material_cprops.Keys())
				SetObjProperty(theitem, key, material_cprops[key]);
			endforeach
			CraftLockable(who, theitem);

			ExpGain(who, recipe.difficulty, check);
         SetObjProperty(who, "#craft_last", recipe.objtype);
			who.SetAchievement("craft_{}".format(lower(craft_skill)), 1);
			SendSysMessageEx(who, "Você criou o item o colocou na mochila.", SSM_INFO);
		
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
			SendSysMessageEx(who, "você falhou ao fazer o item e perdeu material." , SSM_FAIL);
			//return 0;
		endif
		sleepms(100);
	endwhile

	ReleaseItem(theitem);
	return theitems;	

endfunction

function SubtractMaterial(who, targ_materials, amountMod := 1)
	foreach thing in (targ_materials.keys() )
		//sendsysmessageEx(who, " materiais antes " + thing + " " +  targ_materials[thing] );
		targ_materials[thing] := Cint( cint(targ_materials[thing])*amountMod);
		if (targ_materials[thing] == 0 )
			targ_materials[thing] := 1;
		endif
      sleepms(2);
		//sendsysmessageEx(who, " materiais depois " + thing + " " +  targ_materials[thing] );
	endforeach

	//sendsysmessageEx(who, " targmaterial " + targ_materials.keys() );

	//no loop anterior procurou os materiais, agora consome.
	foreach thing in (targ_materials.keys())
      foreach item in (EnumerateItemsInContainer(who.backpack))
         if (cint(item.objtype) == thing)
            if (item.amount >= targ_materials[thing] )
               //sendsysmessageEx(who, " " + item + " " + cint(targ_materials[thing]) + " " + thing);
               var chk := SubtractAmount(item, cint(targ_materials[thing]));
               if (!chk )
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
      if (targ_materials[thing] != 0)
         return 0;
      endif
	endforeach
  
	return 1;
endfunction

function AddMaterialMod(who, recipe, theitem, principalMaterial, materialColor)
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

	if (theitem.isA(POLCLASS_WEAPON) || theitem.isA(POLCLASS_ARMOR))
		var hpmod := GetConfigInt(elem, "hpmod");
		theitem.maxhp_mod += hpmod;
      theitem.hp := theitem.maxhp + theitem.maxhp_mod;
	endif
endfunction

 function AddItemDescription(who, item)
	var repeating := 1;
	var str;
	SendSysMessageEx(who, "Utilize apenas 3 linhas da janela.");
	while (repeating)
		str := SmallRequestGump(who, "Descreva o item", 1);
		if (len(str) < 84)
			break;
		endif
		SendSysMessageEx(who, "Você excedeu o número máximo de caracteres! Utilize apenas 3 linhas da janela.");
	endwhile
	SetObjProperty(item, "description", str);
endfunction

function CraftExceptional(who, theitem, craft_skill)
   SendSysMessageEx(who, "Selecione o material que deseja unir a este objeto.", SSM_REQUEST);
   var special_obj := Target(who);
   if (!special_obj)
      return;
   endif

   var special_props := GetObjProperty(special_obj, "craftproperty");

   if (!special_props)
      var exceptional_cfg := ReadConfigFile(":crafting:exceptional_cfg");
      var item := exceptional_cfg[hex(special_obj.objtype)];
      if (!item)
         SendSysMessageEx(who, "Material inválido, selecione outro.", SSM_FAIL);
         return 2;
      endif
      
      special_props := struct{
         "equips" := GetConfigStringArray(item, "equip"),
         "props" := GetConfigStringDictionary(item, "prop"),
         "skills" := GetConfigStringArray(item, "skill")
      };
   endif

   if (!(craft_skill in special_props.skills))
      SendSysMessageEx(who, "O material não pode ser usado por {}".format(craft_skill), SSM_FAIL);
      return 2;
   endif

	var item_type := "None";
	if (theitem.isA(POLCLASS_ARMOR))
		item_type := "Armaduras";
	elseif (theitem.isBow())
		item_type := "Arcos";
	elseif (theitem.isA(POLCLASS_WEAPON))
		item_type := "Armas";
	elseif (theitem.isA(POLCLASS_EQUIPMENT))
		item_type := "Joias";
	elseif (theitem.isA(POLCLASS_CONTAINER))
		item_type := "Containers";
	endif

   if (!(item_type in special_props.equips))
      SendSysMessageEx(who, "O material não pode ser usado nesse tipo de equipamento.", SSM_FAIL);
      return 2;
   endif

   var custom_name := "";
   var prop_list := special_props.props;
   foreach prop in prop_list
      case (prop)
         "custom_name":
            custom_name := prop_list[prop];
            break;
         "hpmod":
            theitem.maxhp_mod += cint(prop_list[prop]);
            break;
         "equipmod":
            prop := SplitWords(prop_list[prop]);
            var mod := prop[1];
            var value := cint(prop[2]);

            var equip := GetObjProperty(theitem, "equip");
            if (!equip)
               equip := struct{};
            endif
            equip[mod] := value;
            SetObjProperty(theitem, "equip", equip);
            break;
         "dmgbonus":
            SetObjProperty(theitem, "dmgbonus", cint(prop_list[prop]));
            break;
         "armod":
            theitem.ar_mod += cint(prop_list[prop]);
            break;
         "imunity":
            break;
         "resist":
            break;
      endcase
      sleepms(2);
   endforeach

   theitem.quality := 1.1;
   who.SetAchievement("exceptional", 1);
   SetObjProperty(theitem, "madeof", special_obj.name);

   if ( YesNo(who, "Deseja adicionar uma descrição no item?"))
      AddItemDescription(who, theitem);
   endif
   SetName(theitem, theitem.desc);
   DestroyItem(special_obj);
   return 1;
endfunction