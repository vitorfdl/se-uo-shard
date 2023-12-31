include ":timedscripts:timedScripts";
include ":tn:cooldown";
include "include/utils";
include "include/say";

CONST DRINK_SOUND := 0x32;

function CheckIfCanDrink(character, potion, type := 0, cooldown := 0)
   if(!Accessible(character, potion))
      return 0;
   elseif (!potion.movable)
      SendSysMessageEx(character, "Voc� n�o pode beber isto.", SSM_FAIL);
      return 0;
   endif

   // Check to make sure they have at least one hand free to drink the potion

   if (GetObjProperty(character, "noheal") && type == "healingpot")
      SendSysMessageEx(character, "Essa po��o n�o far� efeito nas atuais circunst�cias", SSM_FAIL);
      return 0;    
   endif

   if((type != "red") and (type != "green_potion") and type)
      if(TS_GetCooldown(character, type))
         SendSysMessageEx(character, "Voc� deve esperar um pouco antes de beber outra po��o!", SSM_FAIL);
         return 0;
      elseif (cooldown > 0 )
         TS_StartCooldown(character, type, cooldown);
      endif
   endif

   var equippedright := GetEquipmentByLayer(character, 0x01);
   var equippedleft  := GetEquipmentByLayer(character, 0x02);
   var cfg           := ReadConfigFile(":*:itemdesc");
   var twohanded     := Cint(cfg[equippedleft.objtype].TwoHanded);
   
   if (equippedright and equippedleft || twohanded)
      TS_StartCooldown(character, "noattack", 2);
   endif

   return 1;
endfunction

function EmptyBottle(character, potion, potiontype := 0)
   PlaySoundEffect(character, DRINK_SOUND);
   PrintText(character, "*usa uma po��o*");
   var goles;
   if (!GetObjProperty(potion, "goles"))
      goles := 3;
      SetObjProperty(potion, "goles", goles);
   else
      goles := cint(GetObjProperty(potion, "goles"));
   endif
  
   if(potiontype == 1)  //eh uma po�ao de 1 so gole
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
      endif

      EraseObjProperty(potion, "goles");
      SubtractAmount(potion, 1);
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
      var lvl := Cint(GetObjProperty(potion, "level"));
      TS_StartTimer(who, "defaultPoison", 15, _ifOr(lvl, 1));
      SendSysMessageEx(who, "Voc� foi envenenado", 3, 40);
   0xbf01:
	   TS_StartTimer(who, "strPoison", 120, 2);
		SendSysMessageEx(who, "Voc� foi envenenado", 3, 40);
   0xbf02:
		TS_StartTimer(who, "staminaPoison", 120, 2);
		SendSysMessageEx(who, "Voc� foi envenenado", 3, 40);
   0xbf03:
		TS_StartTimer(who, "paralyzePoison", 120, 2);
		SendSysMessageEx(who, "Voc� foi envenenado", 3, 40);
   0xbf04:
		TS_StartTimer(who, "defaultPoison", 120, 3);
		SendSysMessageEx(who, "Voc� foi envenenado", 3, 40);
   0xbf06:
		TS_StartTimer(who, "manaPoison", 120, 5);
		SendSysMessageEx(who, "Voc� foi envenenado", 3, 40);
   0xbf05:
		TS_StartTimer(who, "defaultPoison", 120, 5);
		SendSysMessageEx(who, "Voc� foi envenenado", 3, 40);
   0xdc04:
		TS_StartTimer(who, "defaultPoison", 120, 2);
		SendSysMessageEx(who, "Voc� foi envenenado", 2, 40);
   endcase

endfunction
