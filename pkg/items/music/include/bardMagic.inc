
use os;
use uo;

include ":musicianship:bard";
include ":attributes:attributeConstants";
include ":attributes:attributes";
include "include/client";
include "include/sounds";
include "include/say";
include ":itemutils:canAccess";
include ":gumps:gumps";
include ":gumps:gumps_ex";
include ":gumps:requestgump";
include ":gumps:gump_prompts";
include ":timedscripts:timedScripts";
include "include/sysEvent";


function BardBuff(who,item,notas,notas2,tempo,magica)

	var x,y;


	x := 0;
	foreach nota in notas
		if (nota != "L")
		x := x + 1;
		endif
	endforeach
	y := 0;
	foreach nota in notas2
		if (nota != "L")
		y := y + 1;
		endif
	endforeach
	

	tempo := Cint(tempo);	

	var duration := ((tempo + (tempo * x) + (tempo * y)) / 1000);

	var mod := 0;//Cint((AP_GetSkill(who, MUSICIANSHIP) ));
	if (magica == 1) //Fortalecimento
		var mobiles := ListMobilesNearLocationEx( who.x, who.y, who.z, 10, 0x01, who.realm );
		foreach mobile in mobiles		
			if ((mobile.connected) && (mobile != who))
			TS_StartTimer(mobile, "strength", duration, mod, who, 0 );
			PrintTextAbove(Mobile, "*Inspirado pela musica*",3,191);
			endif
		endforeach
	elseif (magica == 2) //dexterity
		var mobiles := ListMobilesNearLocationEx( who.x, who.y, who.z, 10, 0x01, who.realm );
		foreach mobile in mobiles		
			if ((mobile.connected) && (mobile != who))
			TS_StartTimer(mobile, "agility", duration, mod, who, 0 );
			PrintTextAbove(Mobile, "*Inspirado pela musica*",3,191);
			endif
		endforeach
	elseif (magica == 3) //Intelligence
		var mobiles := ListMobilesNearLocationEx( who.x, who.y, who.z, 10, 0x01, who.realm );
		foreach mobile in mobiles		
			if ((mobile.connected) && (mobile != who))
			TS_StartTimer(mobile, "intelligence", duration, mod, who, 0 );
			PrintTextAbove(Mobile, "*Inspirado pela musica*",3,191);
			endif
		endforeach
	elseif (magica == 4) //STR + DEX
		var mobiles := ListMobilesNearLocationEx( who.x, who.y, who.z, 10, 0x01, who.realm );
		foreach mobile in mobiles		
			if ((mobile.connected) && (mobile != who))
			TS_StartTimer(mobile, "bless", duration, mod, who, 0 );
			PrintTextAbove(Mobile, "*Inspirado pela musica*",3,191);
			endif
		endforeach
	elseif (magica == 5) //ENFRAQUECER
		var mobiles := ListMobilesNearLocationEx( who.x, who.y, who.z, 10, 0x01, who.realm );
		foreach mobile in mobiles		
			if ((!mobile.connected) && (mobile != who))
			TS_StartTimer(mobile, "weaken", duration, mod, who, 0 );
			PrintTextAbove(mobile, "*Atordoado pela musica*",3,38);
			endif
		endforeach

	elseif (magica == 6) //PARALIZE
		var mobiles := ListMobilesNearLocationEx( who.x, who.y, who.z, 10, 0x01, who.realm );
			foreach mobile in mobiles	
			if (mobile != who)
			var mobilestr := AP_GetStat(mobile, "strength");	
			var chance := cint((mobilestr / 10));
			var rand := RandomInt(15);
			  if (rand > chance)
				PrintTextAbove(mobile, "*Sente uma vontade incontrolavel de parar e ouvir a musica*",3,191);
				TS_StartTimer(mobile, "paralysis", (duration * 1.5));
			else
				sleepms(1);
			  endif
			endif
			endforeach


	elseif (magica == 7) //Bladedance

			var ok := 0;
			var sword;
			var swordanim;
			SendSysMessage(who, "Selecione a espada que ira encantar",3,191);
			while (ok == 0)
				sword := target(who);
				if ((sword.objtype == 5049) || (sword.objtype == 3934) || (sword.objtype == 3936))
					ok := 1;
				else
					if( !sword )
						return 999;
					endif
					SendSysMessage(who, "Essa não e uma espada valida. Escolha outra",3,28);
				endif
			endwhile

			SendSysMessage(who, "Selecione o alvo da espada",3,300);
			var mobile := target(who);
			if( !mobile )
				return 999;
			endif
			
			if ((sword.objtype == 5049) || (sword.objtype == 3934) || (sword.objtype == 3936))
				swordanim := createnpcfromtemplate(":ghaia:lightsword", sword.x, sword.y, sword.z, 0 , who.realm);
			endif
			PrintTextAbove(swordanim, "*A espada comeca a dancar ao som da musica*",3,191);
			
			var oldsword := CreateItemCopyAtLocation(sword.x, sword.y, sword.z, sword, sword.realm);
			
			SetObjProperty(swordanim, "Espada", oldsword.serial);
			TS_StartTimer(swordanim, "bladedance", duration, sword.objtype, mobile, 0 );	
			DestroyItem( sword );
		 	SetObjProperty(who, "#MagicaMusic", Cint(ReadGameClock() + 15));


	elseif (magica == 8) //HamelinSong
			TS_StartTimer(who, "hamelin", duration, mod, who, 0 );
		 	SetObjProperty(who, "#MagicaMusic", Cint(ReadGameClock() + 15));	

	elseif (magica == 9) //EncantarSerpentes
			var mobiles := ListMobilesNearLocationEx( who.x, who.y, who.z, 10, 0x01, who.realm );
			foreach mobile in mobiles	
			if ((mobile.graphic == 52) || (mobile.graphic == 21))
				PrintTextAbove(mobile, "*Encantada pela musica*",3,191);
				TS_StartTimer(mobile, "paralysis", (duration * 0.5));
				sleepms(1);
			endif
			endforeach
		 	SetObjProperty(who, "#MagicaMusic", Cint(ReadGameClock() + 15));

	elseif (magica == 10) //FireDance
			var items := ListItemsNearLocation( who.x, who.y, who.z, 10, who.realm );
			var item2;
			foreach item in items	
			if (item.graphic == 3555)
				var rand := randomint(2);
				if (rand == 1)
					item2 := createitematlocation( item.x , item.y , item.z , 14742 , 1 , item.realm );
					destroyitem( item );
				else 
					item2 := createitematlocation( item.x , item.y , item.z , 14732 , 1 ,  item.realm );
					destroyitem( item );
				endif
				TS_StartTimer(item2, "firedance", duration);
			endif
			endforeach
		 	SetObjProperty(who, "#MagicaMusic", Cint(ReadGameClock() + 15));

	elseif (magica == 11) //Ensurdecer
			var mobiles := ListMobilesNearLocationEx( who.x, who.y, who.z, 10, 0x01, who.realm );
			PrintTextAbove(who, "*A musica alta ensurdece todos a sua volta*",3,191);
			foreach mobile in mobiles	
			if (mobile.connected)
				TS_StartTimer(mobile, "ensurdecer", duration);
				mobile.squelched := 1;
				sleepms(1);
			endif
			endforeach
			SetObjProperty(who, "#MagicaMusic", Cint(ReadGameClock() + 15));

	elseif (magica == 12) //Florescer
			var mobiles := ListMobilesNearLocationEx( who.x, who.y, who.z, 10, 0x01, who.realm );
			PrintTextAbove(who, "*A musica faz o ambiente florescer a sua volta*",3,660);
			TS_StartTimer(who, "florescer", (duration * 1.5));
			SetObjProperty(who, "#MagicaMusic", Cint(ReadGameClock() + 15));

	endif


	return 0;	
endfunction
