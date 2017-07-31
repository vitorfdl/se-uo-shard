function setUsedSpell(who, spellinfo)
	var cleric_list := GetObjProperty(who, "cleric_list");
	if (!cleric_list)
		return;
	endif

	var i := FindInArray(cleric_list, struct{ id: spellinfo.id });
	if (!i)
		return;
	endif

	cleric_list[i].used = 1;

	SetObjProperty(who, "cleric_list", cleric_list);
endfunction

function RemoveUsedSpells(who, byref cleric_list, byref available_slots)
   foreach spell in cleric_list
      if (spell.used && polcore().systime > spell.time)
         var index := FindInArray(cleric_list, spell);
         cleric_list.Erase(spell_found);
         available_slots += spell.circle;
      endif
   endforeach

   SetObjProperty(who, "cleric_list", cleric_list);
endfunction