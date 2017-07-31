include "include/say";
include ":attributes:attributes";
include ":itemUtils:itemdesc";
include ":gumps:yesno";
include ":traps:traps";
include ":charactercreation:habilidades";
include "include/facings";

function examinarAlvo(who)
	SendSysMessageEx(who, "Selecione um alvo.", SSM_REQUEST);
	var targ := Target(who);
	if( !targ )
		SendSysMessageEx(who, "Cancelado.", SSM_FAIL);
		return 0;
	elseif ( !targ.isA(POLCLASS_MOBILE) && !targ.isA(POLCLASS_CONTAINER) && !targ.isA(POLCLASS_DOOR) )
		SendSysMessageEx(who, "Alvo invalido.", SSM_FAIL);
		return 0;
	endif

	var dice_roll := rollAttrDice(who, WISDOM) +  + getProficiencyBonus(who, "Percepção");

	if ( GetObjProperty(targ, "Disfarcado") )
		if ( dice_roll < 20 )
			return 0;
		endif
		var char := GetObjProperty(targ, "chardata");
		var name := char.FirstName;
		var last_name := char.LastName;
		if (last_name)
			name := name+" "+last_name;
		endif
		SendSysMessageEx(who, "Você descobriu que "+targ.name+" é, na verdade, "+name+".", SSM_INFO);
		SendSysMessageEx(who, "ATENCAO: Se voce nao conhece esse personagem, ignore o fato de saber o nome dele.", SSM_INFO);
	elseif ( GetObjProperty(targ, "TrapList") )
		var failed := GetObjProperty(targ, "#awareness_fail");
		if (who.serial in failed)
			return 0;
		elseif ( GetObjProperty(targ, "trapSpoted") )
			SendSysMessageEx(who, "Armadilha ja foi encontrada e esta obvia.", SSM_INFO);
			return 1;
		endif

		var diff;
		var lvl := Cint(GetObjProperty(targ, "level"));
		case (lvl)
			1: diff := 15;
			2: diff := 18;
			3: diff := 23;
			4: diff := 26;
			5: diff := 30;
			default: diff := 18; 
		endcase

		if ( dice_roll < diff )
			failed.append(who.serial);
			SetObjProperty(targ, "#awareness_fail", failed);
			return 0;
		endif

		var revelar := YesNo(who, "Você descobriu uma armadilha, deseja revelá-la?", "Sim.", "Não, deixarei a armadilha escondida.");
		ExpGain(who, diff, dice_roll);
		if (revelar)
			PrintText(targ, "*armadilha revelada por "+who.name+"*");
			TrapFound(targ, who);
		endif
	endif
	
	return 1;
endfunction

function CalculateDHDiff(who, mobile, insneak := 0)
	var difficulty := 9 + AP_GetStat(mobile, attribute, RETURN_BONUS) + getProficiencyBonus(who, "Furtividade");

	//sendsysmessage(mobile, "dificuldade de  "  + who.name + "te achar " + mobile_skill);
	if (GetObjProperty(mobile, "#overridehiding") != error)
		mobile_skill := cint(GetObjProperty(mobile, "#overridehiding"));
	endif

	difficulty += Distance(who, mobile);

	if (IsBehind(mobile, who.x, who.y) && !insneak)
		difficulty := difficulty +4;
	elseif (!IsBehind(mobile, who.x, who.y) && insneak)
		difficulty := difficulty +4;
	endif

	return CInt(difficulty);
endfunction

function examinarArea(who)
	var range := AP_GetStat(mobile, attribute, RETURN_BONUS) + 4;
	var dice_roll := rollAttrDice(who, WISDOM) +  + getProficiencyBonus(who, "Percepção");
	
	var found := 0;
	foreach mobile in ListMobilesNearLocationEX(who.x, who.y, who.z, range, LISTEX_FLAG_HIDDEN)
		if ( mobile == who ); // Do Nothing
		elseif ( mobile.npctemplate && !GetObjProperty(who, "IgnoreHidden") ); // Do Nothing
		elseif ( CheckLineOfSight(who, mobile) && Distance(mobile, who) < range )
			var difficulty := CalculateDHDiff(who, mobile);
			
			if ( dice_roll >= difficulty )
				ExpGain(who, difficulty, dice_roll);
				found := found+1;
				mobile.hidden := 0;
				PrintText(mobile, "*aparece*");
				PrivateSystemReport(who, "Você encontrou "+mobile.name+".", who, SSM_INFO);
				PrivateSystemReport(mobile, who.name+" te encontrou!", mobile, SSM_FAIL);
			elseif ( dice_roll >= difficulty-2 )
				PrivateSystemReport(who, "*Você se sente observado*", who, SSM_INFO);
			endif
		endif
		sleepms(2);
	endforeach

	foreach item in ListItemsNearLocation(who.x, who.y, who.z, range)
		sleepms(2);
		if ( !item.invisible )
			continue;
		endif
		if ( item.isTrap() && dice_roll > BASE_DIFFICULTY)
			ExpGain(who, "minor");
			found := found+1;
			item.invisible := 0;
			SendSysMessageEx(who, "Voce encontrou "+item.desc+".", SSM_INFO);
			PrintTextAbovePrivate(item, "*!*", who);
		elseif (GetObjProperty(item, "FindDiff") && dice_roll >= CInt(GetObjProperty(item, "FindDiff"))
			ExpGain(who, CInt(GetObjProperty(item, "FindDiff"), dice_roll);
			found := found+1;
			item.invisible := 0;
			SendSysMessageEx(who, "Voce encontrou "+item.desc+".", SSM_INFO);
			PrintTextAbovePrivate(item, "*!*", who);						
		endif
	endforeach

	return found;
endfunction
