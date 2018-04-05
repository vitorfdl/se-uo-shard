/* $Id: fishingFunctions.inc 917 2005-11-05 23:51:11Z austin $
 *
 */

function is_fish(theobj)

	if((theobj >= UOBJ_FISH_START) && (theobj <= UOBJ_FISH_END) )
		return 1;
	elseif ((theobj >= UOBJ_SFISH_START) && (theobj <= UOBJ_SFISH_END))
		return 1;
	else
		return 0;
	endif

	return 0;

endfunction


function CarveFish(who, blade, use_on)
	var peso;
	var pesostack;
	var steak;
	var num_steaks;
	if( (!Accessible(who, use_on)) or (!CheckLineOfSight(who, use_on)) or (distance(who, use_on) > 2) )
		SendSysMessageEx(who, "Você não pode usar isto.");
		return 0;
	endif
	if( (!ReserveItem(use_on)) || (use_on.movable == 0) )
		SendSysMessageEx(who, "Você não pode usar isto.");
		return 0;
	endif
	
	var nomepeixe := use_on.name;
	nomepeixe := splitwords(nomepeixe);
	peso := cint(nomepeixe[4]);
	num_steaks := Cint((peso / 2));	

	//Verificacao do tipo de peixe para o tipo certo de filé, Alem do peso stack, que conserta o peso.
	if (use_on.objtype == 0x9987) pesostack := (2 + RandomInt(1)); steak := "0x9970";
 	elseif (use_on.objtype == 0xdd8)  pesostack := (60 + RandomInt(30)); steak :=  "0x9973";
 	elseif (use_on.objtype == 0x9cf) pesostack := (40 + RandomInt(20)) ; steak :=  "0x9978";
 	elseif (use_on.objtype == 0x9ce) pesostack := (25 + RandomInt(10));  steak :=  "0x9975";
 	elseif (use_on.objtype == 0xdd7) pesostack := (6 + RandomInt(1));  steak :=  "0x9971";
	elseif (use_on.objtype == 0x9cd) pesostack := (30 + RandomInt(15)); steak :=  "0x9974";
	elseif (use_on.objtype == 0x9cc) pesostack := (15 + RandomInt(1)); steak :=  "0x9970";
 	elseif (use_on.graphic == 0xdd9) pesostack := (30 + RandomInt(15)); steak :=  "0x9977";
	elseif (use_on.graphic == 0xdd6) pesostack := (2 + RandomInt(1)); steak :=  "0x9976";
	endif	

	//IF para não dar erro se stackarem os peixes (perdem o peso no nome)
	if (use_on.amount > 1)
		num_steaks := Cint(((pesostack / 2) * use_on.amount));
	endif
	
	steak := cint(steak);
	var peixe:= use_on;
	if( DestroyItem(use_on) )
		CreateItemInBackpack(who, steak, num_steaks);
		return 1;
	endif
	sleep(1);

	return 0;

endfunction

function IsInRange(who, x, y, z)
	z := z;
	var xd, yd;
	var retval := 1;
	xd := Abs(who.x - x);
	yd := Abs(who.y - y);
	if((xd > 6)||(yd > 6))
		retval := 0;
	endif

	return retval;
endfunction


function IsWater(thistile, locinfo)
	if(thistile.objtype)
		if((thistile.objtype >= 0x1797)&&(thistile.objtype <= 0x179c))
			return 1;
		elseif((thistile.objtype >= 0x346e)&&(thistile.objtype <= 0x3485))
			return 1;
		elseif((thistile.objtype >= 0x3490)&&(thistile.objtype <= 0x34ab))
			return 1;
		elseif((thistile.objtype >= 0x34b5)&&(thistile.objtype <= 0x34d5))
			return 1;
		elseif (thistile.objtype IN {20739, 2881, 2882, 2883, 2884, 5453})
			return 1;
		endif
	else
		if((locinfo.landtile >= 0x00a8)&&(locinfo.landtile <= 0x00ab))
			return 1;
		elseif((locinfo.landtile >= 0x0136)&&(locinfo.landtile <= 0x0137))
			return 1;
		elseif((locinfo.landtile >= 0x3ff0)&&(locinfo.landtile <= 0x3ff3))
			return 1;
		endif
	endif

	return 0;

endfunction


function IsValidFishingSpot(who, targetsquare, locinfo)
	var retval := 1;
	if(!IsWater(targetsquare, locinfo))
		SendSysMessageEx(who, "Você não pode pescar aqui!");
		retval := 0;
	elseif(!IsInRange(who, targetsquare.x, targetsquare.y, targetsquare.z))
		SendSysMessageEx(who, "Isto esta muito longe.");
		retval := 0;
	elseif(!CheckLosAt(who, targetsquare.x, targetsquare.y, targetsquare.z))
		SendSysMessageEx(who, "Este lugar não esta na sua linha de visao!");
		retval := 0;
	endif

	return retval;

endfunction


function farout(who)
	var x := who.x, y := who.y;
	var chk := 0;

	var theinfo := GetMapInfo( x, y );
	if((theinfo.landtile >= 0x00a8)&&(theinfo.landtile <= 0x00ab))
		chk := 1;
	elseif((theinfo.landtile >= 0x0136)&&(theinfo.landtile <= 0x0137))
		chk := 1;
	elseif((theinfo.landtile >= 0x3ff0)&&(theinfo.landtile <= 0x3ff3))
		chk := 1;
	else
		return 0;
	endif

	var distinfo := GetMapInfo( (x+15), (y+15) );
	if((distinfo.landtile >= 0x00a8)&&(distinfo.landtile <= 0x00ab))
		return 1;
	elseif((distinfo.landtile >= 0x0136)&&(distinfo.landtile <= 0x0137))
		return 1;
	elseif((distinfo.landtile >= 0x3ff0)&&(distinfo.landtile <= 0x3ff3))
		return 1;
	endif

	distinfo := GetMapInfo( (x-15), (y-15) );
	if((distinfo.landtile >= 0x00a8)&&(distinfo.landtile <= 0x00ab))
		return 1;
	elseif((distinfo.landtile >= 0x0136)&&(distinfo.landtile <= 0x0137))
		return 1;
	elseif((distinfo.landtile >= 0x3ff0)&&(distinfo.landtile <= 0x3ff3))
		return 1;
	endif

	distinfo := GetMapInfo( (x+15), (y-15) );
	if((distinfo.landtile >= 0x00a8)&&(distinfo.landtile <= 0x00ab))
		return 1;
	elseif((distinfo.landtile >= 0x0136)&&(distinfo.landtile <= 0x0137))
		return 1;
	elseif((distinfo.landtile >= 0x3ff0)&&(distinfo.landtile <= 0x3ff3))
		return 1;
	endif

	distinfo := GetMapInfo( (x-15), (y+15) );
	if((distinfo.landtile >= 0x00a8)&&(distinfo.landtile <= 0x00ab))
		return 1;
	elseif((distinfo.landtile >= 0x0136)&&(distinfo.landtile <= 0x0137))
		return 1;
	elseif((distinfo.landtile >= 0x3ff0)&&(distinfo.landtile <= 0x3ff3))
		return 1;
	endif

	return 0;
endfunction


function GetSOSTreasure(who, targetsquare, sosmsg)
	var test := rollAttrDice(who, DEXTERITY) + GetSkillBonus(who, PESCARIA);
	if( test > 18 )
		var itemcfg := ReadConfigFile(":*:itemdesc");
		var it;
		if( RandomInt(100) < 85 )
			var newitem;
			var artarray;
			case(RandomInt(3))
				0:	newitem := RandomInt(0x0fcc - 0x0fc4)+ 0x0fc4;
				1:	artarray := array(0xea4, 0xea8, 0xea7, 0xe9f, 0xea0, 0xea3, 0xea6);
					newitem := artarray[RandomInt(7) + 1];
				2:	artarray := array(0x1397, 0x13a4, 0x13a5, 0x13a6, 0x13a7, 0x13a8, 0x13a9, 0x13ab, 0x13ac, 0x13ad, 0x13ae, 0x163a, 0x163b, 0x163c);
					newitem := artarray[RandomInt(14) + 1];
			endcase
			it := CreateItemInBackpack(who, newitem, 1);
			// SetName(it, (oldname + " [Recuperado de um navio submerso]"));
			it.movable := 1;
			SendSysMessageEx(who, "Você achou alguma coisa interessante!");
		else
			// SendSysMessageEx(who, "Você achou um tesouro submerso!");
			// var chest := CreateItemInBackpack(who, 0x9e40, 1);
			// case(RandomInt(4) + 1)
			// 	1: chest.graphic := 0xe40;
			// 	2: chest.graphic := 0xe41;
			// 	3: chest.graphic := 0xe7c;
			// 	4: chest.graphic := 0x9ab;
			// endcase
			// SetName(chest, ("bau [Recuperado de um navio submerso]"));
			// chest.movable := 1;
			// var lvl := RandomDiceRoll("1d5");
			// var lootgroup;
			// case(lvl)
			//       1: lootgroup := "SpawnChest1";  
			//       2: lootgroup := "SpawnChest2";  
			//       3: lootgroup := "SpawnChest3";  
			//       4: lootgroup := "SpawnChest4";  
			//       5: lootgroup := "SpawnChest5";  
			// endcase
			// //MakeLoot(chest, lootgroup);
			// DestroyItem(sosmsg);
		endif
	else
		SendSysMessageEx(who, "Alguma coisa grande caiu de seu anzol!");
	endif

	return 0;

endfunction

function GetFishByBait(who, isca)
   var peixes := array{0x0DD6};
   case (isca)
      // "":
      //    peixes += array{0x4304}; //, "Marlin Negro"
      // "":
      //    peixes += array{0x4306}; //, "Lanterna", "Dragão do Inverno"
      0xc82: // milho
         peixes += array{0x9cc, 0x9cd, 0x9ce, 0x9cf}; //, "Garoupa Amarelo"
         break;
      0x9980: // lambari/peixepequeno
         peixes += array{0x44C3, 0x44E5, 0x44C6, 0x4303};
         break;
      default: 
         peixes += array{0x9cc, 0x9cd, 0x9ce, 0x9cf}; //, "Peixe da Noite", "Truta"
         break;
   endcase

   return peixes;
endfunction

function SelectBait(who, pole)

	SendSysMessageEx(who, "Qual isca sera usada na pescaria?", SSM_REQUEST);
	var bait := Target(who);

   if (!bait.isBait())
      return SendSysMessageEx(who, "Isso não pode ser usado de Isca.", SSM_FAIL);
   endif

   SetObjProperty(pole, "bait", bait.objtype);
	return bait.objtype;
endfunction
