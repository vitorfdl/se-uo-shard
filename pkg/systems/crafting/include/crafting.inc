use polsys;
use datafile;

include ":keys:key";
include ":crafting:include/craftingUtil";
include ":gumps:include/gumps";
include ":gumps:include/gumps_ex";
include ":PacketConfig:settings";
include ":gumps:yesno";
include ":gumps:requestgump";
include "include/say";
include "include/damage";
include "include/objtype";

const OUT_OF_MATERIALS := 1;

var resource_config := ReadConfigFile(":crafting:config/resourceList");

var craft_skill;
var display_skill;
var craftL    := 0;
var rcolor    := 1;
var makermark := 1;

// Function used to target material and check ability for crafting.
// Returns an array of ItemRef for the item and Objtype for the
// resourceList of the material, and the Points bonus the resource
// gives for using it. If chk is passed, it is used for the "cprop"
// to check the item for. This is used for example, for boards that
// was originally colored logs. The colored log objtype is stored
// in the cprop, and is used to get it's "ResourceList" reference.
function MaterialSelection(who, craft_skill, type, chk)
 	SendSysMessage(who, "Escolha o "+lower(type)+" que voce deseja trabalhar.");
	var material := Target(who);

	if(!material)
		SendSysMessage(who, "Cancelado.");
		return 0;
	endif
	
	if(material.container.serial != who.backpack.serial)
		SendSysMessage(who, "O material nao esta na sua mochila.");
		return 0;
	endif
	
	if( (!Accessible(who, material)) || (Distance(who, material) > 2)
       || (material.movable == 0) || (!ReserveItem(material)))
		SendSysMessage(who, "Voce nao pode usar isto.");
		return 0;
	endif
	
	var materialobjtype := 0;
	if(chk)
		if(GetObjProperty(material, chk))
			materialobjtype := GetObjProperty(material, chk);
		else
			materialobjtype := material.objtype;
		endif
	else
		materialobjtype := material.objtype;
	endif
	
	if(!canUseMaterial(who, materialobjtype, craft_skill))
		SendSysMessage(who, "Voce nao possui conhecimento suficiente para fazer algo com isso.");
		return 0;
	endif
	
	var rtype := lower(resource_config[materialobjtype].Type);
	var materialchk := 0;
	if(rtype == type)
		materialchk := 1;
	endif
	if(materialchk == 0)
		SendSysMessage(who, "Esse nao e o material certo.");
		return 0;
	endif
	
	if((material.objtype >= UOBJ_BOLT_START) && (material.objtype <= UOBJ_BOLT_END))
		var totalcloth := material.amount * 50;
		var cloth;
		var materialColor := material.color;
		if(DestroyItem(material))
			cloth := CreateItemInBackpack(who, 0x1767, totalcloth);
			if(!cloth)
				cloth := CreateItemAtLocation(who.x, who.y, who.z, 0x1767, totalcloth);
			endif
			cloth.color := materialColor;
		endif
		material := cloth;
	endif

	var retarray := array;
	retarray.append(material);
	retarray.append(materialobjtype);
	retarray.append(resource_config[materialobjtype].Ptmod);
	return retarray;

endfunction


function CraftItem(who, craft_skill, tool, objtype, amt, canFix, anvil, more := 0)
	var craft_sound  := menu_config[craft_skill].CraftSound;
	var craft_delay  := menu_config[craft_skill].CraftDelay;
	var craft_action := menu_config[craft_skill].CraftAnim;
  	craft_skill := menu_config[craft_skill].Attribute; //pega o nome certo da skill (bs e tinkering usam metalwork)

	var craft_loops  := craft_config[objtype].Delay;
	if (craft_loops == error)
		craft_loops := 2;
	endif
 	craft_loops := cint(craft_loops);

	var theitemElem := FindconfigElem(craft_config, objtype);
	
	var materials := GetConfigStringDictionary(theitemElem, "materials");
	var principalMaterial := GetconfigString(theitemElem, "principalMaterial");

	var skill := craft_config[objtype].skill;
	var theitem;
	var mainSkill := CInt(AP_GetSkill(who, craft_skill));
	var modpts := 0;
	if(mainSkill >= skill)
		modpts := (mainSkill - (skill - 10));
	endif
	var diff := Cint(craft_config[objtype].difficulty);
	var pts  := Cint(craft_config[objtype].points);
	pts := pts + modpts;
	var chk := 0;

	//verifica se tem os objtypes na bag
	var targMaterials := dictionary;
	var chosenPrincipalMaterial := 0;
	var materialColor := 0;
	foreach thing in (materials.keys())
		if (thing == "metal" || thing == "wood" || thing == "glass" 
			|| thing == "cloth" || thing == "leather" || thing == "stone"
			|| thing == "arrowhead")

			var ret := MaterialSelection(who, craft_skill, thing, 0);
			if (!ret)
				SendSysMessageEx(who, "Voce nao possui os materiais necessarios.", SSM_FAIL);
				return OUT_OF_MATERIALS;
			else
				if (ret[1].amount < materials[thing] )
					SendSysMessageEx(who, "Voce nao possui os materiais necessarios." , SSM_FAIL);
					return OUT_OF_MATERIALS;			
				else
					if (thing == "arrowhead")
						var arrowtype := GetconfigInt(theitemElem, "arrowtype");
						if (cint(GetObjProperty(ret[1], "material")) != arrowtype)
							SendSysMessageEx(who, "Voce nao pode fabricar estas flechas com a ponta escolhida." , SSM_FAIL);
							return OUT_OF_MATERIALS;
						endif
					endif
					targMaterials[ret[1].objtype] := materials[thing];
					if (thing == principalMaterial)
						materialColor := ret[1].color;
						chosenPrincipalMaterial := ret[1].objtype;
					endif
				endif
			endif
		else
			foreach item in (EnumerateItemsInContainer(who.backpack))
					//sendsysmessage(who, " " + item.objtype + " " + cint(thing));
				if (item.objtype == cint(thing))
					if (targMaterials[thing])
						targMaterials[thing] := targMaterials[thing] + item.amount;
					else
						targMaterials[thing] :=  item.amount;
					endif
				endif
				sleepms(5);
			endforeach
			if ( targMaterials[thing] < (materials[thing] + more))
				SendSysMessageEx(who, "Voce nao possui os materiais necessarios." , SSM_FAIL);
				return OUT_OF_MATERIALS;
			else
				targMaterials[thing] := materials[thing];
			endif
		endif
		sleepms(5);
	endforeach

	var counter := 0;
	var theitems := array;

	var initialX := who.x;
	var initialY := who.y;
	while (counter < amt)
		counter := counter + 1;
		if ( (who.x != initialX) || (who.y != initialY) )
			SendSysMessageEx(who, "Voce parou de se concentrar no que estava fazendo.", SSM_FAIL);
			return 1;
		endif

		var item := 0;
		if (anvil)
			item := CreateItemAtLocation(anvil.x, anvil.y, anvil.z, objtype, 1, anvil.realm);
			item.movable := 0;
			item.color := 1161;
			PrintTextAbove(item, "*item em brasa*");
			MoveObjectToLocation(anvil.x, anvil.y, (anvil.z+7), anvil.realm);
		endif
		playCraftWork(who, craft_sound, craft_loops, craft_delay, craft_action, craft_config[objtype].FastCraft);
		if (item)
			DestroyItem(item);
		endif

		var check := Skillcheck(who, craft_skill, diff, pts) ;

		if(check > 0)
			if (!SubtractMaterial(who, targMaterials, 1+more))
				SendSysMessageEx(who, "Voce nao possui todos os materiais.", SSM_FAIL);
				return theitems;
			endif
			theitem := CreateItem(who, objtype, more);
			if(!theitem)
				SendSysMessageEx(who, "Voce nao tem espaco suficiente na sua mochila!", SSM_FAIL);
				return theitems;
			endif
			CheckToolWear (who, tool, craft_skill);
			theitem.movable := 1;

			//passando a cor pra materiais coloridos que nao tem Id proprio (como madeira)
			AddMaterialMod(who, theitem, chosenPrincipalMaterial, materialColor);
			CraftExceptional(who, theitem, craft_skill);
			CraftLockable(who, theitem);

			SendSysMessageEx(who, "Voce criou o item o colocou na mochila.", SSM_INFO);
		
			var hisskill := AP_GetSkill(who, craft_skill);

			if (theitem.isa(POLCLASS_WEAPON) || theitem.isa(POLCLASS_ARMOR))	
				SetObjProperty(theitem,"reparos",(hisskill/2));
			endif

			if(theitem.isA(POLCLASS_CONTAINER))
				SetObjProperty(theitem, "trappable", 1);
			endif

			if ((theitem.graphic !=  11058) && (theitem.graphic != 3903))
				SetObjProperty(theitem, "crafterserial", who.serial);
				SetObjProperty(theitem, "crafteracc", who.acctname);
			endif
			if (canFix)
				SetObjProperty(theitem, "canfix", 1);
			endif

			theitems.append( theitem);
		else
			SubtractMaterial(who, targMaterials, 0.5); //consome metade do material
			SendSysMessageEx(who, "Voce falhou ao fazer o item e perdeu material." , SSM_FAIL);
			//return 0;
		endif
		sleepms(100);
	endwhile

	return theitems;	

endfunction

function CreateItem(who, objtype, more:=0)
	var theitem := 0;
	var cfg:=ReadConfigFile(":crafting:stackingitens");
	var elem := FindConfigElem(cfg, "stacks");
	var stackingitens:=ListConfigElemProps( elem );
	if(cstr(objtype) in stackingitens)
		var stacknow := FindObjtypeInContainer( who.backpack, objtype );
		if (objtype == 7124 || objtype == 0x9087)
			more := more + 4;
		endif
		if ( stacknow )
			AddAmount (stacknow, 1+more);
			theitem := stacknow;
		else
			theitem := CreateItemInBackpack(who, objtype, 1+more);
		endif
	else
		theitem := CreateItemInBackpack(who, objtype, 1+more);
		//theitem.quality := 0.6;
		theitem.name := theitem.desc;
	endif
	return theitem;
endfunction


function AddMaterialMod(who, theitem, principalMaterial, materialColor)

	if (!principalMaterial)
		return;
	endif	

	var elem  := FindConfigElem(resource_config, principalMaterial);
	if((craft_config[theitem.objtype].retain == 1))
		if(resource_config[principalMaterial].Color)
			theitem.color := resource_config[principalMaterial].Color;
		else
		
			theitem.color := materialColor;
		endif
	endif
	SetObjProperty(theitem, "material", principalMaterial);

	var DmgMods := GetConfigString(elem, "AoSDamageMod");
	DmgMods := SplitWords(DmgMods);
	var ResistMods := GetConfigString(elem, "AoSResistMod");
	ResistMods := SplitWords(ResistMods);
	var hpmod := GetConfigInt(elem, "hpmod");


	if(theitem.isA(POLCLASS_WEAPON))
		theitem.damage_physical_mod := Cint(DmgMods[1]);
		var cfg:=ReadConfigFile(":combat:itemdesc");
		var elem := FindConfigElem(cfg, theitem.objtype);
		if (elem.Attribute != "RangedCombat")
			theitem.damage_fire_mod := Cint(DmgMods[2]);
			theitem.damage_cold_mod := Cint(DmgMods[3]);
			theitem.damage_poison_mod := Cint(DmgMods[4]);
			theitem.damage_energy_mod := Cint(DmgMods[5]);
		endif
	elseif(theitem.isA(POLCLASS_ARMOR))
		theitem.resist_physical_mod := Cint(ResistMods[1]);
		theitem.resist_fire_mod := Cint(ResistMods[2]);	
		theitem.resist_cold_mod := Cint(ResistMods[3]);
		theitem.resist_poison_mod := Cint(ResistMods[4]);	
		theitem.resist_energy_mod := Cint(ResistMods[5]);
	//flechas e virotes de guerra
	elseif (theitem.objtype == 0x9088 || theitem.objtype == 0x9098)
		SetObjProperty(theitem, "dmgmods", DmgMods);
	endif

endfunction

function CraftExceptional(who, theitem, craft_skill)
	if(craft_config[theitem.objtype].Exceptional == 1)
		theitem.quality := 0.6;
		
		//Checa Habilidades de fundir
		if ((craft_skill == WOMENSWORK && TemHabilidade(who, "Tricô Aprimorado"))
			|| (craft_skill == METALWORKING && TemHabilidade(who, "Fundir"))
			|| (craft_skill == WOODWORKING && TemHabilidade(who, "Modelagem Complexa")))
			if ( YesNo(who, "Deseja unir algum material especial com este objeto?"))
				SendSysMessageEx(who, "Selecione o material", SSM_REQUEST);
				var special_obj := Target(who);
				var special_props := GetObjProperty(special_obj, "craftproperty");
				if (!special_props)
					SendSysMessageEx(who, "Material invalido ou sem propriedades uteis.", SSM_FAIL);
				elseif (!cint(GetObjProperty(special_obj, "identificado")))
					SendSysMessageEx(who, "Material nao identificado.", SSM_FAIL);
				endif
				
				//Inicio de Checagem: Se pode unir o material com o item
				if (special_props.tipo == "Vestes")
					if (special_props.veste == "Alfaiate" && craft_skill != WOMENSWORK)
						SendSysMessageEx(who, "Este material e utilizado por alfaiates.", SSM_FAIL);
						return;
					elseif (special_props.veste == "Ferreiro" && craft_skill != METALWORKING)
						SendSysMessageEx(who, "Este material e utilizado por ferreiros.", SSM_FAIL);
						return;
					endif
					
					//Compara material e o layer do equip, dá return se não passar em nenhum
					if ("Luvas" in special_props.equipamentos && theitem.tile_layer == 7)
					elseif ("Calças" in special_props.equipamentos && (theitem.tile_layer == 4 || theitem.tile_layer == 23 || theitem.tile_layer == 24) )
					elseif ("Capas" in special_props.equipamentos && theitem.tile_layer == 20)
					elseif ("Cabeça" in special_props.equipamentos && theitem.tile_layer == 6)
					elseif ("Ombreiras" in special_props.equipamentos && theitem.tile_layer == 19)
					elseif ("Peitoral" in special_props.equipamentos && (theitem.tile_layer == 17 || theitem.tile_layer == 5 || theitem.tile_layer == 13 || theitem.tile_layer == 22))
					elseif ("Joias" in special_props.equipamentos && (theitem.tile_layer == 8 || theitem.tile_layer == 10 || theitem.tile_layer == 14 || theitem.tile_layer == 18))
					elseif ("Escudos" in special_props.equipamentos && theitem.tile_layer == 2 && theitem.resist_physical >= 1)
					else
						SendSysMessageEx(who, "Material invalido com este objeto [Vestes].", SSM_FAIL);
						return;
					endif

				elseif (special_props.tipo == "Armas")
					if (!theitem.attribute) //se não for arma
						SendSysMessageEx(who, "Material utilizado somente em armas.", SSM_FAIL);
						return;
					endif
					
					//pra pegar o DamageType
					var cfg:=ReadConfigFile(":combat:itemdesc");
					var elem := FindConfigElem(cfg, theitem.objtype);
					
					//Compara material e o tipo de arma, dá return se não passar em nenhum
					if ("Arcos" in special_props.equipamentos && elem.DamageType == "Arco" )
					elseif (("Balestras" in special_props.equipamentos || "Bestas" in special_props.equipamentos) && lower(elem.DamageType) == "balestra")
					elseif ("1h Corte" in special_props.equipamentos && lower(theitem.attribute) == "meleecombat1h" && lower(elem.DamageType) == "corte" )
					elseif ("1h Concussão" in special_props.equipamentos && lower(theitem.attribute) == "meleecombat1h" && lower(elem.DamageType) == "Concussão" )
					elseif ("1h Perfuração" in special_props.equipamentos && lower(theitem.attribute) == "meleecombat1h" && lower(elem.DamageType) == "perfuração" )
					elseif ("2h Corte" in special_props.equipamentos && lower(theitem.attribute) == "meleecombat2h" && lower(elem.DamageType) == "corte" )
					elseif ("2h Concussão" in special_props.equipamentos && lower(theitem.attribute) == "meleecombat2h" && lower(elem.DamageType) == "Concussão" )
					elseif ("2h Perfuração" in special_props.equipamentos && lower(theitem.attribute) == "meleecombat2h" && lower(elem.DamageType) == "perfuração" )
					else
						SendSysMessageEx(who, "Material invalido com este objeto [Armas].", SSM_FAIL);
						return;
					endif
				endif
				//Fim de Checagem: Se pode unir o material com o item
				
				//seta propriedades básicas
				if (special_props.color)
					theitem.color := special_props.color;
				endif
				SetObjProperty(theitem, "SpecialCraft", special_obj.name);
				
				//Inicio da Setagem de Propriedades
				var x := 1;
				var mods := dictionary;
				while (x <= cint(GetObjProperty(special_obj, "identificado")))
					var special_bonus := special_props.bonus[x];
					var bonus := cint(special_bonus.bonus * (AP_GetSkill(who, craft_skill) / 100));
					if (bonus < 1)
						bonus := 1;
					endif
					if (special_bonus.name == "hp_mod") //Durabilidade
						theitem.maxhp_mod := bonus;
						theitem.hp := theitem.maxhp + theitem.maxhp_mod;
					elseif(special_bonus.tipo == "DamageMod")
						if (special_bonus.name == "ColdDamageMod")
							theitem.damage_cold_mod := bonus;
						elseif (special_bonus.name == "FireDamageMod")
							theitem.damage_fire_mod := bonus;
						elseif (special_bonus.name == "PoisonDamageMod")
							theitem.damage_poison_mod := bonus;
						elseif (special_bonus.name == "EnergyDamageMod")
							theitem.damage_energy_mod := bonus;
						endif
					elseif (special_bonus.name == "skillmod") //Pericias
						var skills := special_bonus.skills;
						setobjproperty(theitem, "skillmod", skills);
						var amt := bonus;
						var k := 1;
						foreach skill in skills
							mods[skill] := amt[k];
							k := k + 1;
						endforeach
					else //Qualquer outro mod tratado pela propriedade mods
						mods[special_bonus.name] := cint(mods[special_bonus.name]) + bonus;
					endif
					x+=1;
				endwhile
				if (mods)
					setobjproperty(theitem, "equip", mods);
				endif
				//Fim da Setagem de Propriedades
				
				var destroy := DestroyItem(special_obj); //destroi material se tudo ocorreu bem
			endif
		endif
	endif
endfunction

function CraftExceptionalQuality(who, theitem, material := 0)
	if(craft_config[theitem.objtype].Exceptional == 1)
		SetObjProperty(theitem, "Exceptional", 1);
		if ( YesNo(who, "Voce fez um item obra prima, deseja renomea-lo?"))
			var name := RequestGump(who, "Qual o nome do item?");
			if (name != error)
				SetName(theitem, name);
			endif
			if ( YesNo(who, "Deseja adicionar uma descricao no item?"))
				AddItemDescription(who, theitem);
			endif
			SetName(theitem, theitem.desc);
		endif
		if (material != "cloth")
			if(theitem.isA(POLCLASS_WEAPON))
				AddQualityDamageMod(theitem, EXCEP );
			elseif(theitem.isA(POLCLASS_ARMOR))
				AddQualityResistMod(theitem, EXCEP);
			endif
		endif
		theitem.quality := 1.2;
	endif
endfunction

function CraftGoodQuality(who, theitem, material := 0)
	if(craft_config[theitem.objtype].Exceptional == 1)
		if (material != "cloth")
			if(theitem.isA(POLCLASS_WEAPON))
				AddQualityDamageMod(theitem, 2);
			elseif(theitem.isA(POLCLASS_ARMOR))
				AddQualityResistMod(theitem, 2);
			endif
		endif
		theitem.quality := 1.0;
	endif
endfunction

function CraftPoorQuality(who, theitem, material := 0)
	if(craft_config[theitem.objtype].Exceptional == 1)
		if (material != "cloth")
			if(theitem.isA(POLCLASS_WEAPON))
				AddQualityDamageMod(theitem, 3);
			elseif(theitem.isA(POLCLASS_ARMOR))
				AddQualityResistMod(theitem, 3);
			endif
		endif
		theitem.quality := 0.8;
	endif
endfunction

// amount eh a fracao
function SubtractMaterial(who, targMaterials, amountMod)

	foreach thing in (targMaterials.keys() )
		//sendsysmessage(who, " materiais antes " + thing + " " +  targMaterials[thing] );
		targMaterials[thing] := Cint( cint(targMaterials[thing])*amountMod);
		if (targMaterials[thing] == 0 )
			targMaterials[thing] := 1;
		endif
		//sendsysmessage(who, " materiais depois " + thing + " " +  targMaterials[thing] );
	endforeach

	//sendsysmessage(who, " targmaterial " + targMaterials.keys() );

	//no loop anterior procurou os materiais, agora consome.
	foreach thing in (targMaterials.keys())
			foreach item in (EnumerateItemsInContainer(who.backpack))
				if (cint(item.objtype) == cint(thing))
					if (item.amount >= targMaterials[thing] )
						//sendsysmessage(who, " " + item + " " + cint(targMaterials[thing]) + " " + thing);
						var chk := SubtractAmount(item, cint(targMaterials[thing]));
						if (!chk )
							SendSysMessage(who, "chk " + chk);
							return 0;
						endif
						targMaterials[thing] := 0;
						break;
					else
						targMaterials[thing] := targMaterials[thing] - item.amount;
						if (!SubtractAmount(item, item.amount))
							return 0;
						endif
					endif
				endif
			endforeach
			if (targMaterials[thing] > 0)
				return 0;
			endif

	endforeach
  
	return 1;
endfunction

function canUseMaterial(who, use_on, craft_skill)

  if (Cint(resource_config[use_on].Type) == 0) //se nao tem no cfg entao pode usar (pros recipes)
    return 1;
  endif
	
	PrintText(who, "Teste: " + resource_config[use_on].habilidade);
	if (craft_skill == WOMENSWORK && resource_config[use_on].Type == "Leather" && !TemHabilidade(who, "Trabalhar Couro"))
		return 0;
	elseif (resource_config[use_on].habilidade && !TemHabilidade(who, resource_config[use_on].habilidade))
		return 0;
	endif
  
	if( AP_GetSkill(who, craft_skill) >= Cint(resource_config[use_on].Skill) )
		return 1;
	else
		return 0;
	endif

endfunction


// This function is used to display a craft skill's action,
// sound, the loops to do it in, and the delay between loops.
// Sound is required, loops defaults to 0 if none sent, and delay
// defaults to 2 seconds if none sent, Action defaults to 0
// (none) if none sent. Who is who the info is to be sent to.
// All delays are in seconds.
function playCraftWork(who, craftSound, loops:=0, delay:=2, craftAction:=0, fastcraft := 0)
  if(loops)
    for i := 1 to loops
      if(craftAction)
        PerformAction( who, craftAction );
      endif
      PlaySoundEffect(who, craftSound);
      sleep(delay);
    endfor
  else
    PlaySoundEffect(who, craftSound);
	if (fastcraft)
		sleepms(500);
		return;
	endif
    sleep(delay);
  endif
  
  return;
endfunction

function CheckAnvil(who)
	var anvil_near;
	foreach item in ListItemsNearLocation(who.x, who.y, who.z, 3)
		if((item.objtype == 0x0faf) || (item.objtype == 0x0fb0))
			anvil_near := 1;
			break;
		endif
	endforeach
	if(anvil_near == 0)
		return 0;
	else
		return 1;
	endif
endfunction


function CheckMaterials(who, what, craft_skill) 
	var elem := FindConfigElem( craft_config, what.objtype);
	var materials := GetConfigStringDictionary(elem, "materials");
	var matname := "";
	var matresource := "";
	foreach material in (materials.keys())
		if (FindConfigElem( resources_config, material) || (material == "metal" || material == "wood" || material == "glass" 
															|| material == "leather" || material == "stone" || material == "cloth"))
			materials[material] := floor(cint(materials[material]) /2);
			matname := matname + "\n" + GetMaterialName(material);
			matresource := matresource + "\n" + materials[material];
		else
			materials.erase(material);
		endif
		
	endforeach
	
	var chargump := GFCreateGump(150,150);
	GFDisposable(chargump, 0);
	GFPage(chargump, 0);
	GFResizePic( chargump, 0, 0, GFCfgConst( "Defaults", "BackGround" ), 280, 200 );
	GFResizePic( chargump, 15, 15, GFCfgConst( "Defaults", "ForeGround" ), 250, 170 );
	
	GFTextLine(chargump, 25, 25, 1153, "Lista de Materiais");
	GFTextLine(chargump, 25, 45, 2105, "Você irá gastar os seguintes materiais:");
	
	GFHTMLArea(chargump, 40, 85, 150, 60, ("<basefont color=#ffffff>" + matname));
	GFHTMLArea(chargump, 230, 85, 20, 60, ("<basefont color=#ffffff>" + matresource));
	
	GFAddButton(chargump, 190, 150, 4005, 4007, GF_CLOSE_BTN, 99);
	GFAddButton(chargump, 40, 150, 4017, 4019, GF_CLOSE_BTN, 0);
	
	var input := GFSendGump(who, chargump);

	if (input[0] == 99)
		var targMaterials := dictionary;
		foreach thing in (materials.keys())
			if (thing == "metal" || thing == "wood" || thing == "glass" 
				|| thing == "cloth" || thing == "leather" || thing == "stone"
				|| thing == "arrowhead")

				var ret := MaterialSelection(who, craft_skill, thing, 0);
				if (!ret)
					SendSysMessageEx(who, "Voce nao possui os materiais necessarios.", SSM_FAIL);
					return 0;
				else
					if (ret[1].amount < materials[thing] )
						SendSysMessageEx(who, "Voce nao possui os materiais necessarios." , SSM_FAIL);
						return 0;			
					else
						targMaterials[ret[1].objtype] := materials[thing];
					endif
				endif
			else
				foreach item in (EnumerateItemsInContainer(who.backpack))
						//sendsysmessage(who, " " + item.objtype + " " + cint(thing));
					if (item.objtype == cint(thing))
						if (targMaterials[thing])
							targMaterials[thing] := targMaterials[thing] + item.amount;
						else
							targMaterials[thing] :=  item.amount;
						endif
					endif
				endforeach
				if ( targMaterials[thing] < (materials[thing]))
					SendSysMessageEx(who, "Voce nao possui os materiais necessarios." , SSM_FAIL);
					return 0;
				else
					targMaterials[thing] := materials[thing];
				endif
			endif
		endforeach
		if (!SubtractMaterial(who, targMaterials, 1))
			SendSysMessageEx(who, "Voce nao possui todos os materiais.", SSM_FAIL);
			return 0;
		endif
		if ("cloth" in materials.keys())	
			return "cloth";
		endif
		return 1;		
	else
		return 0;
	endif
endfunction