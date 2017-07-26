/* Resumo de Fun��es
 * sendCraftGump(who, params)
 * createGump(who, recipes, params)
 *
 *
 *
**/

use uo;

include "include/say";
include "include/client";
include ":gumps:gumps";
include ":gumps:gumps_ex";
include ":crafting:fls_crafting";

const BTN_CRAFTIT  := 500;
const BTN_SEARCH   := 4;
const BTN_MAKELAST := 5;
const BTN_REPAIR   := 6;
const BTN_FILTER   := 10;
var search_entry;
var qty_entry;
var recipe_cfg;
/**
 * Expect params
 * {string} params.type
 **/
function sendCraftGump(who, params)
    CloseGump(who, GUMPID_CRAFTING);
    params.+bag    := GetBagList(who);
    params.+search := "";
    params.+qty     := 1;

    recipe_cfg         := params.recipe_cfg;

    var not_closed := 1;
    while(not_closed)
        var player_recipes := GetRecipeList(who, recipe_cfg, params.search);
        var input := createGump(who, player_recipes, params);

        params.search := GFExtractData(input, search_entry);
        if (!params.search)
            params.search := "";
        endif
        params.qty := cint(GFExtractData(input, qty_entry));
        if (!params.qty || params.qty == 0 || params.qty > 10)
            params.qty := 1;
        endif

        if (!input[0] || input[0] == 1)
            not_closed := 0;
        else
            var recipe := player_recipes[input[0]-BTN_CRAFTIT];
            print(params.qty);
            CraftItem(who, params.type, params.tool, recipe, params.qty, 1);
            sleepms(10);
            params.bag := GetBagList(who);
        endif
    endwhile
endfunction

// 0xC86E
function createGump(who, recipes, params)

	var gump := GFCreateGump();
	GFSetID(gump, GUMPID_CRAFTING);

    //Header
	GFGumpPic(gump, 98, 12, 40019, 0);
    GFHtmlShadowed(gump, 102, 16, 121, 20, "#ffffff", "<BASEFONT SIZE=5><center>{}".format(params.type));

    //Background
    if (params.filter)
	    GFResizePic(gump, 311, 48, 30546, 184, 140); //filter
        GFHTMLArea(gump, 385, 52, 50, 20, "<BASEFONT SIZE=2 COLOR=#ffffff>Filtros");
        foreach filter in (params.filter)
            GFAddButton(gump, 326, 83, filter.button, 40015, GF_CLOSE_BTN, BTN_FILTER+_filter_iter);
            GFTextLine(gump, 355, 83, 1149, filter.name);
        endforeach
    endif
    
    if (params.bonus)
	    GFResizePic(gump, 308, 186, 30546, 187, 140); //bonus
        GFHTMLArea(gump, 385, 190, 50, 20, "<BASEFONT SIZE=2 COLOR=#ffffff>Bonus");
    endif
    
	GFResizePic(gump, 308, 324, 30546, 135, 88); //options
	GFResizePic(gump, 13, 36, 40000, 304, 521);

    //Search
	GFResizePic(gump, 31, 50, 30546, 264, 25);
    search_entry := GFTextEntry(gump, 37, 52, 225, 20, 1101, params.search);
	GFAddButton(gump, 268, 51, 9011, 9011, GF_CLOSE_BTN, BTN_SEARCH);
    
    //Crafting Options
	GFTextLine(gump, 323, 333, 1149, "Quantidade");
    GFToolTip(gump, 1062951);
	GFResizePic(gump, 406, 332, 30546, 31, 22);
    qty_entry := GFTextEntry(gump, 411, 334, 24, 20, 1101, ""+params.qty);
    GFToolTip(gump, 1062951);
	GFTextLine(gump, 323, 360, 1149, "Fazer Ultimo");
	GFAddButton(gump, 406, 360, 40017, 40027, GF_CLOSE_BTN, 0);
	GFTextLine(gump, 323, 384, 1149, "Consertar");
	GFAddButton(gump, 406, 385, 40017, 40027, GF_CLOSE_BTN, 0);

    //Recipes
    var r_x := 90;
    var r_y := 88;
    GFPage(gump, 1);
    foreach recipe in (recipes)
        if (r_y >= 530)
	        GFAddButton(gump, 148, 553, 2095, 2095, GF_PAGE_BTN, gump.cur_page+1); //Expand Gump
            GFPage(gump, gump.cur_page+1);
            r_y := 88;
        endif

        if (_recipe_iter == recipes.Size())
	        GFAddButton(gump, 148, 553, 2095, 2095, GF_PAGE_BTN, 1); //Expand Gump
        endif

        GFAddButton(gump, r_x-65, r_y+18, 2103, 2104, GF_CLOSE_BTN, BTN_CRAFTIT+_recipe_iter);

        GFResizePic(gump, r_x-49, r_y, 30546, 45, 45);
        GFTilePic(gump, r_x-55, r_y+4, recipe.graphic);

        GFHtmlShadowed(gump, r_x, r_y-7, 121, 20, "#9999ff", "<BASEFONT SIZE=5>{}".format(recipe.name));
        
        var m_x := r_x;
        var m_y := r_y+8;
        foreach material in (recipe.materials.Keys())
            if (_material_iter == 3)
                m_x += 93;
                m_y := r_y+8;
            endif

            var color := 2058;
            var material_req  := recipe.materials[material];
            var material_have := params.bag[cint(material)];
            if (!material_have) 
               material_have := params.bag[material];
            endif

            if (!material_have)
                material_have := 0;
            elseif (material_have >= recipe.materials[material])
                color := 1271;
            endif

            var material_text := "{}|{} {}".format(material_req, material_have, TruncatePlural(material));
            GFTextLine(gump, m_x, m_y, color, material_text);
            // GFHTMLArea(gump, m_x, m_y, 90, 20, material_text);
            m_y += 14;
        endforeach

        r_y += 50;
    endforeach

	return GFSendGump(who, gump);
endfunction