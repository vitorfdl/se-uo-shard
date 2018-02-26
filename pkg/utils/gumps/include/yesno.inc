/*===============================================================
* Current Version
* YESNO.INC - v1.0
* Updated 9/14/2005 9:23PM
*
* -- Revision v1.0 --
* Austin:
*  Created include file

Gump foi mudado para ser do tamanho equivalente as opcoes. Aioros.
===============================================================*/

use uo;
use os;

include ":gumps:gumps";
include ":gumps:gumps_ex";

/* 
 * YesNo(mobile, prompt, yes_text, no_text)
 *
 * Purpose
 * Template to create a yes / no gump.
 *
 * Parameters
 * mobile:	Mobile to send the gump to.
 * prompt:	Primary text field, instructing the mobile on what the choices are about.
 * yes_text:	Text to be displayed next to the 'YES' button.
 * no_text:	The text to be displayed next to the 'NO' button.
 *
 * Return value
 * Returns '0' if no is selected.
 * Returns '1' if yes is selected.
 *
 */
function YesNo(mobile, prompt:="", yes_text:="Sim", no_text:="Não")
	var yn_gump := GFCreateGump(140, 100);

	var wraps := struct;
	wraps.+prompt := GFWordWrap(prompt, 385);
	wraps.+yes := GFWordWrap(yes_text, 355);
	wraps.+no := GFWordWrap(no_text, 355);
	var wraps_size := len(wraps.prompt) + len(wraps.yes) + len(wraps.no);

	GFClosable(yn_gump, 0);
	GFResizePic(yn_gump, 0, 0, GFCfgConst("Defaults", "BackGround"), 400, wraps_size*20 +80 ); //205);
	GFResizePic(yn_gump, 15, 15, GFCfgConst("Defaults", "ForeGround"), 370, wraps_size*20 + 50); //175);
	
	var y_pos := 20;
	foreach line in (wraps.prompt)
		GFTextLine(yn_gump, 20, y_pos, 1153, line);
		y_pos := y_pos+20;
		SleepMS(2);
	endforeach
	
	//y_pos := 90;
	y_pos += 20;
	GFAddButton(yn_gump, 20, (y_pos+3), 2117, 2118, GF_CLOSE_BTN, 100);
	foreach line in (wraps.yes);
		GFTextLine(yn_gump, 40, y_pos, 1153, line);
		y_pos := y_pos+20;
		SleepMS(2);
	endforeach
		
	//y_pos := 140;
	y_pos += 20;
	GFAddButton(yn_gump, 20, (y_pos+3), 2117, 2118, GF_CLOSE_BTN);
	foreach line in (wraps.no)
		GFTextLine(yn_gump, 40, y_pos, 1153, line);
		y_pos := y_pos+20;
		SleepMS(2);
	endforeach
	
	var input := GFSendGump(mobile, yn_gump);
	input := input[0];
	if ( input == 100 )
		return 1;
	else
		return 0;
	endif	
endfunction
