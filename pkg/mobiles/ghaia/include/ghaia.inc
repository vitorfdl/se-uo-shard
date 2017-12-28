use uo;

include ":loot:include/lootParser";
include ":ghaia:ghaiaInit";
include ":tn:cooldown";
include "include/facings";
include ":fls_core:packets";
include "include/client";
include "include/sounds";
include "include/shapes";
include "include/say";
include ":tn:npcutil";
include ":tn:cooldown";
include ":ghaia:ghaiaUtil";
include ":ghaia:habs";
include ":awareness:awareness";
include ":yggdrasil:boss";

// estados do npc, todos comecam no INIT, depois podem ir pro WANDER, 
// apos algum tempo no wander se não for interrompido eles vao pro SLEEP. do WANDER e do SLEEP podem ir pro FIGHT e desse pro VICTORY
enum STATES
   INITIALIZING := 1,
   WANDERING:= 2,
   FIGHTING := 3,
   SLEEPING := 4,
   VICTORY := 5,
   FLEEING := 6
endenum

var foods;

//alinhamento pra determinar se o player é inimigo ou não pra IA.
//outros inimigos pra IA é definido com a tag enemy
const ALIGN_EVIL := 1;
const ALIGN_GOOD := 2;
const ALIGN_NEUTRAL := 0;

//essa funcao determina se o npc vai enfrentar o alvo ou nao
function CanFight(me, mobile)
   EraseObjProperty(me, "path");
   if ( mobile.concealed || GetObjProperty(mobile, "ignorenpcs") || TS_GetCooldown(me, "peacemade"))
      return 0;
   elseif ( GetObjProperty(mobile, "#fantasma") == 1)
      return 0;
   elseif ( Cint(GetCooldown(mobile, "deathtime" )) > 0  )
   //printtextabove(me, "não vou atacar" + mobile.name);
      return 0;
   endif
 
   if (mobile.hidden && (!mobile.npctemplate || !mobile.master))
      var diff := CalculateDhDiff(me, mobile);
      var roll := rollAttrDice(me, WISDOM) + getProficiencyBonus(me, PERCEPCAO);

      if ( roll > diff )
         mobile.hidden := 0;
         PrintText(mobile, "*aparece*");
         SendSysMessageEx(mobile, "{} te encontrou!".format(me.name), SSM_FAIL);
      endif
   endif

//    var mycategory := lower(getnpccategory( mytemplate ));

   if (!mobile.npctemplate) //se for player olha a tendencia do npc
      if (me.alignment == ALIGN_EVIL) //npcs mme.alignmentos atacam todos os players
         //printtextabove(me, "mytemplate " + mytemplate + " " + cint(GetObjProperty(mobile, "empatiacomanimais"))); 
         // if ( TemHabilidade(mobile, "Aliado da Natureza" )) //verifica se tem empatia por animais
         //     var templateList := array{
         //         "giantbat", "giantrat", "gianttoad", "giantserpent", "snake", "mongbat", "naja", "warg", "polarbear", "lynx", "cougar", "alligator",
         //         "panther", "grizzlybear", "wolf", "sapop"
         //     };
         //     if (mytemplate in templateList)
         //         if (isBoss(mobile)) //verifica se é boss
         //             return 1;
         //         elseif ( TemHabilidade(mobile, "Aliado da Natureza 2" ) || Distance(me, mobile) > 5)
         //             return 0;
         //         endif
         //     endif
         // endif

         // if ( (mycategory == "undead") && (GetObjProperty(mobile, "aparenciacadaverica") == 1) )
         //     return 0;
         // elseif ( GetObjProperty(mobile, "kit") == "Bandido")
         //     if ( (mytemplate == "MercenarioRecruta") || (mytemplate == "MercenarioBesteiro") || (mytemplate == "Mercenario") || (mytemplate == "MercenarioLider") || (mytemplate == "MercenarioExperiente") )
         //         return 0;
         //     endif
         // endif
         return 1;
      else 
         //npcs bons ou neutrals não atacam
         return 0;
      endif
   endif

   if (me.alignment == ALIGN_EVIL)
      if (mobile.master) //se for bicho tamado, é atacado!
         return 1;
      endif
   endif

   var enemies := GetConfigStringArray( npccfgfile, "enemy" );
   if (!enemies)
      return 0;
   endif

   foreach enemy in enemies
      if ( enemy == mobile.my_template() )
         return 1;
      endif
   endforeach
endfunction

function AlertAllies(me, opponent)
   var player_allies  := array;
   var myallies       := array;
   var creatures_near := ListMobilesNearLocation(opponent.x, opponent.y, opponent.z, 15);
   var my_spot        := GetObjProperty(me, "spawnpoint");
   foreach mobile in creatures_near   
      if (!mobile.npctemplate || (mobile.master && !mobile.master.cmdlevel))
      //if (CheckLineOfSight(mobile, me))
         player_allies.append(mobile);
      //endif
      elseif (mobile.serial != me.serial && my_spot == GetObjProperty(mobile, "spawnpoint"))
         myallies.append(mobile);
      endif
      sleepms(2);
   endforeach


   foreach mob in myallies
      var ev := struct;
      ev.type := EVID_ALERT_ALLIES;
      ev.source := me;
      ev.opponent := player_allies.randomentry();
      SendEvent(mob, ev);
      sleepms(2);
   endforeach
endfunction

   //  var spawnpointserial := GetObjProperty(me, "spawnpoint");
   //  var spawnpoint := SystemFindObjectBySerial(spawnpointserial);   
   //  if (!spawnpoint)
   //      return;
   //  endif
   //  var spawn_config := spawnpoint.getProps();
  
//pega a lista de aliados proximos. se o npc for good, os players serao allies pra ele tbm
function GetAlliesNear(me)
   var spawnpointserial := GetObjProperty(mobile, "spawnpoint");
   var spawnpoint       := SystemFindObjectBySerial(spawnpointserial);
   spawnpoint := spawnpoint.GetProps();
   
   var myallies := array{};
   foreach mobile in ListMobilesNearLocation(me.x, me.y, me.z, 5)
      if (mobile.npctemplate && FindElemInArray(spawnpoint.monsters, struct{ "serial" := mobile.serial}))
         myallies.append(mobile);
      elseif (me.alignment == ALIGN_GOOD && !(me in mobile.aggressorto))
         myallies.append(mobile);
      endif
   endforeach

   return myallies;
endfunction
  
//pega a lista de inimigos proximo, se tiver player perto e a tendencia for evil ele vira inimigo
function GetEnemiesNear(me)
   var enemies := GetConfigStringArray( npccfgfile, "enemy" );
   var myenemies := ListHostiles(me, 8);
   foreach mobile in ListMobilesNearLocation(me.x, me.y, me.z, 8)
      if (mobile IN myenemies)
         continue;
      endif

      if (mobile.npctemplate)
         if ( mobile.my_template() in enemies)
               myenemies.append(mobile);
         endif
      else
         if (me.alignment == ALIGN_EVIL)
               myenemies.append(mobile);
         endif
      endif
   endforeach

   return myenemies;
endfunction    
    
//toca um som random do bixo
function PlayIdleSound()
   PlaySoundEffect(me, idlesnd.randomentry());
endfunction

function InitializeNpc(mobile)
   GrantPrivilege(mobile, "runwhilestealth");
   mobile.enable("runwhilestealth");
   //inicializa algumas variaveis que todos os npcs tem
   var elem     := npccfgfile; //TODO: Too Ugly, fix that
   var idlesnd   := GetConfigStringArray(elem, "IdleSound");
   flee_point   := CInt(npccfgfile.FleeLevel)/100.0;
   areasize     := CInt(npccfgfile.AreaSize);
   custombehaviors := GetConfigStringArray(npccfgfile, "CustomBehavior");


   // if (GetObjPRoperty(mobile, "championspawn") == 2)
   //     SetObjProperty(mobile, "#championspawn", 1);
   //     ApplyDamage( mobile, 20000);
   // elseif (GetObjPRoperty(mobile, "championspawn") == 1)
   //     SetObjPRoperty(mobile, "championspawn", 2);
   // endif

//elem := FindConfigElem(npccfgfile, mytemplate);

   var spawnpointserial := GetObjProperty(mobile, "spawnpoint");
   
   if (spawnpointserial)
      var spawnpoint := SystemFindObjectBySerial(spawnpointserial);
      if (spawnpoint)
         var props := spawnpoint.GetProps();

         var range := cint(props.range);
         if (!range)
            range := 6;
         endif
         SetObjProperty(mobile, "anchor", array{ spawnpoint.x , spawnpoint.y , spawnpoint.z});
         SetObjProperty(mobile, "range", range);
         SetAnchor(spawnpoint.x, spawnpoint.y, range, areasize);
         //print("drop anchor " + e);
      endif
   endif

//printtextabove(mobile, "initialize npc");
   if (GetObjProperty(mobile, "started"))
      return 0;
   endif

   if (npccfgfile.CanBeFemale && (randomint(3) == 2))
      mobile.graphic := 401;
      mobile.trueobjtype := 0x191;
      mobile.gender := 1;
   endif

//setup nos equips, loots, nome e cor
   SetupBackPack(mobile);
   SetupColor(mobile);
   SetupName(mobile);
   SetupEquip(mobile);
   var loot := npccfgfile.Loot;
   if (loot)
      Loot_Generate(mobile.backpack, loot);
   endif
//    GenerateRecipeLoot(mobile);
   SetupAmmo(mobile);
   SetObjProperty(mobile, "started", 1);
   mobile.InitPetProperties(0);
endfunction

//substitui o antigo wander() basicamente checa se tem anchor
function MoveAround(byref wanders)
   var anchor := GetObjProperty(me, "anchor");
   var range := GetObjProperty(me, "range");
   if (GetObjProperty(me, "owner"))
      anchor := 0;
   endif

   if (!range)
      range := 6;
   endif
   
   if (anchor == error && !me.hidden)
      wander();
   elseif (CoordinateDistance(me.x, me.y, anchor[1], anchor[2]) > range)
      TurnTowardLocation( anchor[1], anchor[2] );
      WalkTowardLocation( anchor[1], anchor[2] );
      if (wanders == 29)
         MoveObjectToLocation(me, anchor[1], anchor[2], anchor[3], me.realm, MOVEOBJECT_FORCELOCATION);
      endif
   elseif (!me.hidden)
      wander();
   endif
   
   PlayIdleSound();
endfunction


function LookForFood()
   if (!foods)
      foods := ReadConfigFile(":food:itemdesc") + ReadConfigFile(":cooking:itemdesc");
   endif
	if (!me.isTamable())
		return;
	endif

   var foodvalue := 0;
   if (cint(npccfgfile.TameDifficulty) >= 20)
      foodvalue := 4;
   elseif (cint(npccfgfile.TameDifficulty) >= 18)
      foodvalue := 3;
   elseif (cint(npccfgfile.TameDifficulty) >= 14)
      foodvalue := 2;
   endif

	var food;
   var food_list := ListItemsNearLocationWithFlag(me.x, me.y, me.z, 5, 0x00000800);
	foreach item in food_list
		var found := FindConfigElem(foods, hex(item.objtype));
		if (found && found.foodtype == npccfgfile.foodtype && cint(found.foodvalue) >= foodvalue)
			food := item;
		endif
		sleepms(2);
	endforeach

	if (!food) return; endif;

   WalkToward(food);
   WalkToward(food);

	if (Distance(me, food) <= 1)
		DestroyItem(food);
		TS_StartCooldown(me, "peacemade", (cint(food.foodvalue)+1) * 80);
		SetPeaceMade();
      Say("*comendo*");
	endif

	return 1;
endfunction

/////////////////////////////////
// Funcoes Especiais dos Npcs
////////////////////////////////

function hasCombat()
  var str := GetConfigString(npccfgfile,  "SpecialCombat");
  str := splitwords(str);
  //printtextabove(me, " " + str);
  if (str.size() >= 1)
      return 1;
  endif
  return 0;
endfunction

function doCombatSpecial(me)

  var str := GetConfigString(npccfgfile,  "SpecialCombat");
  str := splitwords(str);
  //printtextabove(me, " " + TS_GetCooldown(me, str[1]))
  if (!TS_GetCooldown(me, str[1]))
    //TS_StartCooldown(me, cstr(str[1]), cint(str[2]));
      Start_Script(str[1], array{me, cint(str[2]), str[1]});
      //printtextabove(me, " " + e +  " " + str[1]);
  endif

endfunction

function herd(ev)
   var holder     := ev.data;
   var lx         := holder[1];
   var ly         := holder[2];
   var loops      := 0;
   var walkmode   := cint(holder[3]);
   var saytrigger := cint(holder[4]);
   var idlesound  := cint(holder[5]);
   var waittime   := 0;
   var nome       := ev.source.name;
   var opponent;

   while(1)
      if( !CloseIn(me, lx, ly, walkmode))
         loops := loops + 1;
         waittime := 0;
      else
         loops := 0;
         waittime := 1;
      endif
      if((loops >= 30) or (coordinatedistance(me.x, me.y, lx, ly) <= 1))
         break;
      endif
      ev := wait_for_event(waittime);
      case (ev.type)
         SYSEVENT_DAMAGED: 
               opponent := ev.source;
               SetOpponent(opponent);
               TurnToward(opponent);
         SYSEVENT_ENGAGED: 
               opponent := ev.source;
               SetOpponent(opponent);
               TurnToward(opponent);
      endcase
   endwhile

   if (saytrigger)
      printtext(me, "*atende o chamado de " + nome + "*");
   endif

   if (idlesound)
      PlayIdleSound();
   endif
endfunction
