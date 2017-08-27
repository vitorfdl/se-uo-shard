
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

var resist_types := ["Veneno", "Fogo", "Eletricidade", "Gelo", "Perfuração", "Corte", "Contusão", "Strength", "Dexterity", "Intelligence"];

function GetBossStruct(targ)
	var spawnpoint := SystemFindObjectBySerial( GetObjProperty(targ, "spawnpoint") );
	var monsters   := GetObjProperty(spawnpoint, "monsters");
	var me         := FindElemInArray(monsters, struct{ "serial" := targ.serial });
	return me.uniquestat;
endfunction

function isBoss(targ)
	var spawnpoint := SystemFindObjectBySerial( GetObjProperty(targ, "spawnpoint") );
	var monsters   := GetObjProperty(spawnpoint, "monsters");
	var me         := FindElemInArray(monsters, struct{ "serial" := targ.serial });
	if (me.uniquestat)
		return 1;
	endif

	return 0;
endfunction

function CreateBossStructGump(who, byref boss)
	if (boss.CA)
		boss.+CA := boss.CA;
	endif

	var chargump := GFCreateGump();
	var input;

	var cfg := ReadConfigFile(":ghaia:npcdesc");
	var npctemplate := boss.npctemplate;
	npctemplate := npctemplate[10, len(npctemplate)-9];
	cfg := cfg[npctemplate];

	var template, nome, cor, camod, dmgmod, specialai;
	var str, dex, int, con, wis, hits;

	//GFClosable(chargump, 0);
	GFDisposable(chargump, 0);

	GFResizePic(chargump, 0, 25, 3700, 500, 500);
	GFResizePic(chargump, 15, 40, 0x13BE, 470, 470);

	GFTextMid(chargump, 15, 50, 300, 1153, "Boss Menu");
	GFTextLine(chargump, 50, 80, 1153,  "NpcTemplate:");
	if ( boss.npctemplate )
		template := GFTextEntry( chargump, 140, 80, 150, 20, 1150, boss.npctemplate);
	else
		template := GFTextEntry( chargump, 140, 80, 150, 20, 1150, ":ghaia:");
	endif

	GFTextLine(chargump, 50, 95, 1153, "Nome: ");
	if (boss.name)
		nome := GFTextEntry( chargump, 140, 95, 100, 20, 1150, boss.name);
	else
		nome := GFTextEntry( chargump, 140, 95, 100, 20, 1150, "-");
	endif

	GFTextLine(chargump, 50, 110, 1153, "Cor: ");
	cor := GFTextEntry( chargump, 180, 110, 100, 20, 1150, cint(boss.color) + "");
	if (cfg)
		GFTextLine(chargump, 50, 125, 1153, "Str ["+ cfg.Strength+ "]:");
		GFTextLine(chargump, 50, 140, 1153, "Int [" + cfg.Intelligence + "]:");
		GFTextLine(chargump, 50, 155, 1153, "Dex [" + cfg.Dexterity + "]:");
		GFTextLine(chargump, 50, 170, 1153, "Con [" + cfg.Constitution + "]:");
		GFTextLine(chargump, 50, 185, 1153, "Wis [" + cfg.Wisdom + "]:");
	else
		GFTextLine(chargump, 50, 125, 1153, "Str: ");
		GFTextLine(chargump, 50, 140, 1153, "Int: ");
		GFTextLine(chargump, 50, 155, 1153, "Dex: ");
		GFTextLine(chargump, 50, 170, 1153, "Con: ");
		GFTextLine(chargump, 50, 185, 1153, "Wis: ");
	endif

	str := GFTextEntry( chargump, 180, 125, 100, 20, 1150, cint(boss.str) + "");
	int := GFTextEntry( chargump, 180, 140, 100, 20, 1150, cint(boss.int) + "");
	dex := GFTextEntry( chargump, 180, 155, 100, 20, 1150, cint(boss.dex) + "");
	con := GFTextEntry( chargump, 180, 170, 100, 20, 1150, cint(boss.con) + "");
	wis := GFTextEntry( chargump, 180, 185, 100, 20, 1150, cint(boss.wis) + "");

	if (cfg)
		GFTextLine(chargump, 50, 200, 1153, "HP [" + cfg.Hits+ "]: +");
	else
		GFTextLine(chargump, 50, 200, 1153, "HP: +");
	endif
	hits := GFTextEntry( chargump, 180, 200, 100, 20, 1150, cint(boss.hits) + "");

	var attacks     := GetNpcAttacks(npctemplate);
	var boss_attcks := boss.customAttacks;
	GFTextLine(chargump, 50, 218, 1153, "Attack - Dice - Bonus - Type - Anim");
	var y := 236;
	foreach attack in attacks
		GFTextLine(chargump, 70, y, 1153, "{} - {} - {} - {} - {}".(attack.name, attack.dicedmg, attack.hitbonus, attack.damageType, attack.AttackAnim));
		y+= 18;
		sleepms(2);
	endforeach

	foreach attack in boss_attacks
		attack := SplitWords(attack, " ");
		GFTextLine(chargump, 70, y, 1153, "{} - {} - {} - {} - {}".(attack[1], attack[2], attack[3], attack[4], attack[5]));
		y+= 18;
		sleepms(2);
	endforeach
	GFAddButton( chargump, 120, y, 2103, 2104, 1, 90);
	// dmgmod := GFTextEntry( chargump, 180, 200, 100, 20, 1150, cint(boss.dmg) + "");

  	//if (cfg.specialai != error)
	//	GFTextLine(chargump, 50, 200, 1153, "Special AI [" + cfg.specialai+ "]:"); ///////////////////////
//	else
		//GFTextLine(chargump, 50, 200, 1153, "Special AI: ");
	//endif
  
	GFTextLine(chargump, 250, 80, 1153,  "Resistencias");
	if (cfg)
		GFTextLine(chargump, 250, 95, 1153, "CA [" + cfg.AR + "]:");
	else
		GFTextLine(chargump, 250, 95, 1153, "CA: ");
	endif
	camod := GFTextEntry( chargump, 350, 95, 100, 20, 1150, cint(boss.CA) + "");

	var resists   := GetConfigStringArray(cfg, "Resistencia");
	var imunities := GetConfigStringArray(cfg, "Imunidade");
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

	y := 110;
	foreach resist in resist_types
		var type := "R";
		if (resist in imunities)
			type := "I";
		elseif (resist in vulnerabilities)
			type := "V";
		elseif (!(resist in resists))
			sleepms(2);
			continue;
		endif
		GFTextLine(chargump, 250, y, 1153, "[{}] {}".format(type, resist));
		y+= 18;
	endforeach
	GFAddButton( chargump, 300, y, 2103, 2104, 1, 80);

	y += 18;

	var skills := GetConfigStringArray(cfg, "Pericia");
	if (boss.skills)
		skills += boss.skills;
	endif
	GFTextLine(chargump, 250, y, 1153, "Pericias: ");
	y+= 18;
	foreach skill in skills
		GFTextLine(chargump, 250, y, 1153, "{}".format(skill));
		y+= 18;
		sleepms(2);
	endforeach
	GFAddButton( chargump, 300, y, 2103, 2104, 1, 70);
	// y+= 18;
	
	GFTextLine(chargump, 50, 240, 1153, "Loot: ");
	GFAddButton( chargump, 50, 263, 2103, 2104, 1, 10);
	if (boss.loot)
		var y := 0;
		var i := 0;
		foreach lootinfo in (boss.loot)
			GFAddButton( chargump, 50, 283+y, 2103, 2104, 1, 11+i);
			GFTextLine(chargump, 70, 270+y, 1150, " " + lootinfo.item.desc + " [" + lootinfo.chance + "%]");
			y := y + 15;
			i := i + 1;
		endforeach
	endif

	GFTextLine(chargump, 50, 340, 1153, "Death Events: ");
	GFAddButton( chargump, 50, 363, 2103, 2104, 1, 20);
	if (boss.deathevent)
		var y := 0;
		var i := 0;
		foreach item in (boss.deathevent)
			GFAddButton( chargump, 50, 383+y, 2103, 2104, 1, 21+i);
			GFTextLine(chargump, 70, 370+y, 1150, " " + item.type);
			y := y + 15;
			i := i + 1;
		endforeach
	endif

	GFTextLine(chargump, 250, 340, 1153, "Special: ");
	GFAddButton( chargump, 250, 363, 2103, 2104, 1, 30);
	if (boss.special)
		var y := 0;
		var i := 0;
		foreach item in (boss.special)
			GFAddButton( chargump, 250, 383+y, 2103, 2104, 1, 31+i);
			GFTextLine(chargump, 270, 370+y, 1150, " " + item.name);
			y := y + 15;
			i := i + 1;
		endforeach
	endif


   GFTextLine(chargump, 50, 420, 1153, "Combat Events: ");
	GFAddButton( chargump, 50, 443, 2103, 2104, 1, 40);
	if (boss.combatevent)
		var y := 0;
		var i := 0;
		foreach item in (boss.combatevent)
			GFAddButton( chargump, 50, 463+y, 2103, 2104, 1, 41+i);
			GFTextLine(chargump, 70, 440+y, 1150, " " + item.name);
			y := y + 15;
			i := i + 1;
		endforeach
	endif

	GFAddButton(chargump, 380, 467, 4502, 4502, 1, 1 );
	GFTextLine(chargump, 340, 480, 1153, "Fim");

	input := GFSendGump(who, chargump);	
  
	boss.+npctemplate := Cstr(GFExtractData(input, template));
	boss.+name        := Cstr(GFExtractData(input, nome));
	boss.+color       := Cint(GFExtractData(input, cor));
	boss.+str         := Cint(GFExtractData(input, str));
	boss.+dex         := Cint(GFExtractData(input, dex));
	boss.+int         := Cint(GFExtractData(input, int));
	boss.+wis         := Cint(GFExtractData(input, wis));
	boss.+con         := Cint(GFExtractData(input, con));
	boss.+resist      := Cint(GFExtractData(input, resistmod));
	boss.+hits        := Cint(GFExtractData(input, hits));
	boss.+dmg         := Cdbl(GFExtractData(input, dmgmod));
	boss.+CA    		:= Cint(GFExtractData(input, ca));

	if (input[0] == 1 )
		//SendSysMessage(who, " " + boss + " " );
		//SendSysMessage(who, " " + boss + " " + Cstr(GFExtractData(input, template)) );
		return;
	elseif ( input[0] == 90)
		var boss_attacks := boss.customAttacks;
		var attacks := array{};
		foreach attack in boss_attacks
			attacks.append(attack.name);
		endforeach
		attacks.append("Novo Ataque");
		var opt := RadioGump(who, 180, 200+(attacks.length() * 18), "Escolha o ataque", attacks, 1);
		if (opt != attacks.length())
			var choice := RadioGump(who, 180, 220, "O que deseja fazer?", array{"Remover", "Editar", "Cancelar"}, 1);
			if (choice == 1)
				boss_attacks.Erase(opt);
			elseif (choice == 2)
				var new_attack := RequestGump(who, "Entre com o novo ataque", "Attack name - Dice - Bonus - Type - Anim", attacks[opt], 1);
				boss_attacks[opt] := new_attack;
			endif
		else
			var new_attack := RequestGump(who, "Entre com o novo ataque", "Attack name - Dice - Bonus - Type - Anim", "", 1);
			boss_attacks.append(new_attack);
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
		var opt := RadioGump(who, 180, 220, "Qual tipo deseja adicionar?", array{"Resistência", "Imunidade", "Vulnerabilidade"}, 1);
		var opt_checked := array{};
		foreach resist in resist_types
			if (opt == 2 && resist in boss.imunities)
				opt_checked[_resist_iter] := true;
			elseif (opt == 1 && resist in boss.resists)
				opt_checked[_resist_iter] := true;
			elseif (opt == 3 && resist in boss.vulnerabilities)
				opt_checked[_resist_iter] := true;
			endif
		endforeach
		var opts := CheckBoxGump(who, 180, 200+(resist_types.length() * 18), "Escolha os tipos:", resist_types, opt_checked);
		if (opt == 1)
			boss.resists := opts;
		elseif(opt == 2)
			boss.imunities := opts;
		elseif(opt == 3)
			boss.vulnerabilities := opts;
		endif
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
				SendSysMessageEx(who, "Voce criou o item na sua mochila."); 
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

		CreateBossStructGump(who, boss);

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
		CreateBossStructGump(who, boss);
	elseif ( input[0] >= 40 && input[0] <= 45)
		var index := input[0] - 39;
		if (boss.combatevent[index] != error )
			var yes := YesNo(who, "Editar ou Deletar Evento?", "Editar", "Deletar");
			if (yes == 1)
				var nevent := ConfigureCombatEvent(who, boss.combatevent[index] );
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
		CreateBossStructGump(who, boss);

	elseif ( input[0] >= 30 && input[0] <= 35)
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
			var opt := RadioGump(who, 250, 270, "Editar item", array{"Special Hit Anim", "Life Drain", "Dmg Return", "Custom Hitscript", "Special AI"});
			var spec := struct;
			spec.+name := opt;
			if (!boss.special)
				boss.+special := array;
			endif
			if ( opt == "Dmg Return")
				spec.+amt := Cdbl(RequestGump(who, "Digite quanto sera retornado. 0.1 a 1.0:", "", "", 1));
			elseif (opt == "Custom Hitscript")
					spec.+hitopt := configurehitscript(who);
			elseif (opt == "Special AI")
				var ainame := Cstr(RequestGump(who, "Digite o nome da IA", "", "", 1));
				spec.+ai := ainame;
			elseif (opt == "Special Hit Anim")
				var hitanim := Hex(RequestGump(who, "Digite o hex/int da Animação", "", "", 1));
				spec.+anim := hitanim;
			endif
			boss.special.append(spec);
		endif

		CreateBossStructGump(who, boss);
	endif

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


	AP_SetTrueStat(npc, STRENGTH, Cint(boss.str));
	AP_SetTrueStat(npc, DEXTERITY, Cint(boss.dex));
	AP_SetTrueStat(npc, INTELLIGENCE, Cint(boss.int));
	AP_SetTrueStat(npc, WISDOM, Cint(boss.wis));
	AP_SetTrueStat(npc, CONSTITUTE, Cint(boss.con));
	
	foreach skill in boss.skills
		AP_SetTrueSkill(npc, skill, 1);
		// sleepms(2);
	endforeach

	if (boss.customAttacks && boss.customAttacks[0])
		var customAttacks = array{};
		foreach attack in (boss.customAttacks)
			attack := FormatAttack(npctemplate, attack);
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
   //   printtextabove(npc,  " " + spec.name);
			if (spec.name == "Ice Hit")
				var hit_scripts := GetObjProperty(npc, "HitScripts");
				if ( !hit_scripts )
					hit_scripts := array{};
				endif
				hit_scripts.append(":combat:weaponHitScripts/IceHit");
				SetObjProperty(npc, "HitScripts", hit_scripts);
			elseif (spec.name == "Fire Hit")
				var hit_scripts := GetObjProperty(npc, "HitScripts");
				if ( !hit_scripts )
					hit_scripts := array{};
				endif
				hit_scripts.append(":combat:weaponHitScripts/FireHit");
				SetObjProperty(npc, "HitScripts", hit_scripts);
			elseif (spec.name == "Energy Hit")
				var hit_scripts := GetObjProperty(npc, "HitScripts");
				if ( !hit_scripts )
					hit_scripts := array{};
				endif
				hit_scripts.append(":combat:weaponHitScripts/LightningHit");
				SetObjProperty(npc, "HitScripts", hit_scripts);
			elseif (spec.name == "Life Drain")
				var hit_scripts := GetObjProperty(npc, "HitScripts");
				if ( !hit_scripts )
					hit_scripts := array{};
				endif
				hit_scripts.append(":combat:weaponHitScripts/draintouch");
				SetObjProperty(npc, "HitScripts", hit_scripts);
			elseif (spec.name == "Dmg Return")
				var hit_scripts := GetObjProperty(npc, "ArmorHitScripts");
				if ( !hit_scripts )
					hit_scripts := array{};
				endif
				hit_scripts.append(":combat:armorHitScripts/dmgreturn");
				SetObjProperty(npc, "ArmorHitScripts", hit_scripts);
				SetObjProperty(npc, "dmgreturn",  spec.amt);
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
		return error{"errortext":="Nao foi possivel criar o boss em x " + x + " y " + y + " z  " + z};
	endif
	
	SetBoss(npc, boss);
	
	return npc;
endfunction

function ConfigureCombatEvent(who, event := 0)
	if (!event)
		event := struct;
		event.+name := RequestGump(who, "Digite o nome desse evento de combate", "Ex.: Ataque Multiplo", "", 1);
		event.+cd := cint(RequestGump(who, "De quanto em quanto tempo o npc realiza esse evento?", "", "", 1));
		event.+onlybloodied := cint(YesNo(who, "O NPC so faz esse evento se tiver bloodied?"));
		event.+dist := cint(RequestGump(who, "Qual a distancia que o npc precisa estar do alvo?", "0 para nao checar distancia", "", 1));

		event.+steps := array;
	endif
	var continua := 1;

	while (continua)
	var gumpcombat := GFCreateGump();

	GFDisposable(gumpcombat, 0);
	GFResizePic(gumpcombat, 0, 25, 3700, 250, 430);	
	GFResizePic(gumpcombat, 15, 40, 0x13BE, 220, 400);
	GFTextMid(gumpcombat, 20, 50, 250, 1153, "Combat Events");
	
	var y := 64;
	GFTextLine(gumpcombat, 35, y, 1153, "Adicionar Novo ");
	GFAddButton(gumpcombat, 20, y+2, 2117, 2118, 1, 100);
	
	var i := 1;
	foreach state in (event.steps)
		y := y+ 15;
		GFTextLine(gumpcombat, 35, y, 1153, ""+state.type);
		GFAddButton(gumpcombat, 20, y+2, 2117, 2118, 1, 400+i);
		i := i + 1;
	endforeach

	GFAddButton(gumpcombat, 180, 387, 4502, 4502, 1, 1000 );
	GFTextLine(gumpcombat, 140, 400, 1153, "Fim");
	
	var input := GFSendGump(who, gumpcombat);

	if (input[0] == 100)
		var state := CombatEventsOptions(who, struct{});
		event.steps.append(state);
	elseif (input[0] == 1000)
		continua := 0;
	elseif (input[0] >= 400)
		input := input[0] - 400;
		event.steps.Erase(input);
		if (YesNo(who, "Deseja Alterar ou Remover?","Alterar","Remover") )
			var state := CombatEventsOptions(who, struct{});
			event.steps.Insert(input, state);
		else
			SendSysMessage(who, "Evento Removido");
		endif
	endif
	endwhile



	return event;

endfunction

function CombatEventsOptions(who, state)
		var opt := RadioGump(who, 180, 370, "Adicionar Estado", array{"Move", "Teleport", "TimedScript", "Perform Action", "Text", "Play Sound", "Heal", "Dmg", "Graphic Effect",  "Create Items", "Summon Npcs", "Custom HitScript", "Delay"});	

		state.type := opt;
		if (opt == "Move")
			SendSysMessageEx(who, "Para onde o npc deve andar?");
			var loc := TargetCoordinates(who);
			state.+x := loc.x;
			state.+y := loc.y;
			state.+z := loc.z;
		elseif ( opt == "Teleport")
			state.+target := RadioGump(who, 210, 200, "Quem vai sofrer o teleporte?", array{"Self", "Opponent", "All Opponents"});
			var loc := TargetCoordinates(who);
			state.+x := loc.x;
			state.+y := loc.y;
			state.+z := loc.z;
		elseif ( opt == "TimedScript")
			state.+target := RadioGump(who, 200, 180, "Quem vai realizar a action?", array{"Self", "Opponent", "All Opponents"});
			var timername := RadioGump(who, 270, 580, "Escolha o timedscript", array{"defaultPoison", "hallucination", "Paralyze", "clumsy", "feeblemind", "weaken", "atrasarmagias", "fugademana", "staminaPoison", "manaPoison", "bleed", "darkborer", "agility", "strength", "invisibilidade", "desviarprojetil", "dmgreduction", "evasao", "precisao", "phmod", "fimod", "comod", "pomod", "enmod"});
			state.+timername := timername;
			state.+timerduration := cint(RequestGump(who, "Qual a duracao do timer?", "Nunca mais de 120s", "", 1));
			state.+level := cint(RequestGump(who, "Qual o level do timer?", "", "", 1));

		elseif ( opt == "Perform Action" )
			state.+target := RadioGump(who, 200, 180, "Quem vai realizar a action?", array{"Self", "Opponent", "All Opponents"});
			state.+id := cint(RequestGump(who, "Qual o ID da animacao?", "", "", 1));
		elseif (opt == "Text")
			state.+target := RadioGump(who, 200, 180, "Quem vai falar?", array{"Self", "Opponent", "All Opponents"});
			state.+text := RequestGump(who, "Qual o texto ?", "", "", 1);
			state.+color := cint(RequestGump(who, "Qual a cor do texto?", "", "1000", 1));
		elseif (opt == "Play Sound")
			state.+id := cint(RequestGump(who, "Qual o ID do som?", "", "", 1));
		elseif (opt == "Delay")
			state.+delay := cint(RequestGump(who, "Quanto tempo de delay?", "", "", 1));
		elseif (opt == "Heal")
			state.healamt := cint(RequestGump(who, "Quanto vai healar?", "", "", 1));
		elseif (opt == "Graphic Effect")
			state.+target := RadioGump(who, 250, 200, "Onde vai aparecer o efeito? (fixed loc deve ser Moving)", array{"Self", "Opponent", "All Opponents", "Fixed Loc"});
			state.+graphictype := RadioGump(who, 180, 150, "Qual o tipo de efeito?", array{"Stationary", "Moving"});
			if (state.graphictype == "Stationary")
				var fxid :=  RadioGump(who, 230, 300, "Qual o efeito?", array{"Lightning", "Red Sparkles", "Blue Sparkles", "Blue and Swirl", "Swirl Sparkles", "Blue Sparkles on Ground", "Fire Column", "Outro"});
				if ( fxid == "Lightning" )
					state.+fxid := fxid;
				elseif ( fxid == "Red Sparkles")
					state.+fxid := GFX_RED_SPARKLES;
					state.+speed := 7;
					state.+loops := 7;
				elseif ( fxid == "Blue Sparkles")
					state.+fxid := GFX_BLUE_SPARKLES;
					state.+speed := 10;
					state.+loops := 10;
				elseif (fxid == "Blue and Swirl")
					state.+fxid := GFX_BLUE_SPARKLE_SWIRL;
					state.+speed := 7;
					state.+loops := 16;
				elseif (fxid == "Swirl Sparkles")
					state.+fxid := GFX_BLUE_SPARKLE_BORDER;
					state.+speed := 7;
					state.+loops := 0;
				elseif (fxid == "Blue Sparkles on Ground")
					state.+fxid := GFX_BLUE_SPARKLE_GROUND;
					state.+speed := 7;
					state.+loops := 10;
				elseif (fxid == "Fire Column");
					state.+fxid := GFX_FIRE_COLUMN;
					state.+speed := 7;
					state.+loops := 0;
				elseif (fxid == "Outro")
					state.+fxid := hex(RequestGump(who, "Qual o ID do efeito?", "", "", 1));
					state.+speed := 7;
					state.+loops := 16;
				endif
				
			elseif (state.graphictype == "Moving")
				var fxid := RadioGump(who, 180, 280, "Qual o efeito?", array{"Small Fireball", "Large Fireball", "Wavy Fireball", "Other"});
				state.+direction := RadioGump(who, 200, 180, "Qual vai ser a direcao do efeito?", array{"Mob->Player", "Player->Mob"});
				if (fxid == "Small Fireball")
					state.fxid := GFX_SMALL_FIREBALL;
					state.+speed := 5;
					state.+loops := 0;
				elseif (fxid == "Large Fireball")
					state.fxid := GFX_LARGE_FIREBALL;
					state.+speed := 5;
					state.+loops := 0;
				elseif (fxid == "Wavy Fireball")
					state.fxid := GFX_WAVY_FIREBALL;
					state.+speed := 5;
					state.+loops := 0;
				elseif (fxid == "Other")
					state.+fxid := cint(RequestGump(who, "Qual o ID do efeito?", "", "", 1));
					state.+speed := 5;
					state.+loops := 0;
				endif
				if (state.target == "Fixed Loc")
					var loc := RequestGump(who, "Digite o x y z onde o efeito vai aparecer:", "", "", 1);
	                		loc := splitwords(loc);
        	        		state.+startx :=  loc[1];
         	       			state.+starty :=  loc[2];
               	 			state.+startz :=  loc[3];
 					var radius :=   RequestGump(who, "Qual o raio de onde o efeito vai aparecer:", "0 se for sempre no mesmo lugar", "0", 1);
					state.startradius := radius;
	
					if (state.graphictype == "Moving")
						var loc2 := RequestGump(who, "Digite o x y z da posicao final do efeito:", "", "", 1);
	                			loc2 := splitwords(loc);
        	        			state.+endx :=  loc2[1];
         	       				state.+endy :=  loc2[2];
               	 				state.+endz :=  loc2[3];
 						var radius :=   RequestGump(who, "Qual o raio de onde o efeito vai terminar:", "0 se for sempre no mesmo lugar", "0", 1);
						state.endradius := radius;

					endif

				endif
			endif
		elseif ( opt == "Dmg")
			state.+target := RadioGump(who, 200, 180, "Quem vai sofrer?", array{"Self", "Opponent", "All Opponents"});
			var dmg := cint(RequestGump(who, "Quanto de dano vai causar", "", "", 1));
			var dmgtype := RequestGump(who, "Digite qual o tipo de dano: Perfuração, Concussão, Corte, Fogo, Gelo, Eletrecidade, Veneno", "", "", 1);
			state.+dmg := dmg;
			state.+dmgtype := dmgtype;
			var vitals := CheckBoxGump(who, 230, 180, "Qual tipo de vital sera afetado pelo dano?", array{"Stam", "Hits", "Mana"});
			state.+vitals := vitals;
		elseif ( opt == "Create Items")
			SendSysMessage(who, "Nao implementado");
		elseif (opt == "Summon Npcs")
        	        state.+amount := Cint(RequestGump(who, "Quantos npcs vao ser spawnados:", "", "", 1));
	                var loc := RequestGump(who, "Digite o x y z onde eles vao aparecer:", "Escreva 6 6 6 se for para aparecer onde o boss esta", "", 1);
	                loc := splitwords(loc);
        	        state.+x :=  loc[1];
         	       	state.+y :=  loc[2];
               	 	state.+z :=  loc[3];
               	 	state.+range := Cint(RequestGump(who, "Digite o raio de spawn:", "", "", 1));
                	state.+npctemplate := RequestGump(who, "Digite o npctemplate:", "", "", 1);
					state.+tempo := Cint(RequestGump(who, "Coloque o tempo de vida do monstro(2 minutos default):", "", "", 1));

		elseif (opt == "Custom HitScript")
			SendSysMessage(who, "Nao implementado");
		endif
		return state;
endfunction