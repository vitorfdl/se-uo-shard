include "include/utils";
include "include/colormap";

var resdmg_list := array{DMG_FIRE, DMG_POISON, DMG_COLD, DMG_ENERGY, DMG_PHYSICAL};

function TranslateAtt(attribute)
	case (attribute.lower())
		"strength": return "Força";
		"dexterity": return "Destreza";
		"intelligence": return "Inteligência";
		"wisdom": return "Sabedoria";
		"constitution": return "Constituição";
		default: return attribute;
	endcase
endfunction

function getQuality(num, color := 0)
   if (color) color := GetQualityColor(num); endif
   
	if (num >= 4)
		return "{}Exótico".format(color);
	elseif(num  >= 3)
		return "{}Raro".format(color);
	elseif(num  >= 2)
		return "{}Bom".format(color);
	else
		return "{}Comum".format(color);
	endif
endfunction

function GetQualityColor(num)
	if (num >= 3)
		return PURPLE;
	elseif(num  >= 2)
		return YELLOW;
	elseif(num  >= 1)
		return BLUE;
	else
		return LIGHTGRAY;
	endif
endfunction

function TranslateDamage(dmg)
	case (dmg.lower())
		"physical": return "Físico";
		"fire": return "Calor";
		"cold": return "Frio";
		"energy": return "Energia";
		"poison": return "Veneno";
		default: return dmg;
	endcase
endfunction

function TranslateHit(crit)
	if (crit == "antimountedhit")
		return "Anti-montaria";
	elseif (crit == "bleedhit")
		return "Sangramento";
	elseif (crit == "riposte")
		return "Riposte";
	elseif ( crit == "gopleduplo")
		return "Duelista";
	elseif (crit == "heighthit")
		return "Diferença de Altura";
	elseif ( crit == "lowbarrier")
		return "Devorar Véu";
	elseif ( crit == "earnbarrier")
		return "Barreira";
	elseif ( crit == "powerhit")
		return "Poderoso";
	elseif ( crit == "gigante")
		return "Gigante";
	elseif ( crit == "extase")
		return "Êxtase";
	endif
endfunction

function GetHitsInfo(entry)
	var critstr := "";
	var passives := GetconfigStringArray(entry, "HitScripts");
	foreach crit in passives
		critstr := critstr + TranslateHit(lower(crit)) + "<br>";
	endforeach

	return critstr;
endfunction

function GetWeaponFeat(entry)
	var critstr := "";
	var featid := GetconfigString(entry, "FeatID");
   if (!featid) return; endif

   var feat := getFeatById(featid);

	return feat.Name;
endfunction

function getVelStringDesc(elem, item)
	if(speed >= 55)
		return "Rápido";
	elseif(speed >= 40)
		return "Mediano";
	elseif(speed >= 20)
		return "Lento";
	else
		return "Muito Lento";
	endif
endfunction

function GetItemType(item)
   var item_type := 0;
	if (item.isA(POLCLASS_ARMOR))
      if (item.tile_layer == 6)
         item_type := "Elmos/Mascaras";
      else
		   item_type := "Armaduras";
      endif
	elseif (item.isBow())
		item_type := "Arcos";
	elseif (item.isMagicWeapon())
		item_type := "Armas Mágicas";
	elseif (item.isA(POLCLASS_WEAPON))
		item_type := "Armas";
	elseif (cint(item.tile_layer) in array{6,8,10,14,18})
		item_type := "Joias";
	elseif (item.isA(POLCLASS_CONTAINER))
		item_type := "Containers";
	endif

   return item_type;
endfunction

function ApplyCraftChanges(theitem, level_select)
   if (!level_select) level_select := 1; endif

   if (theitem.isA(POLCLASS_ARMOR) || theitem.isa(POLCLASS_WEAPON) || theitem.isA(POLCLASS_EQUIPMENT))
      theitem.quality := 1;
      var chance := RandomInt(100);
      if (chance <= 5)
         theitem.quality := 3;
      elseif(chance <= 10)
         theitem.quality := 2;
      endif

      if (theitem.quality >= 2)
         theitem.maxhp_mod += 1;
         theitem.hp += 1;
      endif
      if (theitem.quality >= 3)
         theitem.maxhp_mod += 1;
         theitem.hp += 1;
      endif
      if (theitem.quality >= 4)
         theitem.maxhp_mod += 1;
         theitem.hp += 1;
      endif
      
      if (theitem.isa(POLCLASS_WEAPON))
         theitem.dmg_mod := RandomIntMinMax(12 * level_select, 17 * level_select);
      elseif (theitem.isA(POLCLASS_ARMOR) && theitem.tile_layer == 0x2)
         foreach resist in resdmg_list
            var value := RandomIntMinMax(level_select, 2*level_select);
            theitem.set_member("resist_{}_mod".format(resist), value);
            sleepms(2);
         endforeach
     elseif (theitem.isA(POLCLASS_ARMOR))
         foreach resist in resdmg_list
            var value := RandomIntMinMax(2 * level_select, 3*level_select);
            theitem.set_member("resist_{}_mod".format(resist), value);
            sleepms(2);
         endforeach
      endif
      SetObjProperty(theitem, "level", level_select);
      SetName(theitem, theitem.name);
      return 1;
   endif
endfunction

function ApplyCraftColor(theitem, level_select)
   var recipe_cfg := ReadConfigFile(":*:recipes");
   var recipe := FindConfigElem(recipe_cfg, hex(theitem.objtype));
   if (!recipe)
      recipe := FindConfigElem(recipe_cfg, cint(theitem.objtype));
   endif

   if (!recipe) return; 
   elseif (!recipe.main_material) return; endif

   var resource_cfg := ReadConfigFile(":crafting:resourceLevels");
   var the_resource := FindConfigElem(resource_cfg, recipe.main_material);
   if (!the_resource) return; endif
   var resource_levels := GetConfigStringDictionary(the_resource, "Level");
   resource_cfg := ReadConfigFile(":crafting:resourceList");
   the_resource := FindConfigElem(resource_cfg, resource_levels[cstr(level_select)]);
   if (the_resource)
      theitem.color := the_resource.Color;
   endif
endfunction

function TranslatePassive(passive)
   var cfg  := ReadConfigFile(":combat:passive_scripts");
   var elem := FindConfigElem(cfg, passive);
   if (elem) return elem.Name; endif
   
   return passive;
endfunction