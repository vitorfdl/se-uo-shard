/**
  * Includes all the functions related to hitscripts
  *  	SetArmorHitscript(who, hitscriptName, remove)
  *  	SetHitscript(who, hitscriptName, remove)
  *  	SetCriticalHitscript(who, hitscriptName, remove)
  *  	GetHitScriptsName(type)
  *  	GetHitScript(name)
  *  	PlayDeathHitScript(attacker, defender)
  *  	CreateCustomHitscript(who)
***/

use cfgfile;
use uo;
include "include/string";
include "include/arrays";
include ":gumps:requestgump";

const REMOVE_HITSCRIPT_FLAG := 1;
/**
 * SetArmorHitscript function
 * @param {Object} who - The object to set the hitscript on
 * @param {string} hitscript - The hitscript to set
 * @param {number} remove - Optional parameter to remove the hitscript
 */
function SetArmorHitscript(who, hitscriptName := "", remove := 0);
   var armorHitScripts := GetObjProperty(who, "ArmorHitScripts");
   if (!armorHitScripts)
      armorHitScripts := array{};
   endif

   if (!remove)
      if (hitscriptName in armorHitScripts)
         armorHitScripts.append(hitscriptName);
      endif
   else
      armorHitScripts := RemoveFromArray(armorHitScripts, hitscriptName);
   endif

   if (armorHitScripts.size() < 1)
      return EraseObjProperty(who, "ArmorHitScripts");
   endif

   SetObjProperty(who, "ArmorHitScripts", armorHitScripts);
endfunction


/**
 * SetHitscript function
 * @param {Object} who - The object to set the hitscript on
 * @param {string} hitscript - The hitscript to set
 * @param {number} remove - Optional parameter to remove the hitscript
 */
function SetHitscript(who, hitscriptName := "", remove := 0);
   var hitscripts := GetObjProperty(who, "HitScripts");
   if (!hitscripts)
      hitscripts := array{};
   endif

   if (!remove)
      if (hitscriptName in hitscripts)
         hitscripts.append(hitscriptName);
      endif
   else
      hitscripts := RemoveFromArray(hitscripts, hitscriptName);
   endif

   if (hitscripts.size() < 1)
      return EraseObjProperty(who, "HitScripts");
   endif
   SetObjProperty(who, "HitScripts", hitscripts);
endfunction


/**
 * SetCriticalHitscript function
 * @param {Object} who - The object to set the hitscript on
 * @param {string} hitscript - The hitscript to set
 * @param {number} remove - Optional parameter to remove the hitscript
 */
function SetCriticalHitscript(who, hitscriptName := "", remove := 0);
   var criticalhitscript := GetObjProperty(who, "CriticalHitScripts");
   if (!criticalhitscript)
      criticalhitscript := array{};
   endif

   if (!remove)
      if (hitscriptName in criticalhitscript)
         criticalhitscript.append(hitscriptName);
      endif
   else
      criticalhitscript := RemoveFromArray(criticalhitscript, hitscriptName);
   endif

   if (criticalhitscript.size() < 1)
      return EraseObjProperty(who, "CriticalHitScripts");
   endif

   SetObjProperty(who, "CriticalHitScripts", criticalhitscript);
endfunction

/**
 * GetHitScriptsName function
 * @param {string} type - The type of hit scripts to retrieve
 * @returns {array} - An array of hit script names
 */

function GetHitScriptsName(type := "WeaponHits")
   var hitscripts_cfg := ReadConfigFile(":combat:hitScripts");
   var elem := FindConfigElem(hitscripts_cfg, type);
   if (!elem) return array{}; endif

   var hits_list := array{};
   foreach hit in (ListConfigElemProps(elem))
      hits_list.append(StrReplace(GetConfigString(elem, hit), "_", " "));
      sleepms(2);
   endforeach

   return hits_list;
endfunction

/**
 * GetHitScript function
 * @param {string} name - The name of the hit script to retrieve
 * @returns {string} - The hit script found
 */
function GetHitScript(name)
   var hitscripts_cfg := ReadConfigFile(":combat:hitScripts");
   var elem := FindConfigElem(hitscripts_cfg, "WeaponHits");
   var hit_found := 0;

   foreach hit in (ListConfigElemProps(elem))
      if (GetConfigString(elem, hit) == name)
         hit_found := hit;
         break;
      endif
      sleepms(2);
   endforeach

   if (hit_found) return hit_found; endif

   elem := FindConfigElem(hitscripts_cfg, "ArmorHits");
   foreach hit in (ListConfigElemProps(elem))
      if (GetConfigString(elem, hit) == name)
         hit_found := hit;
         break;
      endif
      sleepms(2);
   endforeach

   return hit_found;
endfunction

/**
 * Function to play death hit script. Usually for NPCs
 * @param {object} attacker - The attacker object
 * @param {object} defender - The defender object
 */
function PlayDeathHitScript(attacker, defender)
  var weapon := attacker.weapon;
  var opts := GetObjProperty(weapon, "generichit");
  if (opts == error)//ve no npc
    opts := GetObjProperty(attacker, "generichit");
  endif
  if (opts == error) 
    return;
  endif
  
  var onkill := opts.onkill;
  

  if (onkill.text != "")
    PrintText(defender, onkill.text);
  endif
  if (onkill.fxmorto != error)
    PlayObjectCenteredEffect(defender, onkill.fxmorto[1], onkill.fxmorto[2], onkill.fxmorto[3]);
  endif
  if (onkill.sound != error)
    PlaySoundEffect(defender, onkill.som);
  endif
  if (cint(onkill.mana) > 0)
    HealMana(attacker, onkill.mana);
  endif
  if (onkill.stamina != error)
    HealStamina(attacker, onkill.stamina);
  endif
  if (onkill.hits != error)
    HealDamage(attacker, onkill.hits);
    RecalcVitals(attacker);
  endif
  if (onkill.itemcreate != error)
    CreateItemAtLocation(defender.x, defender.y, defender.z, onkill.itemcreate, onkill.itemamt, attacker.realm);
  endif
  if (onkill.lootcreate != error)
    var itm := CreateItemInContainer(defender, onkill.lootcreate, onkill.lootamt);
    if (itm  == error)
      PrintText(defender, " " + itm);
    endif
  endif
  if (onkill.destroy != error)
    DestroyItem(defender);
  endif

endfunction

/**
 * Allow you to create a hitscript for a weapon or npc
 * @param {object} who - The object for which the hit script is being created
 * @returns {struct} - The hit script options
 */
function CreateCustomHitscript(who)
	var options := CheckBoxGump(who, 300, 400, "Escolha as configuracoes do hitscript", array{"TimedScript", "Bonus Dano", "Dano Area", "Efeito Grafico", "Slayer", "Som", "Texto", "Criar Item", "OnKill"});
	
	var  hitopt := struct;
	foreach option in options
		if (option == "Bonus Dano") //stamina, mana
			var dmg := cdbl(RequestGump(who, "Quanto do rawdmg  da arma vai ser aumentado (de 0.1 a 10.0)?", "", "", 1));
			var dmgtype := RequestGump(who, "Digite qual o tipo de dano: Phisical, Fire, Cold, Poison, Energy ", "", "", 1);
			hitopt.+dmg := dmg;
			hitopt.+dmgtype := dmgtype;
			var vitals := CheckBoxGump(who, 200, 300, "Qual tipo de vital sera afetado pelo dano?", array{"Stam", "Hits", "Mana"});
			hitopt.+vitals := vitals;
		elseif  (option == "Dano Area")
			var area := Cint(RequestGump(who, "Qual a area do dano?", "", "", 1));
			hitopt.+area := area;
		elseif (option == "Som")
			var som := cint(RequestGump(who, "Escreva o id do som?", "", "", 1));
			hitopt .+som := som;
		elseif (option == "Texto")
			var textatacante := RequestGump(who, "Qual msg  vai aparecer no atacante?", "", "", 1);
			var textdefensor := RequestGump(who, "Qual msg  vai aparecer no defensor?", "", "", 1);
			if (textatacante != "")
				hitopt.+textatacante := textatacante;
			endif
			if (textdefensor != "")
				hitopt.+textdefensor := textdefensor;
			endif
		elseif (option  == "Efeito Grafico")
			var fxatacante := RequestGump(who, "Qual o efeito no atacante (id, speed, loop?", "", "", 1);
			var fxdefensor := RequestGump(who, "Qual o efeito no defensor (id, speed, loop?", "", "", 1);
			if (fxatacante != "")
				hitopt.+fxatacante := splitwords(fxatacante);
			endif
			if (fxdefensor != "")
				hitopt.+fxdefensor := splitwords(fxdefensor);
			endif
		elseif (option == "Criar Item")
			var where :=  CheckBoxGump(who, 200, 400, "Onde o item sera criado", array{"Atacante", "Defensor", "Random"});
			var createitem := RequestGump(who, "Qual o item a ser criado?", "", "", 1);
			hitopt.+createitem := createitem;
			hitopt.+itemloc := splitwords(where);
		elseif (option == "TimedScript")
			var timername := CheckBoxGump(who, 300, 500, "Escolha o timedscript", array{"defaultPoison", "hallucination", "Paralyze", "clumsy", "feeblemind", "weaken", "atrasarmagias", "fugademana", "staminaPoison", "manaPoison", "bleed", "darkborer"});

			hitopt.+timername := timername;
			hitopt.+timerduration := cint(RequestGump(who, "Qual a duracao do timer?", "Nunca mais de 120s", "", 1));
			hitopt.+level := cint(RequestGump(who, "Qual o level do timer?", "", "", 1));
		elseif (option == "OnKill")

      var onkillopts := CheckBoxGump(who, 200, 400, "Escolha as configuracoes para onkill", array{"Texto no corpo", "Som", "Efeito no corpo", "Destruir corpo", "Curar Mana", "Curar Stamina", "Curar Hits", "Criar Item", "Criar Item Loot"});
          var killopts := struct;
          foreach op in onkillopts
            if (op == "Texto no corpo")
              killopts.+text := RequestGump(who, "Qual a msg no corpo?", "", "", 1);
            elseif (op == "Som")
              killopts.+sound := cint(RequestGump(who, "Qual o id do som no corpo?", "", "", 1));
            elseif (op == "Efeito no corpo")
              var fxdefensor := splitwords(RequestGump(who, "Qual o efeito no corpo (id, speed, loop?", "", "", 1));
              killopts.+fxmorto := fxdefensor;
            elseif (op == "Destruir corpo")
              killopts.+destroy := 1;
            elseif (op == "Curar Mana")
              killopts.+mana := cint(RequestGump(who, "Quanto de mana sera curado?", "", "", 1));
            elseif (op == "Curar Stamina")
              killopts.+stamina := cint(RequestGump(who, "Quanto de stamina sera curado?", "", "", 1));
            elseif (op == "Curar Hits")
              killopts.+hits := cint(RequestGump(who, "Quanto de hits sera curado?", "", "", 1));
            elseif (op == "Criar Item")
              killopts.+itemcreate := cint(RequestGump(who, "Qual o id do item?", "", "", 1));
              killopts.+itemamt := cint(RequestGump(who, "Qual a quantidade do item?", "", "", 1));
            elseif (op == "Criar Item Loot")
              killopts.+lootcreate := cint(RequestGump(who, "Qual o id do item?", "", "", 1));
              killopts.+lootamt := cint(RequestGump(who, "Qual a quantidade do item?", "", "", 1));              
            endif
          endforeach
          hitopt.+onkill := killopts;
		elseif (option == "Slayer")
			hitopt.+slayer := RequestGump(who, "Qual o tipo de monstro (veja as aterogias no .createnpc)?", "", "", 1);
		endif
	endforeach

	hitopt.+chance:= cint(RequestGump(who, "Digite a chance  do hitscript acontecer (1 a 100)?", "", "", 1));

	return hitopt;
endfunction
