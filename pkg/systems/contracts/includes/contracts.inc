use uo;

var entries_cfg := ReadConfigFile(":contract:contract_entires");

function getEntry(number)
   var elem := entries_cfg[number];
   if (!elem)
      return;
   endif

   var entry_info := struct{
      "number"  := cint(elem["Number"]),
      "type"    := elem["Type"],
      "align"   := elem["Align"],
      "option"  := elem["Option"],
      "text"    := elem["Text"],
      "skill"   := cint(elem["Skill"]),
      "entries" := GetConfigStringArray(elem, "Entry")
   };

   return entry_info;
endfunction

function listEntry(type := 0, align := 0, skill := 0)
   var keys := GetConfigIntKeys(entries_cfg);

   var entries_return := array{};
   foreach key in keys
      var entry := getEntry(key);
      if (!key)
         continue;
      elseif (type && entry.type !== type)
         continue;
      elseif (align && entry.align !== align)
         continue;
      elseif (skill && entry.skill < skill)
         continue;
      endif

      entries_return.append(entry);
   endforeach

   return entries_return;
endfunction
