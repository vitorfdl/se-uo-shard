
function core_listAggro(creature)
   return core_aggroProp(creature);
endfunction

function core_getAggro(creature, serial)
   CheckSerial(serial);
   var aggros := core_aggroProp(creature);
   
   return cint(aggros[serial]);
endfunction

function core_remAggro(creature, serial)
   CheckSerial(serial);
   var aggros := core_aggroProp(creature);
   aggros[serial] := 0;
   core_setAggroProp(creature, aggros);
endfunction

function core_setAggro(creature, serial, number := 1)
   CheckSerial(serial);
   var aggros := core_aggroProp(creature);
   aggros[serial] := cint(aggros[serial]) + number;

   core_setAggroProp(creature, aggros);
endfunction

function core_getLowestAggro(creature)
   var aggros := core_aggroProp(creature);
   var lowest := array{0, 9999999999};
   foreach serial in (aggros.keys())
      if (aggros[serial] < lowest[2])
         lowest := array{serial, aggros[serial]};
      endif
      sleepms(2);
   endforeach

   return lowest[1];
endfunction

function CheckSerial(byref serial)
   if (serial.isA(POLCLASS_MOBILE))
      serial := serial.serial;
   endif
endfunction

function core_aggroProp(creature)
   var aggro_list := GetObjProperty(creature, "aggros");
   if (!aggro_list || typeof(aggro_list) != "Dictionary")
      aggro_list := dictionary{};
   endif

   return aggro_list;
endfunction

function core_setAggroProp(creature, aggro_prop)
   SetObjProperty(creature, "aggros", aggro_prop);
endfunction
