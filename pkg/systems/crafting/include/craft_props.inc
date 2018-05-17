include "include/utils";

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

function getQuality(num)
	if (num >= 1.1)
		return "Excepcional";
	elseif(num  > 0.8)
		return "Normal";
	else
		return "Ruim";
	endif
endfunction

function getWeaponType(type)
   var res;
   case (type)
      "axe": res := "Machado"; break;
      "sword": res := "Espada"; break;
      "greatsword": res := "Espada Gigante"; break;
      "spear": res := "Lança"; break;
      "dagger": res := "Adaga"; break;
      "dualsword": res := "Arma Dupla"; break;
      "bow": res := "Arco"; break;
      "crossbow": res := "Besta"; break;
      "mace": res := "Maça"; break;
      "halberd": res := "Arma de Haste"; break;
   endcase

   return res;
endfunction


function getCondition(item)
	var c := cdbl(item.hp) / cdbl(item.maxhp);

	if (c > 0.9)
		return "Boa";
	elseif (c > 0.6)
		return "Pouco Danificada";
	elseif (c > 0.3)
		return "Bastante Danificada";
	else
		return "Quebradiça";
	endif
endfunction

function TranslateCritical(crit)
	if (crit == "damage3")
		return "Dano Triplo";
	elseif (crit == "damage2")
		return "Dano Duplo";
	elseif (crit == "armorignore")
		return "Ignorar Armadura";
	elseif ( crit == "stun")
		return "Ataque Atordoante";
	elseif (Crit == "bleed")
		return "Alvejar Ponto Vital";
	endif
endfunction

function GetCriticalInfo(entry)
	var criticals := GetconfigStringArray(entry, "CriticalHitScripts");

	var critstr := "";
	
	var str;
	foreach crit in criticals
		critstr := critstr + TranslateCritical(lower(str)) + "";
	endforeach

	return critstr;
endfunction

function TranslateHit(crit)
	if (crit == "antimountedhit")
		return "Anti-montaria";
	elseif (crit == "chargehit")
		return "Carga";
	elseif (crit == "concussionhit")
		return "Concussão";
	elseif ( crit == "freearmorhit")
		return "Movimento Livre";
	elseif (crit == "heighthit")
		return "Diferença de Altura";
	elseif ( crit == "QueimaRoupa")
		return "Tiro a Queima Roupa";
	elseif ( crit == "pushback")
		return "Golpe Esmagador";
	elseif ( crit == "passivedisarm")
		return "Disarme Passivo";
	endif
endfunction

function GetHitsInfo(entry)
	var criticals := GetconfigStringArray(entry, "HitScripts");
	var critstr := "";
	
	foreach crit in criticals
		critstr := critstr + TranslateHit(lower(crit)) + "<br>";
	endforeach

	return critstr;
endfunction

function getVelStringDesc(elem, item)
	if(speed >= 39)
		return "Extrema";
	elseif(speed >= 34)
		return "Muito rapida";
	elseif(speed >= 30)
		return "Rapida";
	elseif(speed >= 26)
		return "Mediana";
	elseif(speed >= 21)
		return "Lenta";
	elseif(speed >= 20)
		return "Lentissima";
	else
		return "Extremamente Lenta";
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

function GetWeaponSpellList(item)
   var return_text := "";
   var spell_list := GetObjProperty(item, "spell_list");
   if (spell_list != error && spell_list.size() > 0)
      var active_spell := GetObjProperty(item, "active_spell");
      var lost_slots := 0;
      var spell_levels := dictionary{};
      return_text += WHITE;
      
      foreach spell in spell_list
         sleepms(2);
         if (spell == 0)
            lost_slots += 1;
            continue;
         endif

         spell_levels[spell] := cint(spell_levels[spell]) + 1;
      endforeach

      foreach spell in (spell_levels.keys())
         sleepms(2);
         if (spell == active_spell)
            return_text += BLUE;
         endif

         var spell_name := GetSpellName(spell);
         spell_name["Encantamento"] := "Enc.";
         return_text += "{} {}<br>".format(spell_name, convertToRoman(spell_levels[spell]));

         if (spell == active_spell)
            return_text += WHITE;
         endif
      endforeach
      
      if (lost_slots > 0)
         return_text += "{}x {}SLOT PERDIDO<br>".format(lost_slots, LIGHTGRAY);
      endif

      var total_slots := GetObjProperty(item, "magicslot") - spell_list.size();
      if (total_slots > 0)
         return_text += "{}{}x SLOT VAZIO<br>".format(total_slots, LIGHTGRAY);
      endif
   endif

   return return_text;
endfunction