use uo;

include "include/say";
include ":unicos:item_template";
include ":timedscripts:timedScripts";
include ":yggdrasil:boss";

function ConfigureEvent(who)
	var evento := struct;
	evento.+type := RadioGump(who, 400, 220, "Escolha o tipo de evento.", array{ "Criar Itens", "Abrir Porta", "Criar Npcs", "Abrir Passagem", "Ultimas Palavras"});
	if ( (evento.type == "Abrir Porta") || (evento.type == "Abrir Passagem") )
		var items := array;
                SendSysMessage(who, "Escolha os itens. ESC para cancelar.");
                var targ := Target(who);
                if (targ)
                  evento.+items := array;
                endif
                while(targ)
                    evento.items.append(targ.serial);
                    targ := Target(who);
                endwhile
	elseif  (evento.type == "Criar Npcs" ) 
         var input_list := array{};
      input_list.append(struct{ "title" := "Qual o npctemplate?", "marked" := ":ghaia:"});
      input_list.append(struct{ "title" := "Quantos npcs vão ser spawnados?", "marked" := "2"});
      input_list.append(struct{ "title" := "Qual o raio de spawn?", "marked" := "5"});
      input_list.append(struct{ "title" := "Qual o tempo de vida dos monstros? (secs)", "marked" := "120"});
      input_list.append(struct{ "title" := "Digite o x y z onde eles vao aparecer: (6 6 6 para onde o boss esta)", "marked" := "6 6 6"});
      var output := QuestionsGump(who, input_list);

		evento.+npctemplate := output[1];
		evento.+amount := Cint(output[2]);
		evento.+duracao := Cint(output[3]);
      evento.time := cint(output[4]);
		var loc := SplitWords(output[5]);
		evento.+y :=  loc[2];
		evento.+z :=  loc[3];
		evento.+x :=  loc[1];
		// var boss := YesNo( who, "Deseja adicionar um npc boss?" );
		// if (boss)
		// 	var bossinfo := struct;
		// 	CreateBossStructGump(who, bossinfo);
		// 	evento.+bossinfo := bossinfo;
		// endif
		// evento.+summontext := RequestGump(who, "Deseja adicionar algum texto nos mobs?", "(IGNORAR para adicionar só no boss)", "", 1);
		// evento.+summonbosstext :=  RequestGump(who, "Deseja adicionar algum texto no Boss?", "", "", 1);

	elseif ( evento.type == "Criar Itens")
		SendSysMessage(who, "Crie os itens e coloque eles no lugar que quer que aparecam. Quando o evento for disparado eles vao aparecer la.");
                var targ := Target(who);
                if (targ)
                  evento.+items := array;
                endif
                while(targ)
		    var iteminfo := struct;
		    iteminfo.+item := CreateItemTemplate(targ);
		    iteminfo.+x := targ.x;
		    iteminfo.+y := targ.y;
		    iteminfo.+z := targ.z;
                    evento.items.append(iteminfo);
                    targ := Target(who);
                endwhile
	elseif ( evento.type == "Ultimas Palavras")
		evento.+text := RequestGump(who, "Digite as ultimas palavras do NPC (caso seja um npc quem executa esse evento)", "", "", 1);
	endif
	return evento;
endfunction

function ExecuteEvent(evento)
//	print( "EVENTO " + evento.type);

	if (evento.type == "Abrir Porta" )
		foreach serial in (evento.items)
			var  item := SystemFindObjectBySerial(serial);
			item.locked := 0;
		endforeach
	elseif (evento.type == "Abrir Passagem")
		var i := 0;
		while ( i < 20)
			foreach serial in (evento.items)
				var item := SystemFindObjectBySerial(serial);
				item.movable := 1;
				var e := MoveObjectToLocation(item, item.x, item.y, item.z + 1, item.realm, MOVEOBJECT_FORCELOCATION);
        //printtextabove(item, "move object " + e );
				item.movable := 0;
			endforeach
			i := i + 1;
		endwhile
	elseif (evento.type == "Criar Npcs")
      var amt := 0;
      // print("criar npcs  2 " + evento.x + " " + evento.y + " " + evento.z +  " amt " + evento.amount ); 
      while (amt < evento.amount)
         var npc := CreateNpcFromTemplate( cstr(evento.npctemplate), Cint(evento.x) + RandomInt(2 * evento.range) - evento.range, Cint(evento.y) + RandomInt(2 * evento.range) - evento.range, Cint(evento.z));
         //print ( "NPC spawn " + npc);
         if (npc != error)
            var duracaonpc := evento.duracao;
            if (!duracaonpc)
               duracaonpc := 120;
            endif
            TS_StartTimer(npc, "summon", duracaonpc);
            amt := amt + 1;
            if (evento.summontext)
               printtext(npc, evento.summontext);
            endif
         endif
         sleepms(2);
      endwhile

      if (evento.bossinfo)
         var success := 0;

         //print("tem boss");

         while ( !success)
            var boss := CreateBoss(evento.bossinfo, Cint(evento.x), Cint(evento.y), Cint(evento.z), REALM_BRITANNIA);
            //print( "boss " + boss);

            if (boss)
               SetObjProperty(boss, "anchor", array{boss.x, boss.y});
               success := 1;

               if (evento.summonbosstext)
                  PrintText(boss, evento.summonbosstext);
               endif
            endif
            sleepms(2);
         endwhile
      endif
	elseif (evento.type == "Criar Itens")
		foreach iteminfo in (evento.items)
			var item := CreateItemAtLocationFromTemplate( cint(iteminfo.x), cint(iteminfo.y), cint(iteminfo.z), _DEFAULT_REALM ,iteminfo.item );
		endforeach
	endif

endfunction

function PrepareEvent(evento)
	if (evento.type == "Abrir Porta" )
		foreach serial in (evento.items)
			var item := SystemFindObjectBySerial(serial);
			item.locked := 1;
		endforeach
	elseif (evento.type == "Abrir Passagem")
		var i := 0;
		while ( i < 20)
			foreach serial in (evento.items)
				var item := SystemFindObjectBySerial(serial);
				item.movable := 1;
				MoveObjectToLocation(item, item.x, item.y, item.z - 1, item.realm, MOVEOBJECT_FORCELOCATION);
				item.movable := 0;
			endforeach
			i := i + 1;
		endwhile
	endif
endfunction
