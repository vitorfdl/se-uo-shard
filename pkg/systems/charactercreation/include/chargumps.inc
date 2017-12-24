use uo;
use cfgfile;

include ":gumps:gumps";
include "include/arrays";
include ":fls_core:fls_chargumps_ex";
include ":attributes:attributes";
include ":gumps:yesno";
include ":charactercreation:characters";
include "include/say";

function ItemPickerGump(who, byref chardata)

	var cfg := ReadConfigFile(":charactercreation:config/itempicker");
	var secoes := GetConfigStringKeys( cfg );
	var chargump := GFCreateGump();
	var input;

	GFClosable(chargump, 0);
	GFDisposable(chargump, 0);
	GFResizePic(chargump, 0, 25, 3600, 600, 480);
	GFResizePic(chargump, 15, 40, 0x13BE, 570, 450);
	GFTextMid(chargump, 15, 45, 600, 1153, "Defina seus Itens Iniciais");
	GFTextMid(chargump, 15, 60, 600, 1153, "Você tem " + chardata.Credits + " creditos.");
	GFAddButton(chargump, 510, 450, 4502, 4502, 1, 999 );
	GFTextLine(chargump, 450, 460, 1153, "Termina");

	var x := 50;
	var y := 75;
	var page := 1;
	foreach entry in secoes
		if (entry != "DefaultAmount")
			GFTextLine(chargump, x, y, 1153, entry );
			GFAddButton(chargump, x-30, y+4, 9762, 9763, GF_PAGE_BTN, page);
			y := y + 20;
			page := page + 1;
		endif
    sleepms(2);
	endforeach

	page := 1;
	var itens;
	x := 170;
	y := 75;
	var itensmap := dictionary;
	var itemcusto := dictionary;
	var i := 1;
	var elem;
	var custo;
	foreach entry in secoes
		if (entry != "DefaultAmount")
			GFPage(chargump, page);
			elem := FindConfigElem(cfg, entry);
			itens := ListConfigElemProps(elem);
			foreach item in itens
				GFTextLine(chargump, x, y, 1153, item );
				custo := GetConfigInt(elem, item);
				GFTextLine(chargump, x+100, y, 1153, ""+custo );
				itemcusto[item] := custo;
				itensmap[i] := item;
				GFAddButton(chargump, x-20, y+4, 2103, 2104, 1, i);
				i := i + 1;
				y := y + 20;
				if ( y >= 450 )
					y := 75;
					x := 320;
				endif
			endforeach
			x := 170;
			y := 75;
			page := page + 1;
		endif
    sleepms(2);
	endforeach

	input := GFSendGump(who, chargump);
	input := input[0];

	if (input == 999)
		return;
	else
		var buyitem := itensmap[input];
		var custo := itemcusto[buyitem];
		var amt := 1;
		var amtelem := FindConfigElem(cfg, "DefaultAmount");
		amt := Cint(GetConfigInt(amtelem, buyitem));
		if ( (amt == 0) || (amt == error) )
			amt := 1;
		endif
		if (chardata.Credits >= custo)
			chardata.Credits := chardata.Credits - custo;
			CreateItemInBackpack( who, buyitem, amt );
			ItemPickerGump(who, chardata);
		else
			SendSysMessage(who, "Você não tem creditos para comprar isto.");
			ItemPickerGump(who, chardata);
		endif
	endif

endfunction
