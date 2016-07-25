include "include/attributes";
include ":poisonwatcher:poisons";
include "include/statMod";
include "include/d20system";
use cfgfile;

const POISON_SOUND := 0x247;
const DRINK_SOUND := 0x32;
//const UOBJ_EMPTY_BOTTLE := 0x0f0e;
const EFFECT_BLUE_SPARKLE  := 0x373a;
const SOUND_EFFECT_CURE	:= 0x1e1;
const EFFECT_SPARKLE  := 0x374a;
const HIT_ACTION := 0x148b;
const MOVING_EFFECT_PURPLE_POTION  := 0x0f0d;
const SOUND_EFFECT_EXPLOSION	:= 0x208;
const SOUND_EFFECT_HEAL	:= 0x1f3;
const EFFECT_EXPLOSION := 0x36b0;

function checkifcandrink(character, type)
  // Check to make sure they have at least one hand free to drink the potion
  var equippedright := GetEquipmentByLayer(character, 0x01);
  var equippedleft := GetEquipmentByLayer(character, 0x02);
  var cfg := ReadConfigFile(":*:itemdesc");
  var twohanded := Cint(cfg[equippedleft.objtype].TwoHanded);
  if (equippedright and equippedleft || twohanded)
    SendSysMessage(character, "Voce precisa de pelo menos uma mao livre!");
    return 0;
  endif
  if((type != "red") and (type != "green_potion"))
    if(GetObjProperty(character, type))
      if(GetObjProperty(character, type) > ReadGameClock())
        SendSysMessage(character, "Voce deve esperar um pouco antes de beber outra pocao!");
        return 0;
      else
        SetObjProperty(character, type, ReadGameClock() + 10);
        return 1;
      endif
    else
      SetObjProperty(character, type, ReadGameClock() + 10);
       return 1;
    endif
  else
    return 1;
  endif
endfunction

function empty_bottle(character, potion, potiontype := 0)
  PlaySoundEffect(character, DRINK_SOUND);
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

    var stacknow := FindObjtypeInContainer( who.backpack, 3854 );
    if (stacknow)
	Addamount(stacknow, 1);
    else
       var pot := CreateItemInBackpack(character, UOBJ_EMPTY_BOTTLE, 1);
	pot.name := "Frasco de Vidro";
    endif

    SubtractAmount(potion, 1);
  endif
  
endfunction

function is_poison(potion)
  var poison_level := GetObjProperty(potion, "poison_level");
  return poison_level;
endfunction

function do_poison(who, potion)
  empty_bottle(who, potion);
  var bonus := cint(getObjproperty(potion, "alchemybonus"));
  var amt;
  case (potion.objtype)
    0xbf00: 
            if (canmod(who, "venenoerva"))
              if(!checksave(who, 11, "for"))
                amt := RandomDiceRoll("1d2");
		DoTempMod(who, "venenoerva", 0-amt*5, 600);
		SendSysMessage(who, "Voce foi envenenado", 3, 40);
	      else
		SendSysMessage(who, "Voce resistiu ao veneno", 3, 90);
              endif
            endif
    0xbf01:
            if (canmod(who, "venenoputrido"))
              if(!checksave(who, 10, "for"))
		DoTempMod(who, "venenoputrido", 0-1*5, 600);
		SendSysMessage(who, "Voce foi envenenado", 3, 40);
	      else
		SendSysMessage(who, "Voce resistiu ao veneno", 3, 90);
              endif
            endif
    0xbf02:
            if (canmod(who, "venenocobra"))
              if(!checksave(who, 11, "for"))
                amt := RandomDiceRoll("1d6");
		DoTempMod(who, "venenocobra", 0-amt*5, 600);
		SendSysMessage(who, "Voce foi envenenado", 3, 40);
	      else
		SendSysMessage(who, "Voce resistiu ao veneno", 3, 90);
              endif
            endif
    0xbf03:
            if (canmod(who, "venenoaranha"))
              if(!checksave(who, 16, "for"))
                amt := RandomDiceRoll("1d6");
		DoTempMod(who, "venenoaranha", 0-amt*5, 600);
		SendSysMessage(who, "Voce foi envenenado", 3, 40);
	      else
		SendSysMessage(who, "Voce resistiu ao veneno", 3, 90);
              endif
            endif

    0xbf04:
            if (canmod(who, "venenoescorpiao"))
              if(!checksave(who, 18, "for"))
                amt := RandomDiceRoll("1d6");
		DoTempMod(who, "venenoescorpiao", 0-amt*5, 60);
		SendSysMessage(who, "Voce foi envenenado", 3, 40);
	      else
		SendSysMessage(who, "Voce resistiu ao veneno", 3, 90);
              endif
            endif
    0xbf05:
            if (canmod(who, "venenowyvern"))
              if(!checksave(who, 17, "for"))
                amt := RandomDiceRoll("2d6");
		DoTempMod(who, "venenowyvern", 0-amt*5, 600);
		SendSysMessage(who, "Voce foi envenenado", 3, 40);
              else
		SendSysMessage(who, "Voce resistiu ao veneno", 3, 90);
              endif
            endif
    0xbf06:
            if (canmod(who, "venenoliche"))
              if(!checksave(who, 17, "for"))
                amt := RandomDiceRoll("2d6");
		DoTempMod(who, "venenoliche", 0-amt*5, 60);
		SendSysMessage(who, "Voce foi envenenado", 3, 40);
              else
                SendSysMessage(who, "Voce resistiu ao veneno", 3, 90);
              endif
            endif
  endcase
 // AddPoison(who, "venenoerva", 120, who, 1);
endfunction

