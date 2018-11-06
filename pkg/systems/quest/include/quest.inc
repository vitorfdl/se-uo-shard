use os;
use datafile;

include ":datafile:datafile";

/**
 * [Commands for SpawnGroups]
 * LoadQuestData() - Get the DataFile document
 * SetSpawnGroupData(group_name, group_struct) - Set changes to a group;
 * GetSpawnGroupData(group_name) - Get all info about a group;
 * RemoveGroupData(group_name) - Remove a group;
 * RemoveGroupMobData(group_name, index_mob) - Remove a mob from mobList. Need index;
 * UpdateGroupMobData(group_name, mob_struct, index) - Update all informations about a mob.
 */

/*=============================================
=       SpawnGroups functions code            =
=============================================*/

function LoadQuestData()
    var df := DFOpenDataFile( ":quest:quests", DF_CREATE, DF_KEYTYPE_STRING);
    return df;
endfunction

function SetQuestData(id, data := struct{})
   id := cstr(id);
   var df   := LoadQuestData();
   var elem := DFFindElement(df, id, DF_CREATE);
   var keys := data.keys();

   if ("title" in keys) elem.SetProp("title", data.title); endif
   if ("category" in keys) elem.SetProp("category", data.category); endif
   if ("active" in keys) elem.SetProp("active", data.active); endif
   if ("description" in keys) elem.SetProp("description", data.description); endif
   if ("level" in keys) elem.SetProp("level", data.level); endif
   if (data.conditions)
      elem.SetProp("conditions", data.conditions); 
   endif

   if (data.extras)
      elem.SetProp("extras", data.extras); 
   endif

   if (data.rewards)
      elem.SetProp("rewards", data.rewards); 
   endif
   //  if (data.mob_list)
   //      foreach entry in (data.mob_list)
   //          entry := ModelMobListData(entry);
   //          sleepms(2);
   //      endforeach
   //      elem.SetProp("mob_list", data.mob_list);
   //  endif

   //  if (data.evententry)
   //      elem.SetProp("evententry", data.evententry);
   //  endif

endfunction

function GetQuestData(id, create := 1)
   id := cstr(id);
    var df   := LoadQuestData();
    var elem := DFFindElement(df, id, create);
   
    if (!elem)
        return 0;
    endif

    var data := struct{};
    data.+title   := DFGetProp(elem, "title", create, "");
    data.+category := DFGetProp(elem, "category", create, "Todos");
    data.+active := DFGetProp(elem, "active", create, 0);
    data.+description := DFGetProp(elem, "description", create, "");
    data.+level := DFGetProp(elem, "level", create, 1);
    data.+rewards := DFGetProp(elem, "rewards", create, array{});
    data.+conditions := DFGetProp(elem, "conditions", create, array{});
    data.+extras := DFGetProp(elem, "extras", create, array{});

    return data;
endfunction

function RemoveQuestData(id)
    var df   := LoadQuestData();
    df.DeleteElement(cstr(id));
endfunction
