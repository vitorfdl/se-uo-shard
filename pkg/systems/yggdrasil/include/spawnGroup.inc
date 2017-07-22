use os;
use datafile;

include ":datafile:datafile";

/**
 * [Commands for SpawnGroups]
 * LoadSpawnGroupData() - Get the DataFile document
 * SetSpawnGroupData(group_name, group_struct) - Set changes to a group;
 * GetSpawnGroupData(group_name) - Get all info about a group;
 * RemoveGroupData(group_name) - Remove a group;
 * RemoveGroupMobData(group_name, index_mob) - Remove a mob from mobList. Need index;
 * UpdateGroupMobData(group_name, mob_struct, index) - Update all informations about a mob.
 */

/*=============================================
=       SpawnGroups functions code            =
=============================================*/

function LoadSpawnGroupData()
    var df := DFOpenDataFile( ":yggdrasil:spawns", DF_CREATE, DF_KEYTYPE_STRING);
    return df;
endfunction

function ModelMobListData(entry := struct{})
    if (lower(entry.daytime) != "noite" && lower(entry.daytime) != "dia" && lower(entry.daytime) != "lua nova" && lower(entry.daytime) != "always")
        entry.+daytime := "always";
    endif

    if (cint(entry.isBoss) != 0 && cint(entry.isBoss) != 1) 
        entry.+isBoss := 0;
    endif

    if (TypeOf(entry.bossParams) != "Struct")
        entry.+bossParams := struct{};
    endif

    if (cint(entry.power) != 1 && cint(entry.power) != 2 && cint(entry.power) != 3) 
        entry.+power := 1;
    endif

    if (lower(entry.type) != "melee" && lower(entry.type) != "ranged" && lower(entry.type) != "boss" && lower(entry.type) != "spellcaster") 
        entry.+power := 1;
    endif

    if (!entry.name)
        entry.+name := "Missing Name";
    endif

    return entry;
endfunction

function SetSpawnGroupData(name, data := struct{})
    var df   := loadSpawnGroupData();
    var elem := DFFindElement(df, name, DF_CREATE);
    if (data.mob_list)
        foreach entry in (data.mob_list)
            entry := ModelMobListData(entry);
        endforeach
        elem.SetProp("mob_list", data.mob_list);
    endif

    if (data.evententry)
        elem.SetProp("evententry", data.evententry);
    endif

endfunction

function GetSpawnGroupData(name, create := 1)
    var df   := loadSpawnGroupData();
    var elem := DFFindElement(df, name, create);

    if (!elem)
        return 0;
    endif

    var data := struct{};
    data.+mob_list   := DFGetProp(elem, "mob_list", create, array{});
    data.+evententry := DFGetProp(elem, "evententry", create, "");

    return data;
endfunction

function RemoveGroupData(name)
    var df   := loadSpawnGroupData();
    df.DeleteElement(name);
endfunction

function RemoveGroupMobData(name, index)
    var elem   := GetSpawnGroupData(name);
    elem.mob_list.Erase(index);
    SetSpawnGroupData(name, elem);
endfunction

function UpdateGroupMobData(name, data := struct{}, index)
    var elem   := GetSpawnGroupData(name);
    elem.mob_list[index] := data;
    SetSpawnGroupData(name, elem);
endfunction

/*=====  End of Section comment block  ======*/