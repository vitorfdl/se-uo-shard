function NewPerformAction(who, animation, action, subaction)
	var outpkt := CreatePacket(0xE2, 10);
    outpkt.SetInt32(1, who.serial);
	outpkt.SetInt16(5, animation);
	outpkt.SetInt16(7, action);
	outpkt.SetInt8(9, subaction);
	outpkt.SendAreaPacket(who.x, who.y, 18, who.realm);
  return 0;
endfunction 
