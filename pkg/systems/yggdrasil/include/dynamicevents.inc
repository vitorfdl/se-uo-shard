use os;
use datafile;

include ":datafile:datafile";

/**
 * [Commands for DynamicEvents]
 * LoadDynamicGroupData() - Get the DataFile document
 * SetDynamicEventData(group_name, group_struct) - Set changes to a group;
 * GetDynamicEventData(group_name) - Get all info about a group;
 * RemoveDynamicGroupData(group_name) - Remove a group;
 * RemoveEventData(group_name, index_mob) - Remove a mob from mobList. Need index;
 * UpdateEventData(group_name, mob_struct, index) - Update all informations about a mob.
 */

/*=============================================
=       DynamicEvents functions code            =
=============================================*/

function LoadDynamicGroupData()
    var df := DFOpenDataFile( ":yggdrasil:dynamicevents", DF_CREATE, DF_KEYTYPE_STRING);
    return df;
endfunction

function SetDynamicEventData(name, data := struct{})
    var df   := LoadDynamicGroupData();
    var elem := DFFindElement(df, name, DF_CREATE);
    if (data.events)
        elem.SetProp("events", data.events);
    endif

    if (data.conditions)
        elem.SetProp("conditions", data.conditions);
    endif
endfunction

function GetDynamicEventData(name, create := 1)
    var df   := LoadDynamicGroupData();
    var elem := DFFindElement(df, name, create);

    if (!elem)
        return 0;
    endif

    var data := struct{};
    data.+events     := DFGetProp(elem, "events", create, array{});
    data.+conditions := DFGetProp(elem, "conditions", create, array{});

    return data;
endfunction

function RemoveDynamicGroupData(name)
    var df   := LoadDynamicGroupData();
    df.DeleteElement(name);
endfunction

function RemoveEventData(name, index)
    var elem   := GetDynamicEventData(name);
    elem.events.Erase(index);
    SetDynamicEventData(name, elem);
endfunction

function UpdateEventData(name, data := struct{}, index)
    var elem   := GetDynamicEventData(name);
    elem.events[index] := data;
    SetDynamicEventData(name, elem);
endfunction

function RemoveConditionData(name, index)
    var elem   := GetDynamicEventData(name);
    elem.conditions.Erase(index);
    SetDynamicEventData(name, elem);
endfunction

function UpdateConditionData(name, data := struct{}, index)
    var elem   := GetDynamicEventData(name);
    elem.conditions[index] := data;
    SetDynamicEventData(name, elem);
endfunction