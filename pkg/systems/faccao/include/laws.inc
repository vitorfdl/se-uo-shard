###########################################
# 
# RemLaw
# AddLaw
# GetLaw
# ListLaw
# DisableLaw
#
###########################################

# Laws:
# $ No LockPick - Guards will not allow lockpick around them
## - Doors
## - Chests
# $ No Fight - Guards will not allow fights around them
# $ No Item - Guards will not allow players using that item around them
# $ No Weapons - Guards will not allow players to use weapons (if no monsters) around them
# $ Nobility Above Laws - Guards will ignore all laws if player have title
# $ No Race 
# $ No Faction


function AddLaw(guild, law)
   if (typeof(guild) == "String")
      guild := FindGuild(guild);
   endif

   if (!guild) return; endif

   var new_law := struct{
      "type" := law.type,
      "opt" := law.opt,
   };

   var laws := ListLaws(guild);
   if (new_law.type != "Item" && new_law.type != "Race")
      if (FindInArray(laws, struct{"type" == new_law.type }))
         return 0;
      endif
   endif

   laws.append(new_law);
   return 1;
endfunction

function ListLaws(guild)
   if (typeof(guild) == "String")
      guild := FindGuild(guild);
   endif

   if (!guild) return; endif

   var laws := guild.GetProp("laws");
   if (!laws) laws := array{}; endif

   return laws;
endfunction

function GetLaw(guild, type)
   var law_list := ListLaws(guild);

   return FindAllElemInArray(law_list, struct{ "type" := type });
endfunction

function RemLaw()

endfunction

function DisableLaw()

endfunction