include ":attributes:attributes";
include "include/utils";

function HaveProficiency(who, proficiency)
   var chardata := GetObjProperty(who, "chardata");
   if (!chardata) return;
   elseif (!chardata.proficiencias) return; endif

   if (proficiency in chardata.proficiencias) return 1; endif
endfunction