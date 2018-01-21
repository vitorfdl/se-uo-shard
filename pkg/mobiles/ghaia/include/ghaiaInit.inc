use uo;
use cfgfile;

var couleurs;
var vetements;
var templt;

function SetupAmmo(mobile)

	if ( npccfgfile[mobile.my_template()].MissileWeapon )
		var weapon := CreateItemAtLocation(mobile.x, mobile.y, mobile.z, npccfgfile[mobile.my_template()].MissileWeapon, 1, mobile.realm);
		weapon.quality := 0.6;
		if ( !EquipItem(mobile, weapon) )
			DestroyItem(weapon);
		endif
	endif

	if ( npccfgfile[mobile.my_template()].AmmoType )
		if ( CInt(npccfgfile[mobile.my_template()].AmmoAmount) > 0 )
			var ammo := CreateItemInBackpack(mobile, npccfgfile[mobile.my_template()].AmmoType, CInt(npccfgfile[mobile.my_template()].AmmoAmount));
			if ( !ammo )
				return ammo;
			endif
			return 1;
		endif
	endif
	
	return 1;

endfunction

function SetupBackPack(mobile)
	if ( !mobile.backpack )
		var backpack := CreateItemAtLocation(mobile.x, mobile.y, mobile.z, "BackPack", 1);
		var result := EquipItem(mobile, backpack);
		if ( !result )
			DestroyItem(backpack);
			return result;
		else
			return backpack;
		endif
	endif

	return mobile.backpack;
endfunction

function SetupColor(who)
	var colors := npccfgfile[who.my_template()].colors;
	if(colors)
		colors := SplitWords(colors);
		var clr := Cint(colors[RandomInt(Len(colors)) + 1]);
		who.color := clr;
		who.truecolor := clr;
	endif
endfunction
		
function SetupName(mobile)
		
	if (!mobile.name["<random>"])
		return 1;
	endif

	var npc_names   := ReadConfigFile(":ghaia:config/names");
	var nametemplate:= GetConfigString(npccfgfile, "NameTemplate");
	if (mobile.graphic == 0x191)
		nametemplate["Male"] := "Female";
	endif
	var name_elem := npc_names[nametemplate];
	
	
	if ( !name_elem )
		//printtextabove(mobile, "npcdesc: " + npccfgfile);
		//printtextabove(mobile, "npc template " + mobile.my_template);
		//printtextabove(mobile, "npc_name: " + npc_names);
		//printtextabove(mobile, "nametemplate: " + nametemplate);
		//printtextabove(mobile, "name_elem: " + name_elem);
		return 0;
	endif

	var name_list := GetConfigStringArray(name_elem, "Name");
	if ( !name_list )
		//printtextabove(mobile, "elem name not found");
		//printtextabove(mobile, "name_elem " + name_elem);
		//printtextabove(mobile, "name_list " + name_list);
		return 0;
	endif

	var my_name := mobile.name;
	//while ( my_name == "<random>")
	my_name["<random>"] := name_list[RandomInt(Len(name_list))+1];
	SetName(mobile, my_name);
	//my_name := mobile.name;
	sleepms(2);
	//endwhile


	return 1;
endfunction

function SetupEquip(who)
	var group;
	var categ := GetConfigString(npccfgfile, "Equip");

	if(who.graphic == 0x190)
				//case(randomint(2))
					//0: 
						who.graphic:= 0x190;
						who.gender := 0;
						categ      := categ + "M";
	elseif (who.graphic == 0x191)
					//1: who.graphic := 0x191;
						 //who.gender := 1;
						 categ := categ + "F";
				//endcase
	 endif
	couleurs := ReadConfigFile(":ghaia:config/colors");
	vetements:= ReadConfigFile(":ghaia:config/cloths");
	templt   := ReadConfigFile(":ghaia:config/equip2");
	//printtextabove(me, " " + couleurs + " " + vetements + " " + templt);
	if(couleurs && vetements && templt)
		group := FindConfigElem(templt, categ);
		//printtextabove(who, " " +templt + " " + categ + " " + group);
		if(group)
			var lastc := 0;
			foreach str in GetConfigStringArray(group, "Equip")
						lastc := EquipItemString(who, str, lastc);
			endforeach
			foreach str in GetConfigStringArray(group, "Armor")
				lastc := EquipItemString(who, str, lastc );
			endforeach
			foreach str in GetConfigStringArray(group, "Weapon")
				//printtextabove(who, " " + str);
				lastc := EquipItemString(who, str, lastc);
			endforeach
		endif
	endif

endfunction

function EquipItemString(who, str, lastcolor)
	var params := SplitWords(str);
	var objname;
	var color;
	var chance;
	var lastc := 0;
	objname := params[1];
	if(CInt(objname))
		objname := CInt(objname);
	endif
	if(CInt(params[2]))
		color := CInt(params[2]);
	else
		color := 0;
		if(params[2] == "lastcolor" || params[2] == "lastcolour")
			color := lastcolor;
		else
			var colorg := FindConfigElem(couleurs, params[2]);
			if(colorg)
				var str := GetConfigStringArray(colorg, "Color");
				var ar := array{};
				var rndint := RandomInt(len(str)) + 1;
				ar := SplitWords(str[rndint]);
				if(len(ar) > 1)
					if(CInt(ar[2]) > 0)
						color := RandomInt(CInt(ar[2]) - CInt(ar[1]))+1;
					else
						color := CInt(str[RandomInt(len(str))+1]);
					endif
				else
					color := CInt(str[RandomInt(len(str))+1]);
				endif
				if(color == 1)
					color := 0;
				endif
			endif
		endif
	endif
	lastc := color;
	if(CInt(params[3]))
		chance := CInt( params[3] );
	else
		chance := 100;
	endif
	if(CInt(RandomInt(99)+1) <= CInt(chance))
		var item := CreateItemAtLocation(5852, 1163, 0, objname, 1);
	item.quality := 0.6;
		if(!item)
			var itemg := FindConfigElem(vetements, objname);
			if(itemg)
				var str := GetConfigStringArray(itemg, "Item");
				var newarray := array{};
				var i := 1;
				var sex := "";
				foreach str3 in str
					var str2 := SplitWords(str3);
					sex := CStr(str2[2]);
					if(sex != error)
						if(sex == "m" || sex == "f")
							if(sex == "m" && who.gender == 0)
								newarray[i] := str2[1];
								i := i + 1;
							elseif(sex == "f" && who.gender == 1)
								newarray[i] := str2[1];
								i := i + 1;
							endif
						else
							newarray[i] := str2[1];
							i := i + 1;
						endif
					else
						newarray[i] := str2[1];
						i := i + 1;
					endif
				endforeach
				objname := CInt(newarray[RandomInt(len(newarray))+1]);
				item := CreateItemAtLocation(5852, 1163, 0, objname, 1);
		item.quality := 0.6;
      item.hp := 0.5;
			endif
		endif
		item.color := color;
		if(CInt(params[4]) == 1)
			MoveItemToContainer(item, who.backpack);
		else
			if(!EquipItem(who, item))
				MoveItemToContainer(item, who.backpack);
			endif
		endif
	endif
	return lastc;
endfunction
