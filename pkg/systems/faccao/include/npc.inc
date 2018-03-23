// ###########################################
// # 
// # AddNPC
// # remNPC
// # editNPC
// # listNPC
// # ContractNpc
// # listContractedNPC
// # DismissNPC
// # CheckPrivilegies
// # GetReports
// # 
// ###########################################

include ":datafile:include/datafile";
include ":unicos:item_template";

function AddNpc(guild, npc)
   if (typeof(guild) == "String")
      guild := FindGuild(guild);
   endif

   if (!guild) return; endif

   var new_npc := struct{
      "template" := npc.template,
      "equipment" := npc.equipment,
      "bossStruct" := npc.bossStruct,
      "price" := npc.price,
      "name" := npc.name
   };
   if (!new_npc.template) 
      return 0;
   elseif (!new_npc.equipment)
      return 0;
   elseif (!new_npc.price)
      return 0;
   elseif (!new_npc.name)
      return 0;
   endif

   var npclist := listNPC(guild);
   npclist.append(new_npc);

   guild.setProp("npcs", npclist);
   return 1;
endfunction

function listNPC(guild)
   if (typeof(guild) == "String")
      guild := FindGuild(guild);
   endif

   if (!guild) return; endif

   var npcs := guild.GetProp("npcs");
   if (!npcs) npcs := array{}; endif

   return npcs;
endfunction

function remNPC(guild, index)
   if (typeof(guild) == "String")
      guild := FindGuild(guild);
   endif
   if (typeof(index) == "Integer") 
      var npc_list := listNPC(guild);
      npc_list.Erase(index);
      guild.setProp("npcs", npc_list);
   endif
endfunction

function listContractedNPC(guild)
   if (typeof(guild) == "String")
      guild := FindGuild(guild);
   endif

   if (!guild) return; endif

   var npcs_active := guild.GetProp("npcs_active");
   if (!npcs_active) npcs_active := array{}; endif

   return npcs_active;
endfunction


function ContractNPC(guild, npc_i, who, loc)
   if (typeof(guild) == "String")
      guild := FindGuild(guild);
   endif

   if (!guild) return; endif

   var npcs := listNpc(guild);
   var template := npcs[npc_i];
   if (!template) return; endif

	var agent := CreateNPCFromTemplate(template.template, loc.x, loc.y, loc.z+2);
   SetObjProperty(agent, "guild", guild.guildid);
   SetObjProperty(agent, "contract_by", who.serial);

   foreach item_ref in (template.equipment)
      var item := CreateIteminBackpackFromTemplate(agent, item_ref);
      EquipItem(agent, item_ref);
   endforeach

   PrintText(agent, "As suas ordens");

   var npc_list := listContractedNPC(guild);
   npc_list.append({
      "serial" := agent.serial,
      "price" := template.price,
      "type" := template.name
   });
   guild.setProp("npcs_active", npc_list);

   return 1;
endfunction