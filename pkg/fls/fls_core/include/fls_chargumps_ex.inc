
use uo;
use cfgfile;

include ":gumps:gumps";
include "include/arrays";
include ":fls_core:fls_characters";

function FLHairDye(who, byref new_look)

	var skingump := GFCreateGump();
	GFResizePic(skingump, 0, 0, 3600, 300, 420);
	GFResizePic(skingump, 15, 15, 0x13BE, 270, 390);

	GFTextMid(skingump, 15, 25, 270, 1890, "Escolha a Cor do Cabelo");
	GFAddButton(skingump, 220, 370, 2128, 2129, 1, OK_BUTTON);

	var skin_cfg := ReadConfigFile(":fls_core:config/racas");
	if ( !skin_cfg )
		SendSysMessage(who, "Error: Could not open :fls_core:config/racas.cfg -> "+skin_cfg.errortext);
	endif
	skin_cfg := skin_cfg[cstr(GetObjProperty(who, "raca"))];

	GFPage(skingump, 1);

	var y_pos := 60;
	var x_pos := 20;
	var place := 1;
	var page_num := 1;
	var line := 1;
	foreach color in GetConfigStringArray(skin_cfg, "HairColor");
		color := CInt(color);
		if (color == new_look.SkinColor)
			GFRadioButton(skingump, x_pos, y_pos, 9727, 9730, 1, color);
		else
			GFRadioButton(skingump, x_pos, y_pos, 9727, 9730, 0, color);
		endif

		GFTextline(skingump, x_pos+35, y_pos+5, color-1, "Hair");

		if (place < 3)
			x_pos := x_pos+80;
			place := place+1;
		else
			place := 1;
			x_pos := 20;
			y_pos := y_pos+30;

			line := line+1;

			if (line > 10)
				GFAddButton(skingump, 265, 340, 2648, 2649, GF_PAGE_BTN, page_num+1);
				page_num := page_num+1;
				GFPage(skingump, page_num);
				GFAddButton(skingump, 265, 60, 2650, 2651, GF_PAGE_BTN, page_num-1);

				line := 1;
				y_pos := 60;
			endif
		endif
	endforeach

	var input := GFSendGump(who, skingump);

	if (input[0] == OK_BUTTON)
		foreach key in (input.keys)
			if ( (key != OK_BUTTON) && (key > 0))
				new_look.HairColor := key;
				break;
			endif
		endforeach
	endif

endfunction

function FLSkinGump(who, byref new_look)

	var skingump := GFCreateGump();
	GFResizePic(skingump, 0, 0, 3600, 300, 420);
	GFResizePic(skingump, 15, 15, 0x13BE, 270, 390);

	GFTextMid(skingump, 15, 25, 270, 1890, "Escolha a Cor de Pele");
	GFAddButton(skingump, 220, 370, 2128, 2129, 1, OK_BUTTON);

	var skin_cfg := ReadConfigFile(":fls_core:config/racas");
	if ( !skin_cfg )
		SendSysMessage(who, "Error: Could not open :fls_core:config/racas.cfg -> "+skin_cfg.errortext);
	endif
	skin_cfg := skin_cfg[cstr(GetObjProperty(who, "raca"))];

	GFPage(skingump, 1);

	var y_pos := 60;
	var x_pos := 20;
	var place := 1;
	var page_num := 1;
	var line := 1;
	foreach color in GetConfigStringArray(skin_cfg, "SkinColor");
		color := CInt(color);
		if (color == new_look.SkinColor)
			GFRadioButton(skingump, x_pos, y_pos, 9727, 9730, 1, color);
		else
			GFRadioButton(skingump, x_pos, y_pos, 9727, 9730, 0, color);
		endif

		GFTextline(skingump, x_pos+35, y_pos+5, color-1, "Skin");

		if (place < 3)
			x_pos := x_pos+80;
			place := place+1;
		else
			place := 1;
			x_pos := 20;
			y_pos := y_pos+30;

			line := line+1;

			if (line > 10)
				GFAddButton(skingump, 265, 340, 2648, 2649, GF_PAGE_BTN, page_num+1);
				page_num := page_num+1;
				GFPage(skingump, page_num);
				GFAddButton(skingump, 265, 60, 2650, 2651, GF_PAGE_BTN, page_num-1);

				line := 1;
				y_pos := 60;
			endif
		endif
	endforeach

	var input := GFSendGump(who, skingump);

	if (input[0] == OK_BUTTON)
		foreach key in (input.keys)
			if ( (key != OK_BUTTON) && (key > 0))
				new_look.SkinColor := key;
				break;
			endif
		endforeach
	endif

endfunction

function FLHairGump(who, byref new_look)

	var hairgump := GFCreateGump();

	GFResizePic(hairgump, 0, 0, 3600, 300, 420);
	GFResizePic(hairgump, 15, 15, 0x13BE, 270, 390);

	GFTextMid(hairgump, 15, 25, 270, 1890, "Escolha seu Cabelo");
	GFAddButton(hairgump, 220, 370, 2128, 2129, 1, OK_BUTTON);

	GFResizePic(hairgump, 220, 55, 2620, 50, 310);

	var config := ReadConfigFile(":fls_core:config/hairtypesfls_chargumps_ex");
	var hair;
	if ( (GetObjProperty(who, "raca") == "anao") || (GetObjProperty(who, "raca") == "hobbit"))
		hair := config["headdwarf"];
	else
		hair := config["head"];
	endif
	var hairgumps := config["hairgump"];
	GFSetRadioGroup(hairgump, 1);

	var y_pos := 65;
	foreach style in GetConfigStringArray(hair, "style")
		style := splitwords(style);
		var hairname := StrReplace(style[1], "_", " ");
		var hairgraphic := cint(style[2]);

		if (hairgraphic == new_look.HairStyle)
			GFRadioButton(hairgump, 20, y_pos, 2151, 2154, 1, hairgraphic);
		else
			GFRadioButton(hairgump, 20, y_pos, 2151, 2154, 0, hairgraphic);
		endif

		GFTextline(hairgump, 55, y_pos+5, 1890, hairname);

		var gumppic := GetConfigStringArray(hairgumps, style[1]);
		gumppic := splitwords(gumppic);
		GFGumpPic(hairgump, 155, y_pos-50, gumppic[new_look.Gender+2]);

		y_pos := y_pos + 30;
	endforeach

	if ( !new_look.HairStyle || new_look.HairStyle == 1 )
		GFRadioButton(hairgump, 20, y_pos, 2151, 2154, 1, 1);
	else
		GFRadioButton(hairgump, 20, y_pos, 2151, 2154, 0, 1);
	endif
	GFTextline(hairgump, 55, y_pos+5, 1890, "Bald");

	var input := GFSendGump(who, hairgump);


	if (input[0] == OK_BUTTON)
		foreach key in (input.keys)
			if ( (key != OK_BUTTON) && (key > 0))
				sendsysmessage(who, " "+ key);
				new_look.HairStyle := key;
				break;
			endif
		endforeach
	endif

endfunction

function FLBeardGump(who, byref new_look)

	var beardgump := GFCreateGump();
	GFResizePic(beardgump, 0, 0, 3600, 300, 420);
	GFResizePic(beardgump, 15, 15, 0x13BE, 270, 390);

	GFTextMid(beardgump, 15, 25, 270, 1890, "Escolha sua barba");
	GFAddButton(beardgump, 220, 370, 2128, 2129, 1, OK_BUTTON);

	GFResizePic(beardgump, 220, 55, 2620, 50, 280);

	var config := ReadConfigFile(":fls_core:config/hairtypes");
	var hair;
	if (GetObjProperty(who, "raca") == "anao")
		if (who.cmdlevel)
			SendSysMessage(who, "lendo barbas de anoes");
		endif
		hair := config["facialhairdwarf"];
	else
		hair := config["facialhair"];
	endif
	var hairgumps := config["hairgump"];
	GFSetRadioGroup(beardgump, 1);

	var y_pos := 65;
	foreach style in GetConfigStringArray(hair, "style")
		style := splitwords(style);
		var hairname := StrReplace(style[1], "_", " ");
		var hairgraphic := cint(style[2]);

		if (hairgraphic == new_look.BeardStyle)
			GFRadioButton(beardgump, 20, y_pos, 2151, 2154, 1, hairgraphic);
		else
			GFRadioButton(beardgump, 20, y_pos, 2151, 2154, 0, hairgraphic);
		endif

		GFTextline(beardgump, 55, y_pos+5, 1890, hairname);

		var gumppic := GetConfigString(hairgumps, style[1]);
		GFGumpPic(beardgump, 155, y_pos-65, gumppic);
		if (who.cmdlevel)
			SendSysMessage(who, "nome do estilo: " + style + " gump: " + gumppic + " id " + hairgraphic);
		endif
		y_pos := y_pos + 30;
	endforeach

	if (!new_look.BeardStyle || new_look.BeardStyle == 1)
		GFRadioButton(beardgump, 20, y_pos, 2151, 2154, 1, 1);
	else
		GFRadioButton(beardgump, 20, y_pos, 2151, 2154, 0, 1);
	endif
	GFTextline(beardgump, 55, y_pos+5, 1890, "Bald");

	var input := GFSendGump(who, beardgump);

	if (input[0] == OK_BUTTON)
		foreach key in (input.keys)
			if ( (key != OK_BUTTON) && (key > 0))
				new_look.BeardStyle := key;
				break;
			endif
		endforeach
	endif

endfunction


function GetTrueSkillName(skill)

	if (skill == "Evidance")
		return "EvaluatingIntelligence";
	elseif ( skill == "ItemLore")
		return "ArmsLore";
	elseif ( skill == "BowCraft");
		return "BowcraftFletching";
	else
		return skill;
	endif

endfunction

function GetSkillDescValue(skill)

	if (!skill || skill == error)
		return "";
	elseif (skill == 1)
		return "Ter";
	elseif (skill == 2)
		return "Sec";
	elseif (skill == 3)
		return "Pri";
	endif

endfunction
