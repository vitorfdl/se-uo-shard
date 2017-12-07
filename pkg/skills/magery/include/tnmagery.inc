include "include/say";
include "include/arrays";
include ":timedscripts:timedScripts";

function WritespellInfo(byref spellinfo, cfg := ":magery:config/mage")
	cfg := ReadConfigFile(cfg);
	if (!cfg)
		return 0;
	endif

	var spell_id := spellinfo;
	if(TypeOf(spell_id) == TypeOf( struct{} ) )
		spell_id := spellinfo.id;
	endif
	
	var elem := FindConfigElem(cfg, spell_id);
	if (elem == error)
		return 0;
	endif

	spellinfo := struct{};
	spellinfo.+id 				:= spell_id;
	spellinfo.+name        := GetConfigString( elem, "Name" );
	spellinfo.+powerwords  := GetConfigStringArray( elem, "PowerWords" );
	spellinfo.+alvo        := GetConfigString( elem, "Target" );
	spellinfo.+type        := GetConfigString( elem, "Type" );
	spellinfo.+range       := GetConfigString( elem, "Range" );
	spellinfo.+targtype    := GetConfigString( elem, "TargetType" );
	spellinfo.+mana        := GetConfigInt( elem, "Mana" );
	spellinfo.+delay       := GetConfigInt( elem, "Delay" );
	spellinfo.+difficulty  := GetConfigInt( elem, "Difficulty" );
	spellinfo.+description := GetConfigString( elem, "Desc" );
	spellinfo.+script      := GetConfigString( elem, "SpellScript" );
	spellinfo.+reagents    := GetConfigStringDictionary(elem, "RegCost");
	spellinfo.+spelltype   := GetConfigString(elem, "Type");
	spellinfo.+align       := GetConfigStringDictionary(elem, "Align");
	spellinfo.+flavortext  := GetConfigString( elem, "FlavorText" );
	spellinfo.+cycles      := GetConfigInt( elem, "Cycles" );
	spellinfo.+cooldown    := GetConfigInt( elem, "Cooldown" );
	spellinfo.+circle      := GetConfigInt( elem, "Circle" );
	spellinfo.+charges     := GetConfigInt( elem, "Charges" );
	spellinfo.+radius      := GetConfigInt( elem, "Radius" );
	spellinfo.+pericia     := GetConfigInt( elem, "Percia" );
	spellinfo.+targettype     := GetConfigInt( elem, "TargetType" );
	if (spellinfo.flavortext == error)
		spellinfo.flavortext := "";
	else
		spellinfo.flavortext := "<I> "  + spellinfo.flavortext + " </I>";
	endif

	return 1;
endfunction

function TNCheckReagents(who, byref spellinfo, item)
	if ( item.isa(POLCLASS_ITEM) && spellinfo.charges )
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

function WriteMagic(who, spell_id)
	var items_bag := EnumerateItemsInContainer(who.backpack, ENUMERATE_IGNORE_LOCKED);
	if (who.cmdlevel < 2 && !who.isClass("Mago"))
		SendSysMessageEx(who, "Você não tem conhecimento de magia", SSM_FAIL);
		return;
	elseif (!SearchArrayOfModel("objtype", cint(0x10e4), EnumerateItemsInContainer(who.backpack))) 
		SendSysMessageEx(who, "Você precisa de uma faca para entalhar", SSM_FAIL);
		return;
	endif

	SendSysMessageEx(who, "Escolha o objeto para inscrever a magia.", SSM_INFO);
	var targ := Target(who);

	var magic_slot   := GetObjProperty(targ, "magicslot");
	var spell_list   := GetObjProperty(targ, "spell_list");
	if (!spell_list)
		spell_list := array{};
	endif
	var magic_user   := GetObjProperty(targ, "serialid");
	var element      := GetObjProperty(targ, "element");
	var elementpower := GetObjProperty(targ, "elementpower");
	if (!targ.isA(POLCLASS_ITEM))
		SendSysMessageEx(who, "Você deve escolher um objeto!", SSM_FAIL);
		return;
	elseif (!magic_slot)
		SendSysMessageEx(who, "Este objeto não tem poder magico!", SSM_FAIL);
		return;
	elseif (GetObjProperty(targ, "serialid") && GetObjProperty(targ, "serialid") != who.serial)
		return;
	elseif (spell_list.size() >= magic_slot)
		SendSysMessageEx(who, "Este objeto não possui mais espaço para magias!", SSM_FAIL);
		return;
	elseif (spell_id in spell_list)
		SendSysMessageEx(who, "O equipamento ja tem essa magia!", SSM_FAIL);
		return;
	elseif(magic_user && magic_user != who.serial)
		SendSysMessageEx(who, "Você não pode gravar em itens de outra pessoa!", SSM_FAIL);
		return;
	endif

	var loc := array{who.x, who.y};
	for i := 1 to 2
		if (who.x != loc[1] && who.y != loc[2])
			SendSysMessageEx(who, "Você se moveu e parou de gravar no objeto", SSM_FAIL);
			return;
		endif
		PrintText(who, "*inscrevendo em {}*".format(targ.desc));
		PlaySoundEffect(who, 0x002b);
		sleep(2);
	endfor

	var spellinfo := struct{ id := spell_id };
	if (!WriteSpellInfo(spellinfo))
		SendSysMessageEx(who, "Ocorreu um erro ao inscrever a magia", SSM_FAIL);
		return;
	endif
	if ((spellinfo.type != "Comando" && spellinfo.type != element) || (spellinfo.circle > elementpower))
		SendSysMessageEx(who, "A inscrição não reage aos elementos da varinha", SSM_FAIL);
		return;
	endif

	var dice_roll := rollAttrDice(who, INTELLIGENCE) + getProficiencyBonus(who, ARCANISMO);
	if (dice_roll <= spellinfo.difficulty+3 && who.cmdlevel < 4)
		SendSysMessageEx(who, "Parece que você errou durante a cópia. O espaço ficou inutilizado.", SSM_FAIL);
		spell_list.append(0);
		SetObjProperty(targ, "spell_list", spell_list);
		return;
	endif

	ExpGain(who, spellinfo.difficulty+3, dice_roll);
	spell_list.append(spell_id);
	SetObjProperty(targ, "serialid", who.serial);
	SetObjProperty(targ, "spell_list", spell_list);
	SetObjProperty(targ, "magicwho", array{ who.serial });
	SetName(targ, targ.desc);
	SendSysMessageEx(who, "Você inscreveu com sucesso a magia na arma.", SSM_INFO);
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

function SetClericSpellUsed(mobile, spellinfo)
	var cleric_list := GetObjProperty(mobile, "cleric_list");
	foreach spell in cleric_list
		if (spell.id == spellinfo.id && !spell.used)
			spell.used := 1;
			break;
		endif
		sleepms(2);
	endforeach

	SetObjProperty(mobile, "cleric_list", cleric_list);
endfunction