use uo;
use util;
use cfgfile;
use os;

include ":loot:magicLoot";

const RECIPE_OBJ := 0xef1f;

function Loot_Generate(container, loot_index)
	if ( !container.IsA(POLCLASS_CONTAINER) )
		return error{"errortext" := "`ect passed was not a container."};
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
		Loot_CreateItem(container, line);
		sleepms(2);
	endforeach

	foreach line in GetConfigStringArray(index_elem, "Stack")
		Loot_CreateStack(container, line);
		sleepms(2);
	endforeach

	foreach line in GetConfigStringArray(index_elem, "Random")
		Loot_CreateRandom(container, line);
		sleepms(2);
	endforeach

	foreach line in GetConfigStringArray(index_elem, "Recipe")
		Loot_CreateItem(container, line, "recipe");
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



function Loot_CreateItem(container, line, type := 0)
	var parsed := Loot_ParseString(line);
	if ( Loot_ChanceCheck(parsed.chance) )
		var i;
		for ( i:=1; i<= parsed.amount; i:=i+1 )
			if (!type)
				var item := CreateItemInContainer(container, parsed.name);
				item.quality := 0.6;
				if ( item.errortext )
				//print("Error - Could not create ["+parsed.name+"] ->"+item.errortext + " " + parsed); 
					return error{"errortext" := "Error - Could not create ["+parsed.name+"] ->"+item.errortext};
				endif
			elseif (type == "recipe")
				var objtype := GetObjtypeByName(parsed.name);

				if (!objtype)
					objtype := Hex(parsed.name);
               if (!objtype)
                  objtype := parsed.name;
               endif
				endif

				if (!objtype)
					return error{"errortext" := "Error - Could not create ["+parsed.name+"] -> Objtype not found"};
				endif

				var item := CreateItemInContainer(container, RECIPE_OBJ);
				SetObjProperty(item, "objtype", objtype);
				if ( item.errortext )
				//print("Error - Could not create ["+parsed.name+"] ->"+item.errortext + " " + parsed); 
					return error{"errortext" := "Error - Could not create ["+parsed.name+"] ->"+item.errortext};
				endif
            
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

function Loot_CreateRandom(container, line)
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

      var num_items := items.size() + recipes.size();
      var i;
      for ( i:=1; i<=parsed.amount; i:=i+1 )
         var random := RandomInt(num_items)+1;
         if (random > items.size())
            random := random - items.size();
            Loot_CreateItem(container, recipes[random], "recipe");
         else
            Loot_CreateItem(container, items[random]);
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

	var level := CInt(line[4]);
	if (!level)
		level := 1;
	endif

	var result := struct;
	result.+name := name;
	result.+amount := amount;
	result.+chance := chance;
	result.+level := level;

	return result;
endfunction

