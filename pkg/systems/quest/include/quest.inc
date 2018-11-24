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
      if (!quest.active)
         continue;
      elseif (classe && !(classe in quest.category))
         continue;
      elseif (classe)
         quest.category := classe;
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
   if ("difficulty" in keys) elem.SetProp("difficulty", data.difficulty); endif
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
    data.+category    := DFGetProp(elem, "category", create, array{});
    data.+active      := DFGetProp(elem, "active", create, 0);
    data.+description := DFGetProp(elem, "description", create, "");
    data.+level       := DFGetProp(elem, "level", create, 1);
    data.+rewards     := DFGetProp(elem, "rewards", create, array{});
    data.+conditions  := DFGetProp(elem, "conditions", create, array{});
    data.+extras      := DFGetProp(elem, "extras", create, array{});
    data.+difficulty      := DFGetProp(elem, "difficulty", create, 1);

    return data;
endfunction

function RemoveQuestData(id)
    var df   := LoadQuestData();
    df.DeleteElement(cstr(id));
endfunction

function GiveQuest(who, quest, classe := 0, forced := 0)
   quest.category := _ifOr(classe, quest.category[1]);

   var quest_data := GetObjProperty(who, "quests");
   if (!quest_data) quest_data := array{}; endif

   var weekly_quests := GetObjProperty(who, "weekly_quests");
   if (!weekly_quests) weekly_quests := array{}; endif

   if (!forced)
      var quests_of_type := FindAllElemInArray(quest_data, struct{ category := quest.category });
      var weekly_quests_total := FindAllElemInArray(weekly_quests, struct{ classe := quest.category });
      var f := _ifElse(quests_of_type, quests_of_type.size(), 0) + _ifElse(weekly_quests_total, weekly_quests_total.size(), 0);
      if (f >= 2) return SendSysMessageEx(who, "Limite de missões dessa classe esgotado.", SSM_FAIL); endif
   endif
   
   var quest_progress := GetObjProperty(who, "quest_progress");
   if (!quest_progress) quest_progress := array{}; endif

   foreach condition in (quest.conditions)
      var quest_id := "{}{}{}".format(quest.id, polcore().systime, _condition_iter);
      condition.id := quest_id;
      quest_progress.append(condition);
      condition := quest_id;
      sleepms(2);
   endforeach

   quest_data.append(quest);
   SetObjProperty(who, "quests", quest_data);
   SetObjProperty(who, "quest_progress", quest_progress);
endfunction

function ProgressQuest(who, search, plus_progress := 1)
   var quest_progress := GetObjProperty(who, "quest_progress");
   foreach item in quest_progress
      sleepms(2);
      if (item.qty && cint(item.qty) <= item.progress) continue;
      elseif (item.done) continue; endif
      
      if (item.party) search.-party; endif
      if (!item.spawnpoint) search.-spawnpoint; endif
      if (!item.onlyboss && search.onlyboss) search.-onlyboss; endif

      var no_progress := 0;
      foreach key in (search.Keys())
         sleepms(2);
         if (key == "color") item[key] := cint(item[key]); search[key] := cint(search[key]); endif
         
         if (item[key] != search[key])
            no_progress := 1;
            break;
         endif
      endforeach

      if (no_progress)
      elseif (item.qty) item.progress := cint(item.progress) + plus_progress;
      else item.done := 1; endif
   endforeach
   SetObjProperty(who, "quest_progress", quest_progress);
endfunction

function CheckQuest(who, quest)
   var quest_progress := GetObjProperty(who, "quest_progress");
   foreach id in (quest.conditions)
      var item := FindElemInArray(quest_progress, struct{ id := id });
      if (item.type == "Coletar Item")
         if (checkItemAmount(who, item.item_objtype) < item.qty)
            return 0;
         endif
      elseif (item.qty)
         if (item.qty > cint(item.progress))
            return 0;
         endif
      elseif (!item.done)
         return 0;
      endif
      sleepms(2);
   endforeach

   return 1;
endfunction

function CompleteQuest(who, quest_pos)
   var quest_data := GetObjProperty(who, "quests");
   var quest := quest_data[quest_pos];
   if (!quest) return; endif

   var quest_progress := GetObjProperty(who, "quest_progress");
   if (!quest_progress) 
      quest_data.Erase(quest_pos);
      SetObjProperty(who, "quests", quest_data);
      return;
   endif

   var consume_list := dictionary{};
   foreach id in (quest.conditions)
      var i := FindInArray(quest_progress, struct{ id := id });
      var item := quest_progress[i];
      if (item.type == "Coletar Item")
         consume_list[item.item_objtype] := item.qty;
      endif

      quest_progress.Erase(i);
      sleepms(2);
   endforeach

   if (consume_list.Keys().size())
      if (!SubtractMaterial(who, consume_list))
         return 0;
      endif
   endif

   var weekly_quests := GetObjProperty(who, "weekly_quests");
   if (!weekly_quests) weekly_quests := array{}; endif
   weekly_quests.append(struct{ classe := quest.category, quest_id := quest.id, when := polcore().systime });
   SetObjProperty(who, "weekly_quests", weekly_quests);
   foreach reward in (quest.rewards)
      case (reward.type)
         "Experiência":
            if (reward.classe == "Personagem")
               ExpGain(who, 100);
               SendSysMessageEx(who, "Você recebeu experiência de personagem.");
            elseif (reward.classe == "Classe Da Quest")
               ClassExpGain(who, quest.category, 1);
               SendSysMessageEx(who, "Você recebeu experiência para a classe {}".format(quest.category));
            else
               ClassExpGain(who, reward.classe, 1);
               SendSysMessageEx(who, "Você recebeu experiência para a classe {}".format(reward.classe));
            endif
            break;
         "Item":
            var item := CreateItemInBackpackFromTemplate(who, reward.item);
            SendSysMessageEx(who, "Você recebeu o item: {}.".format(item.desc));
            break;
      endcase
   endforeach
   
   quest_data.Erase(quest_pos);
   SetObjProperty(who, "quests", quest_data);
   SetObjProperty(who, "quest_progress", quest_progress);
   return 1;
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