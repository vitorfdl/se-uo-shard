include "include/say";
include "include/arrays";
include ":timedscripts:timedScripts";

function TNCheckReagents(who, byref spellinfo, item)
	if ( item.isa(POLCLASS_ITEM) && spellinfo.charges )
      spellinfo.charges += Ceil(cdbl(spellinfo.charges) * (cdbl(spellinfo.powered-1) / 10));

      if (AP_GetTrueSkill(who, "Magia_de_{}".format(spellinfo.type)) >= 6)
         spellinfo.charges /= 2;
      endif

		var charges := cint(GetObjProperty(item, "charges"));
		if (charges < spellinfo.charges)
			SendSysMessageEx(who, "Cargas insuficientes na varinha: {} cargas".format(charges), SSM_FAIL);
			return 0;
		endif

		charges -= spellinfo.charges;
      SetName(item, item.desc);
		SetObjProperty(item, "charges", charges);
		return 1;
	endif

	// //sendsysmessage(who,  " " + reagent_costs);
	// var keys := reagent_costs.keys();
	// var amount;
	// foreach reag in keys
	// 	amount := CInt( reagent_costs[reag] );
	// 	if (!amount)
	// 		amount := 1;
	// 	endif
	// 	var objtype := GetObjTypeByName(reag);
	// 	var dictToAmount := dictionary; //variavel pra ser usada na funcao abaixo
	// 	dictToAmount["objtype"] := objtype;
	// 	var amountinpack := AmountInContainer(who.backpack, dictToAmount);
	// 	//sendsysmessage(who, "reag " + reag + " objtype " + objtype + " pack " + amountinpack);
	// 	if (amountinpack  < amount )
	// 		SendSysMessageEx(who, "Você não possui "+GetObjTypeDesc(objtype, (amount>1)), SSM_FAIL);
	// 		return 0;
	// 	endif
	// 	sleepms(2);
	// endforeach

	// foreach reag in ( keys )
	// 	amount := CInt( reagent_costs[reag] );
	// 	if (!amount)
	// 		amount := 1;
	// 	endif
	// 	ConsumeSubstance(who.backpack, GetObjTypeByName(reag), amount);
	// 	sleepms(2);
	// endforeach

	return 1;
endfunction

function getSpellName(id, cfg := ":magery:config/mage")
	cfg := ReadConfigFile(cfg);
	var elem := FindConfigElem(cfg, id);

	return GetConfigString( elem, "Name" );
endfunction

function MantainSpell(byref info)
	//sendsysmessage(info.mage, " " + info);
	if (info.mage.connected)
		start_script( ":magery:mantainSpell", { info.mage, info.mobserial, info.name, info.duration, info.level  }  ); 
	endif
endfunction

function GetSpellDuration(skill)
	return cint(skill/10)*60 + 300;
endfunction


function dispel(targ);
	var timer_list := TS_GetTimerList(targ);
	var names := array;

	foreach timer in ( timer_list.keys() )
		timer := lower(timer);
		if (timer == "agility" )
			TS_LowerDuration(targ, timer, -1);
		elseif ( timer == "strength");
			TS_LowerDuration(targ, timer, -1);
		elseif ( timer == "intelligence")
			TS_LowerDuration(targ, timer, -1);
		elseif ( timer == "velocidade")
			TS_LowerDuration(targ, timer, -1);
		elseif ( timer == "PhysicalResist")
			TS_LowerDuration(targ, timer, -1);
		elseif ( timer == "bless")
			TS_LowerDuration(targ, timer, -1);
		elseif ( timer == "phmod")
			TS_LowerDuration(targ, timer, -1);
		elseif (timer == "fimod")
			TS_LowerDuration(targ, timer, -1);
		elseif (timer == "comod")
			TS_LowerDuration(targ, timer, -1);
		elseif (timer == "enmod")
			TS_LowerDuration(targ, timer, -1);
		elseif ( timer == "pomod")
			TS_LowerDuration(targ, timer, -1);
		elseif ( timer == "disfarce")
			TS_LowerDuration(targ, timer, -1);
		elseif ( timer == "gloria")
			TS_LowerDuration(targ, timer, -1);
		elseif (timer == "desviarprojetil")
			TS_LowerDuration(targ, timer, -1);
		endif
		
	endforeach
	
	EraseObjProperty(targ, "tirocerteiro");
	EraseObjProperty(targ, "evasao");
endfunction 


function CreateRandomGliph(byref gliphs, x, y, z)
	var gliph_graphics := array{0x485F, 0x4844, 0x4841, 0x483B, 0x4862, 0x4865, 0x4868, 0x487A, 0x487D, 0x485C, 0x485F};
	var gliph := CreateItemAtLocation(x, y, z, 0x790d);
	
	gliph.graphic := gliph_graphics.randomentry();
	
	if (typeof(gliphs) == "Array")
		gliphs.append(gliph);
	else
		return gliph;
	endif
endfunction
