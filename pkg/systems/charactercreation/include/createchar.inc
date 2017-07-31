use uo;
use cfgfile;

include ":gumps:gumps";
include "include/arrays";
include ":attributes:attributes";
include ":gumps:yesno";

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
