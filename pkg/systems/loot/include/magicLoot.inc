use uo;
use util;
use cfgfile;
use os;

include "include/damage";
include ":tn:equipment";
include ":magery:tnmagery";

//Magic Weapons 1 10 3
function Loot_CreateMagic(container, line)
	var parsed := Loot_ParseMagicString(line);

	if ( Loot_ChanceCheck(parsed.chance) )
		var i;
		for ( i:=1; i<= parsed.amount; i:=i+1 )
			var item := CreateMagicItem(container, line);
			if ( item.errortext )
				return error{"errortext" := "Error - Could not create ["+parsed.name+"] ->"+item.errortext};
			endif
			sleepms(2);
		endfor
	endif
	
	return 1;

endfunction

function CreateMagicItem(container, line)
	var parsed := Loot_ParseMagicString(line);
	var groups_cfg := ReadConfigFile(":loot:config/loot-groups");
	if ( groups_cfg.errortext )
		return error{"errortext" := "Error - Could not open loot-groups.cfg -> "+groups_cfg.errortext};
	endif
	
	var groups_elem := groups_cfg[parsed.name];
	if ( !groups_elem )
		return error{"errortext" := "Error - loot-groups.cfg elem ["+parsed.name+"] does not exist."};
	endif
	
	var items := GetConfigStringArray(groups_elem, "Item");
	var num_items := items.size();
	var i;
	var itemName := items[RandomInt(num_items)+1];
	var parsed2 := Loot_ParseMagicString( items[RandomInt(num_items)+1]);
	var itemobj := cint(GetObjTypeByName(itemName));
	var item := CreateItemInContainer(container, parsed2.name, 1);

	SetMagicTemplateBonus(item, parsed);	

	return 1;

endfunction

function SetMagicTemplateBonus(item, parsed)

	var cfg := ReadConfigFile(":loot:config/templates");
	var elem := FindConfigElem(cfg, parsed.template());
	printtextabove(item, "elem " + elem + " cfg " + cfg + " elemname " + parsed.template());

	if (!elem)
		return elem;
	endif

	//rola o dano pra ver que buff sai
	var rand := RandomInt(100) + 1;

	//ve o que pode sair de magico no template
	var possiveis := GetConfigStringDictionary( elem, "chance" );


	var chosen; //escolhe um dos bonus magicos
	foreach x in (possiveis.keys())
	
		var margem := SplitWords(possiveis[x]);
		var minv := Cint(margem[1]);
		var maxv := Cint(margem[2]);
	
		if ((rand >= minv) && (rand <= maxv))
			chosen := x;
			
			break;
		endif
		
	endforeach
	
	//seta o bonus magico no item
	
	if (chosen)
		var bonus := GetConfigStringDictionary( elem, chosen );
		
		if (bonus != error)
			var bonusKeys := bonus.keys();
			var selected := bonusKeys[ RandomInt(bonusKeys.size())+1];

			var mods := dictionary;
			if (chosen == "skill")
				mods[selected]	:= GetBonusAmount(parsed.level, chosen);
			elseif (chosen == "vital")
				mods[selected] := GetBonusAmount(parsed.level, chosen);
			elseif (chosen == "status")
				mods[selected] := GetBonusAmount(parsed.level, chosen);
			endif

			var cores := GetConfigString(elem, "cores");
			if (cores != error)
				cores := SplitWords(cores);
				SetObjProperty(item, "magicColor", cores[RandomInt(cores.size())+1]);
			endif

			SetObjProperty(item, "equip", mods);
			SetObjProperty(item, "magic", 1);

		endif
	endif

endfunction

function GetBonusAmount(level, type)

	if (type == "skill")
		return (level*3)+2;
	elseif (type == "vital")
		return level*4;
	elseif( type == "status")
		return level*3;
	endif

endfunction

function GetRandomResistance()
	
	var resist := RandomDiceRoll("1d5");

	if (resist == 1)
		return "PhysicalResistMod";
	elseif (resist == 2)
		return "FireResistMod";
	elseif (resist == 3)
		return "ColdResistMod";
	elseif (resist == 4)
		return "PoisonResistMod";
	elseif (resist == 5)
		return "EnergyResistMod";
	endif

endfunction

function SetMagicBonus(item, parsed)

	//armas podem ter:
	// 1- dmgmod
	// 2- aumento na chance de acerto
	var mods := dictionary;
	if (item.isA(POLCLASS_WEAPON) )

		var roll := RandomDiceRoll("1d2");
		if ( roll == 1) //aumento no dano
			var dmg_mod := (RandomInt(parsed.level)) + parsed.level;  //de 1 a 10
			mods["dmg_mod"] := dmg_mod;
		elseif ( roll == 3)
			mods["chancetohit"]:= parsed.level * 3;
			//SetObjProperty(item, "chancetohit", parsed.level * 3); //de +3 a +15
		endif

		if (parsed.level > 3) //lvl 4 ou 5 tem 10% de chance de ter hitscript
			if (RandomInt(100) < 10 )
				var hits := RandomDiceRoll("1d3");
				if (hits == 1)
					mods["hitscript"] := ":combat:weaponHitScripts/FireHit";
				elseif (hits == 2)
					mods["hitscript"] := ":combat:weaponHitScripts/IceHit";
				else
					mods["hitscript"] := ":combat:weaponHitScripts/LightningHit";
				endif
			endif
		endif	

	//armaduras podem ter:
	//1- aumento em alguma defesa
	elseif (item.isa(POLCLASS_ARMOR) )
		var resistance := GetRandomResistance();
		//var resistances := GetObjProperty(item, "Resistances");

		//resistances[resistance] := resistances[resistance] + parsed.level * 2; //de +2 a +10
		//SetObjProperty(item, "MagicResistances", resistances);
		mods[resistance] :=  parsed.level * 2;

	//se for outros itens (roupas ou joias)
	// 1- bonus no stat, 
  	// 2- vitals ou
	// 3- skills
	else
		var nroll := RandomDiceRoll("1d3");
		var stat;
		var amt;
		if (nroll == 1)
			stat := RandomDiceRoll("1d3");
			amt := (RandomInt(parsed.level)) + parsed.level*2;  //de 3 a 15
			if (stat == 1)
				mods["strmod"] := amt;
			elseif (stat == 2)
				mods["dexmod"] :=  amt;
			else
				mods["intmod"] := amt;
			endif
		elseif (nroll == 2)
			stat := RandomDiceRoll("1d3");
			amt := (RandomInt(parsed.level)) + (parsed.level)*2;  //de 3 a 15
			if (stat == 1)
				mods["hitsmod"] :=  amt;
			elseif (stat == 2)
				mods["manamod"] :=  amt;
			else
				mods["stammod"] :=  amt;
			endif			
		else
			amt := (RandomInt(parsed.level)) + parsed.level*2;  //de 3 a 15
			SetEquipMod(item, skilllist[RandomInt(skilllist.size())+1 ], amt);
		endif
				
	endif

	SetObjProperty(item, "magic", 1);
	SetObjProperty(item, "equip", mods);

endfunction

function ActivateMagicItem(item)

	var mods := GetObjProperty(item, "equip");

	if (item.isA(POLCLASS_WEAPON))

		if (mods["dmg_mod"])
			item.dmg_mod := mods["dmg_mod"];
		endif

		if (mods["chancetohit"])
			SetObjProperty(item, "chancetohit", mods["chancetohit"]);
		endif

		if (mods["hitscript"])
			var hit_scripts := GetObjProperty(item, "HitScripts");
			if ( !hit_scripts )
				hit_scripts := array{};
			endif
			hit_scripts.append(mods["hitscript"]);
			SetObjProperty(item, "HitScripts", hit_scripts);
		endif

	elseif (item.isA(POLCLASS_ARMOR))
		var equip := GetObjProperty(item, "equip");
		if (!equip)
			equip := dictionary;
		endif
		foreach key in (mods.keys())
			if (key == "PhysicalResistMod")
				item.resist_physical_mod := item.resist_physical_mod + cint(mods[key]);
			elseif (key == "ColdResistMod")
				item.resist_cold_mod := item.resist_cold_mod + cint(mods[key]);
			elseif (key == "FireResistMod")
				item.resist_fire_mod := item.resist_fire_mod + cint(mods[key]);
			elseif (key == "EnergyResistMod")
				item.resist_energy_mod := item.resist_energy_mod + cint(mods[key]);
			elseif (key == "PoisonResistMod")
				item.resist_poison_mod := item.resist_poison_mod + cint(mods[key]);
			else
				equip[key] := mods[key];
			endif
		endforeach
		SetObjProperty(item, "equip", equip);
	else 
	//se for joia ou outro equi que de bonus de hits ou vitals
		var equip := GetObjProperty(item, "equip");
		if (!equip)
			equip := dictionary;
		endif
		foreach key in (mods.keys())
		
		
			if (key == "PhysicalResistMod")
				item.resist_physical_mod := item.resist_physical_mod + cint(mods[key]);
				printtext(item,"yaaaaa");
			elseif (key == "ColdResistMod")
				item.resist_cold_mod := item.resist_cold_mod + cint(mods[key]);
			elseif (key == "FireResistMod")
				item.resist_fire_mod := item.resist_fire_mod + cint(mods[key]);
			elseif (key == "EnergyResistMod")
				item.resist_energy_mod := item.resist_energy_mod + cint(mods[key]);
			elseif (key == "PoisonResistMod")
				item.resist_poison_mod := item.resist_poison_mod + cint(mods[key]);
			else
				equip[key] := mods[key];
			endif
		endforeach
		SetObjProperty(item, "equip", equip);		
	endif

	if (GetObjPRoperty(item, "magicColor") != error)
		item.color := Cint(GetObjPRoperty(item, "magicColor"));
		EraseObjProperty(item, "magicColor");
	endif

	var spellinfo := GetObjProperty(item, "spellinfo");
	if (spellinfo != error)
		if (spellinfo.id < 100)
			var cfg := ReadConfigFile(":tn:config/tnspells");
			var elem := FindConfigElem(cfg, spellinfo.id);
			var name := GetConfigString(elem, "Name");
			//printtextabove(item, " " + name);
			if (name && GetObjProperty(item, "magic") != 2)
				SetName(item, item.desc+" ["+name+"]");
			endif
		endif
	endif

	SetObjProperty(item, "magic", 2);

endfunction

function Loot_ParseMagicString(line)
	line := SplitWords(line);

	var name := CStr(line[1]);
	
	var amount := CStr(line[2]);
	amount := RandomDiceRoll(amount);
	
	var chance := CDbl(line[3]);
	if ( !chance )
		chance := 100.0;
	endif

	var level := CInt(line[4]);
	if (!level)
		level := 1;
	endif

	var template := Cstr(line[5]);

	var result := struct;
	result.+name := name;
	result.+amount := amount;
	result.+chance := chance;
	result.+level := level;
	result.+template := template;

	return result;
endfunction
