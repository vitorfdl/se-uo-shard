use os;
use datafile;

include ":datafile:datafile";
include ":gumps:yesno";
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
    data.+spawnGroup_list := DFGetProp(elem, "spawnGroup_list", create, array{});

    return data;
endfunction

function RemoveSpotGroupData(name)
    var df := LoadSpotGroupData();
    df.DeleteElement(name);
endfunction

function SetSpotGroupData(name, spotGroup)
    var df   := LoadSpotGroupData();
    var elem := DFFindElement(df, name, DF_CREATE);
    if (spotGroup.spawnGroup_list)
        foreach entry in (spotGroup.spawnGroup_list)
            entry := ModelSpawnGroupData(entry);
        endforeach
        elem.SetProp("spawnGroup_list", spotGroup.spawnGroup_list);
    endif

    if (spotGroup.active == 0 || spotGroup.active == 1)
        elem.SetProp("active", spotGroup.active);
    endif

    if (spotGroup.spots)
        elem.SetProp("spots", spotGroup.spots);
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
    var index := FindInArray(elem.spawnGroup_list, struct{"name":=data.name});
    if (index)
        elem.spawnGroup_list[index] := data;
    else
        elem.spawnGroup_list.append(data);
    endif
    SetSpotGroupData(name, elem);
endfunction

function RemoveSpawnGroupInSpotGroup(name, spawngroup_name)
    var elem  := GetSpotGroupData(name);
    var index := FindInArray(elem.spawnGroup_list, struct{"name":=spawngroup_name});
    if (index)
        elem.spawnGroup_list.Erase(index);
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