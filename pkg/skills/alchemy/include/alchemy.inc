include ":timedscripts:timedScripts";
include ":tn:cooldown";
include "include/say";

CONST DRINK_SOUND := 0x32;

function CheckIfCanDrink(character, potion, type := 0, cooldown := 0)
   if(!Accessible(character, potion))
      return 0;
   elseif (!potion.movable)
      SendSysMessageEx(character, "Você não pode beber isto.", SSM_FAIL);
      return 0;
   endif

   // Check to make sure they have at least one hand free to drink the potion
   var equippedright := GetEquipmentByLayer(character, 0x01);
   var equippedleft  := GetEquipmentByLayer(character, 0x02);
   var cfg           := ReadConfigFile(":*:itemdesc");
   var twohanded     := Cint(cfg[equippedleft.objtype].TwoHanded);
   if (equippedright and equippedleft || twohanded)
      SendSysMessageEx(character, "Você precisa de pelo menos uma mão livre!");
      return 0;
   elseif (GetObjProperty(character, "noheal") && type == "healingpot")
      SendSysMessageEx(character, "Essa poção não fará efeito nas atuais circunstâcias", SSM_FAIL);
      return 0;    
   endif

   if((type != "red") and (type != "green_potion") and type)
      if(TS_GetCooldown(character, type))
         SendSysMessageEx(character, "Você deve esperar um pouco antes de beber outra poção!", SSM_FAIL);
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
   PrintText(character, "*usa uma poção*");
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
      var lvl := Cint(GetObjProperty(potion, "strength"));
      if (lvl)
         TS_StartTimer(who, "defaultPoison", 120, lvl);
      else
         TS_StartTimer(who, "defaultPoison", 120, 1);
      endif
      SendSysMessageEx(who, "Você foi envenenado", 3, 40);
   0xbf01:
	   TS_StartTimer(who, "strPoison", 120, 2);
		SendSysMessageEx(who, "Você foi envenenado", 3, 40);
   0xbf02:
		TS_StartTimer(who, "staminaPoison", 120, 2);
		SendSysMessageEx(who, "Você foi envenenado", 3, 40);
   0xbf03:
		TS_StartTimer(who, "paralyzePoison", 120, 2);
		SendSysMessageEx(who, "Você foi envenenado", 3, 40);
   0xbf04:
		TS_StartTimer(who, "defaultPoison", 120, 3);
		SendSysMessageEx(who, "Você foi envenenado", 3, 40);
   0xbf06:
		TS_StartTimer(who, "manaPoison", 120, 5);
		SendSysMessageEx(who, "Você foi envenenado", 3, 40);
   0xbf05:
		TS_StartTimer(who, "defaultPoison", 120, 5);
		SendSysMessageEx(who, "Você foi envenenado", 3, 40);
   0xdc04:
		TS_StartTimer(who, "defaultPoison", 120, 2);
		SendSysMessageEx(who, "Você foi envenenado", 2, 40);
   endcase

endfunction
