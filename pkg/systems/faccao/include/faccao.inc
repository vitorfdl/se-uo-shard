###########################################
#
# CreateFaccao
# GetFaccao
# EditFaccao
# AddMember
# GetMember
# RemMember
# EditMember
# SetLeader
# IsLeader
# 
###########################################

use guilds;

function CreateFaccao(name, rect)
   if (!name || typeof(name) != "String") return 0; endif
   var guild := CreateGuild();
   guild.setProp("name", name);
   guild.setProp("leader", 0);
   guild.setProp("npcs", array{});
   guild.setProp("report", array{"Facção Criada"});
   guild.setprop("area", rect);
   guild.setprop("uniforms", array{});

   return guild;
endfunction

function GetFaccao(guild)
   var faccao_obj := struct{
      "name"     := guild.GetProp("name"),
      "npcs"     := guild.GetProp("npcs"),
      "laws"     := guild.GetProp("laws"),
      "leader"   := guild.GetProp("leader"),
      "area"     := guild.GetProp("area")
      "uniforms" := guild.GetProp("uniforms")
   };

   return faccao_obj;
endfunction

function EditFaccao(guild, props)
   var faccao := GetFaccao(guild);
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

function RemFaccao(guild_obj)
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
      "above_guards" := 0,
      "above_merchants" := 0,
      "addmembers" := 0,
      "removemembers" := 0,
      "giveprivs" := 0,
      "contractnpc" := 0,
      "fixitems" := 0,
      "unlockdoors" := 0,
      "buyuniforms" := 0,
   };

   faccao.SetProp(mobile.serial, member_obj);
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
   faccao.removemember(mobile);
   faccao.EraseProp(mobile.serial);
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