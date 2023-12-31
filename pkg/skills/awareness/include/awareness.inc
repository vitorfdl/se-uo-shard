include "include/say";
include ":attributes:attributes";
include ":itemUtils:itemdesc";
include ":gumps:gump_prompts";
include ":traps:traps";

include ":timedscripts:timedScripts";
include "include/facings";

function examinarAlvo(who, targ)
	if( !targ )
		SendSysMessageEx(who, "Cancelado.", SSM_FAIL);
		return 0;
	elseif ( !targ.isA(POLCLASS_MOBILE) && !targ.isA(POLCLASS_CONTAINER) && !targ.isA(POLCLASS_DOOR) )
		// SendSysMessageEx(who, "Alvo invalido.", SSM_FAIL);
		return 0;
	endif

	var chance := RandomInt(100);
	if ( GetObjProperty(targ, "Disfarcado") )
		if ( chance < 20 )
			return 0;
		endif
      
		var char := GetObjProperty(targ, "chardata");
		var name := char.FirstName;
		var last_name := char.LastName;
		if (last_name)
			name := name+" "+last_name;
		endif

		SendSysMessageEx(who, "Voc� descobriu que "+targ.name+" �, na verdade, "+name+".", SSM_INFO);
		SendSysMessageEx(who, "ATEN��O: Se voc� n�o conhece esse personagem, ignore o fato de saber o nome dele.", SSM_INFO);
	elseif ( GetObjProperty(targ, "TrapList") )
      if ( GetObjProperty(targ, "trapSpoted") )
			// SendSysMessageEx(who, "Armadilha j� foi encontrada e est� obvia.", SSM_INFO);
			return 1;
		endif

		// var diff := getItemDiff(who, targ);

		// if ( chance < diff )
		// 	failed.append(who.serial);
		// 	SetObjProperty(targ, "#awareness_fail", failed);
		// 	return 0;
		// endif

      who.SetAchievement("found_item", 1);
		// var revelar := YesNo(who, "Voc� descobriu uma armadilha, deseja revel�-la?", "Sim.", "N�o, deixarei a armadilha escondida.");
		// ExpGain(who, diff, dice_roll);
		// if (revelar)
      PrintText(targ, "*armadilha revelada por "+who.name+"*");
      TrapFound(targ, who);
		// endif
	   return 1;
	endif
endfunction

// function CalculateDHDiff(who, mobile,)
//    if (TS_GetTimerInfo(mobile, "amor_das_sombras") )
//       if (GetRegionName(mobile)["floresta"] || ListItemsNearLocationWithFlag(who.x, who.y, who.z, 1, TILEDATA_FLAG_WALL).size() > 0)
//          return 30;
//       endif
//    endif

// 	var difficulty := AP_GetStat(mobile, DEXTERITY, RETURN_BONUS) + GetSkillBonus(mobile, FURTIVIDADE);
// 	//sendsysmessage(mobile, "dificuldade de  "  + who.name + "te achar " + mobile_skill);
// 	if (GetObjProperty(mobile, "#overridehiding") != error)
// 		difficulty := cint(GetObjProperty(mobile, "#overridehiding"));
// 	endif

// 	difficulty += Distance(who, mobile);
//    // if (HaveLightNear(mobile))
//    //    difficulty -= 2;
//    // endif
// 	if (IsBehind(who, mobile.x, mobile.y))
// 		difficulty := difficulty +4;
// 	endif

// 	return difficulty;
// endfunction

function examinarArea(who, range, types := array{})
   if (range < 1)
      range := 1;
   endif
   
   if ("Mobile" in types)
      foreach mobile in ListMobilesNearLocationEX(who.x, who.y, who.z, range, LISTEX_FLAG_HIDDEN)
         if ( mobile == who || GetObjProperty(who, "IgnoreHidden") || mobile in who.party.members ); // Do Nothing
         elseif ( CheckLineOfSight(who, mobile) )
            mobile.hidden := 0;
            TS_LowerDuration(mobile, "furtividade", -1);
            TS_StartCooldown(mobile, "cant_hide", 1);
            PrintText(mobile, "*aparece*");
            SendSystemReport(who, "Voc� encontrou "+mobile.name+".", SSM_INFO);
            SendSystemReport(mobile, who.name+" te encontrou!", SSM_FAIL, mobile);
            who.SetAchievement("found_something", 1);
         endif
         sleepms(2);
      endforeach
   endif

   if ("Item" in types)
      foreach item in ListItemsNearLocation(who.x, who.y, who.z, range)
         sleepms(2);
         
         if ( !CheckLineOfSight(who, item))
            continue;
         endif

         if (GetObjProperty(item, "TrapList") && Distance(item, who) <= 2)
            if ( !GetObjProperty(item, "trapSpoted") )
               PrintText(item, "*armadilha revelada por {}*".format(who.name), SSM_INFO);
               TrapFound(item, who);
            endif
         endif

         if ( !item.invisible )
            continue;
         endif

         var custom_diff := cint(GetObjProperty(item, "FindDiff"));

         if ( item.isTrap() || item.IsLever() || custom_diff)
            item.invisible := 0;
            SendSysMessageEx(who, "Voc� encontrou "+item.desc+".", SSM_INFO);
            PrintTextAbovePrivate(item, "*!*", who);
            who.SetAchievement("found_something", 1);
         endif
      endforeach
   endif
endfunction

function getItemDiff(who, item) 
   var diff;
   var lvl := Cint(GetObjProperty(item, "level"));
   case (lvl)
      1: diff := 12;
      2: diff := 15;
      3: diff := 18;
      4: diff := 21;
      5: diff := 24;
      default: diff := 18; 
   endcase

   // if ()
   //    diff += 4;
   // endif

   return diff;
endfunction
