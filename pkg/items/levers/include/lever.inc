const FX_POISON_GAS := 0x113a;
const FX_EXPLODE_3      :=  0x36b0; //
const SFX_SPELL_LIGHTNING           :=  0x002a;

function normalusage(who, item)
  var firstclick := GetObjProperty (item, "firstclick");
  if ( firstclick )
    SetObjProperty (item, "firstclick", 0);
    PrintTextAbove(who, "*Desativa*");
  else
    PrintTextAbove(who, "*Ativa*");
    SetObjProperty (item, "firstclick", 1);
  endif	
  var funct  := GetObjProperty(item, "function");
  if(!funct)
    SendSysMessage(who,"this switch seems to be broken");
    return;
  endif
  var status := GetObjProperty(item, "status");
  if(!status)
    status := "off";
    SetObjProperty( item, "status", status);
  endif
  var mid := 0;
  var fin;
  var snd;
  var obj;
  var gfx;
  var la := GetObjProperty(item,"links");
  if(!la)
    la := array{};
    la.append(item.serial);
    SetObjProperty(item,"links", la);
  endif
  foreach entry in la
    obj := SystemFindObjectBySerial(CInt(entry));
    gfx := obj.graphic;
    mid := 0;
    case(gfx)
      0x108c: mid := 0x108d;
              fin := 0x108e;
              snd := 0x4c;
      0x108e: mid := 0x108d;
              fin := 0x108c;
              snd := 0x4c;
      0x1093: mid := 0x1094;
              fin := 0x1095;
              snd := 0x4c;
      0x1095: mid := 0x1094;
              fin := 0x1093;
              snd := 0x4c;
      0x108f: fin := 0x1090;
              snd := 0x4b;
      0x1090: fin := 0x108f;
              snd := 0x4b;
      0x1091: fin := 0x1092;
              snd := 0x4b;
      0x1092: fin := 0x1091;
              snd := 0x4b;
      default:
		fin := gfx;
		mid := gfx;
    endcase
    if(mid == 0)
      mid := fin;
    endif
    sleepms(500);
    PlaySoundEffect( obj, snd);
    obj.graphic := mid;
    sleepms(500);
    obj.graphic := fin;
  endforeach
   var msg := GetObjProperty(obj, "msg");
   if(!msg)
      msg := "Você ouve algo se movendo longe...";
   endif
   SendSysMessageEx(who, msg);
   if (RunTraps(item, who))
      SetObjProperty(item, "trapSpoted", 1);
      PrintText(who, "*dispara armadilha*");
      item.graphic := mid;
      sleepms(500);
      item.graphic := gfx;
      return;
   endif
  case(funct)
    "setobj"  : stobj(who, item);
    "door"    : linkdoor(who,item);
    "passage" : alterpassage(who, item, gfx, snd);
    "lights"  : lightswitch(who, item);
    "tele"    : teleporter(who, item, mid, gfx, snd);
    "graphic": changegraphiclever(who, item);
    "elevator": doElevator(who, item);
    "moveobjects": moveObjects(who, item, gfx, snd);
    "opendoor": openthedoors(who, item);
  endcase
endfunction


function doElevator(who, item)
	var rect := GetObjProperty(item, "rect");
	var maxz := Cint(GetObjProperty(item, "z"));
	var x, y, z;
	var startz := 0;
	var items := GetObjProperty(item, "items");
	if (items == error)
		SendSysMessage(who, "não foi possivel localizar os itens");
		return;
	endif
	
	var stat := GetObjProperty(item, "status");
	if ( (stat == error) || (stat == "up"))
      var modifier := 1;
      if (maxz < 0)
         modifier := -1;
      endif
		//sendsysmessage(who, "subindo2");
		while (startz != maxz )
			foreach obj in items
				var coisa := SystemFindObjectBySerial(obj);
				//SendSysMessage(who, " " + coisa);
				MoveObjectToLocation(coisa, coisa.x, coisa.y, coisa.z + modifier, coisa.realm, MOVEOBJECT_FORCELOCATION); 
			endforeach
			startz := startz + modifier;
			sleepms(50);
		endwhile
		SetObjProperty(item, "status", "down");
	else
      maxz := maxz * -1;
      var modifier := 1;
      if (maxz < 0)
         modifier := -1;
      endif
		while (startz != maxz )
			foreach obj in items
				var coisa := SystemFindObjectBySerial(obj);
				MoveObjectToLocation(coisa, coisa.x, coisa.y, coisa.z + modifier, coisa.realm, MOVEOBJECT_FORCELOCATION); 
			endforeach
			startz := startz + modifier;
			sleepms(50);
		endwhile
		SetObjProperty(item, "status", "up");		
	endif
	
endfunction

function changegraphiclever(who, item)
  SetObjProperty(item, "#inuse", 1);
  
  var pl := GetObjProperty(item,"partsarray");
  var thing;
  
  foreach thingie in pl
    thing := SystemFindObjectBySerial(CInt(thingie));
    var stat :=  GetObjProperty(item, "status");
    if ( (stat == error) || (stat == "released"))
       thing.graphic := GetObjProperty(item, "graphic");
       setobjproperty(item, "status", "unreleased");
    else
       thing.graphic := GetObjProperty(thing, "originalgraphic");
       setobjproperty(item, "status", "released");
    endif
    
  endforeach
  
  EraseObjProperty(item, "#inuse");
endfunction

function stobj(who, item)
  SetObjProperty(item, "#inuse", 1);
  var il := GetObjProperty(item, "il");
  var note := GetObjProperty(item,"note");
  var subnote := GetObjProperty(item,"subnote");
  var itm;
  var oldnote;
  Detach();
  foreach thingie in il
    itm := SystemFindObjectBySerial(CInt(thingie));
    oldnote := SetObjProperty(itm,"oldnote", GetObjProperty(itm, note));
    SetObjProperty(itm, note, subnote);
  endforeach
  sleep(120);
  foreach thingie in il
    itm := SystemFindObjectBySerial(CInt(thingie));
    oldnote := GetObjProperty(itm,"oldnote");
    SetObjProperty(itm, note, oldnote);
  endforeach
  EraseObjProperty(item, "#inuse");
endfunction

function moveObjects(who, item, stt, snd)
  var increment := GetObjProperty(item,"increment");
  var positionIncrement := GetObjProperty(item,"positionIncrement");
   var la := GetObjProperty(item,"links");
  var obj;
  foreach entry in la
    obj := SystemFindObjectBySerial(CInt(entry));
    SetObjProperty(obj, "#inuse", 1);
  endforeach
  Detach();
  var newsound := CInt(GetObjProperty(item, "sound"));
  var speaker  := GetObjProperty(item, "speaker");
  var x := speaker[1];
  var y := speaker[2];
  var z := speaker[3];
  var speakr :=CreateItemAtLocation( x, y, z, 0xeed, 1);
  speakr.movable := 0;
  speakr.graphic := 0x1;
  var pl := GetObjProperty(item,"partsarray");
  var i;
  var stp;
  var a := 0;
  if(GetObjProperty(item, "status") == "on")
    stp := 1;
    while(stp <= increment)
       foreach entry in pl
        obj := SystemFindObjectBySerial(CInt(entry));
        obj.movable := 1;
        MoveObjectToLocation(obj, obj.x - CInt(positionIncrement[2]), obj.y - CInt(positionIncrement[3]), obj.z - CInt(positionIncrement[4]), obj.realm, MOVEOBJECT_FORCELOCATION);
        obj.movable := 0;
      endforeach
      if(a == 2)
        PlaySoundEffect(speakr, newsound);
        a := 0;
      else
        a := a + 1;
      endif
      stp := stp+1;
      sleep(1);
    endwhile
    foreach entry in la
      obj := SystemFindObjectBySerial(CInt(entry));
      SetObjProperty(obj, "status", "off");
    endforeach
  elseif(GetObjProperty(item, "status") == "off")
    stp := 1;
    while(stp <= increment)
      foreach entry in pl
        obj := SystemFindObjectBySerial(CInt(entry));
        obj.movable := 1;
        MoveObjectToLocation(obj, obj.x + CInt(positionIncrement[2]), obj.y + CInt(positionIncrement[3]), obj.z + CInt(positionIncrement[4]), obj.realm, MOVEOBJECT_FORCELOCATION);
        obj.movable := 0;
      endforeach
      if(a == 2)
        PlaySoundEffect(speakr, newsound);
        a := 0;
      else
        a := a + 1;
      endif
      stp := stp+1;
      sleep(1);
    endwhile
    foreach entry in la
      obj := SystemFindObjectBySerial(CInt(entry));
      SetObjProperty(obj, "status", "on");
    endforeach
  endif
  var timer := CInt(GetObjProperty(item, "timer"));
  if(timer > 0)
    sleep(timer);
    var mid := 0;
    var gfx;
    var fin;
    foreach entry in la
      obj := SystemFindObjectBySerial(CInt(entry));
      gfx := obj.graphic;
      mid := 0;
      case(gfx)
        0x108c: mid := 0x108d;
                fin := 0x108e;
                snd := 0x4c;
        0x108e: mid := 0x108d;
                fin := 0x108c;
                snd := 0x4c;
        0x1093: mid := 0x1094;
                fin := 0x1095;
                snd := 0x4c;
        0x1095: mid := 0x1094;
                fin := 0x1093;
                snd := 0x4c;
        0x108f: fin := 0x1090;
                snd := 0x4b;
        0x1090: fin := 0x108f;
                snd := 0x4b;
        0x1091: fin := 0x1092;
                snd := 0x4b;
        0x1092: fin := 0x1091;
                snd := 0x4b;
      endcase
      if(mid == 0)
        mid := fin;
      endif
      sleepms(500);
      PlaySoundEffect( obj, snd);
      obj.graphic := mid;
      sleepms(500);
      obj.graphic := fin;
    endforeach
    if(GetObjProperty(item, "status") == "on")
      stp := 1;
      while(stp <= increment)
        foreach entry in pl
          obj := SystemFindObjectBySerial(CInt(entry));
          obj.movable := 1;
          MoveObjectToLocation(obj, obj.x - CInt(positionIncrement[2]), obj.y - CInt(positionIncrement[3]), obj.z - CInt(positionIncrement[4]), obj.realm, MOVEOBJECT_FORCELOCATION);
          obj.movable := 0;
        endforeach
        if(a == 2)
          PlaySoundEffect(speakr, newsound);
          a := 0;
        else
          a := a + 1;
        endif
        stp := stp+1;
        sleep(1);
      endwhile
      foreach entry in la
        obj := SystemFindObjectBySerial(CInt(entry));
        SetObjProperty(obj, "status", "off");
      endforeach
    elseif(GetObjProperty(item, "status") == "off")
      stp := 1;
      while(stp <= increment)
        foreach entry in pl
          obj := SystemFindObjectBySerial(CInt(entry));
          obj.movable := 1;
          MoveObjectToLocation(obj, obj.x + CInt(positionIncrement[2]), obj.y + CInt(positionIncrement[3]), obj.z + CInt(positionIncrement[4]), obj.realm, MOVEOBJECT_FORCELOCATION);
          obj.movable := 0;
        endforeach
        if(a == 2)
          PlaySoundEffect(speakr, newsound);
          a := 0;
        else
          a := a + 1;
        endif
        stp := stp+1;
        sleep(1);
      endwhile
      foreach entry in la
        obj := SystemFindObjectBySerial(CInt(entry));
        SetObjProperty(obj, "status", "on");
      endforeach
    endif
  endif
  DestroyItem(speakr);
  EraseObjProperty(item, "#inuse");
  foreach entry in la
    obj := SystemFindObjectBySerial(CInt(entry));
    EraseObjProperty(obj, "#inuse");
  endforeach
endfunction

function alterpassage(who, item, stt, snd);
   var la := GetObjProperty(item,"links");
  var obj;
  foreach entry in la
    obj := SystemFindObjectBySerial(CInt(entry));
    SetObjProperty(obj, "#inuse", 1);
  endforeach
  Detach();
  var newsound := CInt(GetObjProperty(item, "sound"));
  var speaker  := GetObjProperty(item, "speaker");
  var x := speaker[1];
  var y := speaker[2];
  var z := speaker[3];
  var speakr :=CreateItemAtLocation( x, y, z, 0xeed, 1);
  speakr.movable := 0;
  speakr.graphic := 0x1;
  var pl := GetObjProperty(item,"partsarray");
  var i;
  var stp;
  var a := 0;
  if(GetObjProperty(item, "status") == "on")
    stp := 1;
    while(stp <= 11)
       foreach entry in pl
        obj := SystemFindObjectBySerial(CInt(entry));
        obj.movable := 1;
		if (!GetObjProperty(item, "reversepassage" ))
			MoveObjectToLocation(obj, obj.x, obj.y, obj.z - 2, obj.realm, MOVEOBJECT_FORCELOCATION);
		else
			MoveObjectToLocation(obj, obj.x, obj.y, obj.z + 2, obj.realm, MOVEOBJECT_FORCELOCATION);
		endif
        obj.movable := 0;
      endforeach
      if(a == 2)
        PlaySoundEffect(speakr, newsound);
        a := 0;
      else
        a := a + 1;
      endif
      stp := stp+1;
      sleep(1);
    endwhile
    foreach entry in la
      obj := SystemFindObjectBySerial(CInt(entry));
      SetObjProperty(obj, "status", "off");
    endforeach
  elseif(GetObjProperty(item, "status") == "off")
    stp := 1;
    while(stp <= 11)
      foreach entry in pl
        obj := SystemFindObjectBySerial(CInt(entry));
        obj.movable := 1;
		if (!GetObjProperty(item, "reversepassage" ))
			MoveObjectToLocation(obj, obj.x, obj.y, obj.z + 2, obj.realm, MOVEOBJECT_FORCELOCATION);
		else
			MoveObjectToLocation(obj, obj.x, obj.y, obj.z - 2, obj.realm, MOVEOBJECT_FORCELOCATION);
		endif
        obj.movable := 0;
      endforeach
      if(a == 2)
        PlaySoundEffect(speakr, newsound);
        a := 0;
      else
        a := a + 1;
      endif
      stp := stp+1;
      sleep(1);
    endwhile
    foreach entry in la
      obj := SystemFindObjectBySerial(CInt(entry));
      SetObjProperty(obj, "status", "on");
    endforeach
  endif
  var timer := CInt(GetObjProperty(item, "timer"));
  if(timer > 0)
    sleep(timer);
    var mid := 0;
    var gfx;
    var fin;
    foreach entry in la
      obj := SystemFindObjectBySerial(CInt(entry));
      gfx := obj.graphic;
      mid := 0;
      case(gfx)
        0x108c: mid := 0x108d;
                fin := 0x108e;
                snd := 0x4c;
        0x108e: mid := 0x108d;
                fin := 0x108c;
                snd := 0x4c;
        0x1093: mid := 0x1094;
                fin := 0x1095;
                snd := 0x4c;
        0x1095: mid := 0x1094;
                fin := 0x1093;
                snd := 0x4c;
        0x108f: fin := 0x1090;
                snd := 0x4b;
        0x1090: fin := 0x108f;
                snd := 0x4b;
        0x1091: fin := 0x1092;
                snd := 0x4b;
        0x1092: fin := 0x1091;
                snd := 0x4b;
      endcase
      if(mid == 0)
        mid := fin;
      endif
      sleepms(500);
      PlaySoundEffect( obj, snd);
      obj.graphic := mid;
      sleepms(500);
      obj.graphic := fin;
    endforeach
    if(GetObjProperty(item, "status") == "on")
      stp := 1;
      while(stp <= 11)
        foreach entry in pl
          obj := SystemFindObjectBySerial(CInt(entry));
          obj.movable := 1;
          MoveObjectToLocation(obj, obj.x, obj.y, obj.z - 2, obj.realm, MOVEOBJECT_FORCELOCATION);
          obj.movable := 0;
        endforeach
        if(a == 2)
          PlaySoundEffect(speakr, newsound);
          a := 0;
        else
          a := a + 1;
        endif
        stp := stp+1;
        sleep(1);
      endwhile
      foreach entry in la
        obj := SystemFindObjectBySerial(CInt(entry));
        SetObjProperty(obj, "status", "off");
      endforeach
    elseif(GetObjProperty(item, "status") == "off")
      stp := 1;
      while(stp <= 11)
        foreach entry in pl
          obj := SystemFindObjectBySerial(CInt(entry));
          obj.movable := 1;
          MoveObjectToLocation(obj, obj.x, obj.y, obj.z + 2, obj.realm, MOVEOBJECT_FORCELOCATION);
          obj.movable := 0;
        endforeach
        if(a == 2)
          PlaySoundEffect(speakr, newsound);
          a := 0;
        else
          a := a + 1;
        endif
        stp := stp+1;
        sleep(1);
      endwhile
      foreach entry in la
        obj := SystemFindObjectBySerial(CInt(entry));
        SetObjProperty(obj, "status", "on");
      endforeach
    endif
  endif
  DestroyItem(speakr);
  EraseObjProperty(item, "#inuse");
  foreach entry in la
    obj := SystemFindObjectBySerial(CInt(entry));
    EraseObjProperty(obj, "#inuse");
  endforeach
endfunction

function teleporter(who, item, mid, stt, snd)
  SetObjProperty(item, "#inuse", 1);
  Detach();
  var tiled := GetObjProperty( item, "tiled");
  var tile;
  var tilea;
  var visual;
  var origin1, origin2, origin3, origin4;
  var destination1, destination2, destination3, destination4;
  foreach obj in tiled
    tile := SystemFindObjectBySerial(CInt(obj));
    tilea := SystemFindObjectBySerial(CInt(GetObjProperty(tile,"link")));
    foreach person in ListMobilesNearLocation( tile.x, tile.y, tile.z, 0);
      visual := person.graphic;
      SetObjProperty(person, "inside", 1);
      SetObjProperty(person, "visual", visual);
      SetObjProperty(person, "recallgraphic", person.objtype);
      person.frozen :=1;
      PlaySoundEffect(person, 0x210);
    endforeach
    origin1      := CreateItemAtLocation( tile.x,  tile.y,  tile.z, 0x373a, 1);
    destination1 := CreateItemAtLocation(tilea.x, tilea.y, tilea.z, 0x373a, 1);
    origin1.movable := 0;
    destination1.movable := 0;
  endforeach
  sleep(2);
  foreach obj in tiled
    tile := SystemFindObjectBySerial(CInt(obj));
    tilea := SystemFindObjectBySerial(CInt(GetObjProperty(tile,"link")));
    origin2      := CreateItemAtLocation( tile.x,  tile.y,  tile.z, 0x374a, 1);
    destination2 := CreateItemAtLocation(tilea.x, tilea.y, tilea.z, 0x374a, 1);
    origin2.movable := 0;
    destination2.movable := 0;
  endforeach
  sleep(2);
  foreach obj in tiled
    tile := SystemFindObjectBySerial(CInt(obj));
    tilea := SystemFindObjectBySerial(CInt(GetObjProperty(tile,"link")));
    foreach person in ListMobilesNearLocation( tile.x, tile.y, tile.z, 0);
      if(GetObjProperty(person, "inside") == 1)
        person.graphic := 0x33;
      endif
    endforeach
    origin3      := CreateItemAtLocation( tile.x,  tile.y,  tile.z, 0x375a, 1);
    destination3 := CreateItemAtLocation(tilea.x, tilea.y, tilea.z, 0x375a, 1);
    origin3.movable := 0;
    destination3.movable := 0;
  endforeach
    sleep(2);
  foreach obj in tiled
    tile := SystemFindObjectBySerial(CInt(obj));
    tilea := SystemFindObjectBySerial(CInt(GetObjProperty(tile,"link")));
    origin4      := CreateItemAtLocation( tile.x,  tile.y,  tile.z, 0x3779, 1);
    destination4 := CreateItemAtLocation(tilea.x, tilea.y, tilea.z, 0x3779, 1);
    origin4.movable := 0;
    destination4.movable := 0;
  endforeach
  sleep(2);
  PlaySoundEffect( item, snd);
  item.graphic := mid;
  sleepms(500);
  item.graphic := stt;
  foreach obj in tiled
    tile := SystemFindObjectBySerial(CInt(obj));
    tilea := SystemFindObjectBySerial(CInt(GetObjProperty(tile,"link")));
    foreach person in ListMobilesNearLocation( tile.x, tile.y, tile.z, 0);
      if(GetObjProperty(person, "inside") == 1)
        MoveObjectToLocation( person, tilea.x, tilea.y, tilea.z , person.realm, MOVEOBJECT_FORCELOCATION);
      endif
    endforeach
  endforeach
  sleep(2);
  foreach obj in tiled
    tile := SystemFindObjectBySerial(CInt(obj));
    tilea := SystemFindObjectBySerial(CInt(GetObjProperty(tile,"link")));
    foreach thingie in ListItemsNearLocationOfType(tile.x, tile.y, tile.z, 0, 0x373a )
      DestroyItem(thingie);
    endforeach
    foreach thingie in ListItemsNearLocationOfType(tilea.x, tilea.y, tilea.z, 0, 0x373a )
      DestroyItem(thingie);
    endforeach
  endforeach
  sleep(2);
  foreach obj in tiled
    tile := SystemFindObjectBySerial(CInt(obj));
    tilea := SystemFindObjectBySerial(CInt(GetObjProperty(tile,"link")));
    foreach thingie in ListItemsNearLocationOfType(tile.x, tile.y, tile.z, 0, 0x374a )
      DestroyItem(thingie);
    endforeach
    foreach person in ListMobilesNearLocation( tilea.x, tilea.y, tilea.z, 0);
      if(GetObjProperty(person, "inside") == 1)
        EraseObjProperty(person, "inside");
        PlaySoundEffect(person, 0x1fd);
        person.graphic := GetObjProperty(person, "visual");
        if(person.dead)
          person.objtype := 0x192;
          person.graphic := 0x192;
        endif
      endif
    endforeach
    foreach thingie in ListItemsNearLocationOfType(tilea.x, tilea.y, tilea.z, 0, 0x374a )
      DestroyItem(thingie);
    endforeach
  endforeach
  sleep(2);
  foreach obj in tiled
    tile := SystemFindObjectBySerial(CInt(obj));
    tilea := SystemFindObjectBySerial(CInt(GetObjProperty(tile,"link")));
    foreach thingie in ListItemsNearLocationOfType(tile.x, tile.y, tile.z, 0, 0x375a )
      DestroyItem(thingie);
    endforeach
    foreach thingie in ListItemsNearLocationOfType(tilea.x, tilea.y, tilea.z, 0, 0x375a )
      DestroyItem(thingie);
    endforeach
  endforeach
  sleep(2);
  foreach obj in tiled
    tile := SystemFindObjectBySerial(CInt(obj));
    tilea := SystemFindObjectBySerial(CInt(GetObjProperty(tile,"link")));
    foreach thingie in ListItemsNearLocationOfType(tile.x, tile.y, tile.z, 0, 0x3779 )
      DestroyItem(thingie);
    endforeach
    foreach thingie in ListItemsNearLocationOfType(tilea.x, tilea.y, tilea.z, 0, 0x3779)
      DestroyItem(thingie);
    endforeach
    foreach person in ListMobilesNearLocation( tilea.x, tilea.y, tilea.z, 0);
      EraseObjProperty(person, "recallgraphic");
      EraseObjProperty(person, "visual");
      person.frozen := 0;
    endforeach
  endforeach
  EraseObjProperty(item, "#inuse");
endfunction

function linkdoor(who, item)
  SetObjProperty(item, "#inuse", 1);
  Detach();
  var da := GetObjProperty(item, "ds");
  var status := GetObjProperty(item, "status");
  var door;
  if(status == "off");
    foreach dr in da
      door := SystemFindObjectBySerial(CInt(dr));
      door.locked := 0;
      PlaySoundEffect(door, 0x200);
    endforeach
    SetObjProperty(item,"status", "on");
  elseif(status == "on")
    foreach dr in da
      door := SystemFindObjectBySerial(CInt(dr));
      door.locked := 1;
      PlaySoundEffect(door, 0x1f5);
    endforeach
    SetObjProperty(item,"status", "off");
  endif
  EraseObjProperty(item, "#inuse");
endfunction

function openthedoors(who, item)
  SetObjProperty(item, "#inuse", 1);
  Detach();

  var da := GetObjProperty(item, "ds");
  var status := GetObjProperty(item, "status");
  var door;
  
  if(status == "off");
    foreach dr in da
      door := SystemFindObjectBySerial(CInt(dr));
      door.doToggle();
      PlaySoundEffect(door, 0x200);
    endforeach
    SetObjProperty(item,"status", "on");
  elseif(status == "on")
    foreach dr in da
      door := SystemFindObjectBySerial(CInt(dr));
      door.doToggle();
      PlaySoundEffect(door, 0x1f5);
    endforeach
    SetObjProperty(item,"status", "off");
  endif

  EraseObjProperty(item, "#inuse");
endfunction


function lightswitch(who, item )
  SetObjProperty(item, "#inuse", 1);
  var cfg := ReadConfigFile(":*:itemdesc");
  Detach();
  var pl := GetObjProperty(item,"partsarray");
  var thing;
  foreach thingie in pl
    thing := SystemFindObjectBySerial(CInt(thingie));
    if(thing.graphic == thing.objtype)
      if(cfg[thing.objtype]."changesnd")
        PlaySoundEffect(who, cfg[thing.objtype]."changesnd");
      endif
      thing.graphic := cfg[thing.objtype]."change";
    else
      if(cfg[thing.objtype]."originsnd")
        PlaySoundEffect(who, cfg[thing.objtype]."originsnd");
      endif
      thing.graphic := thing.objtype;
    endif
  endforeach
  EraseObjProperty(item, "#inuse");
endfunction


function bindlights(who, item)
  var newarray := array{};
  SendSysMessage(who,"Escolha as luzes que você deseja ligar com a alavanca. cancel to end");
  var trg;
  while(1)
    trg := Target(who);
    if(!trg)
      break;
    endif
    SetObjProperty(trg,"switched", 1);
    newarray.append(trg.serial);
  endwhile
  SetObjProperty(item,"function", "lights");
  SetObjProperty(item,"partsarray", newarray);
endfunction

function createpassage(who, item)
  var newarray := array{};
  var linkarray := array{};
  linkarray.append(item.serial);
  SendSysMessage(who,"Escolhas as peças que você deseja ligar com a alavanca. cancel to end");
  var trg;
  while(1)
    trg := Target(who);
    if(!trg)
      break;
    endif
    newarray.append(trg.serial);
  endwhile
  SendSysMessage(who,"Escolha o item que você quer que o som seja originado");
  var snd := Target(who);
  var speaker := array{};
  speaker.append(snd.x);
  speaker.append(snd.y);
  speaker.append(snd.z);
  SendSysMessage(who,"Agora escolha qualquer outra alavanca que queira sincronizar com esta.");
  while(1)
    trg := Target(who);
    if(!trg)
      break;
    endif
    linkarray.append(trg.serial);
  endwhile  
  var it;
  foreach thing in linkarray
    it := SystemFindObjectBySerial(CInt(thing));    
    SetObjProperty(it, "links", linkarray);
    SetObjProperty(it, "speaker", speaker);
    SetObjProperty(it, "function", "passage");
    SetObjProperty(it, "partsarray", newarray);
  endforeach
endfunction

function doorstatus(who, item)
  var newarray := array{};
  SendSysMessage(who,"Escolha a porta que você deseja linkar com esta alavanca. Esc para cancelar");
  var trg;
  while(1)
    trg := Target(who);
    if(!trg)
      break;
    endif
    newarray.append(trg.serial);
  endwhile
  SetObjProperty(item,"function", "door");
  SetObjProperty(item,"ds", newarray);
  SendSysMessage(who,"A alavanca foi ligada com as porta(s)");
endfunction
function setOpenDoor(who, item)
  var newarray := array{};
  SendSysMessageEx(who,"Escolha a porta que você deseja linkar com esta alavanca. Esc para cancelar");
  var trg;
  while(1)
    trg := Target(who);
    if(!trg)
      break;
    endif
    newarray.append(trg.serial);
    trg.locked := 1;
  SendSysMessageEx(who,"Alavanca ligada.");
  endwhile
  SetObjProperty(item,"function", "opendoor");
  SetObjProperty(item,"ds", newarray);
  SendSysMessageEx(who,"A alavanca foi ligada com as porta(s)");
endfunction
function setprp(who, item, result)
  var newarray := array{};
  var note:=result[12];
  note[1, find(note, ": ", 1)+1]:="";
  var subnote;
  var i;
  subnote:=result[13];
  subnote[1, find(subnote, ": ", 1)+1]:="";
  var trg;
  while(1)
    trg := Target(who);
    if(!trg)
      break;
    endif
    newarray.append(trg.serial);
  endwhile
  SetObjProperty(item, "note", note);
  SetObjProperty(item, "subnote", subnote);
  SetObjProperty(item,"function", "setobj");
  SetObjProperty(item,"il", newarray);
endfunction

function linkteleport(who, item)
  SendSysMessage(who, "target the rune you wish this switch to teleport the user to");
  var darray := array{};
  var trgd, trga;
  while(1)
    SendSysMessage(who,"target the departure tile");
    trgd := Target(who);
    SendSysMessage(who,"target the arrival tile");
    trga := Target(who);
    if((!trgd) ||(!trga))
      break;
    endif
    SetObjProperty(trgd, "link", trga.serial);
    darray.append(trgd.serial);
  endwhile
  SetObjProperty(item, "tiled", darray);
  SetObjProperty(item,"function", "tele");
endfunction

function Resisted(who, amt)
  var magery  := CInt(RandomInt(99)+1);
  var evalint := CInt(RandomInt(99)+1);
  var halfamt := amt/2;
  var rezamt := CInt(((RandomInt(halfamt)+1) * evalint ) / 100);
/*  if(SkillCheck(who, RESISTING_SPELLS, 70))
	SendSysMessage(who, "You feel yourself resisting magical energy!");
	PlaySoundEffect(who, 0x01e7);
	PlayObjectCenteredEffect( who, 0x374a, 5,5);
	if(evalint > 0)
	  amt := rezamt + halfamt;
	else
	  amt := halfamt;
    endif
	return amt;
  else*/
	return amt;
  //endif
endfunction
