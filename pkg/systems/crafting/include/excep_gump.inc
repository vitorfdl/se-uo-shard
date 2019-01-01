
include ":gumps:gumps";
include ":tn:tngumps";
include ":yggdrasil:combatevents";

var selected_item := 0;

function FindDataFile(filter := 0)
   var df := LoadItemEventData();
   var events := array{};
   foreach event in (DFGetElemNames(df))
      var df_event := DFFindElementEx(df, event);
      var name := DFGetProp(df_event, "name", DF_CREATE, "");
      if (!filter || name[filter])
         events.append(GetItemEvent(event, df_event));
      endif
      sleepms(2);
   endforeach

   return events;
endfunction

function UpdateItemEvent(combatevent, index := 0)
   var df := LoadItemEventData();
   if (!index)
      index := DFGetElemNames(df).size() +1;
   endif

   var elem := DFFindElementEx(df, index, DF_CREATE);
   elem.SetProp("name", combatevent.name);
   elem.SetProp("diff", combatevent.diff);
   elem.SetProp("desc", combatevent.desc);
   elem.SetProp("uso", combatevent.uso);
   elem.SetProp("skills", combatevent.skills);
   elem.SetProp("effects", combatevent.effects);
   elem.SetProp("icon", combatevent.icon);

   return index;
endfunction

function GetItemEvent(index := 0, elem := 0)
   var df := LoadItemEventData();

   if (!elem)
      elem := DFFindElementEx(df, index, 0);
      if (!elem) return; endif
   endif

   return struct{
      "index" := index,
      "name" := DFGetProp(elem, "name", DF_CREATE, "No Name"),
      "diff" := cint(DFGetProp(elem, "diff", DF_CREATE, 0)),
      "desc" := DFGetProp(elem, "desc", DF_CREATE, ""),
      "uso" := DFGetProp(elem, "uso", DF_CREATE, array{}),
      "skills" := DFGetProp(elem, "skills", DF_CREATE, array{}),
      "effects" := DFGetProp(elem, "effects", DF_CREATE, array{}),
      "icon" := DFGetProp(elem, "icon", DF_CREATE, 0)
   };
endfunction

function RemoveItemEvent(index)
   if (!index)
      return; 
   endif

   var df := LoadItemEventData();
   df.DeleteElement(index);
endfunction


function applyEffect(item, effects)
	var mods := GetObjProperty(item, "equip");
	if (!mods)
		mods := struct{};
	endif

   foreach event in effects
      if (event.type == "Hitscript")
         SetHitscript(item, event.hitopt);
      elseif (event.type == "Armorscript")
         SetArmorHitscript(item, event.hitopt);
      elseif (event.type == "Criticalscript")
         SetCriticalHitscript(item, event.hitopt);
      elseif (event.type == "Red. de Dano")
         if (!mods.dmg_reduce) mods.dmg_reduce := struct{}; endif
         mods.dmg_reduce[event.dmgtype] := cdbl(mods.dmg_reduce[event.dmgtype]) + (cdbl(event.amount) / 100);
      elseif (event.type == "Monstro +dmg")
         var dmg_increase := GetObjProperty(item, "dmg_increase");
         if (!dmg_increase) dmg_increase := struct{}; endif
         
         dmg_increase[event.property] := cint(dmg_increase[event.property]) + event.amount;
      elseif (event.type == "ObjProperty")
         var value := event.value;
         if (event.vartype == "Integer") 
            value := cint(value);
         elseif (event.vartype == "Float")
            value := cdbl(value);
         endif
         SetObjProperty(item, event.property, value);
      elseif (event.type == "Propriedade")
         item.Set_Member(event.property, item.get_member(event.property) + event.amount);
         if (event.property == "hp_max")
            item.hp += event.amount;
         endif
      elseif (event.type == "Tamanho +dmg")
         var dmg_increase := GetObjProperty(item, "dmg_increase");
         if (!dmg_increase) dmg_increase := struct{}; endif
         
         dmg_increase[event.property] := cint(dmg_increase[event.property]) + event.amount;
      elseif (event.type == "Alterar Equipmod")
         mods[event.vital] := cint(mods[event.vital]) + event.amount;
      elseif (event.type == "Alterar Regen")
         if (!mods.regen_rate) mods.regen_rate := struct{}; endif
         mods.regen_rate[event.vital] := cdbl(mods.regen_rate[event.vital]) + (event.amount*100);
      elseif (event.type == "Regen Moriquendi")
         SetObjProperty(item, "decrease_moriquendi", (cdbl(event.amount) / 100));
      elseif (event.type == "Efeito Ativável")
         SetObjProperty(item, "excep_index", event.index);
         SetObjProperty(item, "active_effect", event.effect);
         SetObjProperty(item, "charges", 10);
         item.script := ":artefacts:active";
      endif
      sleepms(2);
   endforeach

   if (mods.keys().size() >= 1)
      SetObjProperty(item, "equip", mods);
   endif
endfunction

function sendUserGump(who, index := 0)
   if (index) selected_item := index; endif
	var gump     := GFCreateGump();
	var allitems := FindDataFile();

	GFPage(gump, 0);
   GFSetId(gump, 0x7877);
	GFResizePic(gump, 16, 31, 9200, 190, 434);
	GFResizePic(gump, 209, 31, 9200, 466, 434);
	GFResizePic(gump, 217, 37, 9270, 447, 422);

	GFAddButton(gump, 81, 29-5, 5020, 5020, GF_CLOSE_BTN, 2);
	GFTextLine(gump, 89, 31-5, 2100, "Novo");

   GFPage(gump, 1);
   var pos_y := 40;
   foreach item in allitems
      var color := "#333333";
      if (item.index == selected_item)
         color := "#66666";
      endif
      GFHTMLArea(gump, 35, pos_y, 155, 27, "<basefont color={}>{}".format(color, item.name), 1, 0);
      if (item.index != selected_item)
         GFAddButton(gump, 22, pos_y+2, 5540, 5541, GF_CLOSE_BTN, 300+item.index);
      endif

      pos_y += 28;
      if (_item_iter % 14 == 0)
	      GFAddButton(gump, 110, pos_y+5, 5601, 5605, GF_PAGE_BTN, gump.cur_page+1);
         GFPage(gump, gump.cur_page+1);
      	GFAddButton(gump, 80, pos_y+5, 5603, 5607, GF_PAGE_BTN, gump.cur_page-1);
         pos_y := 40;
      endif
      sleepms(2);
   endforeach

	GFPage(gump, 0);

   if (selected_item)
      var iteminfo := GetItemEvent(selected_item);
      GFAddButton(gump, 137, 29-5, 5020, 5020, GF_CLOSE_BTN, 20);
      GFTextLine(gump, 145, 31-5, 2100, "Deletar");

      // GFHTMLArea(gump, 35, 88, 160, 27, "HtmlElement", 1, 0);
      GFTextLine(gump, 239, 50, 2100, "Nome:");
      GFTextLine(gump, 239+50, 50, 2000, "{}".format(iteminfo.name));
      GFAddButton(gump, 230, 45, 5020, 5020, GF_CLOSE_BTN, 21);
      
      GFTextLine(gump, 239, 50+18, 2100, "Dificuldade:");
      GFTextLine(gump, 239+80, 50+18, 2000, "{}".format(iteminfo.diff));
      GFAddButton(gump, 230, 45+18, 5020, 5020, GF_CLOSE_BTN, 22);

      GFTextLine(gump, 239, 50+36, 2100, "Descrição:");
      GFHTMLArea(gump, 239, 50+36+18, 400, 36+18, "{}".format(iteminfo.desc), 1, 0);
      GFAddButton(gump, 230, 45+36, 5020, 5020, GF_CLOSE_BTN, 23);
      
      GFTextLine(gump, 239, 200, 2100, "Equipamentos:");
      GFAddButton(gump, 230, 200, 5020, 5020, GF_CLOSE_BTN, 24);
      var y := 215;
      foreach it in (iteminfo.skills)
         GFTextLine(gump, 245, y, 2000, "{}".format(it));
         y += 18;
      endforeach

      foreach it in (iteminfo.uso)
         GFTextLine(gump, 245, y, 2000, "{}".format(it));
         y += 18;
      endforeach

      GFTextLine(gump, 440, 200, 2100, "Efeitos:");
      GFAddButton(gump, 440-5, 200, 5020, 5020, GF_CLOSE_BTN, 25);
      foreach it in (iteminfo.effects)
         var desc := "{}".format(it.type);
         if (it.vital) desc += " {}".format(it.vital); endif 
         if (it.property) desc += " {}".format(it.property);  endif
         if (it.hitopt) desc += " {}".format(it.hitopt);  endif
         if (it.dmgtype) desc += " {}".format(it.dmgtype);  endif
         if (it.amount)
            if (it.incr)
               desc += " -";
            else
               desc += " +";
            endif
            desc += "{}".format(it.amount); 
         endif
         GFTextLine(gump, 440+15, 205+(_it_iter * 20), 2000, "{}".format(desc));
	      GFAddButton(gump, 440, 208+(_it_iter * 20), 2103, 2104, GF_CLOSE_BTN, 100+_it_iter);
         sleepms(2);
      endforeach

      GFAddButton(gump, 472, 70, 1274, 1274, GF_CLOSE_BTN, 26);
      GFHTMLArea(gump, 470, 65, 129, 27, "<center>Aplicar em Item", 1, 0);

	   GFAddButton(gump, 404, 378, 2075, 248, GF_CLOSE_BTN, 29);
   endif

	var input := GFSendGump(who, gump);
   return HandleInput(who, input[0]);
endfunction

function HandleInput(who, input)
   var iteminfo := GetItemEvent(selected_item);
   if (input == 2)
      var text := GetInput(who, who.backpack, "Qual o nome?");
      if (text)
         var new_item := struct{ "name" := text };
         var index := UpdateItemEvent(new_item);
         selected_item := index;
      else
         SendSysMessageEx(who, "Cancelado");
      endif
   elseif (input == 25)
     var input_list := array{struct{
         "title" := "Qual efeito deseja adicionar?",
         "marked" := 1, 
         "radio" := array{
            "Hitscript",
            "Armorscript",
            "Criticalscript",
            "Alterar Equipmod",
            "Alterar Regen",
            "Efeito Ativável",
            // "Especial",
            "Monstro +dmg",
            "ObjProperty",
            "Propriedade",
            "Tamanho +dmg",
            "Red. de Dano",
            // "Regen Moriquendi"
            }
         }
      };
      var opt := QuestionsGump(who, input_list);
      if (opt)
         var event := ConfigureEvents(who, struct{ type := opt[1] });
         iteminfo.effects.append(event);
         UpdateItemEvent(iteminfo, selected_item);
      endif
   elseif (input == 20)
      if (YesNo(who, "Deseja mesmo deletar {} ?".format(iteminfo.name)))
         RemoveItemEvent(selected_item);
         selected_item := 0;
      endif
   elseif (input == 21)
      var text := GetInput(who, who.backpack, "Qual o novo nome? (aperte esc para cancelar)");
      if (text)
         iteminfo.name := text;
         UpdateItemEvent(iteminfo, selected_item);
      else
         SendSysMessageEx(who, "Cancelado");
      endif
   elseif (input == 22)
      var diffs := array{"0", "1", "2", "3", "4", "5", "6"}; 
      var questions := array{};
      questions.append(struct{ "title" := "Qual a dificuldade?", marked := cstr(iteminfo.diff) in diffs, "radio" := diffs});
      var output := QuestionsGump(who, questions);
      if (output)
         iteminfo.diff := cint(output[1]);
         UpdateItemEvent(iteminfo, selected_item);
      endif
   elseif (input == 23)
      var text := BigRequestGump(who, "Qual a nova descrição?");
      iteminfo.desc := text;
      UpdateItemEvent(iteminfo, selected_item);
   elseif (input == 24)
      ConfigureEquipments(who, iteminfo);
      UpdateItemEvent(iteminfo, selected_item);
   elseif (input == 26)
      return iteminfo;
   elseif (input >= 300 && input <= 500)
      selected_item := input - 300;
   elseif (input >= 100 && input <= 150)
      var pos := input -100;
      if (YesNo(who, "O que deseja fazer com {} ?".format(iteminfo.effects[pos].type), "Editar", "Remover"))
         iteminfo.effects[pos] := ConfigureEvents(who, iteminfo.effects[pos]);
         UpdateItemEvent(iteminfo, selected_item);
      else
         iteminfo.effects.Erase(pos);
         UpdateItemEvent(iteminfo, selected_item);
      endif
   else
      return;
   endif

   return sendUserGump(who);
endfunction

function ConfigureEquipments(who, byref iteminfo)
   var skills := array{"Marcenaria", "Costura", "Ferraria"};
   var item_types := array{"Armaduras", "Arcos", "Armas", "Armas Mágicas", "Joias", "Containers", "Elmos/Mascaras"}; 

   var marked_skills := iteminfo.skills;
   var marked_types := iteminfo.uso;

   foreach it in (marked_skills)
      it := it in skills;
   endforeach

   foreach it in (marked_types)
      it := it in item_types;
   endforeach

   var questions := array{};
   questions.append(struct{ "title" := "Quais craftings podem ser usadas?", marked := marked_skills, "checkbox" := skills});
   questions.append(struct{ "title" := "Quais tipos de item podem ser usados?", marked := marked_types, "checkbox" := item_types} );
   var output := QuestionsGump(who, questions);
   if (output)
      iteminfo.skills := output[1];
      iteminfo.uso := output[2];
   endif
endfunction

function ConfigureEvents(who, event)
   if (event.type == "Hitscript")
      var hits_list := GetHitScriptsName();
      var input_list := array{
         struct{ "title" := "Escolha o Hitscript", "marked" := event.hitopt in hits_list, "radio" := hits_list }
      };
      var output := QuestionsGump(who, input_list, HIDDEN_CANCEL);
      event.hitopt := GetHitScript(StrReplace(output[1], " ", "_"));
   elseif (event.type == "Armorscript")
      var hits_list := GetHitScriptsName("ArmorHits");
      var input_list := array{
         struct{ "title" := "Escolha o Armorscript", "marked" := event.hitopt in hits_list, "radio" := hits_list }
      };
      var output := QuestionsGump(who, input_list, HIDDEN_CANCEL);
      event.hitopt := GetHitScript(StrReplace(output[1], " ", "_"));
   elseif (event.type == "Criticalscript")
      var hits_list := GetHitScriptsName();
      var input_list := array{
         struct{ "title" := "Escolha o Criticalscript", "marked" := event.hitopt in hits_list, "radio" := hits_list }
      };
      var output := QuestionsGump(who, input_list, HIDDEN_CANCEL);
      event.hitopt := GetHitScript(StrReplace(output[1], " ", "_"));
   elseif (event.type == "Red. de Dano")
      var dmgs := array{DMG_PHYSICAL, DMG_FIRE, DMG_COLD, DMG_POISON,DMG_ENERGY,DMG_PHYSICAL,DMG_PHYSICAL,DMG_PHYSICAL, "Sem Resistência" };
      var input_list := array{};
      input_list.append(struct{ "title" := "Qual tipo de dano será reduzido?", "marked" := event.dmgtype in dmgs, "radio" := dmgs});
      if (!event.amount) event.amount := 10; endif
      input_list.append(struct{ "title" := "Qual a porcentagem?", "marked" := ""+cint(event.amount) });
      var output := QuestionsGump(who, input_list, HIDDEN_CANCEL);

      event.dmgtype := output[1];
      event.amount := cint(output[2]);
   elseif (event.type == "Monstro +dmg")
      var input_list := array{};
      if (!event.property) event.property := ""; endif
      if (!event.amount) event.amount := 1; endif
      input_list.append(struct{ "title" := "Qual categoria de monstro?", "marked" := ""+event.property});
      input_list.append(struct{ "title" := "Quanto de dano extra?", "marked" := ""+cint(event.amount) });
      var output := QuestionsGump(who, input_list, HIDDEN_CANCEL);

      event.property := output[1];
      event.amount := cint(output[2]);
   elseif (event.type == "ObjProperty")
      var var_types := array{"Integer", "Float", "String"};
      var input_list := array{};
      if (!event.property) event.property := ""; endif
      if (!event.value) event.value := ""; endif
      if (!event.vartype) event.vartype := 3; endif
      input_list.append(struct{ "title" := "Qual chave da propriedade?", "marked" := ""+event.property});
      input_list.append(struct{ "title" := "Qual tipo da propriedade?", "marked" := event.vartype in var_types, radio := var_types });
      input_list.append(struct{ "title" := "Qual valor da propriedade?", "marked" := ""+cint(event.value) });
      var output := QuestionsGump(who, input_list, HIDDEN_CANCEL);

      event.property := output[1];
      event.vartype := output[2];
      event.value := output[3];
   elseif (event.type == "Propriedade")
      var prop_types := array{"hp_max", "speed_mod", "ar_mod", "color", "dmg_mod", "attribute", "name_suffix", "insured", "newbie"};
      var input_list := array{};
      if (!event.amount) event.amount := 3; endif
      input_list.append(struct{ "title" := "Qual propriedade?", "marked" := event.property in prop_types, radio := prop_types });
      input_list.append(struct{ "title" := "Qual valor da propriedade?", "marked" := ""+cint(event.amount) });
      var output := QuestionsGump(who, input_list, HIDDEN_CANCEL);

      event.property := output[1];
      event.amount := cint(output[2]);
   elseif (event.type == "Tamanho +dmg")
      var size_list := array{"pequeno", "medio", "grande", "gigante"};
      var input_list := array{};
      if (!event.amount) event.amount := "1"; endif
      input_list.append(struct{ "title" := "Qual tamanho?", "marked" := event.property in size_list, radio := size_list });
      input_list.append(struct{ "title" := "Quanto de dano extra?", "marked" := ""+cint(event.amount) });
      var output := QuestionsGump(who, input_list, HIDDEN_CANCEL);

      event.property := output[1];
      event.amount := cint(output[2]);
   elseif (event.type == "Alterar Equipmod")
      var vital_list := array{"hitsmod", "manamod", "stammod", "toughmod", "castmod"};
      var input_list := array{};
      if (!event.amount) event.amount := "1"; endif
      input_list.append(struct{ "title" := "Qual equipmod será alterado?", "marked" := event.vital in vital_list, radio := vital_list });
      input_list.append(struct{ "title" := "Qual o modificador?", "marked" := ""+cint(event.amount) });
      var output := QuestionsGump(who, input_list, HIDDEN_CANCEL);

      event.vital := output[1];
      event.amount := cint(output[2]);
   elseif (event.type == "Alterar Regen")
      var vital_list := array{"hits", "stamina", "mana", "dp", "hunger"};
      var input_list := array{};
      if (!event.amount) event.amount := "1"; endif
      input_list.append(struct{ "title" := "Qual vital a regeneração será alterado?", "marked" := event.vital in vital_list, radio := vital_list });
      input_list.append(struct{ "title" := "Qual o modificador? (100 = 10 pontos p/ segundo)", "marked" := ""+cint(event.amount) });
      var output := QuestionsGump(who, input_list, HIDDEN_CANCEL);

      event.vital := output[1];
      event.amount := cint(output[2]);
   // elseif (event.type == "Especial")
   //    var special_list := array{"Casttime"};
   //    var incr_list := array{"Incrementar", "Reduzir"};
   //    var input_list := array{};
   //    if (!event.amount) event.amount := "10"; endif
   //    if (!event.incr) event.incr := 2; endif
   //    input_list.append(struct{ "title" := "Qual atributo especial deseja alterar?", "marked" := event.special in special_list, radio := special_list });
   //    input_list.append(struct{ "title" := "Incrementar ou Reduzir?", "marked" := 1+event.incr+(cint(event.incr)), radio := incr_list  });
   //    input_list.append(struct{ "title" := "Qual a porcentagem/valor?", "marked" := ""+cint(event.amount) });
   //    var output := QuestionsGump(who, input_list, HIDDEN_CANCEL);

   //    event.special := output[1];
   //    event.incr := output[2] == "Reduzir";
   //    event.amount := cint(output[2]);
   elseif (event.type == "Efeito Ativável")
      if (event.effect)
         event.effect := ConfigureCombatEvent(who, event.effect, 1);
      else
         event.effect := ConfigureCombatEvent(who);
      endif
   endif

   return event;
endfunction