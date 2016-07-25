use cfgfile;

include "include/damage";
include ":tn:tngumps";

const  EXCEP := 1;
const  NORMAL := 2;
const  POOR := 3;
const STR_NULLCHANCE := "nula";
const STR_LOWCHANCE := "baixa";
const STR_INTERMEDIATECHANCE := "media";
const STR_GOODCHANCE := "alta";

//funcao que pergunta por fechadura, caso o item seja lockable
function CraftLockable(who, theitem)
		if (theitem.isDoor())
			SetObjProperty(theitem, "pedreiro", who.serial); 
			SetObjProperty(theitem, "criador", who.acctname);	
			SetObjProperty(theitem, "validade", CInt(ReadGameClock()+360));
			MoveObjectToLocation(theitem, who.x, who.y, who.z, who.realm, MOVEOBJECT_NORMAL);
			theitem.movable := 0;
		endif

		if (KP_IsLockable(theitem ))
			var fechadura := YesNo(who, "Voce deseja por uma fechadura nesse item?");
			if (fechadura)
				SendSysMessageEx(who, "Escolha a fechadura para por neste item.", SSM_REQUEST);
				var targ := Target(who);
				if (targ)
					if (targ.objtype == 4001)
						if (!theitem.isDoor())
							var lvl := Cint(GetObjProperty(targ, "level"));
							if (!lvl)
								lvl := 1;
							endif
							SetObjProperty(theitem, "level", lvl);
						endif
						var lockid := CInt(GetObjProperty(targ, "LockId"));
						SetObjProperty(theitem, "LockId", lockid);
						DestroyItem(targ);
						SendSysMessageEx(who, "Voce instalou a fechadura com sucesso.", SSM_INFO);
					else
						SendSysMessageEx(who, "Isto nao parece uma fechadura.", SSM_FAIL);
					endif
				endif
			endif
		endif
endfunction

function GetMaterialName(obj);

  if ( (obj == "wood") || (obj == "leather") || (obj == "cloth") || (obj == "metal") || (obj == "glass") || (obj == "stone") || obj == "arrowhead")
    return obj;
  else
    var items := ReadconfigFile("::itemdesc");
    if (items[obj])
      return items[obj].desc;
    endif
  endif
  

endfunction

//Reference: http://uo.stratics.com/content/aos/baseproperties.shtml#exceptional
// as stratics says, exceptional weapons add +35 dmg increase

function AddQualityDamageMod( byref theitem, quality )
	var i;
	if (quality == EXCEP)

		var randomDmg;
		var maxBonus := 4;
		if (maxBonus == 0)
			maxBonus := 1;
		endif
		//adiciona 5% a 20%
		for (i := 1; i <= maxBonus; i := i + 1)
			randomDmg := RandomInt(5) + 1;
			var cfg:=ReadConfigFile(":combat:itemdesc");
			var elem := FindConfigElem(cfg, theitem.objtype);
			if (elem.Attribute != "RangedCombat")
				randomDmg := 1;
			endif
			case(randomDmg)
				1: theitem.damage_physical_mod := theitem.damage_physical_mod + 1; break;
				2: theitem.damage_fire_mod     := theitem.damage_fire_mod + 1; break;
				3: theitem.damage_cold_mod     := theitem.damage_cold_mod + 1; break;
				4: theitem.damage_energy_mod   := theitem.damage_energy_mod + 1; break;
				5: theitem.damage_poison_mod   := theitem.damage_poison_mod + 1; break;
			endcase
		endfor
	elseif (quality == NORMAL)
		theitem.damage_physical_mod := theitem.damage_physical_mod + 1; 
	elseif (quality == POOR)
		theitem.damage_physical_mod := theitem.damage_physical_mod - 1; 
	else
		return;
	endif


endfunction

function AddQualityResistMod( byref theitem, quality)
	var i;
	if (quality == EXCEP)

		var randomDmg;
		var maxBonus := 6;
		if (maxBonus == error)
			maxBonus := 6;
		endif
		for (i := 1; i <= maxBonus; i := i + 1)
			randomDmg := RandomInt(5) + 1;

			case(randomDmg)
				1: theitem.resist_physical_mod := theitem.resist_physical_mod +1; break;
				2: theitem.resist_fire_mod     := theitem.resist_fire_mod +1; break;
				3: theitem.resist_cold_mod     := theitem.resist_cold_mod +1; break;
				4: theitem.resist_energy_mod   := theitem.resist_energy_mod +1; break;
				5: theitem.resist_poison_mod   := theitem.resist_poison_mod +1; break;
			endcase

		endfor

	elseif (quality == NORMAL)
		theitem.resist_physical_mod := theitem.resist_physical_mod +1;
	elseif (quality == POOR)
		theitem.resist_physical_mod := theitem.resist_physical_mod -1;
	else
		return;

	endif

endfunction
  
 function AddItemDescription(who, item)
	var repeating := 1;
	var str;
	SendSysMessage(who, "Utilize apenas 3 linhas do gump.");
	while (repeating)
		str := SmallRequestGump(who, "Descreva o item", 1);
		if (len(str) < 84)
			break;
		endif
		SendSysMessage(who, "Voce excedeu o numero maximo de caracteres! Utilize apenas 3 linhas do gump");
	endwhile
	SetObjProperty(item, "description", str);
endfunction

function GetChanceString(value)

	if (value < 1)
		return STR_NULLCHANCE;
	elseif (value <= 33)
		return STR_LOWCHANCE;
	elseif (value <= 66)
		return STR_INTERMEDIATECHANCE;
	else
		return STR_GOODCHANCE;
	endif

endfunction
