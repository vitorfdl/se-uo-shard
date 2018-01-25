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
		critstr := critstr + TranslateHit(lower(crit)) + "";
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