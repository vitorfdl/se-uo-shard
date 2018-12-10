/*
 * $Id*
 *
 */
 
use uo;
use os;
use math;

include "include/facings";
/*
 * GetCoordsInSquare(mid_x, mid_y, radius)
 *
 * Purpose
 * Retrieves the coordinates in the area of a square.
 *
 * Parameters
 * mid_x:	X coordinate of the middle of the square.
 * mid_y:	Y coordinate of the middle of the square.
 * radius:	Size of the square
 *
 * Return value
 * Array of structs (.x, .y)
 * 
 */
function GetCoordsInSquare(mid_x, mid_y, radius)
	var x_row := CInt(mid_x)-CInt(radius);
	var y_col := CInt(mid_y)-CInt(radius);
	var stop_x := CInt(mid_x)+CInt(radius);
	var stop_y := CInt(mid_y)+CInt(radius);

	var point_list := array{};
	var coord := struct{"x", "y"};
	
	for ( x_row:=(mid_x-radius); x_row<=stop_x; x_row:=x_row+1 )
		for ( y_col:=(mid_y-radius); y_col<=stop_y; y_col:=y_col+1 )
			coord.x := x_row;
			coord.y := y_col;
			point_list.Append(coord);
			sleepms(2);
		endfor
		sleepms(2);
	endfor
	
	return point_list;
endfunction

/*
 * GetCoordsInRect(x1, y1, x2, y2)
 *
 * Purpose
 * Retrieves the coordinates in the area of a rectangle.
 *
 * Parameters
 * x1:	X coordinate
 * y1:	Y coordinate
 * x2:	X coordinate
 * y2:	Y coordinate
 *
 * Return value
 * Array of structs (.x, .y)
 * 
 */
function GetCoordsInRect(x1, y1, x2, y2)
	var point_list := array{};
	var coord := struct{"x", "y"};
	
	x1 := CInt(x1);
	y1 := CInt(y1);
	x2 := CInt(x2);
	y2 := CInt(y2);
	
	if ( x1 > x2 )
		x1 := x1*-1;
		x2 := x2*-1;
	endif
	if ( y1 > y2 )
		y1 := y1*-1;
		y2 := y2*-1;
	endif
	
	var sx, sy;
	for ( sx:=x1; sx<=x2; sx:=sx+1 )
		for ( sy:=y1; sy<=y2; sy:=sy+1 )
			coord.x := Abs(sx);
			coord.y := ABs(sy);
			point_list.Append(coord);
			sleepms(2);
		endfor
		sleepms(2);
	endfor
	
	return point_list;
endfunction

/*
 * GetCoordsInCircle(mid_x, mid_y, radius)
 *
 * Purpose
 * Retrieves the coordinates in the area of a circle.
 *
 * Parameters
 * mid_x:	X coordinate of the middle of the square.
 * mid_y:	Y coordinate of the middle of the square.
 * radius:	Size of the circle
 *
 * Return value
 * Array of structs (.x, .y)
 * 
 */
function GetCoordsInCircle(x, y, radius)
   var circle_points := array{ };

   var radius2 := radius * radius;

   var i;
   for (i := 0; i < radius; i+=1)
      var j := 0;
      while (i*i + j*j < radius2 )
      sleepms(2);
         if (i != 0 || j != 0)
            if (j != 0) circle_points.append(struct{ x := x+i, y := y+j }); endif
            if (j != 0 && i != 0) circle_points.append(struct{ x := x-i, y := y-j }); endif
            if (j != 0 || i != 0) circle_points.append(struct{ x := x+i, y := y-j }); endif
            if (i != 0) circle_points.append(struct{ x := x-i, y := y+j }); endif
         endif
         j += 1;
      endwhile
   endfor

   return circle_points;
endfunction

/*
 * GetConeCoordsInFront(x, y, facing, range)
 *
 * Purpose
 * Retrieves the coordinates in the cone front of the facing
 *
 * Parameters
 * x:	X coordinate of the start of the cone
 * y:	Y coordinate of the start of the cone
 * facing:	mobile facing
 * range:	range in front
 *
 * Return value
 * Array of arrays where (x, y)
 * 
 */
function GetConeCoordsInFront(x, y, facing, range)
   var mod := GetModByFacing(facing);
   var coords := array{};
   for i := 1 to range
      var base_x := x+(i*mod[1]);
      var base_y := y+(i*mod[2]);
      coords.append(array{ base_x, base_y});
      // drawn up
      for t := 1 to (i * 2)
         var t_mod;
         if ( t <= i)
            t_mod := GetModByFacing(facing, t);
         else
            t_mod := GetModByFacing(facing, (t-i)*-1);
         endif

         var x_mod := base_x + (t_mod[1] - mod[1]);
         var y_mod := base_y + (t_mod[2] - mod[2]);
         
         coords.append(array{x_mod, y_mod});
         sleepms(2);
      endfor
      sleepms(2);
   endfor

	return coords;
endfunction


/*
 * GetConeCoordsInFront(x, y, facing, range)
 *
 * Purpose
 * Retrieves the coordinates in the cone front of the facing
 *
 * Parameters
 * x:	X coordinate of the start of the cone
 * y:	Y coordinate of the start of the cone
 * facing:	mobile facing
 * range:	range in front
 *
 * Return value
 * Array of arrays where (x, y)
 * 
 */
function GetLineCoordsInFront(x, y, facing, range)
   var mod := GetModByFacing(facing);
   var coords := array{};
   for i := 1 to range
      var base_x := x+(i*mod[1]);
      var base_y := y+(i*mod[2]);
      coords.append(array{ base_x, base_y});
      sleepms(2);
   endfor

	return coords;
endfunction
         //    var t_mod := GetModByFacing(facing, t);
         //    coords.append(array{ x+(i*mod[1])+t_mod[1], y+(i*mod[2])+t_mod[2]});
         // else
         //    var t_mod := GetModByFacing(facing, t*-1);
         //    coords.append(array{ x+(i*mod[1])+t_mod[1], y+(i*mod[2])+t_mod[2]});

/* 
 * GetCoordsInPolygon(coord_list)
 *
 * Purpose
 * Takes in an array of structs containing .x, .y coordinates.
 * Each point is a segment that builds the shape.
 * Finds the coordinates inside of that shape.
 *
 *
 * Parameters
 * coord_list:	Array of structs. Each struct contains an x and y member.
 *
 * Return value
 * Returns an array of structs containing a .x and .y member.
 *
 */
function GetCoordsInPolygon(coord_list)
	var num_coords := coord_list.Size();
	if ( num_coords < 3 )
		return error{"errortext":="Needs atleast three coordinates to form a polygon."};
	endif
	
	//var start_coord := coord_list[1];
	//var end_coord := coord_list[num_coords];
	
	return error{"errortext":="Hasn't been coded yet!"};
endfunction
