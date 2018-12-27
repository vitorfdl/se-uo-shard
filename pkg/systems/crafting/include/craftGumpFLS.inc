/* Resumo de Funções
 * sendCraftGump(who, params)
 * createGump(who, recipes, params)
 *
 *
 *
**/

use uo;
use cfgfile;

include "include/say";
include "include/client";
include ":gumps:gumps";
include ":gumps:gumps_ex";
include ":crafting:fls_crafting";

const BTN_CRAFTIT  := 500;
const BTN_SEARCH   := 4;
const BTN_MAKELAST := 5;
const BTN_REPAIR   := 6;
const BTN_SMELT    := 7;
const BTN_FILTER   := 10;
var search_entry;
var qty_entry;
var recipe_cfg;

var fix_cfg := ReadConfigFile(":fls_core:item_fix");
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
    while(not_closed && who.connected)
        var player_recipes := GetRecipeList(who, recipe_cfg, params.search);
        var input          := createGump(who, player_recipes, params);

        params.search := GFExtractData(input, search_entry);
        if (!params.search)
            params.search := "";
        endif

        params.qty := cint(GFExtractData(input, qty_entry));
        if (!params.qty || params.qty == 0)
            params.qty := 1;
         elseif (params.qty > 10)
            params.qty := 10;
            SendSysMessageEx(who, "O limite de quantidade é 10. Ajustado para 10.", SSM_INFO);
        endif

        if (!input[0] || input[0] == 0 || input[0] == 1)
            not_closed := 0;
        elseif (input[0] == BTN_REPAIR)
            doRepair(who, params.type, recipe_cfg);
         elseif (input[0] == BTN_SMELT)
            SendSysMessageEx(who, "Selecione o item que vai descontruir", SSM_REQUEST);
            var item := Target(who, TGTOPT_NOCHECK_LOS);
            if (item)
               SmeltItem(who, item, recipe_cfg, params.type);
            endif
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
            params.bag := GetBagList(who);
        endif
      sleepms(300);
    endwhile
endfunction

// 0xC86E
function createGump(who, recipes, params)
	var gump := GFCreateGump();
	GFSetID(gump, GUMPID_CRAFTING);

    //Header
	GFGumpPic(gump, 98+120, 12, 40019, 0);
   GFHtmlShadowed(gump, 102+120, 16, 121, 20, "#ffffff", "<BASEFONT SIZE=5><center>{}".format(params.type));

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
   
	GFResizePic(gump, 328+216, 324+100, 30546, 135, 88+24); //options
	GFResizePic(gump, 13, 36, 40000, 540, 590);

    //Search
	GFTextLine(gump, 31+90, 50, 1153, "Filtrar:");
	GFResizePic(gump, 31+140, 50, 30546, 288, 25);
    search_entry := GFTextEntry(gump, 37+140, 52, 225, 20, 1101, params.search);
	GFAddButton(gump, 288+140, 51, 9011, 9011, GF_CLOSE_BTN, BTN_SEARCH);
    
    //Crafting Options
	GFTextLine(gump, 343+216, 333+100, 1153, "Quantidade");
    // GFToolTipText(gump, 1062951);
	GFResizePic(gump, 426+216, 332+100, 30546, 31, 22);
    qty_entry := GFTextEntry(gump, 431+216, 334+100, 24, 20, 1101, ""+params.qty);
    // GFToolTipText(gump, 1062951);
	GFTextLine(gump, 343+216, 360+100, 1153, "Fazer Ultimo");
	GFAddButton(gump, 426+216, 360+100, 40017, 40027, GF_CLOSE_BTN, BTN_MAKELAST);
	GFTextLine(gump, 343+216, 384+100, 1153, "Desconstruir");
	GFAddButton(gump, 426+216, 384+100, 40017, 40027, GF_CLOSE_BTN, BTN_SMELT);

   if (!params.no_fix)
      GFTextLine(gump, 343+216, 408+100, 1153, "Consertar");
      GFAddButton(gump, 426+216, 408+100, 40017, 40027, GF_CLOSE_BTN, BTN_REPAIR);
   endif


	GFTextLine(gump, 260, 600, 1153, "Página");
    //Recipes
    var r_x := 80;
    var r_y := 88;
    GFPage(gump, 1);
    foreach recipe in (recipes)
      if (r_y >= 580)
      //   GFAddButton(gump, 148, 553, 2095, 2095, GF_PAGE_BTN, gump.cur_page+1); //Expand Gump
      //    r_y := 88;
      r_y := 88;
      r_x += 250;
         if (r_x >= 540)
            GFAddButton(gump, 230+75, 603, 2224, 2224, GF_PAGE_BTN, gump.cur_page+1); //Expand Gump
            GFPage(gump, gump.cur_page+1);
            GFAddButton(gump, 200+35, 603, 2223, 2223, GF_PAGE_BTN, gump.cur_page-1); //Expand Gump
            r_x := 80;
         endif
      endif



      //   if (_recipe_iter == recipes.Size())
	      //   GFAddButton(gump, 300, 585, 2223, 2223, GF_PAGE_BTN, 1); //Expand Gump
	      //   GFAddButton(gump, 300, 585, 2223, 2223, GF_PAGE_BTN, 1); //Expand Gump
      //   endif

      GFAddButton(gump, r_x-47, r_y+2, 306, 306, GF_CLOSE_BTN, BTN_CRAFTIT+_recipe_iter);

      GFResizePic(gump, r_x-7, r_y-4, 5120, 205, 49);
      GFResizePic(gump, r_x-49, r_y, 30546, 45, 45);
      var fix_p := GetItemFix(fix_cfg, recipe.graphic);
      GFTilePic(gump, r_x-55+fix_p.x, r_y+4+fix_p.y, recipe.graphic, cint(recipe.color));
      if (recipe.desc || recipe.level)
         var description := array{};
         if (recipe.level)
            var div_amt := 100;
            if (itemdesc_cfg[recipe.objtype].ArmorType) div_amt := 50; endif

            var experience := cint(recipe.exp * 100) / (recipe.level * div_amt);
            description.append("<center>Experiência: {}{}%".format(BLUE, experience));
         endif

         if (recipe.desc)
            description.append("{}{}".format(WHITE, recipe.desc));
         endif
         GFTooltipText(gump, "<br>".join(description));
      endif
      var recipe_name := recipe.name;
      if (recipe.level)
         recipe_name += " Lv.{}".format(recipe.level);
      endif

      GFHtmlShadowed(gump, r_x, r_y-7, 190, 20, "#9999ff", "<BASEFONT SIZE=4>{}".format(recipe_name));
      
      var m_x := r_x;
      var m_y := r_y+9;
      foreach material in (recipe.materials.Keys())
         if (_material_iter == 3)
            m_x += 100;
            m_y := r_y+8;
            GFGumpPicTiled(gump, m_x-5, m_y+6, 1, 20, 5104);  // divisor
         endif
         

         var color := 435;
         var material_req  := recipe.materials[material];
         var material_have := params.bag[cint(material)];
         if (!material_have) 
            material_have := params.bag[material];
         endif

         if (!material_have)
               material_have := 0;
         elseif (material_have >= recipe.materials[material])
               color := 65;
         endif

         var orgname := TruncatePlural(material);

         if (orgname == "demonbone")
               orgname := "osso de moriq.";
         elseif (orgname == "madeira")
               orgname := "tábua";
         endif

         var name := orgname;
         if (len(name) > 12)
            name := "{}.".format(name[1,12]);
         endif

         var material_text := "{} {}".format(material_req, name);
         GFTextLine(gump, m_x, m_y, color, material_text);
         GFTooltipText(gump, orgname);
         // GFHTMLArea(gump, m_x, m_y, 90, 20, material_text);
         m_y += 14;
         sleepms(2);
      endforeach

      r_y += 52;
      sleepms(2);
   endforeach

   return GFSendGump(who, gump);
endfunction

function LoadItemEventData()
    var df := DFOpenDataFile( ":crafting:excepitems", DF_CREATE, DF_KEYTYPE_INTEGER);
    return df;
endfunction