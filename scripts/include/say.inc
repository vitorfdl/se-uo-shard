use uo;
use unicode;

const SSM_FAIL      := 41;
const SSM_REQUEST   := 168;
const SSM_INFO      := 91;
const SSM_HELPFUL   := 168;
const SSM_HARMFUL   := 37;
const SSM_INFO_IC   := 55;
const SSM_INFO_OOC  := 91;
const SSM_INFO_DEBUG:= 946;//1901;
const SSM_GM_NOTICE := 2001;
const SSM_DEFAULT_COLOR := 2001;

// CONSTants for texttype flags in the AI_Speak() function

CONST SPEAK_TEXTTYPE_DEFAULT:= "default";
CONST SPEAK_TEXTTYPE_WHISPER:= "whisper";
CONST SPEAK_TEXTTYPE_YELL   := "yell";

// CONSTants for doevent flag in AI_Speak() function
CONST SPEAK_DOEVENT_DISABLE := 0x0;
CONST SPEAK_DOEVENT_ENABLE  := 0x1;

CONST SYSTEM_REPORT := 493;
CONST SYSTEM_HEAL := 166;

/*
 * SendSysMessageEX: Sends a system message to a character using the specified color and font.
 * Parameters:
 *   who: The character to whom the message will be sent.
 *   message: The message to be sent.
 *   SSM_TYPE: The color of the message (default is _DEFAULT_UCCOLOR).
 *   font_number: The font of the message (default is _DEFAULT_TEXT_FONT).
 * Returns: Nothing.
 */
function SendSysMessageEX(who, message, SSM_TYPE := _DEFAULT_UCCOLOR, font_number := _DEFAULT_TEXT_FONT)
	SendSysMessageUNI(who, ""+message, font_number, SSM_TYPE);
endfunction

/*
 * Document the function
 * Parameters:
 *   above_object: The object above which the text will be displayed.
 *   uc_text: The text to be displayed in Unicode format.
 *   color: The color of the text (default is _DEFAULT_UCCOLOR).
 *   font: The font of the text (default is _DEFAULT_UCFONT).
 *   langcode: The language code (default is "ENG").
 * Returns: The result of the PrintTextAboveUC function.
 */
function PrintText(above_object, uc_text, color := _DEFAULT_UCCOLOR, font := _DEFAULT_UCFONT, langcode := "ENG")
   if (uc_text == error)
      return;
   endif

   if (above_object.concealed || above_object.hidden)
      return 1;
   endif

   return PrintTextAboveUC(above_object, UC(uc_text), langcode, font, color);
endfunction

/*
 * PrintTextPrivate: Displays a private text message above an object in a specified color and font.
 * Parameters:
 *   above_object: The object above which the text will be displayed.
 *   uc_text: The text to be displayed in Unicode format.
 *   for_object: The object for which the text will be displayed.
 *   color: The color of the text (default is _DEFAULT_UCCOLOR).
 *   font: The font of the text (default is _DEFAULT_UCFONT).
 *   langcode: The language code (default is "ENG").
 * Returns: The result of the PrintTextAbovePrivateUC function.
 */
function PrintTextPrivate(above_object, uc_text, for_object, color := _DEFAULT_UCCOLOR, font := _DEFAULT_UCFONT, langcode := "ENG")
   if (uc_text == error)
         return;
   endif

   return PrintTextAbovePrivateUC(above_object, UC(uc_text), langcode, for_object, font, color);
endfunction

/*
 * SendSystemReport: Sends a system report message to a character or all characters.
 * Parameters:
 *   above_object: The object above which the message will be displayed.
 *   text: The message to be displayed.
 *   color: The color of the message (default is SYSTEM_REPORT).
 *   character: The character to whom the message will be sent (default is 0).
 * Returns: Nothing.
 */
function SendSystemReport(above_object, text, color := SYSTEM_REPORT, character := 0)
   if (above_object.npctemplate)
      return;
   elseif (!character)
      character := above_object;
   endif

   if (character == "all")
      return PrintText(above_object, text, 9, color);
   endif
   return PrintTextAbovePrivateUC(above_object, UC(text), "ENG", character, 0x10, color);
endfunction

/*
 * GetInput: Retrieves input from a character with a prompt message.
 * Parameters:
 *   character: The character from which to retrieve input.
 *   item: A placeholder, pass anything.
 *   uc_prompt: The prompt message in Unicode format.
 *   langcode: The language code (default is "ENG").
 * Returns: The input from the character.
 */
function GetInput(character, item, uc_prompt, langcode := "ENG") // item is a placeholder, pass anything
   var request := RequestInputUC(character, item, UC(uc_prompt), langcode);
   if (!request) return; endif

   return CChrZ(request.uc_text);
endfunction


function SendSysMessageUNI(character, uc_text, font := _DEFAULT_UCFONT, color := _DEFAULT_UCCOLOR, langcode := "ENG")
  if (uc_text == error)
	  return;
  endif

  return SendSysMessageUC(character, UC(uc_text), langcode, font, color);
endfunction

/*
 * UC: Converts a string or an array of strings to an array of ASCII values.
 *     If a single string is passed, it converts it to an array of ASCII values.
 *     If an array of strings is passed, it converts each string to an array of ASCII values and concatenates them.
 *     Returns: array of ASCII values.
 */
function UC(strs)
  var result := array {};

  if (TypeOf(strs) != "Array")
    result := CAscZ(CStr(strs));
  else
    foreach str in (strs)
      if (TypeOf(str) != "Array")
        str := CAscZ(CStr(str));
      endif
      result += str;
    endforeach
  endif

  return result;
endfunction
