include ":attributes:advanceCheck";


function CraftCheckSkill(who, craft_skill, difficulty, points, report:=0)

  difficulty := CDbl(difficulty);
  points := CDbl(points);
  if(difficulty == error)
    return 0;
  elseif(difficulty == 0.0)
    if (report)
	return 100;
    endif
    return 1;
  endif

  var chance;
  var effective := CDbl(AP_GetSkill(who, craft_skill));
  if(effective < 0)
    effective := 0.0;
  endif
  chance :=  35.0 + (effective - difficulty);

  if(chance < 0)
    chance := 0;
  elseif(chance > 100)
    chance := 100;
  endif

  if(report)
    return Cdbl(chance);
  endif

  if(chance >= 100)
    points := 0;
  endif
  if(points > 0)
//	CheckSkillAdvance(who, craft_skill, points, chance);
	SkillCheck(who, craft_skill, difficulty, points);
  endif
  if(RandomInt(100) < chance)
    if (report)
	return 100;
    endif
    return 1;
  else
    return 0;
  endif

endfunction

function qualitycraft(who, item, craft_skill, material := 0)
	var chance := Cint(AP_GetSkill(who, craft_skill)/20 ) + cint(GetObjProperty(who, "craftexcepmod"));
	
	if (item.quality < 1 && chance >= 1)
		chance += 15;
	endif
	
	var randomchance := RandomInt(100);
	if(randomchance < chance)
		if (item.quality >= 1)
			CraftExceptionalQuality(who, item, material);
			SendSysMessageEx(who, "Seu objeto se tornou excepcional!", SSM_INFO);
			SetName(item, item.desc);
		elseif (item.quality < 1)
			CraftGoodQuality(who, item, material);
			SendSysMessageEx(who, "Seu objeto atingiu a qualidade boa!", SSM_INFO);
			SetName(item, item.desc);
		endif
	elseif (randomchance >= 95 && item.quality == 1)
		CraftPoorQuality(who, item, material);
		SendSysMessageEx(who, "Voce danificou o objeto e sua qualidade diminui.", SSM_FAIL);
		SetName(item, item.desc);
	else
		//upar skill
			var skill := craft_config[item.objtype].skill;
			var diff := Cint(craft_config[item.objtype].difficulty);
			var pts  := Cint(craft_config[item.objtype].points);
			var modpts := 0;
			var mainSkill := CInt(AP_GetSkill(who, craft_skill));
			if(mainSkill >= skill)
				modpts := (mainSkill - (skill - 10));
			endif
			pts := pts + modpts;
			var check := Skillcheck(who, craft_skill, diff, pts);
		//fim upar skill
		
		SendSysMessageEx(who, "Voce nao obteve sucesso na melhoria do objeto.", SSM_FAIL);
	endif
endfunction

function ExceptionalCraft( who, aidskill)

  var chance := Cint(AP_GetSkill(who, craft_skill)/20 ) + cint(GetObjProperty(who, "craftexcepmod"));

  if(RandomInt(100) < chance)
    return 1;
  else
    return 0;
  endif

endfunction
