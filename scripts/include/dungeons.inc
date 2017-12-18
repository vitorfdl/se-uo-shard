use uo;

const BEETLE_MOUNTPIECE_GRAPHIC :=  0x3ebc;

function InDungeonMountCheck(mobile, coord) //retorna 1 se estiver fora de caverna ou montado num Beetle
	
	var mount_ref := GetEquipmentByLayer(mobile, 0x19); //LAYER_MOUNT
	if ( !mount_ref )
		return 1;
	elseif ( mount_ref.graphic == BEETLE_MOUNTPIECE_GRAPHIC )
		return 1;
	elseif ( !InDungeon(coord) )
		return 1;
	else
		return 0;
	endif
endfunction

function InDungeon(coord) // retorna 1 se tiver numa caverna
	if ( coord.x > 3768 && coord.y > 1141 && coord.x < 4326 && coord.y < 2461 )
		return 1;
	elseif ( coord.x > 4388 && coord.y > 0 && coord.x < 3168 && coord.y < 4104 )
		return 1;
	elseif ( coord.x > 4392 && coord.y > 0 && coord.x < 6131 && coord.y < 4086 )
		return 1;
	else
		return 0;
	endif
endfunction

function HasPlayerNear(x, y, realm := 0)
	var mobs := ListMobilesNearLocationEx( x, y, LIST_IGNORE_Z, 15, LISTEX_FLAG_PLAYERS_ONLY, realm );
	foreach mob in mobs
		if (mob.acctname ) //se for player e priv de player
			return 1;
		endif
	endforeach
	return 0;
endfunction

function HaveLightNear(coord, realm := 0)
	var daytime := GetGlobalProperty("dia");
	if (daytime == "dia" && !InDungeon(coord))
		return 1;
	endif

	var light_cfg := ReadConfigFile(":lighting:itemdesc");
	var mobiles    := ListMobilesNearLocation( coord.x, coord.y, coord.z, 5, realm );
	foreach mobile in mobiles
		var equipleft  := GetEquipmentByLayer(mobile, 0x01);
		var equipright := GetEquipmentByLayer(mobile, 0x02);
		if (FindConfigElem(light_cfg, equipright.objtype) || FindConfigElem(light_cfg, equipleft.objtype))
			return 1;
		endif
		sleepms(2);
	endforeach

	var items := ListItemsNearLocationWithFlag(coord.x, coord.y, coord.z, 5, TILEDATA_FLAG_LIGHTSOURCE, realm );
	if (items.size() > 0)
		return 1;
	endif

	return 0;
endfunction