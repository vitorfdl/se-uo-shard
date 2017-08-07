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