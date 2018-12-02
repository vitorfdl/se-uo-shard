use uo;
use cfgfile;

include ":attributes:attributes";
include "include/client";
include "include/say";
include "include/utils";

const MAX_RESOURCE_AMOUNT := 10;

function Garimpeiro(who)
   var cfg := ReadConfigFile(":gems:itemdesc");
   var keys := GetConfigStringKeys( cfg );

   SendSysMessageEx(who, "Você achou uma pedra preciosa!", SSM_INFO);
   CreateItemInBackPack(who, keys.randomentry(), 1);

	return 1;
endfunction

function GetMapsInBackpack(who)
	var items := EnumerateItemsInContainer(who.backpack);
	var maps := array;
    	foreach item in items
		//sendsysmessageEx(who, " " + isTreasureMap(item) + " " + item.objtype);
		if ( isTreasureMap(item) )
			if ( !isSolved(item) )
				maps.append(item);
			endif
		endif
	endforeach
	return maps;


endfunction

function isTreasureMap(item)

	if ( GetObjProperty(item, "chestx") != error )
		return 1;
	endif

	return 0;

endfunction

function isSolved(map)
    if(GetObjProperty(map, "Solved") == error)

	return 0;
    else
	return 1;

    endif
endfunction

// function DigForMap(who, targ, map)

// 	if (!reserveItem(map))
// 		return;
// 	endif

// 	var chestx := Cint(GetObjProperty( map, "chestx" ));
// 	var chesty := Cint(GetObjProperty( map, "chesty" ));
// 	var chestz := GetWorldHeight(chestx, chesty);

// 	if((!chestx) || (!chesty))
// 		SendSysMessageEx( who, "Este não parece ser um mapa de tesouro.", SSM_FAIL );
// 		return;
// 	endif

// 	if(!CheckLosAt( who, targ.x, targ.y, targ.z ) );
// 		SendSysMessageEx( who, "Você não vê o alvo.", SSM_FAIL );
// 		return;
// 	endif

// 	PerformAction(who, 0x0b);
// 	PlaySoundEffect(who, 0x126);

// 	var range := 5; //GetRange( who );
// 	var distn := coordinatedistance( targ.x, targ.y, chestx, chesty );
// 	var maxrange := 20;
// 	var mobiles := ListMobilesNearLocation(targ.x, targ.y, targ.z, 0, who.realm);

// 	if(distn > maxrange)
// 		SendSysMessageEx(who,"Você parece muito distante.", SSM_INFO);
// 		return;

// 	elseif((distn > range) && (distn <= maxrange))
// 		SendSysMessageEx(who,"Você cavou e cavou mas não encontrou nenhum tesouro.", SSM_INFO);
// 		return;

// 	elseif((distn < range) && (len(mobiles) > 0))
// 		SendSysMessageEx(who,"você patou de cavar porque algo esta sobre o tesouro.");
// 		return;
// 	else

// 		var i := 1;
// 		var mex := who.x;
// 		var mey := who.y;
// 		var parts := {};
// 		var maplevel := CInt(GetObjProperty(map, "level") );
// 		var loot := "treasuremap" + maplevel;

// 		var chest;
// 		chestz := chestz - 8;
// 		sendsysmessageEx(who, " " + chestz);
// 		var chests := CreateTreasureChests(maplevel, 1, 0, chestx, chesty, chestz, who.realm, loot);
// 		chest := chests[1];
// 		if (GetObjProperty(map, "tesouroescondido") == 1)
// 			Loot_Generate(chest, loot);
// 		endif
// 		var ret := MoveObjectToLocation(chest, chest.x, chest.y, chestz, who.realm,  MOVEOBJECT_FORCELOCATION);
// //		sendsysmessageEx(who, " " + ret);

// 		chest.movable := 0;

// 		while((i <= 9) && (who.x == mex) && (who.y == mey))
// 			if((who.x != mex) && (who.y != mey))
// 				foreach thing in parts
// 					DestroyItem(thing);
// 				endforeach
// 				break;
// 			endif

// 			PerformAction(who, 0x0b);
// 			PlaySoundEffect(who, 0x126);
// 			sleep(1);

// 			if(i == 2)
//           			var patch := CreateItemAtLocation(chestx, chesty, chestz, 2321, 1, who.realm);
//           			patch.movable := 0;
// 		          	parts.append(patch);
//         		elseif(i == 3)
//           			var patch := CreateItemAtLocation(chestx, chesty, chestz, 2322, 1, who.realm);
//           			patch.movable := 0;
//           			parts.append(patch);
//         		elseif(i == 4)
//           			var patch := CreateItemAtLocation(chestx, chesty, chestz, 2323, 1, who.realm);
//           			patch.movable := 0;
//           			parts.append(patch);
//         		elseif(i == 5)
//           			var patch := CreateItemAtLocation(chestx, chesty, chestz, 2324, 1, who.realm);
//           			patch.movable := 0;
//           			parts.append(patch);
//           			chest.movable := 1;
//           			MoveObjectToLocation(chest, chest.x, chest.y, (chest.z+2), who.realm,  MOVEOBJECT_FORCELOCATION);
//           			chest.movable := 0;
//         		elseif(i == 6)
//           			var patch := CreateItemAtLocation(chestx, chesty, chestz, 2324, 1, who.realm);
//           			patch.movable := 0;
//           			parts.append(patch);
//           			chest.movable := 1;
//           			MoveObjectToLocation(chest, chest.x, chest.y, (chest.z+2), who.realm,  MOVEOBJECT_FORCELOCATION);
//           			chest.movable := 0;
//         		elseif(i == 7)
//           			var patch := CreateItemAtLocation(chestx, chesty, chestz, 2324, 1, who.realm);
//           			patch.movable := 0;
//           			parts.append(patch);
//           			chest.movable := 1;
//           			MoveObjectToLocation(chest, chest.x, chest.y,( chest.z+2), who.realm,  MOVEOBJECT_FORCELOCATION);
//           			chest.movable := 0;
//         		elseif(i == 8)
//           			chest.movable := 1;
//           			MoveObjectToLocation(chest, chest.x, chest.y, (chest.z+2), who.realm,  MOVEOBJECT_FORCELOCATION);
//           			chest.movable := 0;
//           			foreach thing in parts
//             				DestroyItem(thing);
//           			endforeach
//         		endif
// 			if((who.x != mex) || (who.y != mey))
// 				SendSysMessageEx(who,"Você deve ficar parado enquanto cava.", SSM_FAIL);
// 				DestroyItem(chest);
// 		        	return;
// 		      	endif

//         		i := i + 1;

// 		endwhile

// 		chest.movable := 1;
//       		EraseObjProperty(map, "level");
//       		EraseObjProperty(map, "chestx");
//       		EraseObjProperty(map, "chesty");
//       		SetObjProperty(map, "Solved", 1);
//       		map.name := "mapa de tesouro [completado por " + who.name + "]";

//     endif
// endfunction

function GetRange( who );

  var skill := Cint((AP_GetSkill( who, EXTRACAO )));

  if( skill >= 100 )
    return 5;
  elseif( skill >= 80 )
    return 4;
  elseif( skill >= 60 )
    return 3;
  elseif( skill >= 40 )
    return 2;;
  elseif( skill >= 20 )
    return 1;
  else
    return 0;
  endif

endfunction

function GetDataFile()
	var data_file := DFOpenDataFile(":gathering:OreSettings", DF_CREATE);
	return data_file;
endfunction


function GetDataElem(elem_name)
	var data_file := GetDataFile();
	var data_elem := DFFindElement(data_file, elem_name, DF_CREATE);

	return data_elem;
endfunction

function CanMine(targ, type)
	var config := ReadConfigFile(":gathering:config/miningtiles");
	config := config[type];

	var land_scape := CStr( GetMapInfo(targ.x, targ.y).landtile );
	if ( targ.objtype )
		if ( CStr(targ.objtype) in GetConfigStringArray(config, "static") )
			return 1;
		endif
	elseif ( land_scape in GetConfigStringArray(config, "landscape") )
		return 1;
	endif
	
	return 0;	
endfunction

function ResourceAmount(resource_type, byref elem)
	var amount := DFGetProp(elem, resource_type, DF_CREATE, CStr(MAX_RESOURCE_AMOUNT));
	
	return CInt(amount);
endfunction

function SetResource(resource_type, byref elem, amount)
	elem.setprop( resource_type, CStr(amount) );
endfunction


function CheckLoop(who, tool, my_x, my_y, my_facing)
	if ( who.hidden )
		SendSysMessageEx(who, "Você parou de minerar para ficar escondido.", SSM_FAIL);
		return 0;
	elseif ( (who.x != my_x) || (who.y != My_y) )
		SendSysMessageEx(who, "Você saiu de onde estava.", SSM_FAIL);
		return 0;
	elseif ( who.facing != my_facing )
		SendSysMessageEx(who, "Você virou sua atencão para outra coisa.", SSM_FAIL);
		return 0;
	elseif ( GetEquipmentByLayer(who, LAYER_MOUNT) )
		SendSysMessageEx(who, "Você não pode minerar montado.", SSM_FAIL);
		return 0;
	elseif (!tool)
		return 0;
	else
		return 1;	
	endif
endfunction
