use uo;
use util;
use cfgfile;
use os;

include ":loot:magicLoot";

const RECIPE_OBJ := 0xef1f;
const FEATBOOK_OBJ := 0xee90;

function Loot_Generate(container, loot_index, is_group := 0)
	if ( !container.IsA(POLCLASS_CONTAINER) )
		return error{"errortext" := "`ect passed was not a container."};
	endif

   if (is_group && is_group.by_group)
	   var index_cfg := ReadConfigFile(":loot:config/loot-groups");
      if ( index_cfg.errortext )
         return error{"errortext" := "Error - Could not open loot-index.cfg -> "+index_cfg.errortext};
      endif
      
      var index_elem := index_cfg[loot_index];
      if ( !index_elem )
         return error{"errortext" := "Error - loot-index.cfg elem ["+index_elem+"] does not exist."};
      //print( "Error - loot-index.cfg elem ["+index_elem+"] does not exist.");
      endif

      var type := GetConfigStringArray(index_elem, "Type");
      var line := "{} {} {}".format(is_group.by_group, is_group.rate, is_group.chance);
      Loot_CreateRandom(container, line, is_group.forceitem, is_group.power, is_group.defense);

      return 1;
   endif


	var index_cfg := ReadConfigFile(":loot:config/loot-index");
	if ( index_cfg.errortext )
		return error{"errortext" := "Error - Could not open loot-index.cfg -> "+index_cfg.errortext};
	endif
   
	var index_elem := index_cfg[loot_index];
	if ( !index_elem )
		return error{"errortext" := "Error - loot-index.cfg elem ["+index_elem+"] does not exist."};
    //print( "Error - loot-index.cfg elem ["+index_elem+"] does not exist.");
	endif

	foreach line in GetConfigStringArray(index_elem, "Item")
		Loot_CreateItem(container, line, cint(is_group.power), cint(is_group.defense));
		sleepms(2);
	endforeach

	foreach line in GetConfigStringArray(index_elem, "Stack")
		Loot_CreateStack(container, line);
		sleepms(2);
	endforeach

	foreach line in GetConfigStringArray(index_elem, "Random")
		Loot_CreateRandom(container, line, 0, cint(is_group.power), cint(is_group.defense));
		sleepms(2);
	endforeach

	foreach line in GetConfigStringArray(index_elem, "Recipe")
		Loot_CreateItem(container, line, 0, 0, "recipe");
		sleepms(2);
	endforeach

	foreach line in GetConfigStringArray(index_elem, "TalentBook")
		Loot_CreateItem(container, line, 0, 0, "talentbook");
		sleepms(2);
	endforeach

	foreach line in GetConfigStringArray(index_elem, "Group")
		if ( line != loot_index )
			//To do:
			// Check to make sure were not going back into the same group
			// to avoid a potential endless loop!
			Loot_Generate(container, line);
		endif
		sleepms(2);
	endforeach

	foreach line in GetConfigStringArray(index_elem, "Magic");
		Loot_CreateMagic(container, line);
		sleepms(2);
	endforeach

	return 1;
endfunction



function Loot_CreateItem(container, line, power := 0, defense := 0, type := 0)
	var parsed := Loot_ParseString(line);
	if ( Loot_ChanceCheck(parsed.chance) )
		var i;
		for ( i:=1; i<= parsed.amount; i:=i+1 )
			if (!type)
				var item := CreateItemInContainer(container, parsed.name);
				item.quality := 0.6;
            item.movable := 1;
				if ( item.errortext )
				//print("Error - Could not create ["+parsed.name+"] ->"+item.errortext + " " + parsed); 
					return error{"errortext" := "Error - Could not create ["+parsed.name+"] ->"+item.errortext};
            elseif (item.isA(POLCLASS_ARMOR) || item.isA(POLCLASS_WEAPON))
               if (power) item.dmg_mod := power - RandomInt(20) + RandomInt(20); endif
               if (defense) item.resist_physical_mod := defense - RandomInt(20) + RandomInt(20); endif
				endif
         elseif (type == "talentbook")
            var item := CreateItemInContainer(container, FEATBOOK_OBJ);
            var feat := StrReplace(parsed.name, "_", " ");
            SetObjproperty(item, "feat", feat);

			elseif (type == "recipe")
				var objtype := GetObjtypeByName(parsed.name);

				if (!objtype)
					objtype := Hex(parsed.name);
               if (!objtype)
                  objtype := parsed.name;
               endif
				endif

				if (!objtype || objtype == 0x0 || objtype == 0 || objtype == "0x0")
					return error{"errortext" := "Error - Could not create ["+parsed.name+"] -> Objtype not found"};
				endif

				var item := CreateItemInContainer(container, RECIPE_OBJ);
				if ( item.errortext )
				//print("Error - Could not create ["+parsed.name+"] ->"+item.errortext + " " + parsed); 
					return error{"errortext" := "Error - Could not create ["+parsed.name+"] ->"+item.errortext};
				endif

            var force_lang := GetObjProperty(container, "lang");
            if (force_lang)
               SetObjProperty(item, "lang", force_lang);
            elseif (RandomInt(101) < 10)
               SetObjProperty(item, "lang", "Linguagem Rúnica");
            elseif (RandomInt(101) < 10)
               SetObjProperty(item, "lang", "Linguagem Élfica");
            endif
            
				SetObjProperty(item, "objtype", objtype);
				run_script(":crafting:recipes/oncreate", item);
			endif
			sleepms(2);
		endfor
	endif
	
	return 1;
endfunction

function Loot_CreateStack(container, line)
	var parsed := Loot_ParseString(line);
	if ( Loot_ChanceCheck(parsed.chance) )
		var item := CreateItemInContainer(container, parsed.name, parsed.amount);
		item.quality := 0.6;
		if ( item.errortext )
			return error{"errortext" := "Error - Could not create ["+parsed.name+"] ->"+item.errortext};
		endif
	endif
	
	return 1;
endfunction

function Loot_CreateRandom(container, line, forcetype := 0, power := 0, defense := 0)
	var parsed := Loot_ParseString(line);
	var groups_cfg := ReadConfigFile(":loot:config/loot-groups");
	if ( groups_cfg.errortext )
		return error{"errortext" := "Error - Could not open loot-groups.cfg -> "+groups_cfg.errortext};
	endif
	
	var groups_elem := groups_cfg[parsed.name];
	if ( !groups_elem )
		return error{"errortext" := "Error - loot-groups.cfg elem ["+parsed.name+"] does not exist."};
	endif
	
	if ( Loot_ChanceCheck(parsed.chance) )
		var items := GetConfigStringArray(groups_elem, "Item");
		var recipes := GetConfigStringArray(groups_elem, "Recipe");
		var talentbooks := GetConfigStringArray(groups_elem, "TalentBook");

      var num_items := items.size() + recipes.size() + talentbooks.size();
      var i;
      for ( i:=1; i<=parsed.amount; i:=i+1 )
         var random := RandomInt(num_items)+1;
         if (random > items.size()+recipes.size())
            random := random - items.size()+recipes.size();
            Loot_CreateItem(container, talentbooks[random], 0, 0, "talentbook");
         elseif (random > items.size())
            random := random - items.size();
            if (!forcetype)
               Loot_CreateItem(container, recipes[random], 0, 0, "recipe");
            else
               Loot_CreateItem(container, recipes[random], power, defense);
            endif
         else
            Loot_CreateItem(container, items[random], power, defense);
         endif
         sleepms(2);
      endfor
	endif
	
	return 1;
endfunction

function Loot_ChanceCheck(chance := 1)
	if ( RandomInt(1000)+1 <= CInt(CDbl(chance)*10.0) )
		return 1;
	else
		return 0;
	endif
endfunction

function Loot_ParseString(line)
	line := SplitWords(line);
	
	var name := CStr(line[1]);
	
	var amount := CStr(line[2]);
   if(!amount || amount["error"])
      amount := 1;
   else
	   amount := RandomDiceRoll(amount);
   endif
	
	var chance := CDbl(line[3]);
	if ( !chance )
		chance := 100.0;
	endif

	var type := cstr(line[4]);

	var result := struct;
	result.+name := name;
	result.+amount := amount;
	result.+chance := chance;
	result.+type := type;

	return result;
endfunction

function LootGroupGump(who)
   var loot_cfg  := ReadConfigFile(":loot:config/loot-groups");
   var loot_list := GetConfigStringKeys(loot_cfg);
   var input_list := array{};
   input_list.append(struct{ "title" := "Qual o grupo de loot?", "marked" := 1, "checkbox" := loot_list});
   input_list.append(struct{ "title" := "Quantos itens podem dropar?", "marked" := "1d4" });
   input_list.append(struct{ "title" := "Qual a chance de drop?", "marked" := "60" });
   input_list.append(struct{ "title" := "Qual a base de PODER e DEFESA?", "marked" := array{20, 5}, "multipleanswer" := 2 });
   input_list.append(struct{ "title" := "Itens incomuns podem vir neste tesouro?", "marked" := array{1}, checkbox := array{"Bom", "Raro", "Exóticos"} });
   input_list.append(struct{ "title" := "Retornar itens ao invés de recipes?", "marked" := 2, "radio" := array{"Sim", "Não"} });
   var output := QuestionsGump(who, input_list);
   if (!output)
      return;
   endif

   var loot_array := array{};
   foreach glist in (output[1])
      var loot := struct{
         "by_group":= glist,
         "rate":= output[2],
         "chance":= cint(output[3]),
         "power" := cint(output[4][1]),
         "defense" := cint(output[4][2]),
         "qualities" := output[5],
         "forceitem" := output[6] == "Sim"
      };
      loot_array.append(loot);
      sleepms(2);
   endforeach

   return loot_array;
endfunction