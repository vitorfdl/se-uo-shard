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
include ":loot:include/lootParser";
include ":yggdrasil:combatevents";

var resist_types := array{DMG_COLD, DMG_FIRE, DMG_ENERGY, DMG_POISON, DMG_PHYSICAL};

function GetBossStruct(targ, serial := 0)
   if (!serial) serial := targ.serial; endif
	var spawnpoint := SystemFindObjectBySerial( GetObjProperty(targ, "spawnpoint") );
	var monsters   := GetObjProperty(spawnpoint, "monsters");
	var me         := FindElemInArray(monsters, struct{ "serial" := serial });

	return me.bossParams;
endfunction

function isBoss(targ, serial := 0)
   if (!serial) serial := targ.serial; endif
	var spawnpoint := SystemFindObjectBySerial( GetObjProperty(targ, "spawnpoint") );
	var monsters   := GetObjProperty(spawnpoint, "monsters");
	var me         := FindElemInArray(monsters, struct{ "serial" := serial });

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
	var template, nome, cor;
   var power, hits;
   var phsycal_res, poison_res, fire_res, cold_res, energy_res;

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

	GFTextLine(chargump, x_pos, y_pos+(18*4), 1051, "Poder[{}]: ".format(cint(cfg.AttackDamage)));
	power := GFTextEntry( chargump, x_pos+65, y_pos+(18*4), 100, 20, 2212, cint(boss.power) + "");

	var attacks      := GetNpcAttacks(npctemplate);
	var boss_attacks := boss.customAttacks;
	GFHtmlShadowed(chargump, x_pos+15, y_pos+(18*9), 250, 20,  "#FFA500", "Attack - Dicedmg - Type - Anim", 1);

	var y := y_pos+(18*9)+15;
	foreach attack in attacks
		GFHtmlShadowed(chargump, x_pos+15, y, 250, 20,  "#CCCCCC", "{} - {} - {} - {}".format(attack.name, attack.dicedmg, attack.damageType, attack.AttackAnim), 1);
		y+= 15;
		sleepms(2);
	endforeach

	var i := 1;
	foreach attack in boss_attacks
		attack := SplitWords(attack, " ");
		GFAddButton(chargump, x_pos, y+1, 2103, 2104, 1, 90+i);
		GFHTMLArea(chargump, x_pos+15, y, 250, 20, "<BASEFONT SIZE=1 color=#93c0ff>{} - {} - {} - {}".format(attack[1], attack[2], attack[3], attack[4]) );
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
	hits := GFTextEntry( chargump, cx_pos+45, y_pos, 100, 20, 1935, "{}".format(cint(boss.hits)));

   y_pos += 18;
	GFTextLine(chargump, cx_pos, y_pos, 1051, "Res. Fisica[{}]:".format(cint(cfg.PhysicalResist)));
	phsycal_res := GFTextEntry( chargump, cx_pos+100, y_pos, 100, 20, 1935, cint(boss.phsycal_res) + "");
   y_pos += 18;
	GFTextLine(chargump, cx_pos, y_pos, 1051, "Res. Calor[{}]:".format(cint(cfg.FireResist)));
	fire_res := GFTextEntry( chargump, cx_pos+100, y_pos, 100, 20, 1935, cint(boss.fire_res) + "");
   y_pos += 18;
	GFTextLine(chargump, cx_pos, y_pos, 1051, "Res. Frio[{}]:".format(cint(cfg.ColdResist)));
	cold_res := GFTextEntry( chargump, cx_pos+100, y_pos, 100, 20, 1935, cint(boss.cold_res) + "");
   y_pos += 18;
	GFTextLine(chargump, cx_pos, y_pos, 1051, "Res. Energia[{}]:".format(cint(cfg.EnergyResist)));
	energy_res := GFTextEntry( chargump, cx_pos+100, y_pos, 100, 20, 1935, cint(boss.energy_res) + "");
   y_pos += 18;
	GFTextLine(chargump, cx_pos, y_pos, 1051, "Res. Veneno[{}]:".format(cint(cfg.PoisonResist)));
	poison_res := GFTextEntry( chargump, cx_pos+100, y_pos, 100, 20, 1935, cint(boss.poison_res) + "");


	y := y_pos+(18*3);

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
         var itemname := lootinfo.item.desc;
         if (lootinfo.by_group)
            itemname := lootinfo.by_group;
         endif
			GFTextLine(chargump, cx_pos+10, y_pos+20+y, 1935, " {} [{}%] [E: {}] ".format(itemname, lootinfo.chance, cint(lootinfo.skinning)));
			y += 18;
			i += 1;
		endforeach
	endif

	y_pos := 486;
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
	boss.phsycal_res         := Cint(GFExtractData(input, phsycal_res));
	boss.fire_res         := Cint(GFExtractData(input, fire_res));
	boss.poison_res         := Cint(GFExtractData(input, poison_res));
	boss.cold_res         := Cint(GFExtractData(input, cold_res));
	boss.energy_res         := Cint(GFExtractData(input, energy_res));
	boss.hits        := Cint(GFExtractData(input, hits));
	boss.power        := Cint(GFExtractData(input, power));
	// boss.dmg         := Cdbl(GFExtractData(input, dmgmod));

	if (input[0] == 1 )
		//SendSysMessage(who, " " + boss + " " );
		//SendSysMessage(who, " " + boss + " " + Cstr(GFExtractData(input, template)) );
		return;
	elseif ( input[0] >= 90 && input[0] <= 95)
		var selected := input[0]-90;
		var boss_attacks := boss.customAttacks;
		if (!boss_attacks) boss_attacks := array{}; endif
		if (boss_attacks[selected])
			var choice := RadioGump(who, 180, 0, "O que deseja fazer?", array{"Remover", "Editar", "Cancelar"}, 3);
			if (choice == 1)
				boss_attacks.Erase(selected);
			elseif (choice == 2)
				var new_attack := RequestGump(who, "Entre com o novo ataque", "Attack name - Bonus - Type - Anim", boss_attacks[selected], 1);
				boss_attacks[selected]  := new_attack;
			endif
			boss.customAttacks := boss_attacks;
		else
			var new_attack := RequestGump(who, "Entre com o novo ataque", "Attack name - Bonus - Type - Anim", "", 1);
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
		var opt := RadioGump(who, 180, 0, "Qual tipo deseja adicionar?", array{"resists", "imunities", "vulnerabilities"});
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
      if (!boss.loot) boss.loot := array{}; endif

		if ( boss.loot[index] != error )
			var opt := RadioGump(who, 180, 0, "Editar item", array{"Criar", "Deletar", "Alterar Chance", "Inverter Escapelar/Loot", "Cancelar"});			
			if (opt == "Deletar")
				boss.loot.erase(index);
				SendSysMEssageEx(who, "O item foi removido.", SSM_INFO);
			elseif ( opt == "Criar" )
				var retitem := CreateItemInBackpackFromTemplate(who, boss.loot[index].item);
				if (!retitem)
					SendSysMessageEx(who, "erro: " + retitem);
				endif
				SendSysMessageEx(who, "Você criou o item na sua mochila."); 
			elseif (opt == "Inverter")
            if (cint(boss.loot[index].skinning) == 0)
               boss.loot[index].skinning := 1;
            else
               boss.loot[index].skinning := 0;

            endif
			elseif (opt == "Alterar Chance")
				boss.loot[index].chance := Cint(RequestGump(who, "Digite a chance do item aparecer. 1 a 100:", "", ""+cint(boss.loot[index].chance), 1));
			endif 
		else
         var t_option := RadioGump(who, 180, 165, "Você deseja?:", array{"Adicionar lista", "Adicionar item", "Cancelar"});
         case (t_option)
            "Adicionar lista":
               var loot_cfg  := ReadConfigFile(":loot:config/loot-groups");
               var loot_list := GetConfigStringKeys(loot_cfg);
               var input_list := array{};
               input_list.append(struct{ "title" := "Qual o grupo de loot?", "marked" := 1, "checkbox" := loot_list});
               input_list.append(struct{ "title" := "Quantos itens podem dropar?", "marked" := "1d4" });
               input_list.append(struct{ "title" := "Qual a chance de drop?", "marked" := "60" });
               input_list.append(struct{ "title" := "Forçar items em recipes?", "marked" := 2, "radio" := array{"Sim", "Não"} });
               var output := QuestionsGump(who, input_list);
               if (!output)
                  break;
               endif
               foreach glist in (output[1])
                  var lootinfo := struct{
                     "by_group":= glist,
                     "rate":= output[2],
                     "chance":= cint(output[3]),
                     "forceitem" := output[4] == "Sim"
                  };
                  (boss.loot).append(lootinfo);
                  sleepms(2);
               endforeach
               break;
            "Adicionar item":
               SendSysMessage(who, "Escolha o item do loot.");
               var targ := Target(who);
               if (targ)
                     var lootinfo := struct{};
                     lootinfo.+chance := Cint(RequestGump(who, "Digite a chance do item aparecer. 1 a 100:", "", "", 1));
                     lootinfo.+item := CreateItemTemplate(targ);				
                     (boss.loot).append(lootinfo);
               endif
               break;		
         endcase
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
			var opt := RadioGump(who, 250, 0, "Editar item", array{"Armorscript", "AttackHitscript", "Criticalscript", "Special IA", "Cancelar"});
         var spec := struct{
            "name" := opt
         };

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
	var cfg :=	NPC_GetNPCConfig(boss.npctemplate);

	if (boss.name != "-" && boss.name != "-random-")
		SetName(npc, boss.name);
	endif

	npc.color := boss.color;
	npc.truecolor := boss.color;

   if (boss.power) npc.damage_physical_mod := boss.power; endif
   if (boss.phsycal_res) npc.resist_physical_mod := boss.phsycal_res; endif
   if (boss.fire_res) npc.resist_physical_mod := boss.fire_res; endif
   if (boss.energy_res) npc.resist_physical_mod := boss.energy_res; endif
   if (boss.poison_res) npc.resist_physical_mod := boss.poison_res; endif
   if (boss.cold_res) npc.resist_physical_mod := boss.cold_res; endif
	
	if (boss.customAttacks && boss.customAttacks[0])
		var customAttacks := array{};
		foreach attack in (boss.customAttacks)
			attack := FormatAttack(boss.npctemplate, attack, cint(cfg.AttackDamage) + cint(boss.power));
			customAttacks.append(attack);
		endforeach
		SetObjProperty(npc, "customAttacks", customAttacks);
	endif

  	SetObjProperty(npc, "hitsmod", cint(boss.hits));
  	RecalcVitals(npc);
	AP_RefreshVitals(npc);

	if (!npc.backpack)
		var backpack := CreateItemAtLocation(npc.x, npc.y, npc.z,  2482,  1, npc.realm);
		EquipItem(npc, backpack);
	endif

   var skinning_info := array{};
   foreach loot in (boss.loot)
      if(loot.by_group)
         Loot_Generate(npc.backpack, loot.by_group, loot);
      elseif (loot.skinning)
         skinning_info.append(loot);
      elseif( RandomDiceRoll("1d100") <= loot.chance)
         var item := CreateItemAtLocationFromTemplate(npc.x, npc.y, npc.z, _DEFAULT_REALM, loot.item );
         MoveItemToContainer(item, npc.backpack);
    		item.movable := 1;
      endif
      sleepms(2);
   endforeach

   if (skinning_info.size())
      SetObjProperty(npc, "skinninginfo", skinning_info);
   endif

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

function FormatAttack(npctemplate, new_attack, AttackDamage)
	var attack := SplitWords(attack, " ");

	attack := struct{
		"name"    := attack[1],
		"dicedmg"    := attack[2],
		"DamageType" := attack[3],
		"AttackAnim" := cint(attack[4]),
      "AttackDamage" := AttackDamage,
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