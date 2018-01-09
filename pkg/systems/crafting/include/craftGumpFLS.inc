/* Resumo de Funções
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
   if (ListHostiles(who, 10).size() >= 1)
      return SendSysMessageEx(who, "Você não pode fazer isso enquanto está sendo atacado", SSM_FAIL);
   endif

    params.+bag    := GetBagList(who);
    params.+search := "";
    params.+qty    := 1;

    recipe_cfg := params.recipe_cfg;

    var not_closed := 1;
    while(not_closed)
        var player_recipes := GetRecipeList(who, recipe_cfg, params.search);
        var input          := createGump(who, player_recipes, params);

        params.search := GFExtractData(input, search_entry);
        if (!params.search)
            params.search := "";
        endif

        params.qty := cint(GFExtractData(input, qty_entry));
        if (!params.qty || params.qty == 0 || params.qty > 10)
            params.qty := 1;
        endif

        if (!input[0] || input[0] == 0 || input[0] == 1)
            not_closed := 0;
        elseif (input[0] == BTN_REPAIR)
            doRepair(who, params.type, recipe_cfg);
        elseif (input[0] == BTN_MAKELAST)
            var craft_last := GetObjProperty(who, "#craft_last");
            craft_last := FindElemInArray(player_recipes, struct{ "objtype" := craft_last });
            if (craft_last)
               SendSysMessageEx(who, "Fazendo ultimo item: {}".format(craft_last.name), SSM_INFO);
               CraftItem(who, params.type, params.tool, craft_last, params.qty, 1);
            else
               SendSysMessageEx(who, "Você precisa fazer algum item antes de usar essa opção.", SSM_FAIL);
            endif
        elseif (input[0] == BTN_FILTER)
            //nothing
        elseif (input[0] != BTN_SEARCH)
            var recipe := player_recipes[input[0]-BTN_CRAFTIT];
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
	    GFResizePic(gump, 328, 48, 30546, 184, 140); //filter
        GFHTMLArea(gump, 337, 52, 50, 20, "<BASEFONT SIZE=2 COLOR=#ffffff>Filtros");
        foreach filter in (params.filter)
            GFAddButton(gump, 343, 83, filter.button, 40015, GF_CLOSE_BTN, BTN_FILTER+_filter_iter);
            GFTextLine(gump, 372, 83, 1153, filter.name);
            sleepms(2);
        endforeach
    endif
    
    if (params.bonus)
	    GFResizePic(gump, 324, 186, 30546, 187, 140); //bonus
        GFHTMLArea(gump, 402, 190, 50, 20, "<BASEFONT SIZE=2 COLOR=#ffffff>Bonus");
    endif
    
	GFResizePic(gump, 328, 324, 30546, 135, 88); //options
	GFResizePic(gump, 13, 36, 40000, 324, 521);

    //Search
	GFResizePic(gump, 31, 50, 30546, 288, 25);
    search_entry := GFTextEntry(gump, 37, 52, 225, 20, 1101, params.search);
	GFAddButton(gump, 288, 51, 9011, 9011, GF_CLOSE_BTN, BTN_SEARCH);
    
    //Crafting Options
	GFTextLine(gump, 343, 333, 1153, "Quantidade");
    // GFToolTipText(gump, 1062951);
	GFResizePic(gump, 426, 332, 30546, 31, 22);
    qty_entry := GFTextEntry(gump, 431, 334, 24, 20, 1101, ""+params.qty);
    // GFToolTipText(gump, 1062951);
	GFTextLine(gump, 343, 360, 1153, "Fazer Ultimo");
	GFAddButton(gump, 426, 360, 40017, 40027, GF_CLOSE_BTN, BTN_MAKELAST);
	GFTextLine(gump, 343, 384, 1153, "Consertar");
	GFAddButton(gump, 426, 385, 40017, 40027, GF_CLOSE_BTN, BTN_REPAIR);

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
                m_x += 110;
                m_y := r_y+8;
            endif

            var color := 1271;
            var material_req  := recipe.materials[material];
            var material_have := params.bag[cint(material)];
            if (!material_have) 
               material_have := params.bag[material];
            endif

            if (!material_have)
                material_have := 0;
            elseif (material_have >= recipe.materials[material])
                color := 1936;
            endif

            var name := TruncatePlural(material);
            if (name == "demonbone")
                name := "osso de moriq.";
            elseif (name["monsterpart"])
                name["monsterpart"] := "";
                name := "criatura "+name;
            elseif (name == "madeira")
                name := "tábua";
            endif
            var material_text := "{}/{} {}".format(material_have, material_req, name);
            GFTextLine(gump, m_x, m_y, color, material_text);
            // GFHTMLArea(gump, m_x, m_y, 90, 20, material_text);
            m_y += 14;
            sleepms(2);
        endforeach

        r_y += 50;
        sleepms(2);
    endforeach

	return GFSendGump(who, gump);
endfunction
