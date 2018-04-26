use uo;
use attributes;
use vitals;

include "include/client";
include ":attributes:skills";
include ":datafile:datafile";
include ":attributes:attributes";
include ":attributes:stats";
include ":gumps:requestgump";

function possess(me, npc)
	if (GetObjProperty (me, "lastpossessedserial"))
		var oldnpc := SystemFindObjectBySerial(GetObjProperty(me, "lastpossessedserial"));
		if (oldnpc)
			unpossess (me);
		endif
	endif
	
	var oldbody := CreateNpcFromTemplate(":ghaia:humanmale", npc.x, npc.y, npc.z, 0, npc.realm);
	MoveObjectToLocation(oldbody, 5385, 923, 0, me.realm, MOVEOBJECT_FORCELOCATION);
	SetObjProperty(me, "oldmeserial", oldbody.serial);
	SetObjProperty(me, "lastpossessedserial", npc.serial);
	
	clonar(oldbody, me);
	var itemsinbag := EnumerateItemsInContainer( me.backpack, ENUMERATE_ROOT_ONLY );
	foreach iteminbag in itemsinbag
		MoveItemToContainer( iteminbag, oldbody.backpack);
		sleepms(2);
	endforeach 
	
	clonar(me, npc);
	DestroyItem(me.backpack);
	var bolsa := CreateItemAtLocation(5384, 923, 0, 0x9B2);
	EquipItem(me, bolsa);
	var itemsinbag2 := EnumerateItemsInContainer( npc.backpack, ENUMERATE_ROOT_ONLY);
	foreach iteminbag in itemsinbag2
		MoveItemToContainer( iteminbag, me.backpack );
		sleepms(2);
	endforeach 
	
	npc.concealed := 1;
	me.concealed := 0;
	me.facing := npc.facing;
	
	MoveObjectToLocation(me, npc.x, npc.y, npc.z, npc.realm, MOVEOBJECT_FORCELOCATION);
	MoveObjectToLocation(npc, 5383, 923, 0, npc.realm, MOVEOBJECT_FORCELOCATION);
	npc.frozen := 1;
	oldbody.frozen := 1;
	me.Disable("invul");
	me.Disable("losany");

   // if (!(cint(npc.graphic) in array{400, 401, 600,601}))
   //    SetObjProperty(who, "dicedmg", )
   // endif
	
	if ((GetObjProperty(npc,"npchero"))!=error)
		SetObjProperty(me,"npchero",1);
	endif
		
	return 1;	
endfunction

function unpossess(me, freeze :=1)
	var oldnpcserial := getObjProperty(me, "lastpossessedserial");
	var oldnpc := SystemFindObjectBySerial(oldnpcserial);
	oldnpc.frozen := 0;
	var oldbodyserial := getObjProperty(me, "oldmeserial");
	var oldbody := SystemFindObjectBySerial(oldbodyserial);
	
	var container := CreateItemAtLocation(5384, 923, 0, "backpack", 1, oldnpc.realm);
	container.invi := 1;
	foreach item in ( EnumeratePhysicalItems(oldnpc) )
		MoveItemToContainer(item, container);
		SleepMS(2);
	endforeach
	DestroyItem(container);
	
	clonar(oldnpc, me);
	
	DestroyItem(oldnpc.backpack);
	var bolsa := CreateItemAtLocation(5383, 923, 0, 0x9B2);
	EquipItem(oldnpc, bolsa);
	var itemsinbag := EnumerateItemsInContainer( me.backpack );
	foreach iteminbag in itemsinbag
		MoveItemToContainer(iteminbag, oldnpc.backpack);
	endforeach 
	oldnpc.facing := me.facing;
	MoveObjectToLocation (oldnpc, me.x, me.y, me.z, me.realm, MOVEOBJECT_FORCELOCATION);
	oldnpc.frozen    := 1;
	oldnpc.concealed := 0;
	me.concealed     := 1;

	container := CreateItemAtLocation(5383, 923, 0, "backpack", 1, me.realm);
	container.invi := 1;
	foreach item in ( EnumeratePhysicalItems(me) )
		MoveItemToContainer(item, container);
		SleepMS(2);
	endforeach
	DestroyItem(container);
	
	clonar(me, oldbody);
	DestroyItem(me.backpack);
	bolsa := CreateItemAtLocation(5383, 923, 0, 0x9B2);
	EquipItem(me, bolsa);
	var itemsinbag2 := EnumerateItemsInContainer( oldbody.backpack );
	foreach iteminbag in itemsinbag2
     	var copyitem := CreateItemCopyAtLocation(5383, 923, 0, iteminbag);
		MoveItemToContainer( copyitem, me.backpack );
		destroyitem ( iteminbag );
		sleepms(10);
	endforeach 
	
	SetObjProperty(oldbody, "GuardKill", 1);
	ApplyRawDamage(oldbody, 6000);
	destroyitem(oldbody);
	
	me.enable("invul");
	me.enable("losany");
	
	if (freeze)
		oldnpc.frozen := 0;
	else
		oldnpc.frozen := 1;
	endif

   EraseObjProperty(me, "metamorfose");

	AP_RefreshVitals(me);
	EraseObjProperty(me, "npchero");
	EraseObjProperty(me, "oldmeserial");
	EraseObjProperty(me, "lastpossessedserial");
	EraseObjProperty(me, "ignorenpcs");
	return 1;
endfunction


function clonar(newlook, oldlook)
	var mypack := GetEquipmentByLayer(newlook, 0x15);
	if (!mypack)
		mypack := CreateItemAtLocation(5383, 923, 0, 0xe75, 1, newlook.realm);
      EquipItem(newlook, mypack);
	else
		MoveObjectToLocation(mypack, 5383, 923, 0, newlook.realm, MOVEOBJECT_FORCELOCATION);
	endif

	foreach attrib_name in AP_ListAttributesByType()
		var valor := AP_GetTrueSkill( oldlook, attrib_name );
      var mod := AP_GetSkillMod( oldlook, attrib_name );
		AP_SetTrueSkill(newlook, attrib_name, valor);
		AP_SetSkillMod(newlook, attrib_name, mod);
		sleepms(2);
	endforeach

	var itemlist := ListEquippedItems(newlook);
	var y;
	foreach x in itemlist
		MoveItemToContainer(x, mypack);
      sleepms(2);
	endforeach

	itemlist := ListEquippedItems(oldlook);
	foreach x in itemlist
		y := CreateItemCopyAtLocation(5383, 923, 0, x);
		y.color := x.color;
		y.name := x.name;
		y.desc := x.desc;
		EquipItem(newlook, y);
	endforeach
	
	var desc_oldlook:=GetObjProperty(oldlook,"description");

	if (desc_oldlook!=error) 
		SetObjProperty(newlook,"description",desc_oldlook);
	else
		EraseObjProperty(newlook,"description");
	endif

	newlook.gender       := oldlook.gender;
	newlook.graphic      := oldlook.graphic;
	newlook.color        := oldlook.color;
	newlook.title_race   := oldlook.title_race;
	newlook.title_suffix := oldlook.title_suffix;
	newlook.title_prefix := oldlook.title_prefix;
	newlook.name         := oldlook.name;
	newlook.TrueColor    := oldlook.TrueColor;
	newlook.trueobjtype  := oldlook.trueobjtype;
	
	newlook.murderer := oldlook.murderer;

   case (oldlook.alignment)
      1: newlook.murderer := 1;
   endcase

	var chardata;
   if (GetObjProperty(oldlook, "chardata"))
		chardata := GetObjProperty(oldlook, "chardata");
   else
      chardata := struct{};
	endif
   SetObjProperty(newlook, "chardata", chardata);
   RecalcVitals(newlook);

	var newhits := cdbl(AP_GetVitalMaximumValue(newlook, HITS)) * (((cdbl(AP_GetVital(oldlook, HITS)) * 100) / cdbl(AP_GetVitalMaximumValue(oldlook, HITS))) / 100);
	var newmana := cdbl(AP_GetVitalMaximumValue(newlook, MANA)) * (((cdbl(AP_GetVital(oldlook, MANA)) * 100) / cdbl(AP_GetVitalMaximumValue(oldlook, MANA))) / 100);
	AP_SetVital(newlook, HITS, cint(newhits));
	AP_SetVital(newlook, STAMINA, cint( cdbl(AP_GetVitalMaximumValue(newlook, STAMINA)) * 0.5 ));
	AP_SetVital(newlook, MANA, cint(newmana));
	//Copia o profile do npc.
	var cp_data_file_oldlook  := DFOpenDataFile(":charprofile:CPFile", DF_CREATE);
	var cp_elem_oldlook       := DFFindElement(cp_data_file_oldlook, oldlook.serial, DF_CREATE);
	var cp_profile_oldlook    := DFGetProp(cp_elem_oldlook, "Profile", DF_CREATE);

	var cp_data_file_newlook  := DFOpenDataFile(":charprofile:CPFile", DF_CREATE);
	var cp_elem_newlook       := DFFindElement(cp_data_file_newlook, newlook.serial, DF_CREATE);
	
	cp_elem_newlook.SetProp("Profile", cp_profile_oldlook);
	
	// var newpack := GetEquipmentByLayer(newlook, 0x15);
	// if (newpack)
	// 	DestroyItem(newpack);
	// endif
	// 
	
	if ((GetObjProperty(oldlook,"achievements"))!=error)
		SetObjProperty(newlook,"achievements",GetObjProperty(oldlook,"achievements"));
	endif
	
	if ((GetObjProperty(oldlook,"recipes"))!=error)
		SetObjProperty(newlook,"recipes",GetObjProperty(oldlook,"recipes"));
	endif
	
	if ((GetObjProperty(oldlook,"toughness"))!=error)
		SetObjProperty(newlook,"toughness",GetObjProperty(oldlook,"toughness"));
	endif	
	
	return 1;
endfunction

function EnumeratePhysicalItems(mobile)
	var EquipList := array;

	var SkipLayers := array{11, 16, 21, 25};
	var i;

	for (i:=0; i<=25; i:=i+1)
		if (!(i in SkipLayers))
			var layer := GetEquipmentByLayer(mobile, i);
			if (layer)
				EquipList.append(layer);
			endif
		endif
	endfor

	return EquipList;
endfunction
