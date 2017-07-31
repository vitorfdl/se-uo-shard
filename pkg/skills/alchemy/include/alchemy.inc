include ":timedScripts:timedScripts";
include ":tn:cooldown";
include "include/say";

CONST DRINK_SOUND := 0x32;

function CheckIfCanDrink(character, potion, type := 0, cooldown := 0)
  if(!Accessible(who, potion))
		return 0;
	elseif (!potion.movable)
		SendSysMessageEx(who, "Voce nao pode beber isto.", SSM_FAIL);
		return 0;
	endif

  // Check to make sure they have at least one hand free to drink the potion
  var equippedright := GetEquipmentByLayer(character, 0x01);
  var equippedleft  := GetEquipmentByLayer(character, 0x02);
  var cfg           := ReadConfigFile(":*:itemdesc");
  var twohanded     := Cint(cfg[equippedleft.objtype].TwoHanded);
  if (equippedright and equippedleft || twohanded)
    SendSysMessage(character, "Voce precisa de pelo menos uma mao livre!");
    return 0;
  elseif (GetObjProperty(who, "noheal") && type == "healingpot")
		SendSysMessageEx(who, "Essa pocao nao fara efeito nas atuais circunstacias", SSM_FAIL);
    return 0;    
  endif

  if((type != "red") and (type != "green_potion") and type)
    if(TS_GetCooldown(character, type))
        SendSysMessage(character, "Voce deve esperar um pouco antes de beber outra pocao!");
        return 0;
     elseif (cooldown > 0 )
        TS_StartCooldown(character, type, cooldown);
        return 1;
     endif
  else
    return 1;
  endif
endfunction

function EmptyBottle(character, potion, potiontype := 0)
  PlaySoundEffect(character, DRINK_SOUND);
  PrintText(character, "*usa uma pocao*");
  var goles;
  if (!GetObjProperty(potion, "goles"))
    goles := 3;
    SetObjProperty(potion, "goles", goles);
  else
    goles := cint(GetObjProperty(potion, "goles"));
  endif
  
  if(potiontype == 1)  //eh uma poçao de 1 so gole
   goles := -1;
  endif
  
  if (goles > 0)
    goles := goles-1;
    SetObjProperty(potion, "goles", goles-1);
  else

    var stacknow := FindObjtypeInContainer( character.backpack, 3854 );
    if (stacknow)
      Addamount(stacknow, 1);
    else
       var pot := CreateItemInBackpack(character, 0x0f0e, 1);
      pot.name := "Frasco de Vidro";
    endif

    EraseObjProperty(potion, "goles");
    SubtractAmount(potion, 1);
  endif
  
endfunction

//essa funcao so se mantem pra checar os kindlings. se nao quiser checar isso use .isReagent();
function isotherreagent(what)
	case(what.objtype)
		0xf7a:  return 1;
		0xf7b:  return 1;
		0xf84:  return 1;
		0xf85:  return 1;
		0xf86:  return 1;
		0xf88:  return 1;
		0xf8c:  return 1;
		0xf8d:  return 1;
		0xef1a: return 1;
		0xef1b: return 1;
		0xef1c: return 1;
		0xef1d: return 1;
		0xf7d: return 1;
		0xef1e: return 1;
		0xf87: return 1;
		0xf79: return 1;
		0xf91: return 1;
		0xf89: return 1;
		0xf82: return 1;
		0xF81: return 1;
		0xF78: return 1;
		0xF8F: return 1;
		0xF90: return 1;
    
		0xee25: return 1;
		0xee26: return 1;
		0xee27: return 1;
		0xee28: return 1;
		0xfc5:  return 1;
		0x0dea: return 1;

		0x1F91: return 1;
	endcase
	return 0;
endfunction

function GetReagName(objtype)

  if (objtype ==  0xf7a)
    return "perola negra";
  elseif (objtype == 0xF78 )
   return "asa de morcego";
  elseif (objtype == 0xf79 )
   return "pedra negra";
  elseif (objtype == 0xf7b )
    return "musgo vermelho";
  elseif (objtype == 0xf7c )
    return "cria do sangue";
  elseif (objtype == 0xF7D)
   return "sangue fresco";
  elseif (objtype == 0xF7E)
   return "ossos";
  elseif (objtype == 0xF7F)
   return "enxofre";
  elseif (objtype == 0xF80)
   return "ossos demoniacos";
  elseif (objtype == 0xF81 )
   return "terra fertil";
  elseif (objtype == 0xf82 )
   return "sangue de dragao";
  elseif (objtype == 0xf83 )
   return "cogumelo negro";
  elseif (objtype == 0xf84)
    return "alho";
  elseif (objtype == 0xf85)
    return "ginseng";
  elseif ( objtype ==  0xf86)
    return "raiz de mandragora";
  elseif (objtype == 0xf87 )
   return "olho da salamandra";
  elseif ( objtype == 0xf88)
    return "solanaceae";
  elseif (objtype == 0xf89 )
   return "obsidiana";
  elseif ( objtype == 0xf8c)
    return "cinzas de enxofre";
  elseif (objtype == 0xf8d)
    return "teia";
  elseif (objtype == 0xf8a)
    return "gusa";
  elseif (objtype == 0xf8b)
    return "pedra pomes";
  elseif (objtype == 0xf8e)
    return "escamas de cobra";
  elseif (objtype == 0xf8f)
    return "cinzas vulcanicas";
  elseif (objtype == 0xF90 )
   return "madeira morta";
  elseif (objtype == 0xf91 )
   return "coracao do vorme";
    
  elseif ( objtype == 0xef1a)
   return "glandula de wyvern";
  elseif (objtype == 0xef1b)
   return "glandula de escorpiao";
  elseif (objtype == 0xef1c)
   return "glandula de aranha";
  elseif (objtype == 0xef1d)
   return "glandula de cobra";
  elseif (objtype == 0xef1e)
   return "po de liche";

  elseif (objtype == 0xF8F )
   return "poeira de cripta";
  elseif (objtype == 0xee27 )
   return "cascas vermelhas";
  elseif (objtype == 0xee28 )
   return "cascas verdes";
  elseif (objtype == 0xee26 )
   return "cascas marrom";
  elseif (objtype == 0xee25 )
   return "cascas amarelas";
  elseif (objtype == 0xfc5 )
   return "conchas";
  elseif (objtype == 0x0dea )
   return "cinzas";
  elseif (objtype == 0x1F91 )
   return "copo de agua";

  endif
endfunction


function is_poison(potion)
  var poison_level := GetObjProperty(potion, "poison_level");
  return poison_level;
endfunction

function do_poison(who, potion)
  EmptyBottle(who, potion);
  case (potion.objtype)
    0xbf00: 
     var lvl := Cint(GetObjProperty(potion, "strength"));
     if (lvl)
      TS_StartTimer(who, "defaultPoison", 120, lvl);
     else
        TS_StartTimer(who, "defaultPoison", 120, 1);
     endif
		SendSysMessage(who, "Voce foi envenenado", 3, 40);
    0xbf01:
		TS_StartTimer(who, "strPoison", 120, 2);
		SendSysMessage(who, "Voce foi envenenado", 3, 40);
    0xbf02:
		TS_StartTimer(who, "staminaPoison", 120, 2);
		SendSysMessage(who, "Voce foi envenenado", 3, 40);
    0xbf03:
		TS_StartTimer(who, "paralyzePoison", 120, 2);
		SendSysMessage(who, "Voce foi envenenado", 3, 40);

    0xbf04:
		TS_StartTimer(who, "defaultPoison", 120, 3);
		SendSysMessage(who, "Voce foi envenenado", 3, 40);
    0xbf06:
		TS_StartTimer(who, "manaPoison", 120, 5);
		SendSysMessage(who, "Voce foi envenenado", 3, 40);
    0xbf05:
		TS_StartTimer(who, "defaultPoison", 120, 5);
		SendSysMessage(who, "Voce foi envenenado", 3, 40);
    0xdc04:
		TS_StartTimer(who, "defaultPoison", 120, 2);
		SendSysMessage(who, "Voce foi envenenado", 2, 40);
  endcase

endfunction
