use os;
use uo;
include "include/say";
include "include/objtype";

// This function is used to display a craft skill's action,
// sound, the loops to do it in, and the delay between loops.
// Sound is required, loops defaults to 0 if none sent, and delay
// defaults to 2 seconds if none sent, Action defaults to 0
// (none) if none sent. Who is who the info is to be sent to.
// All delays are in seconds.
function playCraftWork(who, craftSound, loops:=0, delay:=2, craftAction:=0, fastcraft := 0)
  if(loops)
    for i := 1 to loops
      if(craftAction)
        PerformAction( who, craftAction );
      endif
      PlaySoundEffect(who, craftSound);
      sleep(delay);
    endfor
  else
    PlaySoundEffect(who, craftSound);
	if (fastcraft)
		sleepms(500);
		return;
	endif
    sleep(delay);
  endif
  
  return;
endfunction

function CheckAnvil(who)
	var anvil_near;
	foreach item in ListItemsNearLocation(who.x, who.y, who.z, 3)
		if((item.objtype == 0x0faf) || (item.objtype == 0x0fb0))
			anvil_near := 1;
			break;
		endif
	endforeach
	if(anvil_near == 0)
		return 0;
	else
		return 1;
	endif
endfunction