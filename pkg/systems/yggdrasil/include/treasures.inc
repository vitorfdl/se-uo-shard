use uo; 
use math;

include ":loot:include/lootParser";
include ":traps:traps";
include ":timeutils:time";
include ":unicos:item_template";

function ModelTreasure( data := struct{} )
    if (!data.name)
        data.name := "";
    endif

    if (data.active != 0 && data.active != 1)
        data.active := 0;
    endif

    if (TypeOf(data.loots) != "Array")
        data.loots := array{};
    endif

    if (!data.level || data.level < 1 || data.level > 5)
        data.level := 1;
    endif

    if (data.protected != 1 && data.protected != 0)
        data.protected := 0;
    endif

    if (data.trapped != 1 && data.trapped != 0)
        data.trapped := 0;
    endif

    if (!data.serial)
        data.serial := 0x0;
    endif

    if (!data.breakable)
        data.breakable := 0;
    endif

    if (TypeOf(data.yggdrasil) != "Struct")
        data.yggdrasil := struct{};
    endif

    return data;
endfunction

function CleanTreasure(treasure)
    if(!treasure.isA(POLCLASS_CONTAINER))
        SysLog("CleanTreasure used on a not container object");
        return;
    endif
    EraseObjProperty(treasure, "opened_once");
    EraseObjProperty(treasure, "trapSpoted");
    EraseObjProperty(treasure, "trap_inativa");
    EraseObjProperty(treasure, "TrapList");
    EraseObjProperty(treasure, "level");
    EraseObjProperty(treasure, "protected");
    EraseObjProperty(treasure, "breakable");

    foreach item in (EnumerateItemsInContainer( treasure ))
        DestroyItem( item );
        sleepms(2);
    endforeach
endfunction

function SpawnTreasure(treasure, config)
   if(!treasure.isA(POLCLASS_CONTAINER))
      SysLog("SpawnTreasure used on a not container object " + treasure.serial);
      return;
   endif

   if (treasure.desc["[Quebrado]"])
      var old_name := treasure.desc; 
      old_name[" [Quebrado]"] := "";
      SetName(treasure, old_name);
      var graph := GetObjProperty(treasure, "original_graphic");
      treasure.graphic := graph;

      SetObjproperty(treasure, "hits", 1+config.level);
   endif

   if (treasure.desc["[Armadilha]"])
      var old_name := treasure.desc; 
      old_name[" [Armadilha]"] := "";
      SetName(treasure, old_name);
   endif

   CleanTreasure(treasure);
   SetObjProperty(treasure, "level", config.level);
   treasure.movable := 0;

   if (config.level)
      treasure.locked := 1;
   endif

   if (config.protected)
      SetObjProperty(treasure, "protected", treasure.protected);
   endif

   if (config.breakable)
      SetObjProperty(treasure, "breakable", 1);
   endif

   if (config.trapped)
      var trap := RandomInt(3)+1;
      case(trap)
         1: AddTrap(treasure, "Needle", config.level);
         2: AddTrap(treasure, "Explosion", config.level);
         3: AddTrap(treasure, "PoisonGas", config.level);
      endcase
   endif

   foreach loot in (config.loots)
      if(loot.by_group)
         Loot_Generate(treasure, loot.by_group);
      elseif( RandomDiceRoll("1d100") <= loot.chance)
         var item := CreateItemAtLocationFromTemplate(treasure.x, treasure.y, treasure.z, _DEFAULT_REALM, loot.item );
         MoveItemToContainer(item, treasure);
      endif
      sleepms(2);
   endforeach

   var time_respawn := AddTimeTo(ReadGameClock(), 40+RandomDiceRoll("2d20"), "minutes");
   SetObjProperty(treasure, "spawned_time", time_respawn);
endfunction