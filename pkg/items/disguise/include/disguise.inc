include "include/say";
include "include/client";
include ":disguise:disguise";
include ":attributes:attributes";
include ":gumps:gumps";
include ":fls_core:fls_characters";

const RES_BTN_OK    := 10;
const RES_BTN_CLOSE := 0;
const HAIR_BALD     := 0x999999;

function mainGump(who, byref disfarce, byref choice)
	var menus    := array{9762, 9762, 9762, 9762, 9762};
   var gump 	 := GFCreateGump();
	var gender   := CheckGender(who);

	GFPage(gump, 0);
   GFSetID(gump, GUMPID_DISGUISE);
	GFResizePic(gump, 25, 8, 5120, 591, 340); //background
	GFResizePic(gump, 37, 27, 9350, 572, 33); //top paper

	GFResizePic(gump, 36, 69, 9350, 123, 157); //menu paper
	GFHTMLArea(gump, 47, 34, 200, 20, "<BASEFONT COLOR=#7a600d>Configurar  Aparência");
	GFTextLine(gump, 65, 80, 1101, "Cabelos");
	GFTextLine(gump, 64, 120, 1101, "Cor do Cabelo");
	GFTextLine(gump, 64, 140, 1101, "Cor de Pele");
	// GFTextLine(gump, 64, 160, 1101, "Raça");

	if (choice.menu)
		menus[choice.menu] := 9763;
	endif
	GFAddButton(gump, 44, 83, menus[1], 9763, GF_CLOSE_BTN, 1);
	GFAddButton(gump, 44, 123, menus[3], 9763, GF_CLOSE_BTN, 3);
	GFAddButton(gump, 44, 143, menus[4], 9763, GF_CLOSE_BTN, 4);
	// GFAddButton(gump, 44, 163, menus[5], 9763, GF_CLOSE_BTN, 5);
	GFAddButton(gump, 66, 190, 247, 248, GF_CLOSE_BTN, RES_BTN_OK); //OKAY

	if (gender != 2 && choice.barba)
		GFTextLine(gump, 65, 100, 1101, "Barbas");
		GFAddButton(gump, 44, 103, menus[2], 9763, GF_CLOSE_BTN, 2);
	endif

	GFResizePic(gump, 168, 69, 9350, 441, 270); //Center paper

	//Difficulty
	if (choice.is_disguise)
		GFResizePic(gump, 35, 237, 9350, 127, 33);
		GFHTMLArea(gump, 53, 243, 90, 20, "<BASEFONT SIZE=2 COLOR=200><center>Dificuldade: Alta");
		GFAddButton(gump, 564, 32, 4017, 4018, GF_CLOSE_BTN, RES_BTN_CLOSE);
	endif

    
	if (choice.menu == 1)
        var opts := struct{
            "type"  := "{}_head".format(choice.race),
            "param" := "cabelo",
            "gender" := gender
        };

        buildHairGump(gump, disfarce, opts);
	elseif (choice.menu == 2)
        var opts := struct{
            "type" := "{}_facialhair".format(choice.race),
            "param" := "barba",
            "gender" := gender
        };
        buildHairGump(gump, disfarce, opts);
	elseif (choice.menu == 3)
        var opts := struct{
            "param" := "corcabelo",
            "text" := "Cabelo",
			"race" := choice.race
        };
        buildColor(gump, disfarce, opts);
	elseif (choice.menu == 4)
        var opts := struct{
            "param" := "corpele",
            "text" := "Pele",
			"race" := choice.race
        };
        buildColor(gump, disfarce, opts);
	endif

    return gump;
endfunction

function buildHairGump(byref gump, byref disfarce, opt := struct{})
    UnloadConfigFile(":fls_core:config/hairtypes");
	var config := ReadConfigFile(":fls_core:config/hairtypes");
	var hairgumps := config["hairgump"];

	var y_pos := 80;
	var x_pos := 192;
	var hairnames := array{};

	var styles := GetConfigStringDictionary(config[opt.type], "style");
    if (opt.type == "facialhair")
	    styles["Liso"] := HAIR_BALD;
    elseif (opt.type["head"])
        styles["Careca"] := HAIR_BALD;
    endif

	GFPage(gump, gump.cur_page+1);
	foreach style in (styles.Keys())
		var hairname := StrReplace(style, "_", " ");
		var hairgraphic := cint(styles[style]);
        
		if (hairgraphic == disfarce[opt.param])
			GFResizePic(gump, x_pos, y_pos, 9500, 85, 72);
		else
			GFResizePic(gump, x_pos, y_pos, 9568, 85, 72);
		endif
		
		var gumppic;
		if (hairgraphic != HAIR_BALD)
			gumppic := GetConfigString(hairgumps, style);
			gumppic := splitwords(gumppic);
			if (opt.gender == 2)
				gumppic := cint(gumppic[2]);
			else
				gumppic := cint(gumppic[1]);
			endif
		endif

		hairnames.append(hairname);
		GFHTMLArea(gump, x_pos-4, y_pos+49, 98, 20, "<BASEFONT SIZE=2 COLOR=#ffffff><center>{}".format(hairname));
		if (hairgraphic != HAIR_BALD)
			// GFAddButton(gump, x_pos-50, y_pos-34, 30061, 30061, GF_CLOSE_BTN, hairgraphic);
			GFGumpPic(gump, x_pos-50, y_pos-34, gumppic);
		endif

		GFAddButton(gump, x_pos-2, y_pos-7, 30061, 30061, GF_CLOSE_BTN, hairgraphic);
		if (hairgraphic == disfarce[opt.param])
			GFGumpPic(gump, x_pos-2, y_pos-7, 30069);
		endif
		
		x_pos += 100;
		if (x_pos >= 500)
			x_pos := 192;
			y_pos += 85;
		endif

		if (y_pos >= 300)
			GFAddButton(gump, 582, 185, 9903, 9904, GF_PAGE_BTN, gump.cur_page+1);
			GFPage(gump, gump.cur_page+1);
			GFAddButton(gump, 170, 185, 9909, 9910, GF_PAGE_BTN, gump.cur_page-1);
			y_pos := 80;
		endif
	endforeach
endfunction

function buildColor(byref gump, byref disfarce, opts)
	var color_cfg;
    if (opts.param == "corcabelo")
        UnloadConfigFile(":hair:config/hairColors");
        color_cfg := ReadConfigFile(":hair:config/hairColors");
    elseif (opts.param == "corpele")
        UnloadConfigFile(":fls_core:config/skincolors");
        color_cfg := ReadConfigFile(":fls_core:config/skincolors");
    endif

    if ( !color_cfg )
		Print("Error: Could not open skin or hair color -> "+color_cfg.errortext);
	endif

    var index_elem;
    if (opts.race)
        index_elem := color_cfg[opts.race];
    elseif (opts.param == "corpele")
        index_elem := color_cfg["List"];
    elseif (opts.param == "corcabelo")
        index_elem := color_cfg["Humano"];
    endif

	var colors := GetConfigStringArray(index_elem, "Color");
	var y_pos := 80;
	var x_pos := 195;

	if (!opts.param)
		opts.param := colors[1];
	endif
	// Build category buttons on the left first
    GFPage(gump, gump.cur_page+1);
	foreach color in colors;
		color := CInt(color);
		if (color == disfarce[opts.param])
		    GFAddButton(gump, x_pos, y_pos, 2154, 2154, GF_CLOSE_BTN, color);
		else
            GFAddButton(gump, x_pos, y_pos, 2151, 2154, GF_CLOSE_BTN, color);
		endif
		GFTextline(gump, x_pos+35, y_pos+5, color-1, opts.text);

		y_pos += 30;
		if (y_pos >= 310)
			x_pos += 75;
			y_pos := 80;
		endif

		if (x_pos >= 510)
 			GFAddButton(gump, 582, 185, 9903, 9904, GF_PAGE_BTN, gump.cur_page+1);
			GFPage(gump, gump.cur_page+1);
			GFAddButton(gump, 170, 185, 9909, 9910, GF_PAGE_BTN, gump.cur_page-1);
			x_pos := 195;
		endif
	endforeach
endfunction

function disguiseGump(who, choice := struct{}, disfarce := 0);
    if (!disfarce)
        disfarce := struct{
            "barba"     := HAIR_BALD,
            "cabelo"    := HAIR_BALD,
            "corcabelo" := -1,
            "corpele"   := -1,
            "sexo"      := -1,
            "raca"      := "",
            "aparencia" := ""
        };
    endif

	var not_closed := 1;
	while(not_closed && who.connected)
		var gump    := mainGump(who, disfarce, choice);
		var input   := GFSendGump(who, gump);

		if (input[0] >= 1 && input[0] <= 5)
			choice.menu := input[0];
		elseif (input[0] == RES_BTN_OK)
			not_closed := 0;
	        return disfarce;
		elseif (choice.menu == 1 && input[0] != 0)
			disfarce.cabelo := input[0];
		elseif (choice.menu == 2 && input[0] != 0)
			disfarce.barba := input[0];
		elseif (choice.menu == 3 && input[0] != 0)
			disfarce.corcabelo := input[0];
		elseif (choice.menu == 4 && input[0] != 0)
			disfarce.corpele := input[0];
		else
			not_closed := 0;
		endif
	endwhile

    return 0;
endfunction

function applyDisguise()

endfunction

function removeDisguise()

endfunction
