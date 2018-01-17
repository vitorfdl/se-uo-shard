
use uo;

include ":gumps:gumps";
include ":gumps:gumps_ex";
include "include/client";

function AutoCheckBox(mobile, prompt, options, returnKeys := 0)

	var sizex;
	var sizey;

	var columns := options.size()/20;
	if ( columns <= 1 )
		sizey := 40 + options.size()*20;
		if (sizey > 540)
			sizey := 540;
		endif
		if (sizey < 160)
			sizey := 160;
		endif
		sizex := 200;
		if ( (options.size() % 20) > 0)
			sizex := sizex + 150;
		endif
	else
		sizey := 540;
		sizex := 40 + cint(columns) * 150;
		if ( (options.size() % 20) > 0)
			sizex := sizex + 150;
		endif
		if (sizex > 800)
			sizex := 800;
		endif
		
	endif

	var gump := GFCreateGump(40, 40);
	GFClosable(gump, 0);
    GFSetID(gump, GUMPID_TNGUMPS);
	GFResizePic(gump, 0, 0, 3600, sizex, sizey);
	GFResizePic(gump, 15, 15, 0x13BE, sizex-30, sizey-30);
	var y_pos := 20;
	foreach line in (GFWordWrap(prompt, 360))
		GFTextLine(gump, 20, y_pos, 1153, line);
		y_pos := y_pos+20;
		SleepMS(2);
	endforeach

	GFSetRadioGroup(gump, 1);
	var indice := 1;
	y_pos := 70;
	var first := 1;
	
	var y := 0;
	var x := 0;
	while(y <= columns)
		var maxx := 20;
		if (y == columns)
			maxx := options.size() % 20;
		endif
		while (x < maxx)
			GFTextLine(gump, 60 + y*150, y_pos + x*20, 1153, options[indice]);
			GFCheckBox(gump, 30 + y*150, y_pos + x*20, 210, 211, first, indice);
			first := 0;
			indice := indice+1;
			x := x + 1;
		endwhile
		x := 0;
		y := y + 1;
		indice := indice + 1;
	endwhile

	//foreach text in options
	//	GFTextLine(gump, 60, y_pos + indice*20, 1153, options[indice]);
	//	GFCheckBox(gump, 30, y_pos + indice*20, 210, 211, first, indice);
	//	first := 0;
	//	indice := indice+1;
	//endforeach
	
	GFAddButton(gump, sizex-70, sizey-60, 4502, 4502, 1, 999 );
	GFTextLine(gump, sizex-110, sizey-50, 1153, "Próximo");
  
	var ret := array;
  
	var input := GFSendGump(mobile, gump);
	if ( input[0] == 999 )
		 foreach key in (input.keys)
        		if (options[key] )
					if (returnKeys)
						ret.append(key);
					else
						ret.append(options[key]);
					endif
        		endif
     		endforeach
     		return ret;
  	else
      return 0;
	endif	

endfunction

/* CheckBoxGump cria um gump com varias checkbox onde pode retornar varios valores clicados
   RadioGump cria um gump com varias radiosbuttons, mas apenas um botao pode ser retornado */

function CheckBoxGump(mobile, sizex, sizey, prompt:="", options, opts := 0)
   if (!sizex) sizex := 200; endif;
   if (!sizey) sizey := 125 + (options.size() * 20); endif;
   
    
	var gump := GFCreateGump(250, 200);
	GFClosable(gump, 0);
    GFSetID(gump, GUMPID_TNGUMPS);
	GFResizePic(gump, 0, 0, 3600, sizex, sizey);
	GFResizePic(gump, 15, 15, 0x13BE, sizex-30, sizey-30);
	
	var y_pos := 20;
	foreach line in (GFWordWrap(prompt, 360))
		GFTextLine(gump, 20, y_pos, 1153, line);
		y_pos := y_pos+20;
		SleepMS(2);
	endforeach
	
  GFSetRadioGroup(gump, 1);
  var indice := 1;
  y_pos := 30;
  var first := 0;
  foreach text in options
    GFTextLine(gump, 60, y_pos + indice*20, 1153, text);
    if (opts)
      first := opts[indice];
    endif
    GFCheckBox(gump, 30, y_pos + indice*20, 210, 211, first, indice);
    first := 0;
    indice := indice+1;
  endforeach
	
    GFAddButton(gump, sizex-70, sizey-60, 4502, 4502, 1, 999 );
    GFTextLine(gump, sizex-110, sizey-50, 1153, "Próximo");
    
  var ret := array;
  
	var input := GFSendGump(mobile, gump);
	if ( input[0] == 999 )
		 foreach key in (input.keys)
        if (options[key]  )
          ret.append(options[key]);
        endif
     endforeach
     return ret;
  else
      return 0;
	endif	

endfunction

function RadioGump(mobile, sizex, sizey, prompt:="", options, returnkeys := 0, first := 1)
   if (!sizex) sizex := 200; endif;
   if (!sizey) sizey := 120 + (options.size() * 20); endif;

	var gump := GFCreateGump(250, 200);
	GFClosable(gump, 0);
	GFResizePic(gump, 0, 0, 3600, sizex, sizey);
	GFResizePic(gump, 15, 15, 0x13BE, sizex-30, sizey-30);
	
	var y_pos := 20;
	foreach line in (GFWordWrap(prompt, 360))
		GFTextLine(gump, 20, y_pos, 1153, line);
		y_pos := y_pos+20;
		SleepMS(2);
	endforeach
	
  GFSetRadioGroup(gump, 1);
  var indice := 1;
  y_pos := 30;
  foreach text in options
    GFTextLine(gump, 60, y_pos + indice*20, 1153, text);
    GFRadioButton(gump, 30, y_pos + indice*20, 210, 211, (indice == first), indice);
    indice := indice+1;
  endforeach
	
  GFAddButton(gump, sizex-70, sizey-60, 4502, 4502, 1, 999 );
	GFTextLine(gump, sizex-110, sizey-50, 1153, "Próximo");
  
	var input := GFSendGump(mobile, gump);
	if ( input[0] == 999 )
		 foreach key in (input.keys)
        if (options[key])
		    if (returnkeys)
				return key;
			 endif
          return options[key];
        endif
     	endforeach
  else
      return 0;
	endif	
  
endfunction

  
function BigRequestGump(who, question := "")
	var gump := GFCreateGump();
    GFSetID(gump, GUMPID_TNGUMPS);
	GFDisposable(gump, 0);
	GFResizePic(gump, 15, 50, 2620, 310, 380);
	
	var num_lines := 19;
	var msg_array := array;
	var y_pos := 55;
	var i;
	for ( i:=1; i<=num_lines; i:=i+1 )
		msg_array.Append(GFTextEntry(gump, 20, y_pos, 595, 20, 2100, ""));
		y_pos := y_pos+19;
	endfor

	GFResizePic(gump, 15, 20, GFCfgConst("Defaults", "ForeGround"), 310, 30);
	GFResizePic(gump, 15, 431, GFCfgConst("Defaults", "ForeGround"), 310, 30);

	GFAddButton(gump, 285, 441, 4011, 4030, GF_CLOSE_BTN, 20);
	GFTextLine(gump, 225, 441, 2100, "Terminar");
	GFTextLine(gump, 34, 23, 2100, question);

	var input := GFSendGump(who, gump);

	if (input[0] == 20)
		var i;
		for ( i:=1; i<=msg_array.Size(); i:=i+1 )
			var line := GFExtractData(input, msg_array[i]);
      //sendsysmessage(who, "extraindo linha " + line);
			if ( line )
				line := line + " ";
				msg_array[i] := line;
			else
				msg_array[i] := "";
			endif
      //sendsysmessage(who, "linha final " + line);
		endfor
	
		//Clear off empty lines.
		for ( i := msg_array.Size(); i>=1; i:=i-1 )
			if ( Len(msg_array[i]) <= 0 )
				msg_array.Erase(i);
			endif
			SleepMS(2);
		endfor
		
		var text := "";
		for ( i:=1; i<=msg_array.Size(); i:=i+1 )
			text := text + msg_array[i];
			SleepMS(2);
		endfor
		
		return text;
	endif
endfunction


function SmallRequestGump(who, question := "", addBR := 0)
	var gump := GFCreateGump();
   GFSetID(gump, GUMPID_TNGUMPS);
	GFResizePic(gump, 15, 50, 2620, 150, 100);
	
	var num_lines := 5;
	var msg_array := array;
	var y_pos := 55;
	var i;
	for ( i:=1; i<=num_lines; i:=i+1 )
		msg_array.Append(GFTextEntryLimited(gump, 20, y_pos, 595, 20, 2100, "", 0, 22));
		y_pos := y_pos+19;
	endfor

	GFResizePic(gump, 15, 20, GFCfgConst("Defaults", "ForeGround"), 150, 30);
	GFResizePic(gump, 15, 150, GFCfgConst("Defaults", "ForeGround"), 150, 30);

	GFAddButton(gump, 130, 150, 4011, 4030, GF_CLOSE_BTN, 20);
	GFTextLine(gump, 75, 150, 2100, "Termina");
	GFTextLine(gump, 35, 20, 2100, question);

	var input := GFSendGump(who, gump);

	if (input[0] == 20)
		var i;
		for ( i:=1; i<=msg_array.Size(); i:=i+1 )
			var line := GFExtractData(input, msg_array[i]);
			if ( line )
				line := line + " ";
				if (addBR)
					msg_array[i] := line + "<br> ";
				else
					msg_array[i] := line;
				endif
			else
				msg_array[i] := "";
			endif
		endfor
	
		//Clear off empty lines.
		for ( i:=msg_array.Size(); i>=1; i:=i-1 )
			if ( Len(msg_array[i]) <= 0 )
				msg_array.Erase(i);
			endif
			SleepMS(2);
		endfor
		
		var text := "";
		for ( i:=1; i<=msg_array.Size(); i:=i+1 )
			text := text + msg_array[i];
			//sendsysmessage(who, " " + text);
			SleepMS(2);
		endfor
		
		return text;
	endif
endfunction

   // var questions := array{ 
   //    struct{"title" := "Entre com texto", "marked" := "Seu texto"}, 
   //    struct{"title" := "Entre com texto"},
   //    struct{"title" := "Select an Option?", "marked" := 3, "radio" := array{ "Dia", "Noite", "feliz"}},
   //    struct{"title":= "Is a checkbox", "checkbox" := array{"teste", "teste2"}, "marked":= array{1}},
   //    };
   // var res:= QuestionsGump(who, questions);
   // var answer_1 := res[1];
function QuestionsGump(who, questions := array{})
	var total_height := (questions.size() * 2) * 20;
   foreach quest in questions
      if (quest.radio)
         total_height += cint(quest.radio.size() / 5) * 25;
      elseif (quest.checkbox)
         total_height += cint(quest.checkbox.size() / 5) * 25;
      else
         total_height += 20;
      endif
      sleepms(2);
   endforeach

	var gump := GFCreateGump();
   GFSetID(gump, GUMPID_TNGUMPS);
	GFResizePic(gump, 250, 250, cint(0x9C40), 540, 100+total_height);

	var entries := dictionary{};
	var x := 270;
	var y := 270;
	foreach quest in questions
		var x_plus := x;
		GFTextLine(gump, x, y, 2100, quest.title);
		y += 18;
		if (quest.radio)
  			GFSetRadioGroup(gump, _quest_iter);
			foreach entry in (quest.radio)
				GFTextLine(gump, x_plus+25, y+3, 1153, entry);
				GFRadioButton(gump, x_plus, y+3, 210, 211, (_entry_iter == cint(quest.marked)), _entry_iter+(_quest_iter * 100));
				x_plus += 130;
            if (_entry_iter % 4 == 0 && quest.radio[_entry_iter+1])
               y += 25;
               x_plus := x;
            endif
			endforeach
		elseif(quest.checkbox)
			foreach entry in (quest.checkbox)
				GFTextLine(gump, x_plus+25, y+3, 1153, entry);
				GFCheckBox(gump, x_plus, y+3, 210, 211, (_entry_iter in quest.marked), _entry_iter+(_quest_iter * 500));
				x_plus += 130;
           if (_entry_iter % 4 == 0 && quest.checkbox[_entry_iter+1])
               y += 25;
               x_plus := x;
            endif
			endforeach
		else
         var marked := "";
         if (quest.marked)
            marked := quest.marked;
         endif
	      GFResizePic(gump, x, y+2, 0x7752, 500, 30);
			entries[_quest_iter] := GFTextEntry(gump, x+5, y+5, 500, 20, 2100, marked);
		endif

		y += 36;
		sleepms(2);
	endforeach

	GFAddButton(gump, x+150, y, 4011, 4030, GF_CLOSE_BTN, 5); //confirm
   GFTextLine(gump, x+175, y+3, 1153, "Confirmar");
	GFAddButton(gump, x+260, y, 4011, 4030, GF_CLOSE_BTN, 1); //cancel;
   GFTextLine(gump, x+285, y+3, 1153, "Cancelar");

	var input := GFSendGump(who, gump);
	if (input[0] == 1) return 0; endif

	var res := array{};
	foreach quest in questions
		if (quest.radio)
			var result;
			foreach item in (quest.radio)
				if (_item_iter + (_quest_iter * 100) in input.keys)
					result := item;
					break;
				endif
			endforeach
			res[_quest_iter] := result;
		elseif (quest.checkbox)
			var result := array{};
			foreach item in (quest.checkbox)
				if (_item_iter + (_quest_iter * 500) in input.keys)
					result.append(item);
				endif
			endforeach
			res[_quest_iter] := result;
		else
			res[_quest_iter] := GFExtractData(input, entries[_quest_iter]);
		endif
		sleepms(2);
	endforeach

	return res;
endfunction
