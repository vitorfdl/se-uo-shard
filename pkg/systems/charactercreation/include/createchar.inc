use uo;
use cfgfile;

include ":gumps:gumps";
include "include/arrays";
include ":attributes:attributes";
include ":gumps:yesno";

function ApplySkillCap(who)
	var classe := GetObjProperty(who, "classe");
	if (classe != error)

		var secundarias := getSecundarias(who);		
		foreach secundaria in secundarias
			if ( AP_GetTrueSkill(who, secundaria) > 70 )
				AP_SetTrueSkill(who, secundaria, 70);
			endif
		endif

		var terciarias := getTerciarias(who);
		foreach terciaria in terciarias
			if ( AP_GetTrueSkill(who, terciaria) > 40 )
				AP_SetTrueSkill(who, terciaria, 40);
			endif
		endif
	endif
endfunction

function AP_AttributeNameToSkillId(skill)
	if (skill == "Alchemy")
		return 90;
	endif
	var elem := AP_GetAttributeCfgElem(skill);
	var id := elem.SkillId;
	return id;
endfunction 

function AP_SkillIdToAttributeName(skillid)
	if (skillid == 90)
		return "Alchemy";
	endif
	var cfg := AP_GetAttributesCfgFile();
	var keys := GetConfigStringKeys( cfg );
	foreach key in keys
		var elem := FindConfigElem( cfg, key );
		if (elem.SkillId == skillid)
			return key;
		endif
	endforeach
	return "";
endfunction

function CreateBasicRecipes()
	var x := array;

	var spellinfo1 := struct;
 	spellinfo1.+name := "Poção de Cura leve";
 	spellinfo1.+id := 56321;
	spellinfo1.+description := "Sua formula ja esta bastante conhecida entre os alquimistas, basta utilizar duas raizes de ginseng e destilar o extrato. Dificuldade facil.";
	x.append(spellinfo1);

	var spellinfo4 := struct;
	spellinfo4.+name := "Poção de Mana Fraca";
	spellinfo4.+id := 0xbf17;
	spellinfo4.+description := "Uma pedra negra e uma obsidiana quando misturadas liberam um extrato capaz de destravar propriedes do espirito. Dificuldade Media"; 
	x.append(spellinfo4);

	return x;
endfunction
