
include ":tn:tngumps";

function LoadCombatEventData()
    var df := DFOpenDataFile( ":yggdrasil:combatevents", DF_CREATE, DF_KEYTYPE_INTEGER);
    return df;
endfunction

function GetAllVents(filter := 0)
   var df := LoadCombatEventData();
   var events := array{};
   foreach event in (DFGetElemNames(df))
      var df_event := DFFindElementEx(df, event);
      var monsters := DFGetProp(df_event, "monsters", DF_CREATE, "");
      if (!filter || filter[monsters])
         events.append(struct{
            "number" := event,
            "name" := DFGetProp(df_event, "name", DF_CREATE, "No Name"),
            "cd" := DFGetProp(df_event, "cd", DF_CREATE, 0),
            "dist" := DFGetProp(df_event, "dist", DF_CREATE, 0),
            "chance" := DFGetProp(df_event, "chance", DF_CREATE, 0),
            "icon" := DFGetProp(df_event, "icon", DF_CREATE, 0),
            "description" := DFGetProp(df_event, "description", DF_CREATE, ""),
            "hp_percent" := DFGetProp(df_event, "hp_percent", DF_CREATE, 0),
            "combatevent" := DFGetProp(df_event, "combatevent", DF_CREATE, array{}),
            monsters := monsters
         });
      endif
      sleepms(2);
   endforeach

   return events;
endfunction

function UpdateCombatEvent(combatevent, index := 0)
   var df := LoadCombatEventData();
   if (!index)
      index := DFGetElemNames(df).size() +1;
   endif

   var elem := DFFindElementEx(df, index, DF_CREATE);
   elem.SetProp("name", combatevent.name);
   elem.SetProp("cd", combatevent.cd);
   elem.SetProp("dist", combatevent.dist);
   elem.SetProp("chance", combatevent.chance);
   elem.SetProp("icon", combatevent.icon);
   elem.SetProp("hp_percent", combatevent.hp_percent);
   elem.SetProp("monsters", combatevent.monsters);
   elem.SetProp("description", combatevent.description);
   elem.SetProp("combatevent", combatevent.combatevent);
endfunction

function ConfigureCombatEvent(who, event := 0, change_event := 0)
	if (!event || change_event == 1)
      if (!event)
         event := struct{
            name := "",
            cd := 10,
            dist := 0,
            chance := 100,
            hp_percent := 100,
            steps := array{}
         };
      endif
      var input_list := array{};
      input_list.append(struct{ "title" := "Digite o nome desse evento de combate?. Ex: Ataque Multiplo", "marked" := event.name});
      input_list.append(struct{ "title" := "De quanto em quanto tempo o npc realiza esse evento? (segundos)", "marked" := ""+event.cd});
      input_list.append(struct{ "title" := "Qual a distancia que o npc precisa estar do alvo? (0 para não checar)", "marked" := ""+event.dist});
      input_list.append(struct{ "title" := "Qual a chance do npc realizar esta ação? (100 para sempre)", "marked" := ""+event.chance});
      input_list.append(struct{ "title" := "Quanto % de hp o npc deve ter para realizar este evento? (100 para sempre)", "marked" := ""+event.hp_percent});
      input_list.append(struct{ "title" := "Icone para posses (opcional)", "marked" := ""+cint(event.icon)});

      var output := QuestionsGump(who, input_list);

		event.name := output[1];
		event.cd := cint(output[2]);
		event.onlybloodied := 0;
		event.dist := cint(output[3]);
		event.chance := cint(output[4]);
		event.hp_percent := cint(output[5]);
		event.icon := cint(output[6]);
	endif
	var continua := 1;

	while (continua)
      var gumpcombat := GFCreateGump();

      GFDisposable(gumpcombat, 0);
      GFResizePic(gumpcombat, 0, 25, 3700, 250, 430);	
      GFResizePic(gumpcombat, 15, 40, 0x13BE, 220, 400);
      GFTextMid(gumpcombat, 20, 50, 250, 1051, "Combat Events");
      
      var y := 64;
      GFTextLine(gumpcombat, 35, y, 1051, "Adicionar Novo ");
      GFAddButton(gumpcombat, 20, y+2, 2117, 2118, 1, 100);
      
      var i := 1;
      foreach state in (event.steps)
         y := y+ 15;
         GFTextLine(gumpcombat, 35, y, 1051, ""+state.type);
         GFAddButton(gumpcombat, 20, y+2, 2117, 2118, 1, 400+i);
         i := i + 1;
      endforeach

      GFAddButton(gumpcombat, 180, 387, 4502, 4502, 1, 1000 );
      GFTextLine(gumpcombat, 140, 400, 1051, "Aplicar");
      
      var input := GFSendGump(who, gumpcombat);

      if (input[0] == 100)
         // var choice := RadioGump(who, 0, 0, "O que deseja fazer?", array{"Novo Evento", "Copiar de um Boss","Copiar da lista", "Cancelar"}, 0, 1);
         // if (choice["Novo"])
         var state := CombatEventsOptions(who, struct{});
         event.steps.append(state);
         // elseif (choice["Boss"])
         // elseif (choice["lista"])
         // endif
      elseif (input[0] == 1000)
         continua := 0;
      elseif (input[0] >= 400)
         input := input[0] - 400;
         var oldstate := event.steps[input];
         var choice := RadioGump(who, 0, 0, "O que deseja fazer?", array{"Alterar", "Mudar Posição","Remover", "Cancelar"}, 0, 4);
         if (choice == "Alterar")
            var state := CombatEventsOptions(who, oldstate);
            event.steps.Erase(input);
            event.steps.Insert(input, state);
         elseif (choice["Mudar Posi"])
            var position := cint(RequestGump(who, "Qual posição??", "Ex: 1", "1", 1));
            if (position == 0)
               position := 1;
            elseif (position > event.steps.size())
               position := event.steps.size();
            endif
            event.steps.Erase(input);
            event.steps.Insert(position, oldstate);
         elseif(choice["Remover"])
            event.steps.Erase(input);
         endif
      endif
	endwhile

	return event;
endfunction

function CombatEventsOptions(who, state)
   var opt;
   if (!state.type)
      var input_list := array{struct{
         "title" := "Novo estado:",
         "marked" := 1, 
         "radio" := array{
            "Move", 
            "Teleport", 
            "TimedScript", 
            "Play Cast Anim", 
            "Pushback", 
            "Perform Action", 
            "Text", 
            "Play Sound", 
            "Heal", 
            "Dmg", 
            "Graphic Effect", 
            //  "Create Items", 
            "Summon Npcs", 
            // "Custom HitScript", 
            "Set Cooldown", 
            "Create Mine",
            "Spell Trilha",
            "Delay",
            "Stop Monster",
            "Hidden",
            "Steal Player",
            "Metamorfose",
            "Magicball"
            }
         }
      };
      opt := QuestionsGump(who, input_list);
      opt := opt[1];
      state.type := opt;
   else
      opt := state.type;
   endif

   var targ_opts := array{"Self", "Cone in front", "Around Self", "Opponent", "Around Opponent", "All Opponents", "Random Opponent", "In Line"};
   if (opt == "Move")
      SendSysMessageEx(who, "Para onde o npc deve andar?");
      var loc := TargetCoordinates(who);
      state.x := loc.x;
      state.y := loc.y;
      state.z := loc.z;
   elseif ( opt == "Pushback")
      var input_list := array{};
      input_list.append(struct{ "title" := "Quem vai sofrer a pushback?", "marked" := state.target in targ_opts, "radio" := targ_opts});
      input_list.append(struct{ "title" := "Qual o comportamento?", "marked" := "", "radio" := array{"Para frente", "Para trás", "Para frente relacional", "Direção do Monstro"}});
      input_list.append(struct{ "title" := "Quantas vezes será aplicado?", "marked" := ""+cint(state.times)});
      var output := QuestionsGump(who, input_list);

      state.target := output[1];
      state.behaviour := output[2];
      state.times := cint(output[3]);
   elseif ( opt == "Teleport")
      var input_list := array{};
      input_list.append(struct{ "title" := "Quem vai sofrer a teleporte?", "marked" := state.target in targ_opts, "radio" := targ_opts});
      input_list.append(struct{ "title" := "Escolha o comportamento", "marked" := 1, "radio" := array{"Teleportar X,Y,Z", "Teleportar no Alvo (DO NPC)", "Teleportar no NPC", "Aplicar +-X,Y,Z"}});
      input_list.append(struct{ "title" := "Qual o X Y Z? (Apenas para primeira e ultima opção)", "marked" := ""});
      var output := QuestionsGump(who, input_list);
      
      state.target := output[1];
      state.behaviour := output[2];
      if (output[2]["X,Y,Z"])
         var loc := SplitWords(output[3]);
         state.x := cint(loc[1]);
         state.y := cint(loc[2]);
         state.z := cint(loc[3]);
      endif
   elseif ( opt == "TimedScript")
      var input_list := array{};
      input_list.append(struct{ "title" := "Quem vai sofrer a action?", "marked" := state.target in targ_opts, "radio" := targ_opts});
      var timedcfg := TS_GetCfgFile();
      input_list.append(struct{ "title" := "Escolha o timedscript", "marked" := 1, "radio" := GetConfigStringKeys(timedcfg)});
      input_list.append(struct{ "title" := "Qual a duração do timer? [até 120s]", "marked" := ""+cint(state.timerduration)});
      input_list.append(struct{ "title" := "Qual o level do timer?", "marked" := ""+cint(state.level)});
      input_list.append(struct{ "title" := "Teste de resistência?", "checked" := 1, "radio" :=  array{STRENGTH, INTELLIGENCE, DEXTERITY, CONSTITUTION, WISDOM, "Nenhum"} });
      input_list.append(struct{ "title" := "Dificuldade do teste?", "marked" := "12"});
      input_list.append(struct{ "title" := "Distância do cone (cone in front only)?", "marked" := "3"});

      var output := QuestionsGump(who, input_list);
      
      state.target        := output[1];
      state.timername     := output[2];
      state.timerduration := cint(output[3]);
      state.level         := cint(output[4]);
      state.conedist      := cint(output[6]);

      if (output[5] != "Nenhum")
         state.resist_test := output[5];
         state.resist_diff := cint(output[6]);
      endif
   elseif ( opt == "Perform Action" )
      var input_list := array{}; 
      input_list.append(struct{ "title" := "Quem vai realizar a action?", "marked" := state.target in targ_opts, "radio" := targ_opts});
      input_list.append(struct{ "title" := "Qual o ID da animação?", "marked" := ""+cint(state.id)});
      var output := QuestionsGump(who, input_list);

      state.target := output[1];
      state.id := cint(output[2]);
   elseif (opt == "Text")
      var input_list := array{};
      if (!state.color) state.color := 1000; endif
      if (!state.text) state.text := ""; endif
      input_list.append(struct{ "title" := "Quem vai falar?", "marked" := state.target in targ_opts, "radio" := targ_opts});
      input_list.append(struct{ "title" := "Qual o texto?", "marked" := ""+cstr(state.text)});
      input_list.append(struct{ "title" := "Qual a cor do texto?", "marked" := ""+cint(state.color)});
      var output := QuestionsGump(who, input_list);

      state.target := output[1];
      state.text := output[2];
      state.color := cint(output[3]);
   elseif (opt == "Play Sound")
      state.id := cint(RequestGump(who, "Qual o ID do som?", "", ""+cint(state.id), 1));
   elseif (opt == "Delay")
      state.delay := cint(RequestGump(who, "Quanto tempo de delay?", "", ""+cint(state.delay), 1));
   elseif (opt == "Heal")
      state.healamt := cint(RequestGump(who, "Quanto vai healar?", "", ""+cint(state.healamt), 1));
   elseif (opt == "Graphic Effect")
      var effect_opts := array{"Lightning", "Red Sparkles", "Blue Sparkles", "Blue and Swirl", "Swirl Sparkles", "Blue Sparkles on Ground", "Fire Column", "Small Fireball", "Large Fireball", "Wavy Fireball", "Outro"};
      
      var input_list := array{};
      input_list.append(struct{ "title" := "Onde vai surgir o efeito?", "marked" := state.target in targ_opts, "radio" := targ_opts + array{"Fixed Loc"}});
      input_list.append(struct{ "title" := "Qual o tipo de efeito? (fixed loc deve ser Moving)", "marked" := 1+(state.graphictype == "Moving"), "radio" := array{"Stationary", "Moving"}});
      input_list.append(struct{ "title" := "Qual o efeito?", "marked" := 11, "radio" := effect_opts });
      input_list.append(struct{ "title" := "Qual o ID do efeito? (Outro)", "marked" := ""+cint(state.fxid)});
      input_list.append(struct{ "title" := "Qual a direção? (Moving, ignore for Cone)", "marked" := (state.direction == "Mob->Player")+1, "radio" := array{"Mob->Player", "Player->Mob"}});
      input_list.append(struct{ "title" := "Digite o x y z onde o efeito vai aparecer: (stationary, ignore for Moving/cone)", "marked" := "{} {} {}".format(cint(state.startx), cint(state.starty), cint(state.startz))});
      input_list.append(struct{ "title" := "Digite o x y z da posição final do efeito: (stationary, ignore for Moving/cone)", "marked" := "{} {} {}".format(cint(state.endx), cint(state.endy), cint(state.endz))});
      input_list.append(struct{ "title" := "Qual a cor do efeito?", "marked" := ""+cint(state.color)});
      input_list.append(struct{ "title" := "Digite a distância (somente Cone in Front/In Line)", "marked" := ""+cint(state.conedist)});
      var output := QuestionsGump(who, input_list);


      state.target := output[1];
      state.conedist := cint(output[9]);
      state.color := cint(output[8]);
      state.graphictype := output[2];
      if ( output[3] == "Lightning" )
         state.fxid := "Lightning";
      elseif ( output[3] == "Red Sparkles")
         state.fxid := GFX_RED_SPARKLES;
         state.speed := 7;
         state.loops := 7;
      elseif ( output[3] == "Blue Sparkles")
         state.fxid := GFX_BLUE_SPARKLES;
         state.speed := 10;
         state.loops := 10;
      elseif (output[3] == "Blue and Swirl")
         state.fxid := GFX_BLUE_SPARKLE_SWIRL;
         state.speed := 7;
         state.loops := 16;
      elseif (output[3] == "Swirl Sparkles")
         state.fxid := GFX_BLUE_SPARKLE_BORDER;
         state.speed := 7;
         state.loops := 0;
      elseif (output[3] == "Blue Sparkles on Ground")
         state.fxid := GFX_BLUE_SPARKLE_GROUND;
         state.speed := 7;
         state.loops := 10;
      elseif (output[3] == "Fire Column");
         state.fxid := GFX_FIRE_COLUMN;
         state.speed := 7;
         state.loops := 0;
      elseif (output[3] == "Small Fireball")
         state.fxid := GFX_SMALL_FIREBALL;
         state.speed := 5;
         state.loops := 0;
      elseif (output[3] == "Large Fireball")
         state.fxid := GFX_LARGE_FIREBALL;
         state.speed := 5;
         state.loops := 0;
      elseif (output[3] == "Wavy Fireball")
         state.fxid := GFX_WAVY_FIREBALL;
         state.speed := 5;
         state.loops := 0;
      elseif (output[3] == "Outro")
         state.fxid := output[4];
         if (state.graphictype == "Stationary")
            state.speed := 7;
            state.loops := 16;
         else
            state.speed := 5;
            state.loops := 0;
         endif
      endif

      state.direction := output[5];

      if (state.target == "Fixed Loc")
         var loc := splitwords(output[6]);
         state.startx :=  loc[1];
         state.starty :=  loc[2];
         state.startz :=  loc[3];
         state.startradius := 0;

         if (state.graphictype == "Moving")
            var loc2 := splitwords(output[7]);
            state.endx :=  loc2[1];
            state.endy :=  loc2[2];
            state.endz :=  loc2[3];
            // var radius :=   RequestGump(who, "Qual o raio de onde o efeito vai terminar:", "0 se for sempre no mesmo lugar", "0", 1);
            state.endradius := 0;
         endif
      endif
   elseif ( opt == "Dmg")
      var input_list := array{};
      input_list.append(struct{ "title" := "Quem vai sofrer?", "marked" := state.target in targ_opts, "radio" := targ_opts});
      input_list.append(struct{ "title" := "Quanto de dano vai causar? Ex: 1d2", "marked" := "1d2"});
      input_list.append(struct{ "title" := "Qual o tipo de dano?", "marked" := 1, "radio" := array{DMG_PHYSICAL, DMG_FIRE, DMG_COLD, DMG_POISON,DMG_ENERGY,DMG_PIERCING,DMG_SLASH,DMG_BLUDGE, "Sem Resistência" }});
      input_list.append(struct{ "title" := "Qual tipo de vital sera afetado pelo dano?", "marked" := "", "checkbox" := array{"Stam", "Hits", "Mana"}});
      input_list.append(struct{ "title" := "Teste de resistência?", "checked" := 6, "radio" :=  array{STRENGTH, INTELLIGENCE, DEXTERITY, CONSTITUTION, WISDOM, "Nenhum"} });
      input_list.append(struct{ "title" := "Qual a dificuldade do teste?", "marked" := "14"});
      input_list.append(struct{ "title" := "Digite a distância (somente Cone in Front/In Line)", "marked" := "3"});
      var output := QuestionsGump(who, input_list);

      state.target := output[1];
      state.dmg := output[2];
      state.dmgtype := output[3];
      state.conedist := cint(output[7]);
      if (state.dmgtype == "Sem Resistência")
         state.dmgtype := -1;
      endif
      state.vitals := output[4];
      if (output[5] != "Nenhum")
         state.resist_test := output[5];
         state.resist_diff := cint(output[6]);
      endif
   elseif ( opt == "Create Items")
      SendSysMessageEx(who, "Não implementado");
   elseif (opt == "Summon Npcs")
      var input_list := array{};
      input_list.append(struct{ "title" := "Qual o npctemplate?", "marked" := ":ghaia:"});
      input_list.append(struct{ "title" := "Quantos npcs vao ser spawnados?", "marked" := "2"});
      input_list.append(struct{ "title" := "Qual o raio de spawn?", "marked" := "5"});
      input_list.append(struct{ "title" := "Qual o tempo de vida dos monstros?", "marked" := "120"});
      input_list.append(struct{ "title" := "Digite o x y z onde eles vao aparecer: (6 6 6 para onde o boss esta)", "marked" := "6 6 6"});
      input_list.append(struct{ "title" := "Digite o x y z para onde eles vão andar: (6 6 6 para onde o boss esta)", "marked" := "6 6 6"});
      
      var output := QuestionsGump(who, input_list);
      state.npctemplate := output[1];
      state.amount := output[2];
      state.range := cint(output[3]);
      state.tempo := cint(output[4]);
      
      var loc := output[5];
      if (!loc)
         SendSysMessageEx(who, "Onde vão ser spawnados? (Esc para mesmo local do boss)");
         loc := TargetCoordinates(who);
         if (!loc)
            loc := array{6, 6, 6};
         else
            loc := array{loc.x, loc.y, loc.z};
         endif
      else
         loc := splitwords(loc);
      endif
      
      state.x := cint(loc[1]);
      state.y := cint(loc[2]);
      state.z := cint(loc[3]);

      var walk_to := SplitWords(output[5]);
      if (walk_to.size() == 3)
         state.walk_to := walk_to;
      endif
      
   elseif (opt == "Custom HitScript")
      SendSysMessage(who, "Não implementado");
   elseif (opt == "Set Cooldown")
      var input_list := array{};
      input_list.append(struct{ "title" := "Qual o cooldown?", "marked" := "lockaggro" });
      input_list.append(struct{ "title" := "Quanto tempo vai durar?", "marked" := "10"});
      var output := QuestionsGump(who, input_list);
      state.prop := output[1];
      state.time := cint(output[2]);
   elseif (opt == "Create Mine")
      var dmg := "1d5+2";
      var resist := 12;
      if (state.dmg) dmg := state.dmg; endif
      if (state.diff) resist := state.diff; endif

      var targ_types := array{"Random", "Frente", "Costas"};

      var input_list := array{};
      input_list.append(struct{ "title" := "Qual o alvo?", "marked" := state.target in targ_types, radio := targ_types });
      input_list.append(struct{ "title" := "Quantas minas?", "marked" := ""+cint(state.amount)});
      input_list.append(struct{ "title" := "Qual tipo da mina?", "marked" := 1+(state.mine_type == "Gelo"), radio := array{ "Fogo", "Gelo" }});
      input_list.append(struct{ "title" := "Qual dificuldade de resistência?", "marked" := ""+resist});
      input_list.append(struct{ "title" := "Qual o dano?", "marked" := dmg});
      var output := QuestionsGump(who, input_list);
      state.target := output[1];
      state.amount := cint(output[2]);
      state.mine_type := output[3];
      state.diff := cint(output[4]);
      state.dmg := output[5];
   elseif (opt == "Hidden")
      var stealthsteps := 15;
      if (state.stealthsteps) stealthsteps := state.stealthsteps; endif 
      var input_list := array{};
      input_list.append(struct{ "title" := "Quantos passos?", "marked" := ""+stealthsteps });
      input_list.append(struct{ "title" := "Ataca sem sair do modo furtivo?", "marked" := 1+cint(state.attackhidden), radio := array{"Não", "Sim"} });
      input_list.append(struct{ "title" := "Fugir do alvo enquanto furtivo?", "marked" := 1+cint(state.flee), radio := array{"Não", "Sim"} });
      var output := QuestionsGump(who, input_list);
      state.stealthsteps := cint(output[1]);
      state.attackhidden := output[2] == "Sim";
      state.flee := output[3] == "Sim";
   elseif (opt == "Spell Trilha")
      var dmg := "1d5+2";
      var resist := 12;
      var duration := 10;
      if (state.dmg) dmg := state.dmg; endif
      if (state.diff) resist := state.diff; endif
      if (state.duration) duration := state.duration; endif

      var trilha_types := array{"Fogo", "Veneno", "Eletrecidade"};

      var input_list := array{};
      input_list.append(struct{ "title" := "Qual o alvo?", "marked" := state.target in targ_opts, "radio" := targ_opts});
      input_list.append(struct{ "title" := "Qual tipo de trilha?", "marked" := state.trilha_type in trilha_types, radio := trilha_types });
      input_list.append(struct{ "title" := "Qual dado de dano?", "marked" := dmg });
      input_list.append(struct{ "title" := "Qual a dificuldade para esquivar?", "marked" := resist });
      input_list.append(struct{ "title" := "Qual o tempo de duração da mina?", "marked" := duration });
      var output := QuestionsGump(who, input_list);

      state.target      := output[1];
      state.trilha_type := output[2];
      state.dice_dmg    := output[3];
      state.diff        := cint(output[4]);
      state.duration    := cint(output[5]);
   elseif (opt == "Metamorfose")
      var metamorf := ":ghaia:";
      if (state.monsters) metamorf := state.monsters; endif
      var input_list := array{};
      input_list.append(struct{ "title" := "Para qual npctemplate? (separar por espaço para mais opções)", "marked" := metamorf });
      var output := QuestionsGump(who, input_list);

      state.monsters := Splitwords(output[1]);
   elseif (opt == "Magicball")
      var dmg_dice := "1d5+2";
      var resist := 12;
      var duration := 6;
      if (state.dmg_dice) dmg_dice := state.dmg_dice; endif
      if (state.diff) resist := state.diff; endif
      if (state.duration) duration := state.duration; endif

      var input_list := array{};
      input_list.append(struct{ "title" := "Qual o alvo?", "marked" := state.target in targ_opts, "radio" := targ_opts});
      input_list.append(struct{ "title" := "Criar em +X +Y +Z? (vazio para lançar do npc)", marked := "" });
      input_list.append(struct{ "title" := "Enviar na direção ou seguir target?", "marked" := 1+cint(state.to_where == "Target"), radio := array{"Local", "Target"} });
      input_list.append(struct{ "title" := "Qual type?", "marked" := 1+cint(state.ball_type), radio := array{"0 - Single DMG", "1 - Area DMG", "2 - Token Balls"} });
      input_list.append(struct{ "title" := "Qual duração?", "marked" := duration });
      input_list.append(struct{ "title" := "Qual dado de dano?", "marked" := dmg_dice });
      input_list.append(struct{ "title" := "Qual o tipo de dano?", "marked" := 1, "radio" := array{DMG_PHYSICAL, DMG_FIRE, DMG_COLD, DMG_POISON,DMG_ENERGY,DMG_PIERCING,DMG_SLASH,DMG_BLUDGE, "Sem Resistência" }});
      input_list.append(struct{ "title" := "Qual gráfico de ball?", "marked" := ""+cint(state.graphic) });
      input_list.append(struct{ "title" := "Qual cor do ball?", "marked" := ""+cint(state.color) });
      input_list.append(struct{ "title" := "Qual o som de impacto?", "marked" := ""+cint(state.sound) });
      input_list.append(struct{ "title" := "Qual gráfico de sub ball? (type 3)", "marked" := ""+cint(state.subgraphic) });
      input_list.append(struct{ "title" := "Qual cor do sub ball? (type 3)", "marked" := ""+cint(state.subcolor) });
      var output := QuestionsGump(who, input_list);

      state.target     := output[1];
      state.create_at  := output[2];
      state.to_where   := output[3];
      state.ball_type  := cint(SplitWords(output[4])[1]);
      state.duration   := cint(output[5]);
      state.dmg_dice   := output[6];
      state.dmg_type   := output[7];
      state.graphic    := cint(output[8]);
      state.color      := cint(output[9]);
      state.sound      := cint(output[10]);
      state.subgraphic := cint(output[11]);
      state.subcolor   := cint(output[12]);
   endif
   return state;
endfunction


function DFFindElementEx(byref file_ref, elem_name, create:=DF_NO_CREATE)
	var temp := file_ref.FindElement(elem_name);
	if ( !temp && create )
		DF_OutPut("Debug::DFFindElement() - Creating elem: "+CStr(elem_name));
		file_ref.CreateElement(elem_name);
		temp := file_ref.FindElement(elem_name);
	endif

	if ( temp )
		return temp;
	elseif (create)
		var errmsg := error{"errortext":="Error::DFFindElement() - Could not open data elem ["+elem_name+"] - "+temp.errortext};
		DF_OutPut(errmsg.errortext);
		return errmsg;
	endif
endfunction