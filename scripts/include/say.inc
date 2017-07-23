use uo;
use unicode;

const SSM_FAIL      := 39;
const SSM_REQUEST   := 168;
const SSM_INFO      := 91;
const SSM_HELPFU    := 168;
const SSM_HARMFUL   := 37;
const SSM_INFO_IC   := 55;
const SSM_INFO_OOC  := 91;
const SSM_INFO_DEBUG:= 946;//1901;
const SSM_GM_NOTICE := 2001;

// CONSTants for texttype flags in the AI_Speak() function
CONST SPEAK_TEXTTYPE_DEFAULT:= "default";
CONST SPEAK_TEXTTYPE_WHISPER:= "whisper";
CONST SPEAK_TEXTTYPE_YELL   := "yell";

// CONSTants for doevent flag in AI_Speak() function
CONST SPEAK_DOEVENT_DISABLE := 0x0;
CONST SPEAK_DOEVENT_ENABLE  := 0x1;

CONST SYSTEM_REPORT := 117;

CONST MESSAGE_EVENTS := dictionary{
  "EVENT_NOPICKUP":= 500169,

};

function SendSysMessageEX(who, message, SSM_TYPE := _DEFAULT_TEXT_COLOR, font_number := _DEFAULT_TEXT_FONT)
	SendSysMessage(who, ""+message, font_number, SSM_TYPE);
endfunction

function SendSysMessageUNI(who, uc_text, color:=_DEFAULT_UCCOLOR, font:=_DEFAULT_UCFONT, langcode := "ENU")
  if (uc_text == error)
	  return;
  endif

  var theUCtext := array{111, 108, 225};

  print(cAscZ("olá"));
  var foi := SendSysMessageUC(who, theUCtext, langcode, 4, 55);
  print(foi);
  return SendSysMessageUC(who, theUCtext, langcode, 4, 55);
endfunction

function PrintText(above_object, uc_text, color := _DEFAULT_UCCOLOR, font := _DEFAULT_UCFONT, langcode := "ENG")
  if (uc_text == error)
	  return;
  endif

  if (above_object.concealed || above_object.hidden)
	  return 1;
  endif

  return PrintTextAboveUC(above_object, UC(uc_text), langcode, font, color);
endfunction

function PrintTextPrivate(above_object, uc_text, for_object, color := _DEFAULT_UCCOLOR, font := _DEFAULT_UCFONT, langcode := "ENG")
  if (uc_text == error)
      return;
  endif

  return PrintTextAbovePrivateUC(above_object, UC(uc_text), langcode, for_object, font, color);
endfunction

function PrivateSystemReport(above_object, text, character := 0, color := SYSTEM_REPORT)
  if (!character)
    character := above_object;
  endif
  return PrintTextAbovePrivate(above_object, text, character, 9, 2027);
endfunction

function UC(strs)
  var result := array {};
  if (TypeOf(strs) != "Array")
    result := CAscZ(CStr(strs));
  else
    foreach str in (strs)
      if (TypeOf(str) != "Array")
        str := CAscZ(CStr(str));
      endif
      result := result + str;     // POL 2003-06-19 or later required for this operation
/*      foreach elem in (str)
        result.append(elem); 
      endforeach */
    endforeach
  endif
  return result;
endfunction
