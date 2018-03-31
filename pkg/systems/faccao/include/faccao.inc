use guilds;
use os;

// ###########################################
// #
// # CreateFaccao
// # GetFaccao
// # EditFaccao
// # AddMember
// # GetMember
// # RemMember
// # EditMember
// # SetLeader
// # IsLeader
// # 
// ###########################################


function CreateFaccao(name, rect, money := 0)
   if (!name || typeof(name) != "String") return 0; endif
   var guild := CreateGuild();
   guild.setProp("name", name);
   guild.setProp("leader", 0);
   guild.setProp("npcs_active", array{});
   guild.setProp("npcs", array{});
   guild.setProp("report", array{"Facção Criada"});
   guild.setprop("area", rect);
   guild.setprop("money", cint(money));
   guild.setprop("uniforms", array{});
   guild.setprop("link_to", 0);

   return guild;
endfunction

function GetFaccao(guild)
   if (typeof(guild) == "String")
      guild := FindGuild(guild);
   endif

   if (!guild) return; endif

   var faccao_obj := struct{
      "name"     := guild.GetProp("name"),
      "npcs"     := guild.GetProp("npcs"),
      "npcs_active"  := guild.GetProp("npcs_active"),
      "laws"     := guild.GetProp("laws"),
      "leader"   := guild.GetProp("leader"),
      "area"     := guild.GetProp("area"),
      "uniforms" := guild.GetProp("uniforms"),
      "money" := guild.GetProp("money")
   };

   return faccao_obj;
endfunction

function EditFaccao(guild, props)
   if (typeof(guild) == "String")
      guild := FindGuild(guild);
   endif
   if (!guild) return; endif

   var faccao := GetFaccao(guild);
   if (props.name) // strimg
      faccao.name := props.name;
   endif

   if (props.npcs != null) // array
      faccao.npcs := props.npcs;
   endif

   if (props.laws  != null) // array
      faccao.laws := props.laws;
   endif

   if (props.area  != null) // array x y x2 y2
      faccao.area := props.area;
   endif

   if (props.leader != null) // serial string
      faccao.leader := props.leader;
   endif

   if (props.uniforms) // array
      faccao.uniforms := props.uniforms;
   endif


   if (props.link_to != null) // fac string
      faccao.link_to := props.link_to;
   endif

   foreach key in (faccao.keys())
      guild.SetProp(key, faccao[key]);
      sleepms(2);
   endforeach

   return 1;
endfunction

function RemFaccao(guild_obj)
   if (typeof(guild) == "String")
      guild := FindGuild(guild);
   endif
   if (!guild) return; endif

   foreach member in (guild_obj.members)
      guild_obj.removemember(member);
      sleepms(2);
   endforeach

   foreach g in (guild_obj.enemyguilds)
      guild_obj.removeenemyguild(g);
      sleepms(2);
   endforeach

   return DestroyGuild(guild_obj);
endfunction


function AddMember(faccao, mobile)
   faccao.addMember(mobile);
   var member_obj := struct{
      "above_guards" := 0, // can control guards
      "above_merchants" := 0, // can control merchants
      "above_members" := 0, // can add members
      "giveprivs" := 0, // can edit privs
      "contractnpc" := 0, // can contract npc
      "fixitems" := 0, // can fix items
      "unlockdoors" := 0, // can unlock doors
      "buyuniforms" := 0 // can buy uniform recipes
   };
   faccao.SetProp(cstr(mobile.serial), member_obj);
endfunction


function EditMember(guild, serial, props)
   var member := guild.GetProp(serial);
   if (props.name)
      faccao.name := props.name;
   endif

   if (props.npcs)
      faccao.npcs := props.npcs;
   endif

   if (props.laws)
      faccao.laws := props.laws;
   endif

   if (props.area)
      faccao.area := props.area;
   endif

   if (props.leader)
      faccao.leader := props.leader;
   endif

   if (props.uniforms)
      faccao.uniforms := props.uniforms;
   endif

   foreach key in (faccao.keys())
      guild.SetProp(key, faccao[key]);
      sleepms(2);
   endforeach

   return 1;
endfunction

function RemMember(faccao, mobile)
   if (!mobile.isA(POLCLASS_MOBILE))
      mobile := SystemFindObjectBySerial(mobile, SYSFIND_SEARCH_OFFLINE_MOBILES);
   endif

   if (faccao.GetProp("leader") == mobile.serial)
      faccao.SetProp("leader", 0x0);
   endif
   faccao.removemember(mobile);
   faccao.EraseProp(cstr(mobile.serial));
endfunction

function SetLeader(faccao, serial)
   faccao.SetProp("leader", serial);
endfunction

function isLeader(faccao, mobile)
   var leader := faccao.GetProp("leader");
   if (leader == mobile.serial)
      return 1;
   endif

   return 0;
endfunction

function inGuildRect(guild, item := 0)
   if (typeof(guild) == "String")
      guild := FindGuild(guild);
   endif

   if (!guild) return; endif

   var area := guild.getProp("area");
   foreach rect in area
      sleepms(2);
      if (!rect || rect.size() < 4) 
         continue;
      elseif (item.x >= cint(rect[1]) && item.x <= cint(rect[3]) && item.y >= cint(rect[2]) && item.y <= cint(rect[4]));
         return 1;
      endif
   endforeach

   return 0;
endfunction

function AddReport(guild, report)
   if (typeof(guild) == "String")
      guild := FindGuild(guild);
   endif

   if (!guild) return; endif

   var reports := guild.getProp("reports");
   if (!reports) reports := array{}; endif
   reports.Insert(1, report);

   if (reports.size() > 30)
      reports.Erase(reports.size());
   endif

   guild.setProp("reports");
endfunction