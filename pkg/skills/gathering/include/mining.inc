use uo;
use cfgfile;

include ":attributes:attributes";
include "include/client";
include "include/say";

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

function SetOreAmount(realm, x_pos, y_pos, amount)
	var pos_string := CStr(realm+" "+x_pos+"x"+y_pos);
	
	var data_elem := GetDataElem(pos_string);
	
	data_elem.SetProp("Amount", CInt(amount));
endfunction

function GetOreAmount(realm, x_pos, y_pos)

	var pos_string := CStr(realm+" "+x_pos+"x"+y_pos);
	
	var data_elem := GetDataElem(pos_string);
	
	var amount := data_elem.GetProp("Amount");
	if ( amount == error )
		SetOreAmount(realm, x_pos, y_pos, MAX_RESOURCE_AMOUNT);
		amount := MAX_RESOURCE_AMOUNT;
	endif
	
	return CInt(amount);
endfunction

function GetResourceId(oreToSearch)

	case (oreToSearch)
		"ferro": return 0x6300; //TN USED
		"estanho": return 0x6990; //TN USED		
		"ouro": return 0x6305;
		"cristal": return 0x6992;
		"adamantino": return 0x6301;
		"diamante": return 0x4079;
		
		/*"areia": return 0x6075;
		
		"argila": return 0x6074;
		
		"lithtan": return 0x6301;
		
		"helce": return 0x6302;
		
		"sinque": return 0x6303;
		
		"figin": return 0x6304;*/

		
		//"durang": return 0x6306;
		
		"galvorn": return 0x6307; //TN USED
		
		"mithril": return 0x6308; //TN USED

		"argonil": return 0x6451; //TN USED
 
		//"gromril": return 0x6452; //TN USED

		//"cobre": return 0x6450; //TN USED

		"prata": return 0x6992; //TN USED
 
		
		//"ario": return 0x6312;
		
		"pedras": return 0x5001;

		"marmore": return 0x8850;

		"arenito": return 0x8852;				
	
	endcase

endfunction

function DetermineOreType(who, where)
	var cfg := ReadConfigFile(":gathering:config/miningspots");
	var ores := cfg[where].Ores;
	var percent := cfg[where].Rarity;
  if (ores == error)
    var df := opendatafile(":gathering:miningspots");
    var elem := df.FindElement(where);
    ores := elem.GetProp("Ores");
    percent := elem.GetProp("Rarity");
  endif

	var i := 1;
	ores := SplitWords(ores);
	percent := SplitWords(percent);
	var chance := RandomInt(1000);
	var oreToSearch;
	var skill := AP_GetSkill(who, EXTRACAO);
	
	if (GetObjProperty(who, "searchore") == error)
      SendSysMessageEx(who, "Você deve selecionar um minério para procurar. Use .skill extração", SSM_FAIL);
		return 0;
	else
		oreToSearch := GetObjProperty(who, "searchore");
	endif
	
	var achou := 0;
	foreach ore in ores
		if (ore == oreToSearch)
			achou := 1;
			break;
		else
			i := i + 1;
		endif
	endforeach
	
	if (!achou)
		SendSysMessageEx(who, "Não tem o minerio que você procura aqui. Mude em .skill extração", SSM_FAIL);
		return 0;
	endif
	
	if ( chance <= Cint(percent[i]))
		var oreid := GetResourceId(oreToSearch);
		//var oreid := GetResourceId("ario");
		if (oreid == 0x6666)
			return 0x5001;
		endif

		return oreid;
	else
		SendSysMessageEx(who, "Você não conseguiu achar o minerio que procura.", SSM_FAIL);
		return 999;
	endif
endfunction

//dificuldade de PEGAR o minerio
function DetermineMiningDifficulty(oretype)
	case (cint(oretype))
		0xBD11: return 0;  //pedra pequena
		0xBD11: return 0;  //pedra media
		0xBD11: return 0;  //pedra grande
		0xee99: return 0;  //calcario

		// 0x4079: return 10; //diamantemagico
		0x6300: return 14;  //iron ore
		0x6992: return 80; //cristal
		0x6301: return 21; //adamant

		0x6992: return 19; //prata 
 		0x6990: return 17; //estanho 

		0x6075:  return 5; //areia
		
		0x6666: return 0; //id falso de pedra
		0x8850: return 40; //marmore
		0x8852: return 30; //arenito
		0x5001: return 5; //id de stone

		0x6305: return 16; //golden <- não faz nada
		
		default: return 100;
	endcase

endfunction

function GetOreRarityDesc(rarity)

	if (rarity <= 100 )
		return "muito raro";
	elseif (rarity <= 200 )
		return "raro";
	elseif (rarity <= 500 )
		return "incomum";
	elseif (rarity <= 700 )
		return "comum";
	else
		return "muito comum";
	endif 

endfunction

function InRect(x_pos, y_pos, rect)
	var x1 := CInt(rect[1]);
	var y1 := CInt(rect[2]);
	var x2 := CInt(rect[3]);
	var y2 := CInt(rect[4]);

	return ( (x_pos >= x1 && x_pos <= x2) && (y_pos >= y1 && y_pos <= y2) );
endfunction

function GetMiningRegion(x, y)

	var cfg := ReadConfigFile(":gathering:config/miningspots");
	var elems := GetconfigStringKeys(cfg);
  
	foreach elem in elems
	
		var region := FindConfigElem(cfg, elem);
		var rect := GetConfigString(region, "Rect");
		if ( InRect (x, y, SplitWords(rect)))
			return elem;
		endif 
		
	endforeach

  var df := opendatafile(":gathering:miningspots");
  foreach elem in (df.Keys())
  
    var region := df.FindElement(elem);
    var rect := region.getProp("Rect");
    if (inrect(x, y, SplitWords(rect)))
      return elem;
    endif
  
  endforeach

	return "Whole_World";

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
		SendSysMessageEx(who, "Você parou de minerar para ficar escondido.");
		return 0;
	elseif ( (who.x != my_x) || (who.y != My_y) )
		SendSysMessageEx(who, "Você saiu de onde estava.");
		return 0;
	elseif ( who.facing != my_facing )
		SendSysMessageEx(who, "Você virou sua atencao para outra coisa.");
		return 0;
	elseif ( GetEquipmentByLayer(who, LAYER_MOUNT) )
		SendSysMessageEx(who, "Você não pode minerar montado.");
		return 0;
	elseif (!tool)
		return 0;
	else
		return 1;	
	endif
endfunction
