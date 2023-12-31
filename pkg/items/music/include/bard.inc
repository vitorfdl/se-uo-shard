// $Id: bard.inc 834 2005-11-02 14:09:50Z austin $

include "bardInstrument";
include "bardMagic";
include ":ghaia:ghaiaUtil";
include ":timedscripts:timedScripts";
include ":yggdrasil:boss";


function BardNotes(who,item,notas,notas2,tempo,letra,what,emocao)
   while (who.connected)
      sleepms(20);
      if (randomint(5) == 1)
         PrintText(who, "*Compondo uma musica*");
      endif
      var x,y,limitenota;
      var bardgump := GFCreateGump();
      GFSetId(bardgump, GUMPID_BARD);
      GFClosable(bardgump, 1);
      GFResizePic(bardgump, 30, 30, 2600, 750, 290);
      GFResizePic(bardgump, 50, 50, 3500, 710, 250); //55
      GFTextLine(bardgump, 80, 63, 1890, "Notas:" );

      //primeira oitava
      GFTextLine(bardgump, 86, 86, 600, "C" );
      GFTextLine(bardgump, 118, 86, 600, "C#" );
      GFTextLine(bardgump, 150, 86, 600, "D" );
      GFTextLine(bardgump, 182, 86, 600, "D#" );
      GFTextLine(bardgump, 214, 86, 600, "E" );
      GFTextLine(bardgump, 246, 86, 600, "F" );
      GFTextLine(bardgump, 278, 86, 600, "F#" );
      GFTextLine(bardgump, 310, 86, 600, "G" );
      GFTextLine(bardgump, 342, 86, 600, "G#" );
      GFTextLine(bardgump, 374, 86, 600, "A" );
      GFTextLine(bardgump, 406, 86, 600, "A#" );
      GFTextLine(bardgump, 438, 86, 600, "B" );
      GFTextLine(bardgump, 466, 86, 600, "Pausa" );
      GFTextLine(bardgump, 510, 86, 600, "Letra" );

      //segunda oitava
      GFTextLine(bardgump, 86, 116, 600, "C2" );
      GFTextLine(bardgump, 118, 116, 600, "C#2" );
      GFTextLine(bardgump, 150, 116, 600, "D2" );
      GFTextLine(bardgump, 182, 116, 600, "D#2" );
      GFTextLine(bardgump, 214, 116, 600, "E2" );
      GFTextLine(bardgump, 246, 116, 600, "F2" );
      GFTextLine(bardgump, 278, 116, 600, "F#2" );
      GFTextLine(bardgump, 310, 116, 600, "G2" );
      GFTextLine(bardgump, 342, 116, 600, "G#2" );
      GFTextLine(bardgump, 374, 116, 600, "A2" );
      GFTextLine(bardgump, 406, 116, 600, "A#2" );
      GFTextLine(bardgump, 438, 116, 600, "B2" );
      
      //terceira oitava
      GFTextLine(bardgump, 86, 146, 600, "C3" );

      //primeira oitava
      GFAddButton(bardgump, 86, 106, 2103, 2104, 1, 100);// var C
      GFAddButton(bardgump, 118, 106, 2103, 2104, 1, 2);// var Cs 
      GFAddButton(bardgump, 150, 106, 2103, 2104, 1, 3);// var D 
      GFAddButton(bardgump, 182, 106, 2103, 2104, 1, 4);// var Ds 
      GFAddButton(bardgump, 214, 106, 2103, 2104, 1, 5);// var E 
      GFAddButton(bardgump, 246, 106, 2103, 2104, 1, 6);// var F 
      GFAddButton(bardgump, 278, 106, 2103, 2104, 1, 7);// var Fs 
      GFAddButton(bardgump, 310, 106, 2103, 2104, 1, 8);// var G 
      GFAddButton(bardgump, 342, 106, 2103, 2104, 1, 9);// var Gs 
      GFAddButton(bardgump, 374, 106, 2103, 2104, 1, 10);// var A
      GFAddButton(bardgump, 406, 106, 2103, 2104, 1, 11);// var Ass
      GFAddButton(bardgump, 438, 106, 2103, 2104, 1, 12);// var B
      GFAddButton(bardgump, 475, 106, 2103, 2104, 1, 13);// var P
      GFAddButton(bardgump, 520, 106, 2103, 2104, 1, 16);// var L

      //segunda oitava
      GFAddButton(bardgump, 86, 136, 2103, 2104, 1, 17);// var C2 
      GFAddButton(bardgump, 118, 136, 2103, 2104, 1, 18);// var Cs2
      GFAddButton(bardgump, 150, 136, 2103, 2104, 1, 19);// var D2
      GFAddButton(bardgump, 182, 136, 2103, 2104, 1, 20);// var Ds2
      GFAddButton(bardgump, 214, 136, 2103, 2104, 1, 21);// var E2
      GFAddButton(bardgump, 246, 136, 2103, 2104, 1, 22);// var F2
      GFAddButton(bardgump, 278, 136, 2103, 2104, 1, 23);// var Fs2
      GFAddButton(bardgump, 310, 136, 2103, 2104, 1, 24);// var G2
      GFAddButton(bardgump, 342, 136, 2103, 2104, 1, 25);// var Gs2
      GFAddButton(bardgump, 374, 136, 2103, 2104, 1, 26);// var A2
      GFAddButton(bardgump, 406, 136, 2103, 2104, 1, 27);// var Ass2
      GFAddButton(bardgump, 438, 136, 2103, 2104, 1, 28);// var B2	

      //terceira oitava
      var C3  := GFAddButton(bardgump, 86, 166, 2103, 2104, 1, 29);


      GFTextLine(bardgump, 600, 63, 1890, "Tempo(ms):" );
         var tempotxt := GFTextEntry(bardgump, 600, 83, 255, 35, 1153, tempo);

      GFTextLine(bardgump, 80, 186, 1890, "Combinacao das Notas:" );
      GFHTMLArea(bardgump, 80, 206, 650, 60, notas + notas2,3,1);

      GFTextLine(bardgump, 100, 266, 1890, "Limpar Combinacao" );
      GFAddButton(bardgump, 80, 270, 2103, 2104, 1, 14);

      GFTextLine(bardgump, 240, 266, 1890, "Limpar Ultimo" );
      GFAddButton(bardgump, 220, 270, 2103, 2104, 1, 60);

      GFTextLine(bardgump, 630, 185, 1890, "Compor Partitura" );
      GFAddButton(bardgump, 610, 189, 2103, 2104, 1, 61);

      if (what == 1) GFTextLine(bardgump, 680, 63, 66, "TOCAR!!" );
      elseif (what == 2) GFTextLine(bardgump, 680, 63, 66, "COMPOR!!" );
      endif

      var tocar  := GFAddButton(bardgump, 675, 80, 5553, 5554, 1, 15);

      var retv := GFSendGump(who, bardgump );

      if (retv[0] == 100)
         notas.append("C");
      elseif (retv[0] == 2)
         notas.append("C#");
      elseif (retv[0] == 3)
         notas.append("D");
      elseif (retv[0] == 4)
         notas.append("D#");
      elseif (retv[0] == 5)
         notas.append("E");
      elseif (retv[0] == 6)
         notas.append("F");
      elseif (retv[0] == 7)
         notas.append("F#");
      elseif (retv[0] == 8)
         notas.append("G");
      elseif (retv[0] == 9)
         notas.append("G#");
      elseif (retv[0] == 10)
         notas.append("A");
      elseif (retv[0] == 11)
         notas.append("A#");
      elseif (retv[0] == 12)
         notas.append("B"); 
      elseif (retv[0] == 17)
         notas.append("C2");
      elseif (retv[0] == 18)
         notas.append("C#2");
      elseif (retv[0] == 19)
         notas.append("D2");
      elseif (retv[0] == 20)
         notas.append("D#2");
      elseif (retv[0] == 21)
         notas.append("E2");
      elseif (retv[0] == 22)
         notas.append("F2");
      elseif (retv[0] == 23)
         notas.append("F#2");
      elseif (retv[0] == 24)
         notas.append("G2");
      elseif (retv[0] == 25)
         notas.append("G#2");
      elseif (retv[0] == 26)
         notas.append("A2");
      elseif (retv[0] == 27)
         notas.append("A#2");
      elseif (retv[0] == 28)
         notas.append("B2"); 
      elseif (retv[0] == 29)
         notas.append("C3"); 
      elseif (retv[0] == 13)
         notas.append("|"); 
      elseif (retv[0] == 16)
         notas.append("L"); 
         letra.append(Requestgump(who,"Qual o verso a ser adicionado?","","",0));
      elseif (retv[0] == 14)
         notas := array;
         notas2 := array;
      elseif (retv[0] == 60)
         notas.erase(notas.size());
      elseif (retv[0] == 61)
         SendSysMessageEx(who, "Selecione o papel pautado em branco.", SSM_REQUEST);
         var partitura := Target(who);
         if (!partitura.isA(POLCLASS_ITEM) || partitura.objtype != 0x9923)
            SendSysMessageEx(who, "Isso n�o parece um papel pautado", SSM_REQUEST);
         else
            return escrevepartitura(who,partitura,notas,notas2,tempo,letra);
         endif
      elseif (retv[0] == 15)
         var tempo := GFExtractData(retv, tempotxt);
         if (!tempo)
            tempo := Requestgump(who,"Qual � o tempo entre as notas?","","");
         endif

         while (cint(tempo) > 2000)
            tempo := Requestgump(who,"O tempo n�o pode ser t�o grande, escolha um menor:","","");
         endwhile

         if (what == 1)		
            tocarsound(who,item,notas,notas2,tempo,letra,emocao);
         elseif (what == 2)
            escrevepartitura(who,item,notas,notas2,tempo,letra);
         endif
      else
         break;
      endif
      
   endwhile

endfunction

function tocarsound(who,item,notas,notas2,tempo,letra,emocao)
	var c,cs,d,ds,e,f,fs,g,gs,a,ass,b,c2,cs2,d2,ds2,e2,f2,fs2,g2,gs2,a2,ass2,b2,c3,inst,L,dif,emocaouse;

	if ( emocao && !HaveFeatSecondary(who, 73) && who.cmdlevel < 3)
      SendSysMessageEx(who, "Voc� n�o entende essa partitura.", SSM_FAIL);
      return 0;
	endif

	if( Cint(GetObjProperty(who, "#emocaoMusic")) > ReadGameClock() )
		SendSysMessage(who, "Voc� deve esperar um momento antes de tocar uma musica emocao novamente.",3,38);
		return 0;
	endif

   var total := notas.size() + notas2.size();
   // var max_allowed := AP_GetTrueSkill(who, INSTRUMENTO_MUSICAL) * 15;
   // if (max_allowed < 5)
   //    max_allowed := 5;
   // endif

   // //REDUCAO PARA MADEIRAS DIFERENTES
   // case(Cstr(item.color))
   //    "2307": max_allowed += 20;
   //    "2109": max_allowed += 20;
   //    "2312": max_allowed += 10;
   //    "2317": max_allowed += 10;
   //    "2117": max_allowed += 4;
   // endcase

   // //REDUCAO PARA INSTRUMENTOS EXCEP
   // case(Cstr(item.quality))
   //    "1.1": max_allowed += 10;
   //    "1.2": max_allowed += 15;
   //    "1.3": max_allowed += 20;
   //    "1.4": max_allowed += 25;
   // endcase
   
   if (total > 250)
		SendSysMessageEx(who, "O m�ximo de nota que voc� pode tocar � {}".format(200),SSM_FAIL);
		return 0;
   endif
		
      //DIFICULDADE PELO TANTO DE NOTAS
		// dif := 2 + cint(notas.size() / 2) + cint(notas2.size() / 2);

      // //DIFICULDADE PELA VELOCIDADE
		// if (Cint(tempo) < 100) dif := (dif + 1);
		// elseif (Cint(tempo) < 200) dif := (dif + 2);		
		// elseif (Cint(tempo) < 400) dif := (dif + 3);
		// elseif (Cint(tempo) < 600) dif := (dif + 4);
		// endif

		// var difskill := (dif / 2);

		L := 1;	
		case(item.graphic)
			0x0eb1:  c := 0x49E; cs := 0x4A1; d := 0x4A3 ; ds := 0x4A5; e := 0x4A7; f := 0x4A9; fs := 0x4AB; g := 0x4AD; gs := 0x4AF; a := 0x498; ass := 0x49A; b := 0x49C; inst := "Harpa Grande";
				  c2 := 0x49F; cs2 := 0x4A2; d2 := 0x4A4 ; ds2 := 0x4A6; e2 := 0x4A8; f2 := 0x4AA; fs2 := 0x4AC; g2 := 0x4AE; gs2 := 0x4B0; a2 := 0x499; ass2 := 0x49B; b2 := 0x49D; inst := "Harpa Grande";
				  c3 := 0x4A0;

			0x0eb2:  c := 0x3D1; cs := 0x3D4; d := 0x3D6 ; ds := 0x3D8; e := 0x3DA; f := 0x3DC; fs := 0x3DE; g := 0x3E0; gs := 0x3E2; a := 0x3CB; ass := 0x3CD; b := 0x3CF; inst := "Harpa Pequena";
				  c2 := 0x3D2; cs2 := 0x3D5; d2 := 0x3D7 ; ds2 := 0x3D9; e2 := 0x3DB; f2 := 0x3DD; fs2 := 0x3DF; g2 := 0x3E1; gs2 := 0x3E3; a2 := 0x3CC; ass2 := 0x3CE; b2 := 0x3D0;
				  c3 := 0x3D3;
			0x0eb3:
         0x0eb4:
              c := 0x405; cs := 0x408; d := 0x40A ; ds := 0x40D; e := 0x40F; f := 0x411; fs := 0x413; g := 0x415; gs := 0x417; a := 0x3FE; ass := 0x400; b := 0x402; inst := "Alaude";
				  c2 := 0x406; cs2 := 0x409; d2 := 0x40B ; ds2 := 0x40E; e2 := 0x410; f2 := 0x412; fs2 := 0x414; g2 := 0x416; gs2 := 0x418; a2 := 0x3FF; ass2 := 0x401; b2 := 0x403;
				  c3 := 0x407;
		endcase
		tempo := Cint(tempo);

		// if (emocao != error)
		// 	emocaouse := BardBuff(who,item,notas,notas2,tempo,emocao);
		// endif
		// if (emocaouse == 999)
		// 	return 0;
		// endif

		// var result := rollAttrDice(who, DEXTERITY) + GetSkillBonus(who, INSTRUMENTO_MUSICAL);
		// if (result <= 10 && !who.cmdlevel)
		// 	tocarerrado(who,item,notas,notas2,tempo,letra,emocao);
		// 	return 1;
		// endif

		PrintText(who, "*Tocando uma melodia com " + inst + "*");
		var x,y;
		x := 0;
		foreach nota in notas
			if (nota != "L")
				x := x + 1;
			endif
		endforeach
		y := 0;
		foreach nota in notas2
			if (nota != "L")
				y := y + 1;
			endif
		endforeach	
		
		var duration := ((tempo + (tempo * x) + (tempo * y)) / 1000);
		TS_StartTimer(who, "staminaPoison", duration, 1);

		sleepms(tempo);

      var emote_text := "";
      case (emocao)
         "Enconrajar":  emote_text := "*Voc� se sente encorajado*"; break;
         "Entristecer": emote_text := "*Voc� se sente triste*"; break;
         "Alegrar":     emote_text := "*Voc� se sente alegre*"; break;
         "Enfurecer":   emote_text := "*Voc� se sente com raiva*"; break;
      endcase

      ActiveMusicEffect(who, emocao, duration);

		foreach nota in notas
         if (_nota_iter == cint(notas.size() / 2) && emocao)
            var mobs := ListMobilesNearLocationEx(who.x, who.y, LIST_IGNORE_Z, 8, LISTEX_FLAG_NORMAL|LISTEX_FLAG_HIDDEN|LISTEX_FLAG_PLAYERS_ONLY);
            foreach mob in mobs
               SendSystemReport(mob, emote_text, SSM_INFO);
               sleepms(2);
            endforeach
         endif
         who.SetWarmode(0);

			if (nota == "C") PlaySoundEffect(who, c); sleepms(tempo);
				elseif (nota == "C#") PlaySoundEffect(who, cs); sleepms(tempo);
				elseif (nota == "D") PlaySoundEffect(who, d); sleepms(tempo);
				elseif (nota == "D#") PlaySoundEffect(who, ds); sleepms(tempo);
				elseif (nota == "E") PlaySoundEffect(who, e); sleepms(tempo);
				elseif (nota == "F") PlaySoundEffect(who, f); sleepms(tempo);
				elseif (nota == "F#") PlaySoundEffect(who, fs); sleepms(tempo);
				elseif (nota == "G") PlaySoundEffect(who, g); sleepms(tempo);
				elseif (nota == "G#") PlaySoundEffect(who, gs); sleepms(tempo);
				elseif (nota == "A") PlaySoundEffect(who, a); sleepms(tempo);
				elseif (nota == "A#") PlaySoundEffect(who, ass); sleepms(tempo);
				elseif (nota == "B") PlaySoundEffect(who, b); sleepms(tempo);
				elseif (nota == "C2") PlaySoundEffect(who, c2); sleepms(tempo);
				elseif (nota == "C#2") PlaySoundEffect(who, cs2); sleepms(tempo);
				elseif (nota == "D2") PlaySoundEffect(who, d2); sleepms(tempo);
				elseif (nota == "D#2") PlaySoundEffect(who, ds2); sleepms(tempo);
				elseif (nota == "E2") PlaySoundEffect(who, e2); sleepms(tempo);
				elseif (nota == "F2") PlaySoundEffect(who, f2); sleepms(tempo);
				elseif (nota == "F#2") PlaySoundEffect(who, fs2); sleepms(tempo);
				elseif (nota == "G2") PlaySoundEffect(who, g2); sleepms(tempo);
				elseif (nota == "G#2") PlaySoundEffect(who, gs2); sleepms(tempo);
				elseif (nota == "A2") PlaySoundEffect(who, a2); sleepms(tempo);
				elseif (nota == "A#2") PlaySoundEffect(who, ass2); sleepms(tempo);
				elseif (nota == "B2") PlaySoundEffect(who, b2); sleepms(tempo);
				elseif (nota == "C3") PlaySoundEffect(who, c3); sleepms(tempo);
				elseif (nota == "|") sleepms(tempo);    
				elseif (nota == "L") 
				PrintText(who, letra[L]);
				L := L + 1;		
			endif 				
		endforeach

      if (!GetCooldown(who, "musician_exp") && randomInt(100) < 40)
         ExpGain(who, 2);
         SetCooldown(who, "musician_exp", 200);
      endif

		// if( Cint(GetObjProperty(who, "#emocaoMusic")) > ReadGameClock() )
		// 	return 0;
		// else 
      playagain(who,item,notas,notas2,tempo,letra,emocao);
		// endif
endfunction

function tocarerrado(who,item,notas,notas2,tempo,letra,emocao)
   var c,cs,d,ds,e,f,fs,g,gs,a,ass,b,c2,cs2,d2,ds2,e2,f2,fs2,g2,gs2,a2,ass2,b2,c3,inst,L,notanow;
      
   L := 1;
   case(item.graphic)
      0x0eb1:  c := 0x49E; cs := 0x4A1; d := 0x4A3 ; ds := 0x4A5; e := 0x4A7; f := 0x4A9; fs := 0x4AB; g := 0x4AD; gs := 0x4AF; a := 0x498; ass := 0x49A; b := 0x49C; inst := "Harpa Grande";
            c2 := 0x49F; cs2 := 0x4A2; d2 := 0x4A4 ; ds2 := 0x4A6; e2 := 0x4A8; f2 := 0x4AA; fs2 := 0x4AC; g2 := 0x4AE; gs2 := 0x4B0; a2 := 0x499; ass2 := 0x49B; b2 := 0x49D; inst := "Harpa Grande";
            c3 := 0x4A0;
      0x0eb2:  c := 0x3D1; cs := 0x3D4; d := 0x3D6 ; ds := 0x3D8; e := 0x3DA; f := 0x3DC; fs := 0x3DE; g := 0x3E0; gs := 0x3E2; a := 0x3CB; ass := 0x3CD; b := 0x3CF; inst := "Harpa Pequena";
            c2 := 0x3D2; cs2 := 0x3D5; d2 := 0x3D7 ; ds2 := 0x3D9; e2 := 0x3DB; f2 := 0x3DD; fs2 := 0x3DF; g2 := 0x3E1; gs2 := 0x3E3; a2 := 0x3CC; ass2 := 0x3CE; b2 := 0x3D0;
            c3 := 0x3D3;
      0x0eb3:  c := 0x405; cs := 0x408; d := 0x40A ; ds := 0x40D; e := 0x40F; f := 0x411; fs := 0x413; g := 0x415; gs := 0x417; a := 0x3FE; ass := 0x400; b := 0x402; inst := "Alaude";
            c2 := 0x406; cs2 := 0x409; d2 := 0x40B ; ds2 := 0x40E; e2 := 0x410; f2 := 0x412; fs2 := 0x414; g2 := 0x416; gs2 := 0x418; a2 := 0x3FF; ass2 := 0x401; b2 := 0x403;
            c3 := 0x407;
      0x0eb4:  c := 0x405; cs := 0x408; d := 0x40A ; ds := 0x40D; e := 0x40F; f := 0x411; fs := 0x413; g := 0x415; gs := 0x417; a := 0x3FE; ass := 0x400; b := 0x402; inst := "Alaude";
            c2 := 0x406; cs2 := 0x409; d2 := 0x40B ; ds2 := 0x40E; e2 := 0x410; f2 := 0x412; fs2 := 0x414; g2 := 0x416; gs2 := 0x418; a2 := 0x3FF; ass2 := 0x401; b2 := 0x403;
            c3 := 0x407;
   endcase
   tempo := Cint(tempo);

   PrintText(who, "*Errando todas as notas*",38);

   sleepms(tempo);

   var notarray := array{b, f, cs, fs, ass, g, c, d, a, ds, gs, e};
   foreach nota in notas
      notanow := notarray.randomentry();
      PlaySoundEffect(who, notanow); 
      sleepms(tempo);
      if (nota == "L") 
         PrintText(who, letra[L],0);
         L := L + 1;    
      endif 		
   endforeach

   sleep(2);
   playagain(who,item,notas,notas2,tempo,letra,emocao);
endfunction


function playagain(who,item,notas,notas2,tempo,letra,emocao)
   var tocardenovo := YesNo(who, "Deseja tentar tocar essa musica denovo?", "Sim", "N�o");
   if (tocardenovo == 1)
      tocarsound(who,item,notas,notas2,tempo,letra,emocao);
      return 0;
   else
      start_script(":musicianship:continueMusic", struct{
         "who"    := who,
         "item"   := item,
         "notas"  := notas,
         "notas2" := notas2,
         "tempo"  := tempo,
         "letra"  := letra,
         "emocao" := emocao,
         "what"   := 1
      });
      return 0;
   endif
endfunction

function escrevepartitura(who,item,notas,notas2,tempo,letra)
   PrintText(who, "*Escrevendo a partitura*");
   sleepms(2000);
   PrintText(who, "*Escrevendo a partitura*");
   sleepms(2000);
   var roll := RandomInt(100);
   if (roll > 20)
      PrintText(who, "*Escrevendo a partitura*");
   else 
      SendSysMessage(who, "Voc� arruinou a partitura");
      PrintText(who, "*Erra ao escrever as notas*",38);
      DestroyItem(item);
      var tryagain := YesNo(who, "Deseja tentar escrever essa partitura denovo?", "Sim", "Nao");
      if (tryagain == 1)
         TentarDenovo(who,item,notas,notas2,tempo,letra);
         return 0;
      else
         return 0;
      endif
   endif

   PrintText(who, "*Escreve a ultima nota*");
   SendSysMessage(who, "Voc� terminou de escrever a partitura",191);
   DestroyItem(item);
   var partitura := CreateItemInBackPack(who, 0x9924, 1);

   Setobjproperty(partitura, "compositor", who.name);	
   Setobjproperty(partitura, "notas", notas);
   Setobjproperty(partitura, "notas2", notas2);
   Setobjproperty(partitura, "tempo", tempo);
   Setobjproperty(partitura, "letra", letra);				

   var nomedamusica := Requestgump(who,"Qual o nome da Musica?","","");
   Setname(partitura,nomedamusica);		
endfunction


function TentarDenovo(who,item,notas,notas2,tempo,letra)
	SendSysMessage(who, "Escolha um novo papel pautado em branco:",191);
	item := target(who);
	if (item.objtype == 0x9923)
		escrevepartitura(who,item,notas,notas2,tempo,letra);
	else
		SendSysMessageEx(who, "Cancelado.",38);
		return 0;
	endif
endfunction

function ActiveMusicEffect(who, emocao, duration)
   if (duration >= 10 || !cint(duration)) duration := 10; endif
   
   case (emocao)
      "Enconrajar":
         var allies := ListMobilesNearLocationEx(who.x, who.y, LIST_IGNORE_Z, 8, LISTEX_FLAG_NORMAL|LISTEX_FLAG_HIDDEN);
         foreach ally in allies
            if (CheckifisAlly(who, ally))
               TS_StartTimer(ally, "aumentar_poder", duration, 15);
            endif
            sleepms(2);
         endforeach
         break;
      "Entristecer": 
         var enemies := CheckifisAlly(who, ListMobilesNearLocationEx(who.x, who.y, LIST_IGNORE_Z, 8, LISTEX_FLAG_NORMAL|LISTEX_FLAG_HIDDEN));
         foreach enemy in enemies
            TS_StartTimer(enemy, "diminuir_poder", duration, 10);
            sleepms(2);
         endforeach
         break;
      "Alegrar":
         var allies := ListMobilesNearLocationEx(who.x, who.y, LIST_IGNORE_Z, 8, LISTEX_FLAG_NORMAL|LISTEX_FLAG_HIDDEN);
         foreach ally in allies
            if (CheckifisAlly(who, ally))
               TS_StartTimer(ally, "healovertime", duration, 1);
            endif
            sleepms(2);
         endforeach
         break;
      "Enfurecer":
         var enemies := CheckifisAlly(who, ListMobilesNearLocationEx(who.x, who.y, LIST_IGNORE_Z, 8, LISTEX_FLAG_NORMAL|LISTEX_FLAG_HIDDEN));
         foreach enemy in enemies
            if (enemy.isA(POLCLASS_MOBILE) && !isBoss(enemy) && RandomInt(100) <= 15)
               var event := struct{"type":=SYSEVENT_ENGAGED, "source":= enemy.randomentry()};
               SendEvent(enemy, event);
            endif
            sleepms(2);
         endforeach
         break;
   endcase
endfunction