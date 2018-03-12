/* $Id: canAccess.inc 903 2005-11-05 08:10:21Z muaddiblsd $
 *
 */
function can_access(who, item, flags := 0)
 	var owner := Cint(GetObjProperty(item, "Owner"));
 	if( (owner) and (owner != who.serial) )
		SendSysMessageEx(who, "Você não pode usar isso (1) " + item.desc);
		return 0;
	elseif(GetObjProperty(item, "Vendored"))
		SendSysMessageEx(who, "Você não pode usar isso (2) " + item.desc);
		return 0;
	elseif(!Accessible(who, item))
		SendSysMessageEx(who, "Você não pode usar isso (3) " + item.desc);
		return 0;
	elseif((!flags["distance"]) and ((Distance(who, item) > 2)))
		SendSysMessageEx(who, "That item is too far away.");
		return 0;
	elseif((!flags["unmovable"]) and (item.movable == 0))
		SendSysMessageEx(who, "Você não pode usar isso enquanto estiver preso no chão.");
		return 0;
	elseif(!ReserveItem(item))
		SendSysMessageEx(who, "O item já está em uso.");
		return 0;
	else
		return 1;
	endif
endfunction
