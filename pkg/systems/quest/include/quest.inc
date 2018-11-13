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

function ListQuests(classe := 0)
   var quest_list := DFGetElemNames(LoadQuestData());
   var return_list := array{};
   foreach quest_id in quest_list
      sleepms(2);
      var quest := GetQuestData(quest_id, 0);
      if (classe && classe != quest.category)
         continue;
      endif
      return_list.append(quest);
   endforeach

   return return_list;
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
    data.+id       := id;
    data.+title       := DFGetProp(elem, "title", create, "");
    data.+category    := DFGetProp(elem, "category", create, "Todos");
    data.+active      := DFGetProp(elem, "active", create, 0);
    data.+description := DFGetProp(elem, "description", create, "");
    data.+level       := DFGetProp(elem, "level", create, 1);
    data.+rewards     := DFGetProp(elem, "rewards", create, array{});
    data.+conditions  := DFGetProp(elem, "conditions", create, array{});
    data.+extras      := DFGetProp(elem, "extras", create, array{});

    return data;
endfunction

function RemoveQuestData(id)
    var df   := LoadQuestData();
    df.DeleteElement(cstr(id));
endfunction

function GiveQuest(who, quest)
   var quest_data := GetObjProperty(who, "quests");
   if (!quest_data) quest_data := array{}; endif

   var weekly_quests := GetObjProperty(who, "weekly_quests");
   if (!weekly_quests) weekly_quests := array{}; endif

   var quests_of_type := FindAllElemInArray(quest_data, struct{ category := quest.category });
   var weekly_quests_total := FindAllElemInArray(weekly_quests, struct{ classe := quest.category });
   var f := _ifElse(quests_of_type, quests_of_type.size(), 0) + _ifElse(weekly_quests_total, weekly_quests_total.size(), 0);
   if (f >= 2) return SendSysMessageEx(who, "Limite de missões dessa classe esgotado.", SSM_FAIL); endif

   quest_data.append(quest);
   SetObjProperty(who, "quests", quest_data);
endfunction

function ProgressQuest(who, search, params)
   var quest_data := GetObjProperty(who, "quests");
   if (!quest_data) return;
   elseif (quest_data.size() <= 0) return; endif

   var quest_indexes := _ProgressCompare(quest_data, search);
   if (!quest_indexes.size()) return; endif
   foreach i in quest_indexes
      if (!quest_data[i].done)
         var can_done := 1;
         foreach condition in (quest_data[i].conditions)
            if (condition.done || condition.type != search.type) continue; endif
            foreach p in (params.Keys())
               condition[p] -= params[p];
               sleepms(2);
               if (condition[p] > 0)
                  can_done := 0;
               endif
            endforeach

            if (can_done) 
               condition.done := 1;
            endif
         endforeach
      endif
      sleepms(2);
   endforeach

   SetObjProperty(who, "quests", quest_data);
endfunction

function _ProgressCompare(quests, search)
   var return_keys := array{};
   foreach quest in ( quests )
      var conditions := quest.conditions;
      var can_add := 0;
      foreach condition in conditions
         var condition_pass := 1;
         foreach key in (search.Keys())
            if (key == "spawngroup" && condition[key] == "Qualquer Um")
            elseif (key == "party")
               if (search[key] == 1 && condition[key] == 0)
                  condition_pass := 0;
               endif
            elseif (condition[key] != search[key])
               condition_pass := 0;
            endif
         endforeach
         if (condition_pass == 1) return_keys.append(_quest_iter); break; endif
         sleepms(2);
      endforeach
      sleepms(2);
   endforeach

   return return_keys;
endfunction

function CheckQuest(who, quest)
   foreach condition in (quest.conditions)
      if (!CheckCondition(who, condition)) return 0; endif
      sleepms(2);
   endforeach

   return 1;
endfunction

function CheckCondition(who, condition)
   if (condition.done) 
      return 1;
   elseif (condition.type == "Coletar Item")
      if (checkItemAmount(who, condition.objtype) < condition.qty)
         return 1;
      endif
   endif
endfunction

function checkItemAmount(who, objtype)
   var backpack := EnumerateItemsInContainer(who.backpack, ENUMERATE_IGNORE_LOCKED);
   var list := FindAllElemInArray(backpack, struct{ objtype := objtype });
   var amount := 0;
   foreach item in list
      amount += GetAmount(item);
      sleepms(2);
   endforeach

   return amount;
endfunction