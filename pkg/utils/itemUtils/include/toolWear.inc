include ":attributes:attributes_ex";

function CheckToolWear (who, tool, skill)
   var swings := GetObjProperty(tool,"swings");
   if (!swings)
      swings := Cint(50 * tool.quality);
      SetObjProperty(tool,"swings", swings);
   endif

  if(swings <= 0)
    DestroyItem(tool);
    SendSysMessageEx(who,"Sua ferramenta quebrou.");
   endif
endfunction 

function SetToolDurability(who, tool, skillid)

  var toolfile := ReadConfigFile(":*:itemdesc");
  var toolscript := toolfile[tool.objtype].Script;

  if(!toolscript.errortext)
    if(!tool.isA(POLCLASS_CONTAINER) && !tool.isA(POLCLASS_ARMOR))
      var myskill := GetEffectiveSkill(who, skillid);
      var swings := 0;
      var mods := GetObjProperty(tool, "durability");
      var bonus := 0;
      case(mods)
        "durable":        bonus := 10;
        "substantial":    bonus := 20;
        "massive":        bonus := 30;
        "fortified":      bonus := 40;
        "indestructable": bonus := 50;
        default: bonus := 0;
      endcase
      swings := Cint(50 + myskill) + bonus;
      SetObjProperty(tool, "swings", swings);
      if(!tool.quality or tool.quality == error)
        tool.quality := 1.0;
      endif
    endif
  endif

endfunction 
