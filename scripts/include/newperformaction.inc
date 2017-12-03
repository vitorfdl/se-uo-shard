use uo;
use os;
use polsys;
include "include/client";

function NewPerformAction(who, animation, action, subaction)
	var outpkt := CreatePacket(0xE2, 10);
  	outpkt.SetInt32(1, who.serial);
	outpkt.SetInt16(5, animation);
	outpkt.SetInt16(7, action);
	outpkt.SetInt8(9, subaction);
	outpkt.SendAreaPacket(who.x, who.y, 18, who.realm);
  return 0;
endfunction 

function OldPerformAction(who, action, frame_count := 5, rpeat := 1, fwd := 0, rpeat_flag := 0, frame_delay := 1)
	action := cint(action);
	var outpkt := CreatePacket(0x6E, 14);
	outpkt.SetInt32(1, who.serial);
	outpkt.SetInt16(5, action);
	outpkt.SetInt8(7, 0);
	outpkt.SetInt16(8, frame_count);
	outpkt.SetInt16(9, rpeat);
	outpkt.SetInt16(11, fwd);
	outpkt.SetInt16(12, rpeat_flag);
	outpkt.SetInt16(13, frame_delay);
	outpkt.SendAreaPacket(who.x, who.y, 18, who.realm);
	return 0;
endfunction 