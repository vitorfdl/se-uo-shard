/* $Id: canAccess.inc 903 2005-11-05 08:10:21Z muaddiblsd $
 *
 */
function can_access(who, item, flags := 0)
 	var owner := Cint(GetObjProperty(item, "Owner"));
 	if( (owner) and (owner != who.serial) )
		SendSysMessage(who, "Você não pode usar isso (1) " + item.desc);
		return 0;
	elseif(GetObjProperty(item, "Vendored"))
		SendSysMessage(who, "Você não pode usar isso (2) " + item.desc);
		return 0;
	elseif(!Accessible(who, item))
		SendSysMessage(who, "Você não pode usar isso (3) " + item.desc);
		return 0;
	elseif((!flags["distance"]) and ((Distance(who, item) > 2)))
		SendSysMessage(who, "That item is too far away.");
		return 0;
	elseif((!flags["unmovable"]) and (item.movable == 0))
		SendSysMessage(who, "Você não pode usar isso enquanto estiver preso no chão.");
		return 0;
	elseif(!ReserveItem(item))
		SendSysMessage(who, "O item já está em uso.");
		return 0;
	else
		return 1;
	endif
endfunction
