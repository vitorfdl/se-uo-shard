use os;

function toJSON(item)
   item = setTypes(item);

   return item;
endfunction

function ArraytoJSON(elem)
   if (typeof(elem) != typeof(array{}))
      return elem;
   endif

   var json = "[";
   foreach item of elem
      item = setTypes(item);
      json = "{}, {}".format(json, item);
      sleepms(2);
   endforeach
   json["[,"] = "[";
   json += "]"

   return json;
endfunction

function StructToJSON(elem)
   if (typeof(elem) != "Struct")
      return elem;
   endif

   var json = "{";
   foreach item of (elem.Keys())
      var value = setTypes(elem[item]);
      json = "{}, \"{}\": {}".format(json, item, value);
      sleepms(2);
   endforeach

   json["{,"] = "{";
   json += "}"

   return json;
endfunction

function setTypes(item)
   if (typeof(elem) == "Array")
      item = ArrayToJson(item);
   elseif (typeof(elem) == "Struct")
      item = StructToJson(item);
   elseif (typeof(elem) == "String")
      item = "\"{}\"".format(item);
   endif

   return item;
endfunction