
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

      var output := QuestionsGump(who, input_list);

		event.name := output[1];
		event.cd := cint(output[2]);
		event.onlybloodied := 0;
		event.dist := cint(output[3]);
		event.chance := cint(output[4]);
		event.hp_percent := cint(output[5]);
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
      GFTextLine(gumpcombat, 140, 400, 1051, "Apply");
      
      var input := GFSendGump(who, gumpcombat);

      if (input[0] == 100)
         var state := CombatEventsOptions(who, struct{});
         event.steps.append(state);
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
            "Magicball",
            }
         }
      };
      opt := QuestionsGump(who, input_list);
      state.type := opt[1];
   else
      opt := state.type;
   endif

   var targ_opts := array{"Self", "Cone in front", "Around Self", "Opponent", "Around Opponent", "All Opponents", "Random Opponent", "In Line"};
   if (opt == "Move")
      SendSysMessageEx(who, "Para onde o npc deve andar?");
      var loc := TargetCoordinates(who);
      state.+x := loc.x;
      state.+y := loc.y;
      state.+z := loc.z;
   elseif ( opt == "Pushback")
      var input_list := array{};
      input_list.append(struct{ "title" := "Quem vai sofrer a pushback?", "marked" := 1, "radio" := targ_opts});
      input_list.append(struct{ "title" := "Qual o comportamento?", "marked" := "", "radio" := array{"Para frente", "Para trás", "Para frente relacional", "Direção do Monstro"}});
      input_list.append(struct{ "title" := "Quantas vezes será aplicado?", "marked" := "1"});
      var output := QuestionsGump(who, input_list);

      state.target := output[1];
      state.behaviour := output[2];
      state.times := cint(output[3]);
   elseif ( opt == "Teleport")
      var input_list := array{};
      input_list.append(struct{ "title" := "Quem vai sofrer a teleporte?", "marked" := 1, "radio" := targ_opts});
      input_list.append(struct{ "title" := "Escolha o comportamento", "marked" := 1, "radio" := array{"Teleportar X,Y,Z", "Teleportar no Alvo (DO NPC)", "Teleportar no NPC", "Aplicar +-X,Y,Z"}});
      input_list.append(struct{ "title" := "Qual o X Y Z? (Apenas para primeira e ultima opção)", "marked" := ""});
      var output := QuestionsGump(who, input_list);
      
      state.target := output[1];
      state.behaviour := output[2];
      if (output[2]["X,Y,Z"])
         var loc := SplitWords(output[3]);
         state.+x := cint(loc[1]);
         state.+y := cint(loc[2]);
         state.+z := cint(loc[3]);
      endif
   elseif ( opt == "TimedScript")
      var input_list := array{};
      input_list.append(struct{ "title" := "Quem vai sofrer a action?", "marked" := 1, "radio" := targ_opts});
      var timedcfg := TS_GetCfgFile();
      input_list.append(struct{ "title" := "Escolha o timedscript", "marked" := 1, "radio" := GetConfigStringKeys(timedcfg)});
      input_list.append(struct{ "title" := "Qual a duração do timer? [até 120s]", "marked" := ""+cint(state.timerduration)});
      input_list.append(struct{ "title" := "Qual o level do timer?", "marked" := ""+cint(state.level)});
      input_list.append(struct{ "title" := "Teste de resistência?", "checked" := 1, "radio" :=  array{STRENGTH, INTELLIGENCE, DEXTERITY, CONSTITUTION, WISDOM, "Nenhum"} });
      input_list.append(struct{ "title" := "Dificuldade do teste?", "marked" := "12"});
      input_list.append(struct{ "title" := "Distância do cone (cone in front only)?", "marked" := "3"});

      var output := QuestionsGump(who, input_list);
      
      state.+target        := output[1];
      state.+timername     := output[2];
      state.+timerduration := cint(output[3]);
      state.+level         := cint(output[4]);
      state.+conedist      := cint(output[6]);

      if (output[5] != "Nenhum")
         state.+resist_test := output[5];
         state.+resist_diff := cint(output[6]);
      endif
   elseif ( opt == "Perform Action" )
      var input_list := array{}; 
      input_list.append(struct{ "title" := "Quem vai realizar a action?", "marked" := 1, "radio" := targ_opts});
      input_list.append(struct{ "title" := "Qual o ID da animação?", "marked" := ""+cint(state.id)});
      var output := QuestionsGump(who, input_list);

      state.+target := output[1];
      state.+id := cint(output[2]);
   elseif (opt == "Text")
      var input_list := array{};
      if (!state.color) state.color := 1000; endif
      if (!state.text) state.text := ""; endif
      input_list.append(struct{ "title" := "Quem vai falar?", "marked" := 1, "radio" := targ_opts});
      input_list.append(struct{ "title" := "Qual o texto?", "marked" := ""+cstr(state.text)});
      input_list.append(struct{ "title" := "Qual a cor do texto?", "marked" := ""+cint(state.color)});
      var output := QuestionsGump(who, input_list);

      state.+target := output[1];
      state.+text := output[2];
      state.+color := cint(output[3]);
   elseif (opt == "Play Sound")
      state.+id := cint(RequestGump(who, "Qual o ID do som?", "", ""+cint(state.id), 1));
   elseif (opt == "Delay")
      state.+delay := cint(RequestGump(who, "Quanto tempo de delay?", "", ""+cint(state.delay), 1));
   elseif (opt == "Heal")
      state.healamt := cint(RequestGump(who, "Quanto vai healar?", "", ""+cint(state.healamt), 1));
   elseif (opt == "Graphic Effect")
      var input_list := array{};
      input_list.append(struct{ "title" := "Onde vai surgir o efeito?", "marked" := 1, "radio" := targ_opts + array{"Fixed Loc"}});
      input_list.append(struct{ "title" := "Qual o tipo de efeito? (fixed loc deve ser Moving)", "marked" := 1, "radio" := array{"Stationary", "Moving"}});
      input_list.append(struct{ "title" := "Qual o efeito?", "marked" := 11, "radio" := array{"Lightning", "Red Sparkles", "Blue Sparkles", "Blue and Swirl", "Swirl Sparkles", "Blue Sparkles on Ground", "Fire Column", "Small Fireball", "Large Fireball", "Wavy Fireball", "Outro"}});
      input_list.append(struct{ "title" := "Qual o ID do efeito? (Outro)", "marked" := ""+cint(state.fxid)});
      input_list.append(struct{ "title" := "Qual a direção? (Moving, ignore for Cone)", "marked" := (state.direction == "Mob->Player")+1, "radio" := array{"Mob->Player", "Player->Mob"}});
      input_list.append(struct{ "title" := "Digite o x y z onde o efeito vai aparecer: (stationary, ignore for Moving/cone)", "marked" := "{} {} {}".format(cint(state.startx), cint(state.starty), cint(state.startz))});
      input_list.append(struct{ "title" := "Digite o x y z da posição final do efeito: (stationary, ignore for Moving/cone)", "marked" := "{} {} {}".format(cint(state.endx), cint(state.endy), cint(state.endz))});
      input_list.append(struct{ "title" := "Qual a cor do efeito?", "marked" := ""+cint(state.color)});
      input_list.append(struct{ "title" := "Digite a distância (somente Cone in Front/In Line)", "marked" := ""+cint(state.conedist)});
      var output := QuestionsGump(who, input_list);


      state.+target := output[1];
      state.+conedist := cint(output[9]);
      state.+color := cint(output[8]);
      state.+graphictype := output[2];
      if ( output[3] == "Lightning" )
         state.+fxid := "Lightning";
      elseif ( output[3] == "Red Sparkles")
         state.+fxid := GFX_RED_SPARKLES;
         state.+speed := 7;
         state.+loops := 7;
      elseif ( output[3] == "Blue Sparkles")
         state.+fxid := GFX_BLUE_SPARKLES;
         state.+speed := 10;
         state.+loops := 10;
      elseif (output[3] == "Blue and Swirl")
         state.+fxid := GFX_BLUE_SPARKLE_SWIRL;
         state.+speed := 7;
         state.+loops := 16;
      elseif (output[3] == "Swirl Sparkles")
         state.+fxid := GFX_BLUE_SPARKLE_BORDER;
         state.+speed := 7;
         state.+loops := 0;
      elseif (output[3] == "Blue Sparkles on Ground")
         state.+fxid := GFX_BLUE_SPARKLE_GROUND;
         state.+speed := 7;
         state.+loops := 10;
      elseif (output[3] == "Fire Column");
         state.+fxid := GFX_FIRE_COLUMN;
         state.+speed := 7;
         state.+loops := 0;
      elseif (output[3] == "Small Fireball")
         state.fxid := GFX_SMALL_FIREBALL;
         state.+speed := 5;
         state.+loops := 0;
      elseif (output[3] == "Large Fireball")
         state.fxid := GFX_LARGE_FIREBALL;
         state.+speed := 5;
         state.+loops := 0;
      elseif (output[3] == "Wavy Fireball")
         state.fxid := GFX_WAVY_FIREBALL;
         state.+speed := 5;
         state.+loops := 0;
      elseif (output[3] == "Outro")
         state.+fxid := output[4];
         if (state.graphictype == "Stationary")
            state.+speed := 7;
            state.+loops := 16;
         else
            state.+speed := 5;
            state.+loops := 0;
         endif
      endif

      state.+direction := output[5];

      if (state.target == "Fixed Loc")
         var loc := splitwords(output[6]);
         state.+startx :=  loc[1];
         state.+starty :=  loc[2];
         state.+startz :=  loc[3];
         state.startradius := 0;

         if (state.graphictype == "Moving")
            var loc2 := splitwords(output[7]);
            state.+endx :=  loc2[1];
            state.+endy :=  loc2[2];
            state.+endz :=  loc2[3];
            // var radius :=   RequestGump(who, "Qual o raio de onde o efeito vai terminar:", "0 se for sempre no mesmo lugar", "0", 1);
            state.endradius := 0;
         endif
      endif
   elseif ( opt == "Dmg")
      var input_list := array{};
      input_list.append(struct{ "title" := "Quem vai sofrer?", "marked" := 1, "radio" := targ_opts});
      input_list.append(struct{ "title" := "Quanto de dano vai causar? Ex: 1d2", "marked" := "1d2"});
      input_list.append(struct{ "title" := "Qual o tipo de dano?", "marked" := 1, "radio" := array{DMG_PHYSICAL, DMG_FIRE, DMG_COLD, DMG_POISON,DMG_ENERGY,DMG_PIERCING,DMG_SLASH,DMG_BLUDGE, "Sem Resistência" }});
      input_list.append(struct{ "title" := "Qual tipo de vital sera afetado pelo dano?", "marked" := "", "checkbox" := array{"Stam", "Hits", "Mana"}});
      input_list.append(struct{ "title" := "Teste de resistência?", "checked" := 6, "radio" :=  array{STRENGTH, INTELLIGENCE, DEXTERITY, CONSTITUTION, WISDOM, "Nenhum"} });
      input_list.append(struct{ "title" := "Qual a dificuldade do teste?", "marked" := "14"});
      input_list.append(struct{ "title" := "Digite a distância (somente Cone in Front/In Line)", "marked" := "3"});
      var output := QuestionsGump(who, input_list);

      state.+target := output[1];
      state.+dmg := output[2];
      state.+dmgtype := output[3];
      state.+conedist := cint(output[7]);
      if (state.dmgtype == "Sem Resistência")
         state.dmgtype := -1;
      endif
      state.+vitals := output[4];
      if (output[5] != "Nenhum")
         state.+resist_test := output[5];
         state.+resist_diff := cint(output[6]);
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
      state.+npctemplate := output[1];
      state.+amount := output[2];
      state.+range := cint(output[3]);
      state.+tempo := cint(output[4]);
      
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
      
      state.+x := cint(loc[1]);
      state.+y := cint(loc[2]);
      state.+z := cint(loc[3]);

      var walk_to := SplitWords(output[5]);
      if (walk_to.size() == 3)
         state.+wal_to := walk_to;
      endif
      
   elseif (opt == "Custom HitScript")
      SendSysMessage(who, "Não implementado");
   elseif (opt == "Set Cooldown")
      var input_list := array{};
      input_list.append(struct{ "title" := "Qual o cooldown?", "marked" := "lockaggro" });
      input_list.append(struct{ "title" := "Quanto tempo vai durar?", "marked" := "10"});
      var output := QuestionsGump(who, input_list);
      state.+prop := output[1];
      state.+time := cint(output[2]);
   elseif (opt == "Create Mine")
      var input_list := array{};
      input_list.append(struct{ "title" := "Qual o alvo?", "marked" := 1, radio := array{"Random", "Frente", "Costas"} });
      input_list.append(struct{ "title" := "Quantas minas?", "marked" := "1"});
      input_list.append(struct{ "title" := "Qual tipo da mina?", "marked" := 1, radio := array{ "Fogo", "Gelo" }});
      input_list.append(struct{ "title" := "Qual dificuldade de resistência?", "marked" := "12"});
      input_list.append(struct{ "title" := "Qual o dano?", "marked" := "1d5+2"});

      var output := QuestionsGump(who, input_list);
   elseif (opt == "Hidden")
      var input_list := array{};
      input_list.append(struct{ "title" := "Quantos passos?", "marked" := "15" });
      input_list.append(struct{ "title" := "Ataca sem sair do modo furtivo?", "marked" := 1+cint(state.attackhidden), radio := array{"Não", "Sim"} });
      input_list.append(struct{ "title" := "Fugir do alvo enquanto furtivo?", "marked" := 1+cint(state.flee), radio := array{"Não", "Sim"} });
      var output := QuestionsGump(who, input_list);
      state.+passos := cint(output[1]);
      state.+attackhidden := output[2] == "Sim";
      state.+flee := output[3] == "Sim";
   elseif (opt == "Spell Trilha")
      var input_list := array{};
      input_list.append(struct{ "title" := "Qual o alvo?", "marked" := 1, "radio" := targ_opts});
      input_list.append(struct{ "title" := "Qual tipo de trilha?", "marked" := 1, radio := array{"Fogo", "Veneno", "Eletrecidade", "Gelo"} });
      input_list.append(struct{ "title" := "Qual dado de dano?", "marked" := "1d5+2" });
      var output := QuestionsGump(who, input_list);

      state.+target   := output[1];
      state.+type     := output[2];
      state.+dice_dmg := output[3];
   elseif (opt == "Metamorfose")
      var metamorf := ":ghaia:";
      if (state.monsters) metamorf := state.monsters; endif
      var input_list := array{};
      input_list.append(struct{ "title" := "Para qual npctemplate? (separar por espaço para mais opções)", "marked" := metamorf });
      var output := QuestionsGump(who, input_list);

      state.+monsters := Splitwords(output[1]);
   elseif (opt == "Magicball")
      var input_list := array{};
      input_list.append(struct{ "title" := "Qual o alvo?", "marked" := 1, "radio" := targ_opts});
      input_list.append(struct{ "title" := "Criar em +X +Y +Z? (6 6 6 para usar alvo)", marked := "6 6 6" });
      input_list.append(struct{ "title" := "Enviar no local ou no target?", "marked" := 1, radio := array{"Local", "Target"} });
      input_list.append(struct{ "title" := "Qual type?", "marked" := 1, radio := array{"0 - Single DMG", "2 - Area DMG", "3 - Token Balls"} });
      input_list.append(struct{ "title" := "Qual duração?", "marked" := "6" });
      input_list.append(struct{ "title" := "Qual dado de dano?", "marked" := "2d6+5" });
      input_list.append(struct{ "title" := "Qual o tipo de dano?", "marked" := 1, "radio" := array{DMG_PHYSICAL, DMG_FIRE, DMG_COLD, DMG_POISON,DMG_ENERGY,DMG_PIERCING,DMG_SLASH,DMG_BLUDGE, "Sem Resistência" }});
      input_list.append(struct{ "title" := "Qual gráfico de efeito?", "marked" := ""+cint(state.graphic) });
      input_list.append(struct{ "title" := "Qual cor do efeito?", "marked" := ""+cint(state.color) });

      var output := QuestionsGump(who, input_list);
   endif
   return state;
endfunction
