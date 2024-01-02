/* $Id: gumps_ex.inc 375 2006-06-17 19:26:32Z austin|vitor $
 *
 * Disguise Gump Wrappers
 * 	buildHairGump(byref gump, x_pos := 0, y_pos := 0, width := 400, height := 400, byref disfarce, opt := struct{})
 * 	buildColor(byref gump, x_pos := 0, y_pos := 0, width := 400, height := 400, byref disfarce, opt := struct{})
 * 	mainGump(who, byref disfarce, byref choice, opts := 0)
 * 	disguiseGump(who, choice := struct{}, disfarce := 0, opts := 0)
 * 	
 * Apply Disguise
 * 	ApplyDisguise(who, disguise)
 * 	RemoveDisguise(who)
 *
 * Documenting the disfarce struct
 * disfarce := struct{
 *     "barba"     := HAIR_BALD,
 *     "cabelo"    := HAIR_BALD,
 *     "corcabelo" := -1,
 *     "corpele"   := -1,
 *     "sexo"      := -1,
 *     "raca"      := "",
 *     "aparencia" := ""
 * };
 */

include "include/say";
include "include/client";
include ":disguise:disguise";
include ":attributes:attributes";
include ":gumps:gumps";
include ":gumps:gumps_ex";
include ":fls_core:fls_characters";


const RES_BTN_OK    := 10;
const RES_BTN_CLOSE := 0;
const HAIR_BALD     := 0x999999;

function getHairDescription(menuName := "")
	case(menuName)
		"cabelo":
			return "Cabelos";
		"corcabelo":
			return "Cor do Cabelo";
		"corpele":
			return "Cor de Pele";
		"barba":
			return "Barbas";
	endcase
endfunction

/**
 * Function to create the main gump for disguise options
 * @param who - The user interacting with the gump
 * @param disfarce - The disguise options
 * @param choice - The user's choice
 * @param opts - Optional array of options
 */
function mainGump(who, byref disfarce, byref choice, opts := 0, gumpSettings := struct{ }, byref gump := 0)
   if (!opts) 
		opts := array{ "cabelo", "barba", "corcabelo", "corpele"};
	endif

	if (!choice.menu)
		choice.menu := 1;
	endif

	if (!gump)
		gump := GFCreateGump();
	endif
	var gender   := CheckGender(who);

   var baseX := _IfOr(gumpSettings.baseX, -4);
	var baseY := _IfOr(gumpSettings.baseY, 5);
	var width := _IfOr(gumpSettings.width, 480);
	var height := _IfOr(gumpSettings.height, 514);

	if (!gumpSettings.noBackground)
		GFPage(gump, 0);
		if (who.cmdlevel)
			GFClosable(gump, 1);
		endif
   	GFSetID(gump, GUMPID_DISGUISE);

   	GFResizePic(gump, baseX, baseY, DEFAULT_BACKGROUND, width, height);

		var headerContainerX := GFCenterRectangle(ALIGN_CENTER, 194, baseX, width);
      GFResizePic(gump, headerContainerX, baseY+20, DEFAULT_MENU_BACKGROUND, 194, 57);
      GFTextMid(gump, baseX-15, baseY+20+18, 490, DEFAULT_FONT_COLOR, "Alterar Aparência");

		baseY += 90;
	endif

	var menuWidth := !gumpSettings.noBackground ? width : width-100;
	var columnWidthArray := array{};
	foreach opt in opts
		columnWidthArray.append(GFTextWidth(getHairDescription(opt)));
	endforeach

	var menuColumnPos := GFColumnPositionByWidthArray(columnWidthArray, width, baseX);
	
   foreach opt in opts
      GFAddButton(gump, menuColumnPos[_opt_iter], baseY - 2, G_UNSELECTED + (choice.menu == _opt_iter), G_SELECTED, GF_CLOSE_BTN, _opt_iter);
      var description := getHairDescription(opt);
      var textPosition := menuColumnPos[_opt_iter] + 23;

      if (opt != "barba" || (gender != 2 && choice.barba))
         GFTextLine(gump, textPosition, baseY, HIGHLIGHT_FONT_COLOR, description);
      endif
   endforeach

	//Difficulty
	if (choice.is_disguise)
		GFResizePic(gump, 35, 237, 9350, 127, 33);
		GFHTMLArea(gump, 53, 243, 90, 20, "<BASEFONT SIZE=2 COLOR=200><center>Dificuldade: Alta");
		GFAddButton(gump, 564, 32, 4017, 4018, GF_CLOSE_BTN, RES_BTN_CLOSE);
	endif

	baseY += 40;
	var totalHeighUsed := 0;

	if (choice.menu == 1)
        var opts := struct{
            "type" := $"{choice.race}_head",
            "param" := "cabelo",
            "gender" := gender
        };

        totalHeighUsed := buildHairGump(gump, baseX+35, baseY, width-40, height-180, disfarce, opts);
	elseif (choice.menu == 2)
        var opts := struct{
            "type" := $"{choice.race}_facialhair",
            "param" := "barba",
            "gender" := gender
        };
        totalHeighUsed := buildHairGump(gump, baseX+35, baseY, width-40, height-180, disfarce, opts);
	elseif (choice.menu == 3)
        var opts := struct{
            "param" := "corcabelo",
            "text" := "Cabelo",
				"race" := choice.race
        };
        totalHeighUsed := buildColor(gump, baseX+35, baseY, width-40, height-180, disfarce, opts);
	elseif (choice.menu == 4)
        var opts := struct{
            "param" := "corpele",
            "text" := "Pele",
				"race" := choice.race
        };
        totalHeighUsed := buildColor(gump, baseX+35, baseY, width-40, height-180, disfarce, opts);
	endif

	GFPage(gump, 0);
	var btnPosX := GFCenterRectangle(ALIGN_CENTER, 34, baseX, width);
	GFAddButton(gump, btnPosX, totalHeighUsed+15, 247, 248, GF_CLOSE_BTN, RES_BTN_OK); //OKAY

	return gump;
endfunction

/**
 * Builds the hair gump for the disguise menu.
 * @param {byref} gump - The gump to build on.
 * @param {int} x_pos - The x position of the gump.
 * @param {int} y_pos - The y position of the gump.
 * @param {int} width - The width of the gump.
 * @param {int} height - The height of the gump.
 * @param {byref} disfarce - The disguise object.
 * @param {struct} opt - The options for building the gump.
 */

function buildHairGump(byref gump, x_pos := 0, y_pos := 0, width := 400, height := 400, byref disfarce, opt := struct{})
	UnloadConfigFile(":fls_core:config/hairtypes");
	var config := ReadConfigFile(":fls_core:config/hairtypes");
	var hairgumps := config["hairgump"];
	var hairnames := array{};

	var styles := GetConfigStringDictionary(config[opt.type], "style");
	if (opt.type == "facialhair")
		styles["Liso"] := HAIR_BALD;
	elseif (opt.type["head"])
		styles["Careca"] := HAIR_BALD;
	endif

	var nOfHairs := styles.Size();

	var selectorHeight := 80;
	var selectorWidth := 90;

	var table := GFTableProps(width, height, selectorWidth, selectorHeight, nOfHairs);

	GFResizePic(gump, x_pos-10, y_pos-10, 9350, width+2, height); //Center paper
	if (table.nOfPages == 1)
		x_pos += 8;
	endif

	var columnPos := GFColumnPosition(selectorWidth, table.numberOfColumns, width-30, x_pos);
	var rowPos := GFRowPosition(0, selectorHeight, table.numberOfRows, height, y_pos);

	var stylePages := BreakArray(styles.Keys(), table.itemsPerPage);
	var nColumns := table.numberOfColumns;

	foreach page in stylePages
		GFPage(gump, gump.cur_page+1);
		var notAvailable := 0;
		foreach style in page
			var hairname := StrReplace(style, "_", " ");
			var hairgraphic := cint(styles[style]);

			var i := cint(((_style_iter -1 - notAvailable) % nColumns) + 1); // columnPos
         var j := cint(Floor((_style_iter - 1 - notAvailable) / nColumns) + 1); // rowPos

			var gumppic;
			if (hairgraphic != HAIR_BALD)
				gumppic := GetConfigString(hairgumps, style);
				gumppic := splitwords(gumppic);
				if (opt.gender == 2)
					gumppic := cint(gumppic[2]);
				else
					gumppic := cint(gumppic[1]);
				endif

				if (!gumppic) 
					notAvailable += 1;
					continue; 
				endif
			endif

			if (hairgraphic == disfarce[opt.param])
				GFResizePic(gump, columnPos[i], rowPos[j], 9500, 85, 72);
			else
				GFResizePic(gump, columnPos[i], rowPos[j], 9568, 85, 72);
			endif

			hairnames.append(hairname);
			GFHTMLArea(gump, columnPos[i]-4, rowPos[j]+49, 98, 20, "<BASEFONT SIZE=2 COLOR=#ffffff><center>{}".format(hairname));
			if (hairgraphic != HAIR_BALD)
				GFGumpPic(gump, columnPos[i]-50, rowPos[j]-34, gumppic, disfarce.corcabelo);
			endif

			GFAddButton(gump, columnPos[i]-2, rowPos[j]-7, 30061, 30061, GF_CLOSE_BTN, hairgraphic);
			if (hairgraphic == disfarce[opt.param])
				GFGumpPic(gump, columnPos[i]-2, rowPos[j]-7, 30069);
			endif

			sleepms(2);
		endforeach
		if (table.nOfPages > 1)
			var scrollBarXPos := x_pos + (nColumns * selectorWidth) + 10;
			GFPageScrollBar(gump, scrollBarXPos, y_pos, table.nOfPages, gump.cur_page, height-40);
		endif
	endforeach

	// return the total height used
	return y_pos+(table.numberOfRows * selectorHeight);
endfunction

/**
 * Function to build color options for disguise gump
 * @param gump - The gump to build the color options for
 * @param x_pos - The x position of the gump
 * @param y_pos - The y position of the gump
 * @param width - The width of the gump
 * @param height - The height of the gump
 * @param disfarce - The disguise options
 * @param opt - Optional array of options
 */

function buildColor(byref gump, x_pos := 0, y_pos := 0, width := 400, height := 400, byref disfarce, opt := struct{})
	var color_cfg;
    if (opt.param == "corcabelo")
        UnloadConfigFile(":hair:config/hairColors");
        color_cfg := ReadConfigFile(":hair:config/hairColors");
    elseif (opt.param == "corpele")
        UnloadConfigFile(":fls_core:config/skincolors");
        color_cfg := ReadConfigFile(":fls_core:config/skincolors");
    endif

    if ( !color_cfg )
		Print("Error: Could not open skin or hair color -> "+color_cfg.errortext);
	endif

    var index_elem;
    if (opt.race)
        index_elem := color_cfg[opt.race];
    elseif (opt.param == "corpele")
        index_elem := color_cfg["List"];
    elseif (opt.param == "corcabelo")
        index_elem := color_cfg["Humano"];
    endif

	var colors := GetConfigStringArray(index_elem, "Color");

	if (!opt.param)
		opt.param := colors[1];
	endif

	var nOfColors := colors.Size();

	var selectorHeight := 40;
	var selectorWidth := 80;

	var table := GFTableProps(width, height, selectorWidth, selectorHeight, nOfColors);

	GFResizePic(gump, x_pos-10, y_pos-10, 9350, width-5, height); //Center paper
	if (table.nOfPages == 1)
		x_pos += 8;
	endif

	var columnPos := GFColumnPosition(selectorWidth, table.numberOfColumns, width-30, x_pos);
	var rowPos := GFRowPosition(0, selectorHeight, table.numberOfRows, height, y_pos);

	var colorPages := BreakArray(colors, table.itemsPerPage);
	var nOfColumns := table.numberOfColumns;

	foreach page in colorPages
		GFPage(gump, gump.cur_page+1);
		foreach color in page
			var i := cint(((_color_iter -1) % nOfColumns) + 1); // columnPos
         var j := cint(Floor((_color_iter -1) / nOfColumns) + 1); // rowPos

			color := CInt(color);
			if (color == disfarce[opt.param])
				GFAddButton(gump, columnPos[i], rowPos[j], 2154, 2154, GF_CLOSE_BTN, color);
			else
				GFAddButton(gump, columnPos[i], rowPos[j], 2151, 2154, GF_CLOSE_BTN, color);
			endif
			GFTextline(gump, columnPos[i]+35, rowPos[j]+5, color-1, opt.text);
	
			sleepms(2);
		endforeach

		if (table.nOfPages > 1)
			GFPageScrollBar(gump, x_pos+(nOfColumns*selectorWidth)+10, y_pos, table.nOfPages, gump.cur_page, height-40);
		endif
	endforeach

	return y_pos+(table.numberOfRows * selectorHeight);
endfunction

/**
 * Function to create the main gump for disguise options
 * @param who - The user interacting with the gump
 * @param disfarce - The disguise options
 * @param choice - The user's choice
 * @param opts - Optional array of options
 * @purpose This function creates the main gump for disguise options, allowing the user to interact with various disguise options such as hair, beard, hair color, and skin color.
 */

function disguiseGump(who, choice := struct{}, disfarce := 0, opts := 0);
   if (!opts) 
		opts := array{ "cabelo", "barba", "corcabelo", "corpele"}; 
	endif

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
		var gump    := mainGump(who, disfarce, choice, opts);
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

/**
 * Function to apply a disguise to a character
 * @param who - The character to apply the disguise to
 * @param disguise - The disguise options to apply
 * @purpose This function applies a disguise to a character, changing their hair, beard, hair color, skin color, name, and profile.
 */

function ApplyDisguise(who, disguise)
	who.color    := disguise.corpele;
	who.truecolor:= disguise.corpele;
	//barba
	var mybarba  := GetEquipmentByLayer(who, LAYER_BEARD);
	DestroyItem(mybarba);

	var newbarba := CreateItemAtLocation(who.x, who.y, who.z, disguise.barba, 1, who.realm);
	newbarba.color := disguise.corcabelo;
	EquipItem(who, newbarba);
	
	var myhair := GetEquipmentByLayer(who, LAYER_HAIR);
	DestroyItem(myhair);

	var newhair:= CreateItemAtLocation(who.x, who.y, who.z, disguise.cabelo, 1, who.realm);
	newhair.color := disguise.corcabelo;
	EquipItem(who, newhair);

   if (disguise.name)
      who.name := disguise.name;
   endif

  if (disguise.profile)
      var cp_data_file_newlook  := DFOpenDataFile(":charprofile:CPFile", DF_CREATE);
      var cp_elem_newlook       := DFFindElement(cp_data_file_newlook, who.serial, DF_CREATE);
      
      cp_elem_newlook.SetProp("Profile", disguise.profile);
   endif
endfunction

/**
 * Function to document the purpose of the RemoveDisguise function
 * @param who - The character to remove the disguise from
 * @purpose This function removes the disguise from a character, restoring their original appearance and profile.
 */

function RemoveDisguise(who)
   var old_look := GetObjProperty(who, "disfarce_oldlook");
   if (!old_look)
      return 0;
   endif

	var cp_data_file_oldlook  := DFOpenDataFile(":charprofile:CPFile", DF_CREATE);
	var cp_elem_oldlook       := DFFindElement(cp_data_file_oldlook, who.serial, DF_CREATE);
	var cp_profile_oldlook    := DFGetProp(cp_elem_oldlook, "Profile", DF_CREATE);
   var my_disguise := GetObjProperty(who, "my_disguise");
   my_disguise.profile := cp_profile_oldlook;
   SetObjProperty(who, "my_disguise", my_disguise);

   ApplyDisguise(who, old_look);

   EraseObjProperty(who, "disfarce_oldlook");
   return 1;
endfunction