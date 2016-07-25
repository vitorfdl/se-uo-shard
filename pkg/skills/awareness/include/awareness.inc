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

	var found := 0;
	if ( GetObjProperty(targ, "Disfarcado") )
		if ( SkillCheck(who, AWARENESS, -1) > 0 )
			found:=1;
			var char := GetObjProperty(targ, "chardata");
			var name := char.FirstName;
			var last_name := char.LastName;
			if (last_name)
				name := name+" "+last_name;
			endif
			SendSysMessageEx(who, "Você descobriu que "+targ.name+" é, na verdade, "+name+".", SSM_INFO);
			SendSysMessageEx(who, "ATENCAO: Se voce nao conhece esse personagem, ignore o fato de saber o nome dele.", SSM_INFO);
		endif
	elseif ( GetObjProperty(targ, "TrapList") )
		if ( GetObjProperty(targ, "trapSpoted") )
			SendSysMessageEx(who, "Armadilha ja foi encontrada e esta obvia.", SSM_INFO);
			found:=1;
		else
		var diff;
		var lvl := Cint(GetObjProperty(targ, "level"));
		case (lvl)
			1: diff := 15;
			2: diff := 40;
				if (!TemHabilidade(who, "Caçador de Armadilhas Noviço"))
					SendSysMessageEx(who, "Voce nao entende a complexibilidade dessa armadilha.", SSM_INFO);
					return;
				endif
			3: diff := 60;
				if (!TemHabilidade(who, "Caçador de Armadilhas Aprendiz"))
					SendSysMessageEx(who, "Voce nao entende a complexibilidade dessa armadilha.", SSM_INFO);
					return;
				endif
			4: diff := 86;
				if (!TemHabilidade(who, "Caçador de Armadilhas Perito"))
					SendSysMessageEx(who, "Voce nao entende a complexibilidade dessa armadilha.", SSM_INFO);
					return;
				endif
			5: diff := 90;
				if (!TemHabilidade(who, "Caçador de Armadilhas Hábil"))
					SendSysMessageEx(who, "Voce nao entende a complexibilidade dessa armadilha.", SSM_INFO);
					return;
				endif
			6: diff := 110;
				if (!TemHabilidade(who, "Caçador de Armadilhas Mestre"))
					SendSysMessageEx(who, "Voce nao entende a complexibilidade dessa armadilha.", SSM_INFO);
					return;
				endif
			default: diff := 40; 
		endcase
			if ( SkillCheck(who, AWARENESS, diff) > 0 )
				found :=1;
				var revelar := YesNo(who, "Você descobriu uma armadilha, deseja revelá-la?", "Sim.", "Não, deixarei a armadilha escondida.");
				if (revelar)
					PrintText(targ, "*armadilha revelada por "+who.name+"*");
					TrapFound(targ, who);
				endif
			endif
		endif
	endif

	return found;
endfunction

function CalculateDHDiff(who, mobile, insneak := 0)

	var mobile_skill := CInt(AP_GetSkill(mobile, SNEAK));

	//sendsysmessage(mobile, "dificuldade de  "  + who.name + "te achar " + mobile_skill);
	if (GetObjProperty(mobile, "#overridehiding") != error)
		mobile_skill := cint(GetObjProperty(mobile, "#overridehiding"));
	endif
	var dist := Distance(who, mobile);
	var difficulty;

		if (dist >=8)
			difficulty := mobile_skill;
		elseif (dist >= 7)
			if (TemHabilidade(mobile, "Discricao"))
				difficulty := mobile_skill;
			else
				difficulty := mobile_skill*0.95;
			endif
		elseif (dist >= 6)
			if (TemHabilidade(mobile, "Discricao 2"))
				difficulty := mobile_skill;
			else
				difficulty := mobile_skill*0.9;
			endif
		elseif ( dist >= 5 )
			if (TemHabilidade(mobile, "Discricao 3"))
				difficulty := mobile_skill;
			else
				difficulty := mobile_skill*0.85;
			endif
		elseif ( dist >= 4 )
			if (TemHabilidade(mobile, "Discricao 4"))
				difficulty := mobile_skill;
			else
				difficulty := mobile_skill*0.8;
			endif
		elseif ( dist >= 3 )
				difficulty := CInt(mobile_skill * 0.7);
		elseif ( dist >= 2 )
			difficulty := CInt(mobile_skill * 0.6);
		elseif ( dist >= 1 )
			difficulty := CInt(mobile_skill * 0.5);
		else
			difficulty := CInt(mobile_skill * 0.25);
		endif
		
		if (IsBehind(mobile, who.x, who.y) && !insneak)
			difficulty := difficulty +10;
		elseif (!IsBehind(mobile, who.x, who.y) && insneak)
			difficulty := difficulty +10;
		endif

	return CInt(difficulty);

endfunction

function examinarArea(who)
	var range := CInt( AP_GetSkill(who, AWARENESS) / 10 ) + 1;

	var found := 0;
	foreach mobile in ListMobilesNearLocationEX(who.x, who.y, who.z, range, LISTEX_FLAG_HIDDEN)
		if ( mobile == who ); // Do Nothing
		elseif ( !mobile.npctemplate && CInt(GetObjProperty(who, "IgnoreHidden")) ); // Do Nothing
		else
			if ( CheckLineOfSight(who, mobile) )
				if (Distance(mobile, who) > range)
					continue;
				endif
				var difficulty := CalculateDHDiff(who, mobile);
				
				if ( SkillCheck(who, AWARENESS, difficulty) > 5 )
					found := found+1;
					mobile.hidden := 0;
					PrintText(mobile, "*aparece*");
					PrintTextPrivate(who, "Você encontrou "+mobile.name+".", who, SSM_INFO);
					PrintTextPrivate(mobile, who.name+" te encontrou!", mobile, SSM_FAIL);
				elseif ( SkillCheck(who, AWARENESS, difficulty) > -10 )
					PrintTextPrivate(who, "Você se sente observado", who, SSM_INFO);
				endif
			endif
		endif
		sleepms(2);
	endforeach

	foreach item in ListItemsNearLocation(who.x, who.y, who.z, range)
		if ( item.invisible )
			if ( item.isTrap() )
				if ( SkillCheck(who, AWARENESS, -1) > 0 )
					found := found+1;
					item.invisible := 0;
					SendSysMessageEx(who, "Voce encontrou "+item.desc+".", SSM_INFO);
					PrintTextAbovePrivate(item, "*!*", who);
				endif
			elseif (TemHabilidade(who, "Notar Objetos"))
				var diff := CInt(GetObjProperty(item, "FindDiff"));
				if ( diff )
					if ( SkillCheck(who, AWARENESS, diff) > 0 )
						found := found+1;
						item.invisible := 0;
						SendSysMessageEx(who, "Voce encontrou "+item.desc+".", SSM_INFO);
						PrintTextAbovePrivate(item, "*!*", who);						
					endif
				endif
			endif
		endif
		sleepms(2);
	endforeach

	return found;
endfunction
