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


function createGump(who, recipes, params)
   var gump := GFCreateGump();
   GFSetID(gump, GUMPID_CRAFTING);
   var baseX := 0;
   var baseY := 12;

   GFResizePic(gump, baseX, baseY, 302, 640-40, 720); // background

   var gumpMiddleWidth := 570;
   var gumpMiddleHeight := 690;

   baseY += 30;
   baseX += 15;
      //Header
   // var headerXPos := GFCenterRectangle(ALIGN_CENTER, 100, baseX, gumpMiddleWidth);
   GFGumpPic(gump, baseX+20, baseY+2, 1764, 0);
   GFHtmlShadowed(gump, baseX+20+4, baseY+17, 190, 20, "#ffffff", "<BASEFONT SIZE=5><center>{}".format(params.type));

      //Background
   // if (params.filter)
   //    GFResizePic(gump, baseX+208, baseY+36, 30546, 184, 140); //filter
   //    GFHTMLArea(gump, baseX+217, baseY+40, 50, 20, "<BASEFONT SIZE=2 COLOR=#ffffff>Filtros");
   //    foreach filter in (params.filter)
   //       GFAddButton(gump, baseX+223, baseY+71, filter.button, 40015, GF_CLOSE_BTN, BTN_FILTER+_filter_iter);
   //       GFTextLine(gump, baseX+252, baseY+71, 1153, filter.name);
   //       sleepms(2);
   //    endforeach
   // endif
      
   // if (params.bonus)
   //    GFResizePic(gump, baseX+204, baseY+174, 30546, 187, 140); //bonus
   //    GFHTMLArea(gump, baseX+282, baseY+178, 50, 20, "<BASEFONT SIZE=2 COLOR=#ffffff>Bonus");
   // endif

      //Search
   var searchXPos := GFCenterRectangle(ALIGN_RIGHT, 300, baseX, gumpMiddleWidth) - 10;

   GFResizePic(gump, searchXPos, baseY+3+12, 9350, 338-40, 25);
   GFTextLine(gump, searchXPos+5, baseY+6+12, 1153, "Filtrar:");
   search_entry := GFTextEntry(gump, searchXPos+57, baseY+6+12, 225-40, 20, 1101, params.search);
   GFAddButton(gump, searchXPos+308-40, baseY+5+12, 9011, 9011, GF_CLOSE_BTN, BTN_SEARCH);
   // end Search

   GFResizePic(gump, baseX+544, baseY+424, 30546, 135, 112); //options
      //Crafting Options
   GFTextLine(gump, baseX+559, baseY+433, 1153, "Quantidade");
      // GFToolTipText(gump, 1062951);
   GFResizePic(gump, baseX+642, baseY+432, 30546, 31, 22);
      qty_entry := GFTextEntry(gump, baseX+647, baseY+434, 24, 20, 1101, ""+params.qty);
      // GFToolTipText(gump, 1062951);
   GFTextLine(gump, baseX+559, baseY+460, 1153, "Fazer Ultimo");
   GFAddButton(gump, baseX+642, baseY+460, 40017, 40027, GF_CLOSE_BTN, BTN_MAKELAST);
   GFTextLine(gump, baseX+559, baseY+484, 1153, "Desconstruir");
   GFAddButton(gump, baseX+642, baseY+484, 40017, 40027, GF_CLOSE_BTN, BTN_SMELT);

   if (!params.no_fix)
      GFTextLine(gump, baseX+559, baseY+508, 1153, "Consertar");
      GFAddButton(gump, baseX+642, baseY+508, 40017, 40027, GF_CLOSE_BTN, BTN_REPAIR);
   endif


   var recipeWidth := 250;
   var recipeHeight := 65;

   baseY += 55;
   //Recipes
   var table := GFTableProps(gumpMiddleWidth, gumpMiddleHeight-100, recipeWidth, recipeHeight, 10);

	var columnPos := GFColumnPosition(recipeWidth, table.numberOfColumns, gumpMiddleWidth-30, baseX+5);
	var rowPos := GFRowPosition(0, recipeHeight, table.numberOfRows, gumpMiddleHeight, baseY);

	var recipePages := BreakArray(recipes, table.itemsPerPage);
	var nColumns := table.numberOfColumns;

   // GFAddAlphaRegion(gump, baseX, baseY, recipeWidth, recipeHeight);

   foreach page in (recipePages)
      GFPage(gump, 1);

      // if (table.nOfPages > 1)
         GFPageScrollBar(gump, gumpMiddleWidth-17, baseY, table.nOfPages, gump.cur_page, gumpMiddleHeight-200);
      // endif
      
      foreach recipe in (recipes)
			var i := cint(((_recipe_iter -1) % nColumns) + 1); // columnPos
         var j := cint(Floor((_recipe_iter - 1) / nColumns) + 1); // rowPos

         var btnX := columnPos[i];
         var btnY := rowPos[j];

         GFAddButton(gump, btnX, btnY, 306, 306, GF_CLOSE_BTN, BTN_CRAFTIT+_recipe_iter);
         GFResizePic(gump, btnX, btnY, 1579, 240, 62); // text background
         // GFResizePic(gump, btnX+3, btnY+3, 30546, 45, 45); // item background

         btnX += 5;
         btnY += 5;

         var fix_p := GetItemFix(fix_cfg, recipe.graphic);
         GFTilePic(gump, btnX + fix_p.x, btnY+8 + fix_p.y, recipe.graphic, cint(recipe.color));
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

         var recipe_name := $"{recipe.name}";
         if (recipe.level)
            recipe_name += $" Lv.{recipe.level}";
         endif

         var textOffset := btnX+50;
         GFHtmlShadowed(gump, textOffset, btnY, 190, 20, "#9999ff", "<BASEFONT SIZE=4>{}".format(recipe_name));
         
         var m_x := textOffset;
         var m_y :=btnY + 15;
         foreach material in (recipe.materials.Keys())
            if (_material_iter == 3)
               m_x += 100;
               m_y := rowPos[_recipe_iter] + 8;
               GFGumpPicTiled(gump, m_x - 5, m_y + 6, 1, 20, 5104);  // divisor
            endif
            

            var color := RED_FONT_COLOR;
            var material_req  := recipe.materials[material];
            var material_have := params.bag[cint(material)];
            if (!material_have) 
               material_have := params.bag[material];
            endif

            if (!material_have)
                  material_have := 0;
            elseif (material_have >= recipe.materials[material])
                  color := GREEN_FONT_COLOR;
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
            m_y += 14;
            sleepms(2);
         endforeach

         sleepms(2);
      endforeach
   endforeach

   return GFSendGump(who, gump);
endfunction

function LoadItemEventData()
    var df := DFOpenDataFile( ":crafting:excepitems", DF_CREATE, DF_KEYTYPE_INTEGER);
    return df;
endfunction