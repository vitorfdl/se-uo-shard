// $Id: advanceCheck.inc 386 2005-10-03 00:22:39Z muaddiblsd $

/*===============================================================
* Current Version
* ADVANCECHECK.INC - v1.0
* Updated 9/28/2005 8:29PM
*
* -- Revision v1.0 --
* Austin:
*  Created include file
===============================================================*/

use uo;
use os;
use util;

include ":charactercreation:characters";
include ":timeutils:time";

function GetCharAge(character, factor := "days")
	var create_date := cint(GetObjProperty(character, "createdate"));
	var days        := GetTimeBetween(cint(create_date), GetTime(), factor);
	return days;
endfunction
  
function GetExpCap(who)
	if (who.npctemplate)
		return 0;
	endif

	var char_age := GetCharAge(who, "weeks") +1;
	var char_level := GetCharLevel(who) - 1; //hack
	var result := (char_age*1000) - (char_level*2000);
	return result;
endfunction

/*
 * CheckSkillAdvance(mobile, stat_name, stat_mult)
 *
 * Purpose
 * Does an advancement check on a stat.
 *
 * Parameters
 * mobile:	Mobile to do the skill advancement check on.
 * skill_name:	Name of the skill to check.
 * award_diff:	Difficulty to use for advancement award.
 * pass_chance:	Chance of getting a gain check.
 * 
 *
 * Return value
 * Returns 1
 *
 */
function ExpGain(mobile, difficulty, value := 0)
	if ( GetObjProperty(mobile, "NoGains") ) //exp gain blocked
		SkillDbgMsg(mobile, "Flag NoGains setada.");
		return 0;
	endif

  if ( GetExpCap(who) == 0)
  		SkillDbgMsg(mobile, "Chegou no maximo do limite por tempo: {}".format(GetExpCap(mobile)));
		return 0;
   endif
  
	var settings := AP_GetSettingsCfgElem("Skills");
	//verifica se ja nao subiu skills em 30 segundos
	var next_check := CInt(GetObjProperty(mobile, "#LastSkillGain")) + 10;
	if ( next_check > ReadGameClock() )
	//	printtextabove(mobile, "nao posso subir skill agora");
		SkillDbgMsg(mobile, "Nao pode subir skill ainda. Next: " + next_check + " Current: " + ReadGameClock()  );
		return 0;
	endif

	//calcula a chance de passar
	var up_amount := 0;
	if (difficulty == "major")
		up_amount := 8;
	elseif (difficulty == "minor")
		up_amount := 4;
	elseif (TypeOf(difficulty) == TypeOf( 1 ))
		var diff := difficult - value;
		if (diff <= -1)
			up_amount := 1;
		elseif(diff >= 10)
			up_amount := 2;
		elseif(diff >= 6)
			up_amount := 4;
		elseif(diff >= 4)
			up_amount := 6;
		else
			up_amount := 8;
		endif
	else
		SysLog("Erro em difficulty para ExpGain: {}".format(difficulty));
	endif

	//guarda as skills dos npcs tamados
	if (mobile.npctemplate && GetObjProperty(mobile, "owner") != error)
		// TODO: Tamed exp gain
	elseif (up_amount > 0)
		var exp := GetExpPoints(mobile);
		SetExpPoints(mobile, exp+up_amount);
	endif

	SetObjProperty(mobile, "#LastSkillGain", ReadGameClock());
	CheckExpUp(mobile);
	return 1;
endfunction

function CheckExpUp(who)
	var level := GetCharLevel(who);
	if (level < 20)
		var maxexp := (GetCharLevel(who) + 1) * 1000; //starts at 2000
		var exp := GetExpPoints(who);

		if (exp >= maxexp)
			exp-=maxexp;
			SetCharLevel(who, level+1);
			SetAttribPoints(who, GetAttribPoints(who)+1);
			IncreaseVitalPoints(who);
			SetExpPoints(who, exp);
		endif
	endif
endfunction

function IncreaseVitalPoints(who)
	var chardata  := GetObjProperty(who, "chardata");
	var class_cfg := FindConfigElem(ReadConfigFile(":charactercreation:config/classes"), cstr(chardata.classe) );

	var increase_hitpoint := RandomDiceRoll("1d{}".format(class_cfg.HITS)) + GetBonusAttr(AP_GetTrueStat(mobile, CONSTITUTION));
	var increase_manapoint := RandomDiceRoll("1d{}".format(class_cfg.MANA)) + GetBonusAttr(AP_GetTrueStat(mobile, INTELLIGENCE));
	// var increase_stampoint := RandomDiceRoll("1d{}".format(class_cfg.STAM)) + GetBonusAttr(AP_GetTrueStat(mobile, DEXTERITY));

	chardata.hitpoints  += increase_hitpoint;
	chardata.manapoints += increase_manapoint;
	// chardata.stampoints += increase_stampoint;

	SetObjProperty(who, "chardata", chardata);
endfunction