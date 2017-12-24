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
include ":timeUtils:time";

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
	var char_level := who.getLevel() - 1; //hack
	var result := (char_age*1000) - (char_level*1000);
	return result;
endfunction

/*
 * ExpGain(mobile, stat_name, stat_mult)
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

function SkillDbgMsg(mobile, msg)
	if (mobile.cmdevel >4)
		SendSysMessage(mobile, msg);
	endif
endfunction

function ExpGain(mobile, difficulty, value := 0)
	if ( GetObjProperty(mobile, "NoGains") ) //exp gain blocked
		SkillDbgMsg(mobile, "Flag NoGains setada.");
		return 0;
	endif

  if ( GetExpCap(mobile) == 0)
  		SkillDbgMsg(mobile, "Chegou no maximo do limite por tempo: {}".format(GetExpCap(mobile)));
		return 0;
   endif
  
	var settings := AP_GetSettingsCfgElem("Skills");
	//verifica se ja não subiu skills em 30 segundos
	var next_check := CInt(GetObjProperty(mobile, "#LastSkillGain")) + 10;
	if ( next_check > ReadGameClock() )
	//	printtextabove(mobile, "não posso subir skill agora");
		SkillDbgMsg(mobile, "Não pode subir skill ainda. Next: " + next_check + " Current: " + ReadGameClock()  );
		return 0;
	endif

	//calcula a chance de passar
	var up_amount := 0;
	if (difficulty == "major")
		up_amount := 8;
	elseif (difficulty == "minor")
		up_amount := 4;
	elseif (TypeOf(difficulty) == TypeOf( 1 ))
		var diff := difficulty - value;
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
		var exp := mobile.GetExp();
		mobile.SetExp(exp+up_amount);
		CheckExpUp(mobile);
	endif

	SetObjProperty(mobile, "#LastSkillGain", ReadGameClock());
	return 1;
endfunction

function CheckExpUp(who)
	var level := who.getLevel();
	if (level < 20)
		var maxexp := who.getLevel() * 1000; //starts at 2000
		var exp := who.GetExp();

		if (exp < maxexp)
			return;
		endif
		exp-=maxexp;
		who.SetExp(exp);
		who.setLevel(level+1);
		IncreaseVitalPoints(who);
		var levelup_cfg := ReadConfigFile(":attributes:config/levelup");
      levelup_cfg := levelup_cfg[level+1];
		if (cint(levelup_cfg.AttributePoints))
			who.SetPoints(1);
		endif

		if (cint(levelup_cfg.TalentPoints))
			who.SetTalentPoints(1);
		endif

	endif
endfunction

function IncreaseVitalPoints(who)
	var chardata  := GetObjProperty(who, "chardata");
	var class_cfg := FindConfigElem(ReadConfigFile(":charactercreation:config/classes"), cstr(chardata.classe) );

   var hit_dice := RandomDiceRoll("1d{}+5".format(class_cfg.HITS/2));
   if (HaveFeat(who, "Resiliência"))
      var dice2 := RandomDiceRoll("1d{}+5".format(class_cfg.HITS/2));
      if (hit_dice < dice2)
         hit_dice := dice2;
      endif
   endif

	var increase_hitpoint := hit_dice + AP_GetStat(who, CONSTITUTION, RETURN_BONUS);
	var increase_manapoint := RandomDiceRoll("1d{}+5".format(class_cfg.MANA/2)) + AP_GetStat(who, INTELLIGENCE, RETURN_BONUS);
	// var increase_stampoint := RandomDiceRoll("1d{}".format(class_cfg.STAM)) + GetBonusAttr(AP_GetTrueStat(who, DEXTERITY));

	chardata.hitpoints  += cint(increase_hitpoint);
	chardata.manapoints += increase_manapoint;
	// chardata.stampoints += increase_stampoint;

	SetObjProperty(who, "chardata", chardata);
   RecalcVitals(who);
endfunction
