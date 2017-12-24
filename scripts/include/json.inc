use os;

function toJSON(item)
   item := setTypes(item);

   return item;
endfunction

function ArraytoJSON(elem)
   if (typeof(elem) != typeof(array{}))
      return elem;
   endif

   var json := "[";
   foreach item in elem
      item := setTypes(item);
      json := "{}, {}".format(json, item);
      sleepms(2);
   endforeach
   json["[,"] := "[";
   json += "]";

   return json;
endfunction

function StructToJSON(elem)
   if (typeof(elem) != "Struct")
      return elem;
   endif

   var json := "{";
   foreach item in (elem.Keys())
      var value := setTypes(elem[item]);
      json := "{}, \"{}\": {}".format(json, item, value);
      sleepms(2);
   endforeach

   json["{,"] := "{";
   json += "}";

   return json;
endfunction

function setTypes(item)
   if (typeof(item) == "Array")
      item := ArrayToJson(item);
   elseif (typeof(item) == "Struct")
      item := StructToJson(item);
   elseif (typeof(item) == "String")
      item := "\"{}\"".format(item);
   endif
   
   // just return the item if its integer

   return item;
endfunction
