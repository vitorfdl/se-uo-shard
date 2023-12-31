
function Hallucinate(who)
// Removido temporariamente!
  var graphicarray := {51, 42, 39, 31, 29, 28, 6, 5, 48, 7, 33, 30, 26, 24, 21, 17, 22, 18, 16, 15, 14, 13, 8, 47, 12, 10, 9, 5, 4, 3, 1, 2, 400, 401};
  var graphic := graphicarray[RandomInt(len(graphicarray)) + 1];
  var color := RandomInt(99) + 2;
  DrawGamePlayer(who, graphic, color);
  foreach mobile in ListMobilesNearLocation(who.x, who.y, who.z, 30)
    graphic := graphicarray[RandomInt(len(graphicarray)) + 1];
    color := RandomInt(99) + 2;
    if (mobile.serial != who.serial)
      DeleteObject(who, mobile);
      DrawObject(who, mobile, graphic, color, 1);
    endif
    sleepms(2);
  endforeach

  foreach item in ListItemsNearLocation(who.x, who.y, who.z, 30)
    color := RandomInt(99) + 2;
    if ((item.amount == 1) and (!item.isa(POLCLASS_CORPSE)) and (!item.invisible))
      DeleteObject(who, item);
      DrawObject(who, item, item.graphic, color);
    endif
    sleepms(2);
  endforeach
endfunction

function EndHallucinate(who)
  DrawGamePlayer(who, who.graphic, who.color);
  foreach mobile in ListMobilesNearLocation(who.x, who.y, who.z, 30)
    if (mobile.serial != who.serial)
      DeleteObject(who, mobile);
      DrawObject(who, mobile, mobile.graphic, mobile.color, 1);
    endif
    sleepms(2);
  endforeach

  foreach item in ListItemsNearLocation(who.x, who.y, who.z, 30)
    if ((item.amount == 1) and (!item.isa(POLCLASS_CORPSE)) and (!item.invisible))
      DeleteObject(who, item);
      DrawObject(who, item, item.graphic, item.color);
    endif
    sleepms(2);
  endforeach
endfunction
// the following functions have been pasted from SendPacket.inc

function ConvertIntToHex(astring, alength)
  astring := Hex(astring);
  astring := CStr(astring);
  if (astring["0x"])
    astring := astring[3, len(astring)];
  endif
  if (len(astring) > alength * 2)
    return;
  endif
  while (len(astring) < alength * 2)
    astring := "0"+ astring;
  endwhile    
  return astring;
endfunction

function DrawObject(ktowhom, kobj, kgraphic, kcolor, keqitrandcolor := 0)
  var PacketHexStringPart1 := "78";
  var PacketHexStringPart2 := ConvertIntToHex(kobj.serial, 4) + ConvertIntToHex(kgraphic, 2);
  PacketHexStringPart2 := PacketHexStringPart2 + ConvertIntToHex(kobj.x, 2);
  PacketHexStringPart2 := PacketHexStringPart2 + ConvertIntToHex(kobj.y, 2);
  PacketHexStringPart2 := PacketHexStringPart2 + ConvertIntToHex(kobj.z, 1);
  PacketHexStringPart2 := PacketHexStringPart2 + ConvertIntToHex(kobj.facing, 1);
  PacketHexStringPart2 := PacketHexStringPart2 + ConvertIntToHex(kcolor, 2);
  PacketHexStringPart2 := PacketHexStringPart2 +"0000";
  if ((kobj.isa(POLCLASS_MOBILE)) and ((kobj.graphic == 400) or (kobj.graphic == 401)))
    var color;
    var graphic;
    foreach item in ListEquippedItems(kobj)
      PacketHexStringPart2 := PacketHexStringPart2 + ConvertIntToHex(item.serial, 4);
      if (keqitrandcolor == 1)
        color := RandomInt(99) + 2;

      else
        color := item.color;
      endif
      if (color == 0)
        graphic := item.graphic;

      else
        graphic := item.graphic + 32768;
      endif
      PacketHexStringPart2 := PacketHexStringPart2 + ConvertIntToHex(graphic, 2);
      PacketHexStringPart2 := PacketHexStringPart2 + ConvertIntToHex(item.layer, 1);
      if (color != 0)
        PacketHexStringPart2 := PacketHexStringPart2 + ConvertIntToHex(color, 2);
      endif
    endforeach
  endif
  PacketHexStringPart2 := PacketHexStringPart2 +"00000000";
  var FullPacketHexStringLength := (len(PacketHexStringPart1) + len(PacketHexStringPart2)) / 2 + 2;
  var FullPacketHexString := PacketHexStringPart1 + ConvertIntToHex(FullPacketHexStringLength, 2) + PacketHexStringPart2;
  SendPacket(ktowhom, FullPacketHexString);
endfunction

function DeleteObject(who, lobj)
  var PacketHexString := "1D"+ ConvertIntToHex(lobj.serial, 4);
  SendPacket(who, PacketHexString);
endfunction
