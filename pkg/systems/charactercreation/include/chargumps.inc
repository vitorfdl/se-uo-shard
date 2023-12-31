use uo;
use cfgfile;

include ":gumps:gumps";
include "include/arrays";
include ":attributes:attributes";
include ":gumps:gump_prompts";
include ":charactercreation:characters";
include "include/say";

function ItemPickerGump(who, byref chardata)
   if (!who.connected) return; endif
	var cfg := ReadConfigFile(":charactercreation:config/itempicker");
	var secoes := GetConfigStringKeys( cfg );
	var chargump := GFCreateGump();
	var input;

	GFSetID(chargump, GUMPID_CREATECHAR);//9260
	GFClosable(chargump, 0);
	GFDisposable(chargump, 0);
	GFResizePic(chargump, 0, 20, 0x9c40, 600, 450);
	GFGumpPicTiled(chargump, 6, 30,30,425,10460);
	GFGumpPicTiled(chargump, 574, 30,30,425,10460);

	GFTextMid(chargump, 20, 30, 600, 1153, "Defina seus Itens Iniciais");
	GFTextMid(chargump, 15, 50, 600, 1153, "Voc� tem " + chardata.Credits + " cr�ditos.");
	GFAddButton(chargump, 500, 415, 4502, 4502, 1, 999 );
	GFTextLine(chargump, 450, 430, 1153, "Finalizar");

	var x := 100;
	var y := 90;
	foreach entry in secoes
		if (entry != "DefaultAmount")
			GFTextLine(chargump, x, y, 1153, StrReplace(entry, "_", " ") );
			GFAddButton(chargump, x-30, y+4, 9762, 9763, GF_PAGE_BTN, _entry_iter);
			y := y + 20;
		endif
    sleepms(2);
	endforeach

	x := 250;
	y := 90;
	var itensmap := dictionary;
	var itemcusto := dictionary;
   var amtelem := FindConfigElem(cfg, "DefaultAmount");
	var elem;
   var itemcfg := ReadConfigFile(":*:itemdesc");
	var i := 1;
	foreach entry in secoes
		if (entry != "DefaultAmount")
			GFPage(chargump, _entry_iter);
			elem := FindConfigElem(cfg, entry);
			var itens := GetConfigStringDictionary(elem, "item");
			foreach item in (itens.keys())
            var item_name := item;
            var objtype := GetObjtypeByName( item );
            if (objtype && itemcfg[objtype].desc)
               item_name := itemcfg[objtype].desc;
               item_name["%s%"] := "";
            endif

            if (Cint(GetConfigInt(amtelem, item)))
               item_name += " [{}]".format(Cint(GetConfigInt(amtelem, item)));
            endif

            item_name[1] := Upper(item_name[1]);

				GFTextLine(chargump, x, y, 1153, StrReplace(item_name, "_", " ") );
				var custo := itens[item];
				GFTextLine(chargump, x+175, y, 1153, ""+custo );
				itemcusto[i] := custo;
            itensmap[i] := GetConfigString(elem, item);
            if (!itensmap[i])
				   itensmap[i] := array{ item };
            else
               itensmap[i] := SplitWords(itensmap[i], ",");
            endif
				GFAddButton(chargump, x-20, y+4, 2103, 2104, 1, i);
				i := i + 1;
				y := y + 20;
				if ( y >= 450 )
					y := 90;
					x := 320;
				endif
            sleepms(2);
			endforeach
			x := 250;
			y := 90;
		endif
    sleepms(2);
	endforeach

	input := GFSendGump(who, chargump);
	input := input[0];

	if (input == 999)
		return;
	else
		var custo := cint(itemcusto[input]);
      if (chardata.Credits < custo)
         SendSysMessage(who, "Voc� n�o tem cr�ditos para comprar isto.");
         return ItemPickerGump(who, chardata);
      endif

      chardata.Credits := chardata.Credits - custo;
		var item_list := itensmap[input];
      foreach buyitem in item_list
         if (buyitem["Recipe_"])
            buyitem["Recipe_"] := "";
            if (Hex(buyitem) != error && cint(Hex(buyitem)) != 0)
   			   buyitem := hex(buyitem);
            endif
            var obj := CreateItemInBackpack(who, 0xef1f);
            SetObjProperty(obj, "objtype", buyitem);
            Run_Script( ":crafting:recipes/oncreate", obj );
         elseif (buyitem["Book_"])
            buyitem["Book_"] := "";
            SysBook_CreateBook(who.backpack, buyitem);
         else
            if (buyitem["/"])
               var sex_choose := SplitWords(buyitem, "/");
               if (who.gender == 0)
                  buyitem := sex_choose[1];
               else
                  buyitem := sex_choose[2];
               endif
            endif

            var amt := 1;

            amt := Cint(GetConfigInt(amtelem, buyitem));
            if ( (amt == 0) || (amt == error) )
               amt := 1;
            endif
               
            var created := CreateItemInBackpack( who, buyitem, amt );
            if (created.isA(POLCLASS_WEAPON))
               created.dmg_mod := 15;
               created.quality := 2;
               SetName(created, created.desc);
            endif
         endif

         sleepms(2);
      endforeach
	endif

   ItemPickerGump(who, chardata);
endfunction
