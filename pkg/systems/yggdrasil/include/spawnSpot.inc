use os;
use datafile;

include ":datafile:datafile";
include ":gumps:yesno";
include ":yggdrasil:boss";
include "include/arrays";

/*=============================================
=        SpotGroups functions code            =
=============================================*/


function LoadSpotGroupData()
    var df := DFOpenDataFile( ":yggdrasil:spotgroups", DF_CREATE, DF_KEYTYPE_STRING);
    return df;
endfunction

function GetSpotGroupData(name, create := 1)
    var df   := LoadSpotGroupData();
    var elem := DFFindElement(df, name, create);
    if (!elem)
        return 0;
    endif

    var data := struct{};
    data.+active          := DFGetProp(elem, "active", create, 0);
    data.+spots           := DFGetProp(elem, "spots", create, array{});
    data.+spawngroup_list := DFGetProp(elem, "spawngroup_list", create, array{});
    data.+activegroup     := DFGetProp(elem, "activegroup", create, 0);
    data.+mob_number      := DFGetProp(elem, "mob_number", create, 0);

    return data;
endfunction

function RemoveSpotGroupData(name)
    var df   := LoadSpotGroupData();
    var data := GetSpotGroupData(name);

    foreach serial in (data.spots)
        var spot := SystemFindObjectBySerial(serial);
        SetObjProperty(spot, "GM_FLAG", 1);
        DestroyItem(spot);
        sleepms(10);
    endforeach
    
    df.DeleteElement(name);
endfunction

function SetSpotGroupData(name, spotGroup)
    var df   := LoadSpotGroupData();
    var elem := DFFindElement(df, name, DF_CREATE);
    if (spotGroup.spawngroup_list)
        foreach entry in (spotGroup.spawngroup_list)
            entry := ModelSpawnGroupData(entry);
            sleepms(5);
        endforeach
        elem.SetProp("spawngroup_list", spotGroup.spawngroup_list);
    endif

    if (spotGroup.active == 0 || spotGroup.active == 1)
        elem.SetProp("active", spotGroup.active);
    endif
    
    if (spotGroup.activegroup)
        elem.SetProp("activegroup", spotGroup.activegroup);
    endif

    if (spotGroup.spots)
        elem.SetProp("spots", spotGroup.spots);
    endif

    if (spotGroup.mob_number && typeof(spotGroup.mob_number) == typeof(1))
        elem.SetProp("mob_number", spotGroup.mob_number);
    endif
endfunction

function ModelSpawnGroupData(spotGroup := struct{})
    if (!spotGroup.name)
        spotGroup.+name := "Missing Name";
    endif

    if (spotGroup.influence != 1 && spotGroup.influence != 2 && spotGroup.influence != 3 && spotGroup.influence != 4)
        spotGroup.+influence := 1;
    endif

    if (TypeOf(spotGroup.allies) != "Array")
        spotGroup.+allies := array{};
    endif

    if (TypeOf(spotGroup.enemies) != "Array")
        spotGroup.+enemies := array{};
    endif

    return spotGroup;
endfunction

function UpdateSpawnGroupInSpotGroup(name, data := struct{})
    var elem  := GetSpotGroupData(name);
    var index := FindInArray(elem.spawngroup_list, struct{"name":=data.name});
    if (index)
        elem.spawngroup_list[index] := data;
    else
        elem.spawngroup_list.append(data);
    endif

    if (elem.spawngroup_list.size() == 1)
        elem.activegroup := data.name;
    endif
    SetSpotGroupData(name, elem);
endfunction

function RemoveSpawnGroupInSpotGroup(name, spawngroup_name)
    var elem  := GetSpotGroupData(name);
    var index := FindInArray(elem.spawngroup_list, struct{"name":=spawngroup_name});
    if (index)
        elem.spawngroup_list.Erase(index);
        SetSpotGroupData(name, elem);
        return 0;
    endif
    return "No Index Found";
endfunction

/*=====  End of Section comment block  ======*/


/*===========================================
=            Spot function codes            =
===========================================*/

function AddSpotToSpotGroup(name, serial)
    var elem  := GetSpotGroupData(name);
    if (serial in elem.spots)
        return "Already Exists";
    endif

    elem.spots.append(serial);
    SetSpotGroupData(name, elem);

    var spot := SystemFindObjectBySerial(serial);
    spot.SetProps(struct{ "spotgroup" := name} );
    return 0;
endfunction

function RemoveSpotInSpotGroup(name, serial)
    var elem  := GetSpotGroupData(name);
    var index := FindInArray(elem.spots, serial);
    if (!index)
        return "Can\'t Find Index";
    endif

    elem.spots.Erase(index);
    SetSpotGroupData(name, elem);
    return 0;
endfunction

/*=====  End of Spot function codes  ======*/
function KillAllSpots(name)
    var elem  := GetSpotGroupData(name);
    foreach serial in (elem.spots)
        var spot := SystemFindObjectBySerial(serial);
        if (!spot)
            return;
        endif
        spot.ForceKill();
        sleepms(10);
    endforeach
endfunction

function SpawnAllSpots(name)
    var elem  := GetSpotGroupData(name);
    foreach serial in (elem.spots)
        var spot := SystemFindObjectBySerial(serial);
        if (!spot)
            return;
        endif
        spot.ForceSpawn();
        sleepms(10);
    endforeach
endfunction

/*===========================================
=            Spawnpoint Monster Code        =
===========================================*/


function removeFromSpawn(spawnpoint, monster := 0)
   spawnpoint := SystemFindObjectBySerial(spawnpoint);
   if (!spawnpoint)
      return;
   endif

   var diedmonsters := GetObjProperty(spawnpoint, "diedmonsters");
   if (!diedmonsters)
      diedmonsters := array{};
   endif

   var boss := isBoss(monster);
   diedmonsters.append(struct{"time":= polcore().systime, "boss" := boss });

   spawnpoint.setProps(struct{ "diedmonsters" := diedmonsters });
endfunction