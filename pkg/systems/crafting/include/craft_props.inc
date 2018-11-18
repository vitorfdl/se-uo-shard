include "include/utils";
include "include/colormap";

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
	elseif ( crit == "barrierdestroy")
		return "Devorar Véu";
	elseif ( crit == "barrier")
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