/* $Id: facings.inc 1310 2006-05-11 21:53:20Z austin $
 *
 */
use uo;
use os;
use math;


/*
 * TurnObjectToward(object, x_pos, y_pos)
 *
 * Purpose
 * Sets an objects .facing member to face an x, y coordinate.
 *
 * Purpose
 * object:	Object to set the facing on.
 * x_pos:	X position to check.
 * y_pos:	Y position to check.
 *
 * Return value
 * Object's original facing
 *
 */
function TurnObjectToward(object, x_pos, y_pos)
	var orig := object.facing;

	object.facing := GetFacing(object.x, object.y, x_pos, y_pos);

	return orig;
endfunction


/*
 * IsFacing(mobile, x_pos, y_pos)
 *
 * Purpose
 * Determines if a mobile is facing a set of coordinates.
 *
 * Parameters
 * mobile:	Mobile to check the coordinates with.
 * x_pos:	X position to check.
 * y_pos:	Y position to check.
 *
 * Return value
 * Returns 1 if the mobile is facing the coordinates.
 * Returns 0 if not.
 *
 */
 function IsFacing(mobile, x, y)
	return ( mobile.facing == GetFacing(mobile.x, mobile.y, x, y) );
endfunction


/*
 * IsBehind(mobile, x_pos, y_pos)
 * Determines if a set of coordinates are behind a character.
 * Example ( ^ - character * - behind )
 *         ^
 *        ***
 *       *****
 *      *******
 *
 * Parameters
 * mobile:	Mobile to check the coordinates with.
 * x_pos:	X position to check.
 * y_pos:	Y position to check.
 *
 * Return value
 * Returns 1 if the coordinates are behind the mobile.
 * Returns 0 if not.
 *
 */
function IsBehind(tgt_mob, locx, locy)
	if (tgt_mob.x == locx && tgt_mob.y == locy)
		return 0;
	endif

	var frot := Abs(((tgt_mob.facing+4)%8)-GetFacing(tgt_mob.x,tgt_mob.y,locx,locy));
	if (frot > 4)
		frot := 8-frot;
	endif

	return (frot < 2);
endfunction

// 0: xmod := 0;  ymod := -1;

// 1: up: x: +1 / x: -1
// 2: right: y: -1 / y: +1
// 4: down: x: +1 / x: -1
// 4: left: y: -1 / y: +1

// leftdown: -1 -1 / +1 +1


function GetModByFacing(facing, mod := 0)
	var xmod := 0, ymod := 0;
	case ( facing ) // Should find a more sleek mathish way to do this!
		0: xmod := 0+mod;  ymod := -1; break;
		1: xmod := +1+mod; ymod := -1+mod; break;
		2: xmod := +1; ymod := 0+mod;  break;
		3: xmod := +1+mod; ymod := +1-mod; break;
		4: xmod := 0+mod;  ymod := +1; break;
		5: xmod := -1-mod; ymod := +1-mod; break;
		6: xmod := -1; ymod := 0+mod;  break;
		7: xmod := -1-mod; ymod := -1+mod; break;
		default:
			return error{"errortext":="Invalid facing."};
			break;
	endcase

   return array{ xmod, ymod };
endfunction
/*
 * GetPointsByfacing(x_pos, y_pos, facing, dist)
 *
 * Purpose
 * Starts at an X, Y position, then based off the facing and distance, returns coordinates on that line.
 *
 * Parameters
 * x_pos:	X coordinate to start on.
 * y_pos:	Y coordinate to start on.
 * facing:	Direction (0-7) to go on the line.
 * dist:	Distance to go along the line.
 * 
 * Return Value
 * Returns an array of structs containing .x .y members for coordinates.
 *
 */
function GetPointsByFacing(x_pos, y_pos, facing, dist)
	var coord := struct{"x", "y"};
	var coord_list := array{};
	
   var mods := GetModByFacing(facing);
	
	var i;
	for( i:=1; i<=dist; i:=i+1 )
		coord.x := x_pos+(i*mods[1]);
		coord.y := y_pos+(i*mods[2]);
		coord_list.Append(coord);
		sleepms(2);
	endfor	
		
	return coord_list;
endfunction

function GetFacingToDest(tgt_mob, locx, locy)
	if (tgt_mob.x == locx && tgt_mob.y == locy)
		return 0;
	endif

	var frot := Abs(((tgt_mob.facing)%8)-GetFacing(tgt_mob.x,tgt_mob.y,locx,locy));
	if (frot > 4)
		frot := 8-frot;
	endif

	return (frot < 2);
endfunction
