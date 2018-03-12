use uo;
use vitals;
include "include/say";
include ":gumps:requestgump";
include ":gumps:yesno";
include ":unicos:item_template";
include ":attributes:attributes";
include ":tn:tngumps";
include ":tn:questevent";
include ":gumps:yesno";
include "include/client";
include "include/arrays";
include ":combat:hitscripts";

var resist_types := array{"Veneno", "Fogo", "Eletricidade", "Gelo", "Perfuração", "Corte", "Contusão", "Strength", "Dexterity", "Intelligence"};

function GetBossStruct(targ)
	var spawnpoint := SystemFindObjectBySerial( GetObjProperty(targ, "spawnpoint") );
	var monsters   := GetObjProperty(spawnpoint, "monsters");
	var me         := FindElemInArray(monsters, struct{ "serial" := targ.serial });
	return me.bossParams;
endfunction

function isBoss(targ)
	var spawnpoint := SystemFindObjectBySerial( GetObjProperty(targ, "spawnpoint") );
	var monsters   := GetObjProperty(spawnpoint, "monsters");
	var me         := FindElemInArray(monsters, struct{ "serial" := targ.serial });
	if (me.isBoss == 1)
		return 1;
	endif

	return 0;
endfunction

function CreateBossStructGump(who, byref boss)
	if (typeOf(boss) != typeOf(struct{}))
		boss := struct{};
	endif
	var chargump := GFCreateGump();
	var input;

	var cfg :=	NPC_GetNPCConfig(boss.npctemplate);
	var npctemplate := boss.npctemplate;
	var template, nome, cor, camod;
	var str, dex, int, con, wis, hits;

	//GFClosable(chargump, 0);
	GFDisposable(chargump, 0);
	GFClosable( chargump, 0 );
	GFResizePic( chargump, 0, 0, GFCfgConst( "Defaults", "BackGround" ), 450, 605 );
	GFResizePic( chargump, 15, 15, GFCfgConst( "Defaults", "ForeGround" ), 420, 25 );
	GFResizePic( chargump, 15, 45, GFCfgConst( "Defaults", "ForeGround" ), 420, 540 );

	// GFResizePic(chargump, 0, 25, 3700, 500, 500);
	// GFResizePic(chargump, 15, 40, 0x13BE, 470, 470);
	GFHtmlShadowed(chargump, 15, 17, 420, 20, "#ffc04c", "<center>Boss Menu");

	var x_pos := 20;
	var y_pos := 60;
	if ( !boss.npctemplate )
		boss.npctemplate := ":ghaia:";
	endif
	GFTextLine(chargump, x_pos, y_pos, 1051,  "NpcTemplate:");
	template := GFTextEntry( chargump, x_pos+80, y_pos, 150, 20, 1935, boss.npctemplate);
	GFAddButton(chargump, x_pos+200, y_pos, 0xFBD, 0xFBE, GF_CLOSE_BTN, 101);

	if (!boss.name)
		boss.name := "-";
	endif
	GFTextLine(chargump, x_pos, y_pos+18, 1051, "Nome: ");
	nome := GFTextEntry( chargump, x_pos+60, y_pos+18, 120, 20, 1935, boss.name);

	GFTextLine(chargump, x_pos, y_pos+(18*2), 1051, "Cor: ");
	cor := GFTextEntry( chargump, x_pos+60, y_pos+(18*2), 100, 20, 1935, cint(boss.color) + "");
	if (cfg)
		GFTextLine(chargump, x_pos, y_pos+(18*3), 1051, "Str ["+ cfg.Strength+ "]:");
		GFTextLine(chargump, x_pos, y_pos+(18*4), 1051, "Int [" + cfg.Intelligence + "]:");
		GFTextLine(chargump, x_pos, y_pos+(18*5), 1051, "Dex [" + cfg.Dexterity + "]:");
		GFTextLine(chargump, x_pos, y_pos+(18*6), 1051, "Con [" + cfg.Constitution + "]:");
		GFTextLine(chargump, x_pos, y_pos+(18*7), 1051, "Wis [" + cfg.Wisdom + "]:");
	else
		GFTextLine(chargump, x_pos, y_pos+(18*3), 1051, "Str: ");
		GFTextLine(chargump, x_pos, y_pos+(18*4), 1051, "Int: ");
		GFTextLine(chargump, x_pos, y_pos+(18*5), 1051, "Dex: ");
		GFTextLine(chargump, x_pos, y_pos+(18*6), 1051, "Con: ");
		GFTextLine(chargump, x_pos, y_pos+(18*7), 1051, "Wis: ");
	endif

	str := GFTextEntry( chargump, x_pos+60, y_pos+(18*3), 100, 20, 1935, cint(boss.str) + "");
	int := GFTextEntry( chargump, x_pos+60, y_pos+(18*4), 100, 20, 1935, cint(boss.int) + "");
	dex := GFTextEntry( chargump, x_pos+60, y_pos+(18*5), 100, 20, 1935, cint(boss.dex) + "");
	con := GFTextEntry( chargump, x_pos+60, y_pos+(18*6), 100, 20, 1935, cint(boss.con) + "");
	wis := GFTextEntry( chargump, x_pos+60, y_pos+(18*7), 100, 20, 1935, cint(boss.wis) + "");

	var attacks      := GetNpcAttacks(npctemplate);
	var boss_attacks := boss.customAttacks;
	GFHtmlShadowed(chargump, x_pos+15, y_pos+(18*9), 250, 20,  "#FFA500", "Attack - Dice - Bonus - Type - Anim", 1);

	var y := y_pos+(18*9)+15;
	foreach attack in attacks
		GFHtmlShadowed(chargump, x_pos+15, y, 250, 20,  "#CCCCCC", "{} - {} - {} - {} - {}".format(attack.name, attack.dicedmg, attack.hitbonus, attack.damageType, attack.AttackAnim), 1);
		y+= 15;
		sleepms(2);
	endforeach

	var i := 1;
	foreach attack in boss_attacks
		attack := SplitWords(attack, " ");
		GFAddButton(chargump, x_pos, y+1, 2103, 2104, 1, 90+i);
		GFHTMLArea(chargump, x_pos+15, y, 250, 20, "<BASEFONT SIZE=1 color=#93c0ff>{} - {} - {} - {} - {}".format(attack[1], attack[2], attack[3], attack[4], attack[5]) );
		y+= 15;
		i += 1;
	endforeach
	GFAddButton( chargump, x_pos, y+2, 2103, 2104, 1, 90);

	// dmgmod := GFTextEntry( chargump, 180, 200, 100, 20, 1935, cint(boss.dmg) + "");

  	//if (cfg.specialai != error)
	//	GFTextLine(chargump, 50, 200, 1051, "Special AI [" + cfg.specialai+ "]:"); ///////////////////////
//	else
		//GFTextLine(chargump, 50, 200, 1051, "Special AI: ");
	//endif
  
	var cx_pos := 280;

	var cfg_hp := "";
	if (cfg)
		cfg_hp := " [{}]".format(cfg.Hits);
	endif
	GFTextLine(chargump, cx_pos, y_pos, 1051, "HP{}:".format(cfg_hp));
	hits := GFTextEntry( chargump, cx_pos+80, y_pos, 100, 20, 1935, cint(boss.hits) + "");

	var cfg_ca := "";
	if (cfg)
		cfg_ca := " [{}]".format(cfg.AR);
	endif
	GFTextLine(chargump, cx_pos, y_pos+18, 1051, "CA{}:".format(cfg_ca));
	camod := GFTextEntry( chargump, cx_pos+60, y_pos+18, 100, 20, 1935, cint(boss.CA) + "");

	GFTextLine(chargump, cx_pos, y_pos+(18*2), 1051,  "Resistencias");
	var resists         := GetConfigStringArray(cfg, "Resistencia");
	var imunities       := GetConfigStringArray(cfg, "Imunidade");
	var vulnerabilities := GetConfigStringArray(cfg, "Vulnerabilidade");
	if (boss.resists)
		resists += boss.resists;
	endif
	if (boss.imunities)
		imunities += boss.imunities;
	endif
	if (boss.vulnerabilities)
		vulnerabilities += boss.vulnerabilities;
	endif

	y := y_pos+(18*3);
	foreach resist in resist_types
		var type  := "+";
		var color := 171;
		if (resist in imunities)
			type := "*";
			color := 51;
		elseif (resist in vulnerabilities)
			type := "-";
			color := 236;
		elseif (!(resist in resists))
			sleepms(2);
			continue;
		endif
		GFTextLine(chargump, cx_pos, y, color, "{} {}".format(type, resist));
		y+= 18;
	endforeach
	GFAddButton( chargump, cx_pos, y+3, 2103, 2104, 1, 80);

	y += 20;

	var skills := GetConfigStringArray(cfg, "Pericia");
	if (boss.skills)
		skills += boss.skills;
	endif
	GFTextLine(chargump, cx_pos, y, 1051, "Pericias: ");
	y+= 18;
	foreach skill in skills
		GFTextLine(chargump, cx_pos, y, 1051, "{}".format(skill));
		y+= 18;
		sleepms(2);
	endforeach
	GFAddButton( chargump, cx_pos, y+2, 2103, 2104, 1, 70);
	// y+= 18;
	
	y_pos := 340;
	GFTextLine(chargump, x_pos, y_pos, 1051, "Death Events: ");
	GFAddButton( chargump, x_pos, y_pos+24, 2103, 2104, 1, 20);
	if (boss.deathevent)
		var y := 0;
		var i := 0;
		foreach item in (boss.deathevent)
			GFAddButton( chargump, x_pos, y_pos+43+y, 2103, 2104, 1, 21+i);
			GFTextLine(chargump, x_pos+10, y_pos+20+y, 1935, " " + item.type);
			y += 18;
			i += 1;
		endforeach
	endif

	GFTextLine(chargump, cx_pos, y_pos, 1051, "Loot: ");
	GFAddButton( chargump, cx_pos, y_pos+24, 2103, 2104, 1, 10);
	if (boss.loot)
		var y := 0;
		var i := 0;
		foreach lootinfo in (boss.loot)
			GFAddButton( chargump, cx_pos, y_pos+43+y, 2103, 2104, 1, 11+i);
			GFTextLine(chargump, cx_pos+10, y_pos+20+y, 1935, " " + lootinfo.item.desc + " [" + lootinfo.chance + "%]");
			y += 18;
			i += 1;
		endforeach
	endif

	y_pos := 450;
   GFTextLine(chargump, x_pos, y_pos, 1051, "Combat Events: ");
	GFAddButton( chargump, x_pos, y_pos+23, 2103, 2104, 1, 40);
	if (boss.combatevent)
		var y := 0;
		var i := 0;
		foreach item in (boss.combatevent)
			GFAddButton( chargump, x_pos, y_pos+43+y, 2103, 2104, 1, 41+i);
			GFTextLine(chargump, x_pos+10, y_pos+20+y, 1935, " " + item.name);
			y += 18;
			i += 1;
		endforeach
	endif

	GFTextLine(chargump, cx_pos, y_pos, 1051, "Custom Scripts: ");
	GFAddButton( chargump, cx_pos, y_pos+23, 2103, 2104, 1, 30);
	if (boss.special)
		var y := 0;
		var i := 0;
		foreach item in (boss.special)
			GFAddButton( chargump, cx_pos, y_pos+43+y, 2103, 2104, 1, 31+i);
         var item_name := item.name;
         if (item.hitopt)
            item_name += " {}".format(item.hitopt);
         endif
			GFTextLine(chargump, cx_pos+12, y_pos+19+y, 1935, item_name );
			y += 18;
			i += 1;
		endforeach
	endif

	GFAddButton(chargump, 380, 537, 4502, 4502, 1, 1 );
	GFTextLine(chargump, 350, 550, 1051, "Apply");

	input := GFSendGump(who, chargump);	
  
	boss.npctemplate := Cstr(GFExtractData(input, template));
	boss.name        := Cstr(GFExtractData(input, nome));
	boss.color       := Cint(GFExtractData(input, cor));
	boss.str         := Cint(GFExtractData(input, str));
	boss.dex         := Cint(GFExtractData(input, dex));
	boss.int         := Cint(GFExtractData(input, int));
	boss.wis         := Cint(GFExtractData(input, wis));
	boss.con         := Cint(GFExtractData(input, con));
	boss.hits        := Cint(GFExtractData(input, hits));
	// boss.dmg         := Cdbl(GFExtractData(input, dmgmod));
	boss.CA    		:= Cint(GFExtractData(input, camod));

	if (input[0] == 1 )
		//SendSysMessage(who, " " + boss + " " );
		//SendSysMessage(who, " " + boss + " " + Cstr(GFExtractData(input, template)) );
		return;
	elseif ( input[0] >= 90 && input[0] <= 95)
		var selected := input[0]-90;
		var boss_attacks := boss.customAttacks;
		if (!boss_attacks) boss_attacks := array{}; endif
		if (boss_attacks[selected])
			var choice := RadioGump(who, 180, 220, "O que deseja fazer?", array{"Remover", "Editar", "Cancelar"}, 1);
			if (choice == 1)
				boss_attacks.Erase(selected);
			elseif (choice == 2)
				var new_attack := RequestGump(who, "Entre com o novo ataque", "Attack name - Dice - Bonus - Type - Anim", boss_attacks[selected], 1);
				boss_attacks[selected]  := new_attack;
			endif
			boss.customAttacks := boss_attacks;
		else
			var new_attack := RequestGump(who, "Entre com o novo ataque", "Attack name - Dice - Bonus - Type - Anim", "", 1);
			if (new_attack && new_attack != "")
				boss_attacks.append(new_attack);
				boss.customAttacks := boss_attacks;
			endif
		endif
	elseif ( input[0] == 70)
		// var opt_checked := array{};
		// foreach resist in resist_types
		// 	if (resist in boss.resists)
		// 		opt_checked := true;
		// 	endif
		// endforeach
		// var opts := CheckBoxGump(who, 180, 200+(resist_types.length() * 18), "Escolha as resistências:", resist_types, opt_checked);
		// boss.resists := opts;
	elseif ( input[0] == 80)
		var opt := RadioGump(who, 180, 160, "Qual tipo deseja adicionar?", array{"resists", "imunities", "vulnerabilities"});
		var opt_checked := array{};
		foreach resist in resist_types
			if (resist in boss[opt])
				opt_checked[_resist_iter] := 1;
			endif
		endforeach
		var opts := CheckBoxGump(who, 180, 260+(resist_types.length() * 20), "Escolha os tipos:", resist_types, opt_checked);
		boss[opt] := opts;
	elseif ( input[0] >= 10 && input[0] <= 15)
		var index := input[0] - 9;
		//sendsysmessage(who, " " + input[0] + " " + boss.loot[index]);

		if ( boss.loot[index] != error )
			var opt := RadioGump(who, 180, 220, "Editar item", array{"Criar", "Deletar", "Alterar Chance"});			
			if (opt == "Deletar")
				boss.loot.erase(index);
				SendSysMEssageEx(who, "O item foi removido.", SSM_INFO);
			elseif ( opt == "Criar" )
				var retitem := CreateItemInBackpackFromTemplate(who, boss.loot[index].item);
				if (!retitem)
					SendSysMessageEx(who, "erro: " + retitem);
				endif
				SendSysMessageEx(who, "Você criou o item na sua mochila."); 
			elseif (opt == "Alterar Chance")
				boss.loot[index].chance := Cint(RequestGump(who, "Digite a chance do item aparecer. 1 a 100:", "", "", 1));
			endif 
		else
			SendSysMessage(who, "Escolha o item do loot.");
			var targ := Target(who);
			if (targ)
				if (!boss.loot)
					boss.+loot := array;
				endif
				var lootinfo := struct;
				lootinfo.+chance := Cint(RequestGump(who, "Digite a chance do item aparecer. 1 a 100:", "", "", 1));
				lootinfo.+item := CreateItemTemplate(targ);				
				(boss.loot).append(lootinfo);
			endif
		endif
	elseif ( input[0] >= 20 && input[0] <= 25)
		var index := input[0] - 19;
		if (boss.deathevent[index] != error )
			var yes := YesNo(who, "Deletar o evento?");
			if (yes == 1)
				boss.deathevent.erase(index);				
				SendSysMessageEx(who, "Evento deletado.", SSM_INFO);
			else
				SendSysMessageEx(who, "Cancelado.", SSM_INFO);
			endif
		else
				var nevent := ConfigureEvent(who);
				if (nevent)
					if (!boss.deathevent)
						boss.+deathevent := array;
					endif
					(boss.deathevent).append(nevent);
				endif
		endif
	elseif ( input[0] >= 40 && input[0] <= 45)
		var index := input[0] - 39;
		if (boss.combatevent[index] != error )
			var yes := YesNo(who, "Editar ou Deletar Evento?", "Editar", "Deletar");
			if (yes == 1)
				var nevent := ConfigureCombatEvent(who, boss.combatevent[index], 1);
				(boss.combatevent).erase(index);
				if (nevent)
					(boss.combatevent).append(nevent);
				endif
			else
				boss.combatevent.erase(index);				
				SendSysMessageEx(who, "Evento deletado.", SSM_INFO);
			endif
		else
				var nevent := ConfigureCombatEvent(who);
				if (nevent)
					if (!boss.combatevent)
						boss.+combatevent := array;
					endif
					(boss.combatevent).append(nevent);
				endif
		endif
	elseif ( input[0] >= 30 && input[0] <= 35)
      if (!boss.special) boss.special := array{}; endif
		var index := input[0] - 29;

		if (boss.special[index] != error)
			var yes := YesNo(who, "Deletar a habilidade?");
			if (yes == 1)
				boss.special.erase(index);				
				SendSysMessageEx(who, "Habilidade deletada.", SSM_INFO);
			else
				SendSysMessageEx(who, "Cancelado.", SSM_INFO);
			endif

		else
			var opt := RadioGump(who, 250, 270, "Editar item", array{"Armorscript", "AttackHitscript", "Criticalscript", "Special IA", "Cancelar"});
         var spec := struct{
            "name" := opt
         };

         var hitscripts_cfg := ReadConfigFile(":combat:hitScripts");
         if (opt["Armor"])
            var input_list := array{
               struct{ "title" := "Escolha o ArmorHitScript", "marked" := 1, "radio" := GetHitScriptsName("ArmorHits") }
            };
            var output := QuestionsGump(who, input_list);
            spec.+hitopt := GetHitScript(StrReplace(output[1], " ", "_"));
         elseif (opt["Critical"])
            var input_list := array{
               struct{ "title" := "Escolha o CriticalHitscript", "marked" := 1, "radio" := GetHitScriptsName() }
            };
            var output := QuestionsGump(who, input_list);
            spec.+hitopt := GetHitScript(StrReplace(output[1], " ", "_"));
         elseif (opt["Attack"])
            var input_list := array{
               struct{ "title" := "Escolha o AttackHitscript", "marked" := 1, "radio" := GetHitScriptsName() }
            };
            var output := QuestionsGump(who, input_list);
            spec.+hitopt := GetHitScript(StrReplace(output[1], " ", "_"));
         elseif (opt["Special IA"])
				var ainame := Cstr(RequestGump(who, "Digite o nome da IA", "", "", 1));
				spec.+ai := ainame;
         endif
         
         if (!opt["Cancelar"])
			   boss.special.append(spec);
         endif
		endif
	endif
	CreateBossStructGump(who, boss);
	//return 0;
endfunction

  //transforma um npc em boss
function SetBoss(npc, boss)
	if (boss.name != "-" && boss.name != "-random-")
		SetName(npc, boss.name);
	endif
	npc.color := boss.color;
	npc.truecolor := boss.color;

	if (boss.CA)
		npc.CA := Cint(boss.CA);
	endif

   if (boss.str) AP_SetTrueStat(npc, STRENGTH, Cint(boss.str)); endif
   if (boss.dex) AP_SetTrueStat(npc, DEXTERITY, Cint(boss.dex)); endif
   if (boss.int) AP_SetTrueStat(npc, INTELLIGENCE, Cint(boss.int)); endif
   if (boss.wis) AP_SetTrueStat(npc, WISDOM, Cint(boss.wis)); endif
   if (boss.con) AP_SetTrueStat(npc, CONSTITUTION, Cint(boss.con)); endif
	
	foreach skill in (boss.skills)
		AP_SetTrueSkill(npc, skill, 1);
		// sleepms(2);
	endforeach

	if (boss.customAttacks && boss.customAttacks[0])
		var customAttacks := array{};
		foreach attack in (boss.customAttacks)
			attack := FormatAttack(boss.npctemplate, attack);
			customAttacks.append(attack);
		endforeach
		SetObjProperty(npc, "customAttacks", customAttacks);
	endif

	if (boss.resists)
		SetObjProperty(npc, "resists", boss.resists);
	endif
	if (boss.imunities)
		SetObjProperty(npc, "imunities", boss.imunities);
	endif
	if (boss.vulnerabilities)
		SetObjProperty(npc, "vulnerabilities", boss.vulnerabilities);
	endif

  	SetObjProperty(npc, "hitsmod", cint(boss.hits));
  	RecalcVitals(npc);
	AP_RefreshVitals(npc);

	if (!npc.backpack)
		var backpack := CreateItemAtLocation(npc.x, npc.y, npc.z,  2482,  1, npc.realm);
		EquipItem(npc, backpack);
	endif

	foreach iteminfo in (boss.loot)
    		var amt := iteminfo.item.Amount;
    		if (!amt)
      			amt := 1;
    		endif
		//printtextabove(npc, " " + iteminfo);
		if (RandomInt(100) < iteminfo.chance)
			var theitem := CreateItemInBackpackFromTemplate(npc, iteminfo.item);
			//printtextabove(npc, " " + theitem);
    			theitem.movale := 1;
    			if (theitem.amount == 1)
        			Setname(theitem, iteminfo.item.desc);
	    		endif
		endif
	endforeach

//printtextabove(npc, " " + boss.special);
	if (boss.deathevent)
		var lwprop := array;
		foreach nevent in (boss.deathevent)
			if (nevent.type == "Ultimas Palavras")
				lwprop.append(nevent.text);
			else
				PrepareEvent(nevent);
			endif
		endforeach
		if (lwprop.size() > 0)
			SetObjProperty(npc, "Ultimas Palavras", lwprop);
		endif
	endif

  if (boss.customai)
    npc.script := boss.customai;
    restartscript(npc);
  endif

	if (boss.special)
		foreach spec in (boss.special)
         if (spec.name["Armor"])
            SetArmorHitscript(npc, spec.hitopt);
         elseif (spec.name["Critical"])
            SetCriticalHitscript(npc, spec.hitopt);
         elseif (spec.name["Attack"])
            SetHitscript(npc, spec.hitopt);
         elseif (spec.name == "Custom Hitscript")
            SetObjProperty(npc, "generichit", spec.hitopt);
            var hit_scripts := GetObjProperty(npc, "HitScripts");
            if ( !hit_scripts )
               hit_scripts := array{};
            endif
            hit_scripts.Append(":combat:weaponHitScripts/generic");
            SetObjProperty(npc, "HitScripts", hit_scripts);
         elseif (spec.name == "Special AI")
            npc.script := spec.ai;
      //      printtextabove(npc, " " + spec.ai);
            restartscript(npc);
         endif
         sleepms(2);
		endforeach
	endif	

	return npc;
  
endfunction

function FormatAttack(npctemplate, new_attack)
	var attack := SplitWords(attack, " ");

	attack := struct{
		"name"    := attack[1],
		"dicedmg"    := attack[2],
		"hitbonus"   := cint(attack[3]),
		"DamageType" := attack[4],
		"AttackAnim" := cint(attack[5]),
		"Attribute"  := npctemplate.AttackAttribute,
		"MaxRange"   := npctemplate.MaxRange,
		"MinRange"   := npctemplate.MinRange
	};

	return attack;
endfunction

//cria um boss
function CreateBoss(boss, x, y, z, realm)
	var npc := CreateNpcFromTemplate(boss.npctemplate, x, y, z, 0 , realm);
	if (npc == error)
    //print("createboss " + npc + " " + boss.npctemplate);
		return error{"errortext":="Não foi possivel criar o boss em x " + x + " y " + y + " z  " + z};
	endif
	
	SetBoss(npc, boss);
	
	return npc;
endfunction

function ConfigureCombatEvent(who, event := 0, change_event := 0)
	if (!event || change_event == 1)
      if (!event)
         event := struct{
            name := "",
            cd := 10,
            dist := 0,
            chance := 100,
            hp_percent := 100,
            steps := array{}
         };
      endif
      var input_list := array{};
      input_list.append(struct{ "title" := "Digite o nome desse evento de combate?. Ex: Ataque Multiplo", "marked" := event.name});
      input_list.append(struct{ "title" := "De quanto em quanto tempo o npc realiza esse evento? (segundos)", "marked" := ""+event.cd});
      input_list.append(struct{ "title" := "Qual a distancia que o npc precisa estar do alvo? (0 para não checar)", "marked" := ""+event.dist});
      input_list.append(struct{ "title" := "Qual a chance do npc realizar esta ação? (100 para sempre)", "marked" := ""+event.chance});
      input_list.append(struct{ "title" := "Quanto % de hp o npc deve ter para realizar este evento? (100 para sempre)", "marked" := ""+event.hp_percent});

      var output := QuestionsGump(who, input_list);

		event.name := output[1];
		event.cd := cint(output[2]);
		event.onlybloodied := 0;
		event.dist := cint(output[3]);
		event.chance := cint(output[4]);
		event.hp_percent := cint(output[5]);
	endif
	var continua := 1;

	while (continua)
      var gumpcombat := GFCreateGump();

      GFDisposable(gumpcombat, 0);
      GFResizePic(gumpcombat, 0, 25, 3700, 250, 430);	
      GFResizePic(gumpcombat, 15, 40, 0x13BE, 220, 400);
      GFTextMid(gumpcombat, 20, 50, 250, 1051, "Combat Events");
      
      var y := 64;
      GFTextLine(gumpcombat, 35, y, 1051, "Adicionar Novo ");
      GFAddButton(gumpcombat, 20, y+2, 2117, 2118, 1, 100);
      
      var i := 1;
      foreach state in (event.steps)
         y := y+ 15;
         GFTextLine(gumpcombat, 35, y, 1051, ""+state.type);
         GFAddButton(gumpcombat, 20, y+2, 2117, 2118, 1, 400+i);
         i := i + 1;
      endforeach

      GFAddButton(gumpcombat, 180, 387, 4502, 4502, 1, 1000 );
      GFTextLine(gumpcombat, 140, 400, 1051, "Apply");
      
      var input := GFSendGump(who, gumpcombat);

      if (input[0] == 100)
         var state := CombatEventsOptions(who, struct{});
         event.steps.append(state);
      elseif (input[0] == 1000)
         continua := 0;
      elseif (input[0] >= 400)
         input := input[0] - 400;
         var oldstate := event.steps[input];
         event.steps.Erase(input);
         var choice := RadioGump(who, 0, 0, "O que deseja fazer?", array{"Alterar", "Mudar Posição","Remover"});
      if (choice == "Alterar")
            var state := CombatEventsOptions(who, struct{});
            event.steps.Insert(input, state);
         elseif (choice == "Mudar Posição")
            var position := cint(RequestGump(who, "Qual posição??", "Ex: 1", "1", 1));
            if (position == 0)
               position := 1;
            elseif (position > event.steps.size())
               position := event.steps.size()+1;
            endif
         endif
      endif
	endwhile

	return event;

endfunction

function CombatEventsOptions(who, state)
   var opt := RadioGump(who, 180, 0, "Adicionar Estado", array{"Move", "Teleport", "TimedScript", "Play Cast Anim", "Perform Action", "Text", "Play Sound", "Heal", "Dmg", "Graphic Effect",  "Create Items", "Summon Npcs", "Custom HitScript", "Set Cooldown", "Delay"});	

   var targ_opts := array{"Self", "Cone in front", "Around Self", "Opponent", "Around Opponent", "All Opponents", "Random Opponent"};
   state.type := opt;
   if (opt == "Move")
      SendSysMessageEx(who, "Para onde o npc deve andar?");
      var loc := TargetCoordinates(who);
      state.+x := loc.x;
      state.+y := loc.y;
      state.+z := loc.z;
   elseif ( opt == "Teleport")
      state.+target := RadioGump(who, 200, 180, "Quem vai sofrer?", targ_opts);
      var loc := TargetCoordinates(who);
      state.+x := loc.x;
      state.+y := loc.y;
      state.+z := loc.z;
   elseif ( opt == "TimedScript")
      var input_list := array{};
      input_list.append(struct{ "title" := "Quem vai sofrer a action?", "marked" := 1, "radio" := targ_opts});
      var timedcfg := TS_GetCfgFile();
      input_list.append(struct{ "title" := "Escolha o timedscript", "marked" := 1, "radio" := GetConfigStringKeys(timedcfg)});
      input_list.append(struct{ "title" := "Qual a duração do timer? [até 120s]", "marked" := ""});
      input_list.append(struct{ "title" := "Qual o level do timer?", "marked" := "1"});
      input_list.append(struct{ "title" := "Teste de resistência?", "checked" := 1, "radio" :=  array{STRENGTH, INTELLIGENCE, DEXTERITY, CONSTITUTION, WISDOM, "Nenhum"} });
      input_list.append(struct{ "title" := "Dificuldade do teste?", "marked" := "12"});
      input_list.append(struct{ "title" := "Distância do cone (cone in front only)?", "marked" := "3"});

      var output := QuestionsGump(who, input_list);
      
      state.+target        := output[1];
      state.+timername     := output[2];
      state.+timerduration := cint(output[3]);
      state.+level         := cint(output[4]);
      state.+conedist      := cint(output[6]);

      if (output[5] != "Nenhum")
         state.+resist_test := output[5];
         state.+resist_diff := cint(output[6]);
      endif
   elseif ( opt == "Perform Action" )
      var input_list := array{};
      input_list.append(struct{ "title" := "Quem vai realizar a action?", "marked" := 1, "radio" := targ_opts});
      input_list.append(struct{ "title" := "Qual o ID da animação?", "marked" := ""});
      var output := QuestionsGump(who, input_list);

      state.+target := output[1];
      state.+id := cint(output[2]);
   elseif (opt == "Text")
      var input_list := array{};
      input_list.append(struct{ "title" := "Quem vai falar?", "marked" := 1, "radio" := targ_opts});
      input_list.append(struct{ "title" := "Qual o texto?", "marked" := ""});
      input_list.append(struct{ "title" := "Qual a cor do texto?", "marked" := "1000"});
      var output := QuestionsGump(who, input_list);

      state.+target := output[1];
      state.+text := output[2];
      state.+color := cint(output[3]);
   elseif (opt == "Play Sound")
      state.+id := cint(RequestGump(who, "Qual o ID do som?", "", "", 1));
   elseif (opt == "Delay")
      state.+delay := cint(RequestGump(who, "Quanto tempo de delay?", "", "", 1));
   elseif (opt == "Heal")
      state.healamt := cint(RequestGump(who, "Quanto vai healar?", "", "", 1));
   elseif (opt == "Graphic Effect")
      var input_list := array{};
      input_list.append(struct{ "title" := "Onde vai aparecer o efeito?", "marked" := 1, "radio" := targ_opts + array{"Fixed Loc"}});
      input_list.append(struct{ "title" := "Qual o tipo de efeito? (fixed loc deve ser Moving)", "marked" := 1, "radio" := array{"Stationary", "Moving"}});
      input_list.append(struct{ "title" := "Qual o efeito?", "marked" := "", "radio" := array{"Lightning", "Red Sparkles", "Blue Sparkles", "Blue and Swirl", "Swirl Sparkles", "Blue Sparkles on Ground", "Fire Column", "Small Fireball", "Large Fireball", "Wavy Fireball", "Outro"}});
      input_list.append(struct{ "title" := "Qual o ID do efeito? (Outro)", "marked" := "1000"});
      input_list.append(struct{ "title" := "Qual a direção? (Moving, ignore for Cone)", "marked" := "", "radio" := array{"Mob->Player", "Player->Mob"}});
      input_list.append(struct{ "title" := "Digite o x y z onde o efeito vai aparecer: (stationary, ignore for Cone)", "marked" := "1000"});
      input_list.append(struct{ "title" := "Digite o x y z da posição final do efeito: (stationary, ignore for Cone)", "marked" := "1000"});
      input_list.append(struct{ "title" := "Digite a distância do cone (somente Cone in Front)", "marked" := "3"});
      var output := QuestionsGump(who, input_list);


      state.+target := output[1];
      state.+conedist := cint(output[8]);
      state.+graphictype := output[2];
      if ( output[3] == "Lightning" )
         state.+fxid := "Lightning";
      elseif ( output[3] == "Red Sparkles")
         state.+fxid := GFX_RED_SPARKLES;
         state.+speed := 7;
         state.+loops := 7;
      elseif ( output[3] == "Blue Sparkles")
         state.+fxid := GFX_BLUE_SPARKLES;
         state.+speed := 10;
         state.+loops := 10;
      elseif (output[3] == "Blue and Swirl")
         state.+fxid := GFX_BLUE_SPARKLE_SWIRL;
         state.+speed := 7;
         state.+loops := 16;
      elseif (output[3] == "Swirl Sparkles")
         state.+fxid := GFX_BLUE_SPARKLE_BORDER;
         state.+speed := 7;
         state.+loops := 0;
      elseif (output[3] == "Blue Sparkles on Ground")
         state.+fxid := GFX_BLUE_SPARKLE_GROUND;
         state.+speed := 7;
         state.+loops := 10;
      elseif (output[3] == "Fire Column");
         state.+fxid := GFX_FIRE_COLUMN;
         state.+speed := 7;
         state.+loops := 0;
      elseif (output[3] == "Small Fireball")
         state.fxid := GFX_SMALL_FIREBALL;
         state.+speed := 5;
         state.+loops := 0;
      elseif (output[3] == "Large Fireball")
         state.fxid := GFX_LARGE_FIREBALL;
         state.+speed := 5;
         state.+loops := 0;
      elseif (output[3] == "Wavy Fireball")
         state.fxid := GFX_WAVY_FIREBALL;
         state.+speed := 5;
         state.+loops := 0;
      elseif (output[3] == "Outro")
         state.+fxid := output[4];
         if (state.graphictype == "Stationary")
            state.+speed := 7;
            state.+loops := 16;
         else
            state.+speed := 5;
            state.+loops := 0;
         endif
      endif

      state.+direction := output[5];

      if (state.target == "Fixed Loc")
         var loc := splitwords(output[6]);
         state.+startx :=  loc[1];
         state.+starty :=  loc[2];
         state.+startz :=  loc[3];
         state.startradius := 0;

         if (state.graphictype == "Moving")
            var loc2 := splitwords(output[7]);
            state.+endx :=  loc2[1];
            state.+endy :=  loc2[2];
            state.+endz :=  loc2[3];
            // var radius :=   RequestGump(who, "Qual o raio de onde o efeito vai terminar:", "0 se for sempre no mesmo lugar", "0", 1);
            state.endradius := 0;
         endif
      endif
   elseif ( opt == "Dmg")
      var input_list := array{};
      input_list.append(struct{ "title" := "Quem vai sofrer?", "marked" := 1, "radio" := targ_opts});
      input_list.append(struct{ "title" := "Quanto de dano vai causar? Ex: 1d2", "marked" := "1d2"});
      input_list.append(struct{ "title" := "Qual o tipo de dano?", "marked" := 1, "radio" := array{DMG_PHYSICAL, DMG_FIRE, DMG_COLD, DMG_POISON,DMG_ENERGY,DMG_PIERCING,DMG_SLASH,DMG_BLUDGE, "Sem Resistência" }});
      input_list.append(struct{ "title" := "Qual tipo de vital sera afetado pelo dano?", "marked" := "", "checkbox" := array{"Stam", "Hits", "Mana"}});
      input_list.append(struct{ "title" := "Teste de resistência?", "checked" := 6, "radio" :=  array{STRENGTH, INTELLIGENCE, DEXTERITY, CONSTITUTION, WISDOM, "Nenhum"} });
      input_list.append(struct{ "title" := "Qual a dificuldade do teste?", "marked" := "14"});
      input_list.append(struct{ "title" := "Digite a distância do cone (somente Cone in Front)", "marked" := "3"});
      var output := QuestionsGump(who, input_list);

      state.+target := output[1];
      state.+dmg := output[2];
      state.+dmgtype := output[3];
      state.+conedist := cint(output[7]);
      if (state.dmgtype == "Sem Resistência")
         state.dmgtype := -1;
      endif
      state.+vitals := output[4];
      if (output[5] != "Nenhum")
         state.+resist_test := output[5];
         state.+resist_diff := cint(output[6]);
      endif
   elseif ( opt == "Create Items")
      SendSysMessageEx(who, "Não implementado");
   elseif (opt == "Summon Npcs")
      var input_list := array{};
      input_list.append(struct{ "title" := "Qual o npctemplate?", "marked" := ":ghaia:"});
      input_list.append(struct{ "title" := "Quantos npcs vao ser spawnados?", "marked" := "2"});
      input_list.append(struct{ "title" := "Qual o raio de spawn?", "marked" := "5"});
      input_list.append(struct{ "title" := "Qual o tempo de vida dos monstros?", "marked" := "120"});
      input_list.append(struct{ "title" := "Digite o x y z onde eles vao aparecer: (6 6 6 para onde o boss esta)", "marked" := "6 6 6"});
      
      var output := QuestionsGump(who, input_list);
      state.+npctemplate := output[1];
      state.+amount := output[2];
      state.+range := cint(output[3]);
      state.+tempo := cint(output[4]);
      
      var loc := output[5];
      if (!loc)
         SendSysMessageEx(who, "Onde vão ser spawnados? (Esc para mesmo local do boss)");
         loc := TargetCoordinates(who);
         if (!loc)
            loc := array{6, 6, 6};
         else
            loc := array{loc.x, loc.y, loc.z};
         endif
      else
         loc := splitwords(loc);
      endif
      
      state.+x := cint(loc[1]);
      state.+y := cint(loc[2]);
      state.+z := cint(loc[3]);

   elseif (opt == "Custom HitScript")
      SendSysMessage(who, "Não implementado");
   elseif (opt == "Set Cooldown")
      var input_list := array{};
      input_list.append(struct{ "title" := "Qual o cooldown?", "marked" := "lockaggro" });
      input_list.append(struct{ "title" := "Quanto tempo vai durar?", "marked" := "10"});
      var output := QuestionsGump(who, input_list);
      state.+prop := output[1];
      state.+time := cint(output[2]);
   endif
   return state;
endfunction
