/*
 * Purpose
 * TODO
 *
 * Function List
 * GFAddButton(byref gump, x, y, off_id, on_id, btn_type:=GF_PAGE_BTN, btn_value:=0)
 * GFAddHTMLLocalized(byref gump, x, y, width, height, cliloc, background, scrollbar, hue := 0)
 * GFCheckBox(byref gump, x, y, unc_id, che_id, status:=0, btn_value:=0)
 * GFCreateGump(x:=0, y:=0, width:=0, height:=0)
 * GFDisposable(byref gump, bool)
 * GFSetID(byref gump, hex)
 * GFMovable(byref gump, bool)
 * GFExtractData(input, data_id)
 * GFGumpPic(byref gump, x, y, gump_id, hue := 0)
 * GFHTMLArea(byref gump, x, y, width, height, text, background:=0, scrollbar:=0)
 * GFPage(byref gump, page_num)
 * GFRadioButton(byref gump, x, y, unp_id, pres_id, status:=0, btn_value:=0)
 * GFResizePic(byref gump, x, y, gump_id, width:=0, height:=0)
 * GFSendGump(mobile, byref gump)
 * GFSetRadioGroup(byref gump, group_id)
 * GFTextCrop(byref gump, x, y, width, height, hue:=0, text:="")
 * GFTextEntry(byref gump, x, y, width, height, txt_color, ini_text:="")
 * GFTextLine(byref gump, x, y, hue:=0, text:="")
 * GFTilePic(byref gump, x, y, tile_id, hue := 0)
 * GFDrawWrappedText(byref gump, x, y, text_lines, txt_color)
 *
 * Private Function List
 * XGFError(text)
 * XGFAddToLayout(byref gump, string)
 *
 */
use uo;
use os;
use basic;

//ADDED by gha
//pra não ter q ler um cfg
CONST DEFAULT_BACKGROUND := 0x13BE;
CONST DEFAULT_FOREGROUND := 0x2486;

// CONSTants for the GFAddButton() function
CONST GF_PAGE_BTN	:= 0x0;
CONST GF_CLOSE_BTN	:= 0x1;
CONST GF_DUMMY_BTN	:= 0x2;

function GFCreateGump(x:=0, y:=0, width:=0, height:=0)
	var gump := struct;

	gump.+base := struct;
	// X- and Y-offsets for gump operations
	gump.base.+x := CInt(x);
	gump.base.+y := CInt(y);
	// Background width and length for gump
    gump.base.+width  := CInt(width);
    gump.base.+height := CInt(height);

	//Used for tracking the location of certain gump settings
	//in the layout array. This allows them to be enabled/disabled at will.
    gump.base.+MoveLoc    := 0;	// Movable flag location
    gump.base.+CloseLoc   := 0;	// NoClose flag location
    gump.base.+DisposeLoc := 0;	// NoDispose flag location
    
    gump.+layout := array;	// Main layout array for entire gump
    gump.+data   := array;	// Storage array for static-text and
					// text-entry initialisation strings
					// Indexes/KeyIDs for...

	// Starting points for data values
	gump.+data_id	:= 1;

	// Current page the gump is writing to
	gump.+cur_page := 0;

    //Get a random PID for gumpid
    gump.gumpid := 0;

	return(gump);
endfunction

function GFClosable(byref gump, bool)
	case ( bool )
		0:
			if ( gump.base.CloseLoc )
				return XGFError("Gump already contains the 'NoClose' flag.");
			endif

			(gump.layout).Append("NoClose");
			gump.base.CloseLoc := (gump.layout).Size();
			break;
		1:
			if ( !gump.base.CloseLoc )
				return XGFError("Gump is already closable.");
			endif

			(gump.layout).Erase(gump.base.CloseLoc);
			gump.base.CloseLoc := 0;
			break;
		default:
			return XGFError("gumps::Closable: Invalid bool value '"+bool+"'");
	endcase
	return 1;
endfunction

function GFDisposable(byref gump, bool)
	case ( bool )
		0:
			if ( gump.base.DisposeLoc )
				return XGFError("Gump already contains the 'NoDispose' flag.");
			endif

			(gump.layout).Append("NoDispose");
			gump.base.DisposeLoc := (gump.layout).Size();
			break;
		1:
			if ( !gump.base.CloseLoc )
				return XGFError("Gump is already disposable.");
			endif

			(gump.layout).Erase(gump.base.DisposeLoc);
			gump.base.DisposeLoc := 0;
			break;
		default:
			return XGFError("gumps::Disposable: Invalid bool value '"+bool+"'");
	endcase
	return 1;
endfunction

function GFSetID(byref gump, gumpid)
    gump.gumpid := gumpid;
    return 1;
endfunction

function GFMovable(byref gump, bool)
	case ( bool )
		0:
			if ( gump.base.MoveLoc )
				return XGFError("Gump already contains the 'NoMove' flag.");
			endif

			(gump.layout).Append("NoMove");
			gump.base.MoveLoc := (gump.layout).Size();
			break;
		1:
			if ( !gump.base.MoveLoc )
				return XGFError("Gump is already movable.");
			endif

			(gump.layout).Erase(gump.base.MoveLoc);
			gump.base.MoveLoc := 0;
			break;
		default:
			return XGFError("gumps::Movable: Invalid bool value '"+bool+"'");
	endcase
   
	return 1;
endfunction

function GFPage(byref gump, page_num)
	gump.cur_page := CInt(page_num);
	(gump.layout).Append("Page {}".format(gump.cur_page));

	return 1;
endfunction

function GFResizePic(byref gump, x, y, gump_id, width, height)
	width := CInt(width);
	height := CInt(height);
	if ( !width )
		width := gump.base.width;
	endif
	if ( !height )
		height := gump.base.width;
	endif

	(gump.layout).Append("ResizePic {} {} {} {} {}".format(cint(x), cint(y), cint(gump_id), cint(width), cint(height))); 
	return 1;
endfunction

function GFAddAlphaRegion(byref gump, x, y, width, height)
	(gump.layout).Append("CheckerTrans {} {} {} {}".format(cint(x), cint(y), cint(width), cint(height))); 
	return 1;
endfunction

function GFAddButton(byref gump, x, y, off_id, on_id, btn_type:=GF_PAGE_BTN, btn_value:=0)
	var line := "Button {} {} {} {}".format(cint(x), cint(y), cint(off_id), cint(on_id));

	btn_value := CInt(btn_value);
	if ( btn_value < 1 ) // Catch errors (0) and invalid (negative) values
		gump.data_id += 1;
		btn_value := gump.data_id;
	endif

	if ( btn_type == GF_CLOSE_BTN )
		line += " 1 0 {}".format(btn_value);
	elseif ( btn_type == GF_DUMMY_BTN )
		line += " 0 0 {}".format(btn_value);;
	else // Assume GF_PAGE_BTN
		line += " 0 {}".format(btn_value);
	endif

	(gump.layout).Append(line);

	return (btn_value);
endfunction

function GFCheckBox(byref gump, x, y, unc_id, che_id, status:=0, btn_value:=0)

	btn_value := CInt(btn_value);
	if ( btn_value < 1 ) // Catch errors (0) and invalid (negative) values
		gump.data_id += 1;
		btn_value := gump.data_id;
	endif

	(gump.layout).Append("CheckBox {} {} {} {} {} {}".format(cint(x), cint(y), cint(unc_id), cint(che_id), cint(status), cint(btn_value))); 

	return (btn_value);
endfunction

function GFSetRadioGroup(byref gump, group_id)
	(gump.layout).Append("Group {}".format(group_id));
	return 1;
endfunction

function GFHTMLArea(byref gump, x, y, width, height, text, background:=0, scrollbar:=0)
	gump.data.Append(CStr(text));
	var index := gump.data.Size()-1;

	(gump.layout).Append("HTMLGump {} {} {} {} {} {} {}".format(cint(x), cint(y), cint(width), cint(height), cint(background), cint(scrollbar))); 
	return (index);
endfunction

// function GFAddHTMLLocalized(byref gump, x, y, width, height, cliloc, background:=0, scrollbar:=0, hue:=0)
// 	if(hue == 0)
// 		(gump.layout).Append("XMFHTMLGump "+CStr(CInt(x))+" "+CStr(CInt(y))+" "
// 					+ CStr(CInt(width))+" "+CStr(CInt(height))+" "+CStr(cliloc)+" "
// 					+ CStr(CInt(background))+" "+CStr(CInt(scrollbar)));
// 	else
// 		(gump.layout).Append("XMFHTMLGumpColor "+CStr(CInt(x))+" "+CStr(CInt(y))+" "
// 					+ CStr(CInt(width))+" "+CStr(CInt(height))+" "+CStr(CInt(cliloc))+" "
// 					+ CStr(CInt(background))+" "+CStr(CInt(scrollbar))+" "+CStr(CInt(hue)));
// 	endif
// 	return 1;
// endfunction

function GFGumpPic(byref gump, x, y, gump_id, hue := 0)
	(gump.layout).Append("GumpPic {} {} {}".format(cint(x), cint(y), cint(gump_id))); 

	hue := CInt(hue);
	if ( hue )
		gump.layout[gump.layout.size()] += " hue={}".format(hue);
	endif

	return 1;
endfunction

function GFGumpPicTiled(byref gump, x, y, width, height, gump_id)
	(gump.layout).Append("GumpPicTiled {} {} {} {} {} {} {}".format(cint(x), cint(y), cint(width), cint(height), cint(gump_id))); 

	return 1;
endfunction

function GFTilePic(byref gump, x, y, tile_id, hue := 0)
	var text := "{} {} {}".format(cint(x), cint(y), cint(tile_id));

	hue := CInt(hue);
	if ( hue )
		text := "TilePicHue {} {}".format(text, hue);
	else
		text := "TilePic {}".format(text);
	endif

	(gump.layout).Append(text);
	return 1;
endfunction

function GFTextLine(byref gump, x, y, hue:=0, text:="")
	gump.data.Append(CStr(text));
	var index := gump.data.Size()-1;
	(gump.layout).Append("Text {} {} {} {}".format(cint(x), cint(y), cint(hue),  cint(index))); 

	return (index);
endfunction

function GFTextCrop(byref gump, x, y, width, height, hue:=0, text:="")
	gump.data.Append(CStr(text));
	var index := gump.data.Size()-1;

	(gump.layout).Append("CroppedText {} {} {} {} {} {}".format(cint(x), cint(y),  cint(width), cint(height), cint(hue),  cint(index))); 

	return (index);
endfunction

function GFRadioButton(byref gump, x, y, unp_id, pres_id, status:=0, btn_value:=0)
	btn_value := CInt(btn_value);
	if ( btn_value < 1 ) // Catch errors (0) and invalid (negative) values
		gump.data_id += 1;
		btn_value := gump.data_id;
	endif

	(gump.layout).Append("Radio {} {} {} {} {} {}".format(cint(x), cint(y),  cint(unp_id), cint(pres_id), cint(status),  cint(btn_value))); 
	return (btn_value);
endfunction

function GFTextEntry(byref gump, x, y, width, height, txt_color, ini_text:="", txt_id:=0)
	gump.data.Append(CStr(ini_text));
	var index := gump.data.Size()-1;

	txt_id := CInt(txt_id);
	if ( txt_id < 1 ) // Catch errors (0) and invalid (negative) values
		gump.data_id += 1;
		txt_id := gump.data_id;
	endif

	(gump.layout).Append("TextEntry {} {} {} {} {} {} {}".format(cint(x), cint(y),  cint(width), cint(height), cint(txt_color),  cint(txt_id), cint(index))); 

	return (txt_id);
endfunction

function GFSendGump(mobile, byref gump)
	var ret_val := SendDialogGump(mobile, gump.layout, gump.data, gump.base.x, gump.base.y, 0, gump.gumpid);
	if ( ret_val.errortext )
		//Sometimes comes up when the gump has too much information.
		SendSysMessage(mobile, "Error - Unable to display gump.");
		SendSysMessage(mobile, "Error text is ->"+ret_val.errortext);
	endif

	return ret_val;
endfunction

function GFDrawWrappedText( byref gump, byref x, byref y, byref text_lines, txt_color := 2100 )
	foreach line in ( text_lines )
		GFTextLine( gump, x, y, txt_color, line );
		y += 20;
		SleepMS(2);
	endforeach

	return 1;
endfunction

function XGFError(text, log:=0)
	if ( log )
		SysLog("Gump Error:: "+text);
	endif

	return error{"errortext":=text};
endfunction

function GFTextEntryLimited(byref gump, x, y, width, height, txt_color, ini_text:="", txt_id:=0,lentxt := 0)
	if (lentxt == 0)
		lentxt := cint((width * 22) / 20); //três simples
	endif

	gump.data.Append(ini_text); 
	var index := gump.data.Size()-1; 
	txt_id := CInt(txt_id);  
	
	if ( txt_id < 1 ) // Catch errors (O) and invalid (negative) values  
		gump.data_id := gump.data_id+1; 
		txt_id := gump.data_id;  
	endif
	
	(gump.layout).Append("TextEntryLimited "+CStr(CInt(X))+" "+CStr(CInt(Y))+" "
		+ CStr(CInt(width))+" "+CStr(CInt(height))+" "
		+ CStr(CInt(txt_color))+" "+CStr(txt_id)+" "+CStr(index)+" "+Cstr(lentxt)); 

	return (txt_id);   
endfunction

//Vitor Lima Entry
/*
 * GFTooltip(byref gump, cliloc_id)
 *
 * Purpose
 * Adds a tooltip to the previous layoutarray entry
 *
 * Parameters
 * gump: Reference to the gump
 * cliloc_id:  The decimal number reference to the cliloc entry which will be used as the tooltip.
 *
 *
 * Return value
 * No return value.
 *
 */

function GFTooltipText(byref gump, entrytext)
   (gump.layout).Append("Tooltip 1114057 @{}@'".format(entrytext));

   return 1;
endfunction


