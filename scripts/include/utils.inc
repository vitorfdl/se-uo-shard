use uo;
use os;
use math;

function randomPlace(loc, radius := 1)
	var y, x;
   case (RandomInt(radius+1))
      1: y := RandomInt(3) * -1; x := RandomInt(3);
      2: y := RandomInt(3); 		x := RandomInt(3) * -1;
      3: y := RandomInt(3) * -1; x := RandomInt(3) * -1;
      4: y := RandomInt(3); 		x := RandomInt(3);
   endcase

	var new_loc := struct{
		"x" := loc.x+x,
		"y" := loc.y+y
	};

   return new_loc;
endfunction

function GetPointsInRadius(mid_x, mid_y, radius)
	var x_row := mid_x-radius;
	var y_col := mid_y-radius;
	var stop_x := mid_x+radius;
	var stop_y := mid_y+radius;

	var point_list := array();
	var coord := struct;
	coord.+x;
	coord.+y;

	for ( x_row:=(mid_x-radius); x_row<=stop_x; x_row:=x_row+1 )
		sleepms(2);
		for ( y_col:=(mid_y-radius); y_col<=stop_y; y_col:=y_col+1 )
			coord.x := x_row;
			coord.y := y_col;
			point_list.append(coord);
			sleepms(2);
		endfor
	endfor

	return point_list;
endfunction

function PushBack(who, dist, delay, invert := 0)
	var x := 0;
	var y := 0;

	if (who.facing == 0)
		y :=  1;
	elseif (who.facing == 1)
		y :=  1;
		x := - 1;
	elseif (who.facing == 2)
		x :=  - 1;
	elseif (who.facing == 3)
		x :=  - 1;
		y :=  - 1;
	elseif (who.facing == 4)
		y :=  - 1;
	elseif (who.facing == 5)
		x :=   1;
		y :=  - 1;
	elseif (who.facing == 6)
		x :=   1;
	elseif (who.facing == 7)
		x :=  1;
		y :=  1;
	endif

	if (invert)
		x := x * -1;
		y := y * -1;
	endif

	var i := 0;
	for( i := 1; i <= dist; i := i + 1)
		var finalx := (who.x + x);
		var finaly := (who.y + y);
		if (!CheckLosAt(who, finalx, finaly, who.z))
			break;
		endif
		var err := MoveObjectToLocation( who, finalx, finaly, who.z, who.realm );
		sleepms(delay);
		if (err == error)
		//	PrintText(who, "*bate no obstaculo*");
			break;
		endif
	endfor
endfunction


function FLSCreateItem(who, item, amount := 1)
   var created := CreateItemAtLocation(who.x, who.y, who.z, item, amount, who.realm);
	if (who.cmdlevel < 4)
		SetObjProperty(created, "gm_createdby", who.acctname + " " + who.name);
	endif
   
	if ( !created )
		if ( amount > 10 )
			SendSysMessage(who, "Item is not stackable. No more than 10 will be created at a time.");
			amount := 10;
		endif
		
		for( amount; amount > 0; amount := amount-1 )
			created := CreateItemAtLocation(who.x, who.y, who.z, item, 1, who.realm);
			if (who.cmdlevel < 4)
				SetObjProperty(created, "gm_createdby", who.acctname + " " + who.name);
			endif
		endfor	
	endif
	
	if ( !created )
		SendSysMessage(who, "Error: Could not create "+Hex(item));
	endif
endfunction

function FLSCreateItemBackpack(who, item, amount := 1)
   var created := CreateItemInBackPack(who, item, amount);
	if (who.cmdlevel < 4)
		SetObjProperty(created, "gm_createdby", who.acctname + " " + who.name);
	endif
   
	if ( !created )
		if ( amount > 10 )
			SendSysMessage(who, "Item is not stackable. No more than 10 will be created at a time.");
			amount := 10;
		endif
		
		for( amount; amount > 0; amount := amount-1 )
			created := CreateItemInBackPack(who, item, amount);
			if (who.cmdlevel < 4)
				SetObjProperty(created, "gm_createdby", who.acctname + " " + who.name);
			endif
		endfor	
	endif
	
	if ( !created )
		SendSysMessage(who, "Error: Could not create "+Hex(item));
	endif
endfunction