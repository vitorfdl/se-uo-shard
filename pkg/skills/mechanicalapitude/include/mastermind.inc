var stones := array{6255, 6250, 6251, 6252, 6253, 6254, 6249, 6256};
var stands := array{"", "", "", "", ""};
var last := array{};
var enigma := array{};
var counter := 2;

function mastermind(who)


	enigma := RandomSet(who);

while(who.connected)

	if(counter < 1)
//		SendSysMessage(who, "You lose!");
		return 0;
	endif

	var gump := GFCreateGump(50, 50, 500, 400);

	GFResizePic(gump, 0, 0, 3500, 500, 400);

	GFGumpPic(gump, 30, 30, 103 );

	GFTextLine(gump, 100, 50, 998, "A");
	GFTextLine(gump, 75, 70, 998, "LockPicking");
	GFTextLine(gump, 90, 90, 998, "Puzzle");

	GFTextLine(gump, 200, 40, 194, "Escolha a sequencia correta dos cilindros");
	GFTextLine(gump, 200, 60, 194, "necessaria para abrir a tranca.");
	GFTextLine(gump, 200, 100, 194, "Cuidado! Uma tentativa errada pode ser fatal!");


	//GFTextLine(gump, 30, 370, 37, "Tentativas restantes: " +counter);

//---------------------------------------------------------------------

	GFResizePic(gump,30,150,5100,30,30);

	GFAddButton(gump, 70, 160, 5032, 5032, GF_CLOSE_BTN, 11 );	//button left
	GFTilePic(gump, 25, 160, 6255 );		//white

	GFResizePic(gump,30,200,5100,30,30);
	GFAddButton(gump, 70, 210, 5032, 5032, GF_CLOSE_BTN, 12 );	//button left
	GFTilePic(gump, 25, 210, 6250 );		//blue

	GFResizePic(gump,30,250,5100,30,30);
	GFAddButton(gump, 70, 260, 5032, 5032, GF_CLOSE_BTN, 13 );	//button left
	GFTilePic(gump, 25, 260, 6251 );		//green

	GFResizePic(gump,30,300,5100,30,30);
	GFAddButton(gump, 70, 310, 5032, 5032, GF_CLOSE_BTN, 14 );	//button left
	GFTilePic(gump, 25, 310, 6252 );		//orange

//--------------------------------------------------------------------

	GFResizePic(gump,440,150,5100,30,30);
	GFAddButton(gump, 420, 160, 5032, 5032, GF_CLOSE_BTN, 15 );	//button right
	GFTilePic(gump, 435, 160, 6253 );		//purple

	GFResizePic(gump,440,200,5100,30,30);
	GFAddButton(gump, 420, 210, 5032, 5032, GF_CLOSE_BTN, 16 );	//button right
	GFTilePic(gump, 435, 210, 6254 );		//red

	GFResizePic(gump,440,250,5100,30,30);
	GFAddButton(gump, 420, 260, 5032, 5032, GF_CLOSE_BTN, 17 );	//button right
	GFTilePic(gump, 435, 260, 6249 );		//black

	GFResizePic(gump,440,300,5100,30,30);
	GFAddButton(gump, 420, 310, 5032, 5032, GF_CLOSE_BTN, 18 );	//button right
	GFTilePic(gump, 435, 310, 6256 );		//yellow

//---------------------------------------------- previus quess ------
	
	GFTextLine(gump,120, 215, 188, "Sua tentativa anterior: ");
	GFResizePic(gump,260,210,5100,120,30);

	if(last[1])
		GFTilePic(gump, 260, 220, last[1] );
	endif
	if(last[2])
		GFTilePic(gump, 280, 220, last[2] );
	endif
	if(last[3])
		GFTilePic(gump, 300, 220, last[3] );
	endif
	if(last[4])
		GFTilePic(gump, 320, 220, last[4] );
	endif
	if(last[5])
		GFTilePic(gump, 340, 220, last[5] );
	endif
	
	GFTextLine(gump, 220, 176, 188, "Dicas:");
	GFResizePic(gump, 260, 171, 5100, 120, 30);
	
	
	

//----------------------------------------- stands -----------

	GFTilePic(gump, 107, 260, 2818 );
	GFTilePic(gump, 109, 259, 2818 );
	GFTilePic(gump, 113, 259, 2816 );
	GFTilePic(gump, 110, 262, 2816 );
	GFTilePic(gump, 110, 260, 1310 );

	GFTilePic(gump, 167, 260, 2818 );
	GFTilePic(gump, 169, 259, 2818 );
	GFTilePic(gump, 173, 259, 2816 );
	GFTilePic(gump, 170, 262, 2816 );
	GFTilePic(gump, 170, 260, 1310 );

	GFTilePic(gump, 227, 260, 2818 );
	GFTilePic(gump, 229, 259, 2818 );
	GFTilePic(gump, 233, 259, 2816 );
	GFTilePic(gump, 230, 262, 2816 );
	GFTilePic(gump, 230, 260, 1310 );

	GFTilePic(gump, 287, 260, 2818 );
	GFTilePic(gump, 289, 259, 2818 );
	GFTilePic(gump, 293, 259, 2816 );
	GFTilePic(gump, 290, 262, 2816 );
	GFTilePic(gump, 290, 260, 1310 );

	GFTilePic(gump, 347, 260, 2818 );
	GFTilePic(gump, 349, 259, 2818 );
	GFTilePic(gump, 353, 259, 2816 );
	GFTilePic(gump, 350, 262, 2816 );
	GFTilePic(gump, 350, 260, 1310 );

//------------------------------------------------- stones on stands
	if(stands[1])
		GFTilePic(gump, 110, 275, stands[1] );
	endif
	if(stands[2])
		GFTilePic(gump, 170, 275, stands[2] );
	endif
	if(stands[3])
		GFTilePic(gump, 230, 275, stands[3] );
	endif
	if(stands[4])
		GFTilePic(gump, 290, 275, stands[4] );
	endif
	if(stands[5])
		GFTilePic(gump, 350, 275, stands[5] );
	endif

// ---------------------------------------------------- radio under stands
	GFSetRadioGroup(gump, 1);
	GFRadioButton(gump, 118, 325, 2151, 2154, 0, 101 );
	GFRadioButton(gump, 178, 325, 2151, 2154, 0, 102 );
	GFRadioButton(gump, 238, 325, 2151, 2154, 0, 103 );
	GFRadioButton(gump, 298, 325, 2151, 2154, 0, 104 );
	GFRadioButton(gump, 358, 325, 2151, 2154, 0, 105 );


	GFAddButton(gump, 225, 365, 2130, 2129, GF_CLOSE_BTN, 999 );	//	O K


	var Res := GFSendGump(who, gump);

	if(Res[0] < 1)
		if(YesNo(who, "Deseja mesmo cancelar ?"))
			return 0;
		else
			continue;
		endif
	endif

	if(Res[0] > 10 && Res[0] < 20)
		foreach key in (Res.keys)
			if(key > 100 && key < 106)
				if (stones[res[0]-10] in stands)
					SendSysMessageEx(who, "Nao e possível usar os mesmos cilindros!", SSM_FAIL);
				else
					stands[key-100] := stones[res[0]-10];
				endif
			endif
		endforeach
	endif

	if(res[0] == 999)
		if(stands[1] && stands[2] && stands[3] && stands[4] && stands[5])
			counter := counter - 1;
			if(CheckAnswer(who))
				return 1;
			endif
			last := stands;
			stands := {"", "", "", "", ""};
		endif
	endif


endwhile

endfunction


function RandomSet(who)

	var temp := array{};
	var stonerandom;
	var tempstone := stones;
	
	//setar resolução do enigma
	var i;
	for (i:=1;i<=5;i+=1)
		stonerandom := RandomInt(9-i)+1;
		temp[i] := tempstone[stonerandom];
		tempstone.Erase(stonerandom);	
	endfor

//developer debug  
	if(who.cmdlevel > 4)
		var gump := GFCreateGump(50, 50, 200, 100);
		GFResizePic(gump,0,0,3500,200,100);
		GFTilePic(gump, 30, 30, temp[1] );
		GFTilePic(gump, 60, 30, temp[2] );
		GFTilePic(gump, 90, 30, temp[3] );
		GFTilePic(gump, 120, 30, temp[4] );
		GFTilePic(gump, 150, 30, temp[5] );
		GFSendGump(who, gump);
	endif

	return temp;

endfunction


function CheckAnswer(who)

	var x;
	var hits := 0;
	var colors := 0;
	var desc;
	var desc2;
	var EnigmaTemp := enigma;
	var StandsTemp := stands;

	for(x := 1; x <= 5; x := x + 1)
		if(StandsTemp[x])
			if(StandsTemp[x] == EnigmaTemp[x])
				hits := hits + 1;
				EnigmaTemp[x] := -10;
				StandsTemp[x] := -20;
			endif
		endif
	endfor

	for(x := 1; x <= 5; x := x + 1)
//		if(StandsTemp[x])
			if(StandsTemp[x] in EnigmaTemp)
				colors := colors + 1;
				EnigmaTemp[FindInArray(EnigmaTemp, StandsTemp[x])] := -10;
				StandsTemp[x] := -20;
			endif
//		endif
	endfor
	
	case(colors)
		0: 		desc2 := "0 elemento em cor correta.";
		1: 		desc2 := "1 elemento em cor correta.";
		default:	desc2 := CInt(colors)+" elementos estao em cores corretas.";
	endcase
	
	
	if(hits == 5)
		return 1;
	else
		SendSysMessageEx(who, "Sua combinacao falhou!", SSM_FAIL);
		SendSysMessageEx(who, desc2, SSM_INFO);
		sleepms(30);
	endif
	
	return 0;

endfunction
