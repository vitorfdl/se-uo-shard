use math;
use util;

include "include/dungeons";
include ":yggdrasil:boss";
include ":yggdrasil:spawnSpot";
include ":yggdrasil:spawnGroup";

function KillAll(spawnpoint)
   SetObjProperty(spawnpoint, "spawnqueue", array{});
   var monsters := GetObjProperty(spawnpoint, "monsters");
   
   foreach m in monsters
      KillSpawn(spawnpoint, (monsters.size()+1) - _m_iter);
      sleepms(10);
   endforeach

   SetObjProperty(spawnpoint, "monsters", array{});
endfunction

function SpawnAll(spawnpoint)
   var monsters := GetObjProperty(spawnpoint, "monsters");

   foreach m in monsters
      KillSpawn(spawnpoint, (monsters.size()+1) - _m_iter);
      sleepms(10);
   endforeach
   SetObjProperty(spawnpoint, "monsters", array{});
   SetObjProperty(spawnpoint, "diedmonsters", array{});

   FillQueue(spawnpoint);
   var queue := GetObjProperty(spawnpoint, "spawnqueue");
   foreach q in queue
      DoSpawnQueue(spawnpoint, 1);
      sleepms(2);
   endforeach
endfunction

function KillSpawn(spawnpoint, index)
   var monsters := GetObjProperty(spawnpoint, "monsters");
   var monster_obj := monsters[index];
   if (!monster_obj)
      return 0;
   endif

   var monster := SystemFindObjectBySerial(monster_obj.serial);
   if (monster && !monster.master)
      SetObjProperty(monster, "summoned", 1);
      SetObjProperty(monster, "spawnkill", 1);
      for i := 1 to 2
         RevokePrivilege(monster, "invul" );
         ApplyRawDamage(monster, CInt(AP_GetVitalMaximumValue(monster, "Hits")));
         if (!monster || monster.dead) break; endif
      endfor
   endif

   monsters.Erase(index);
   SetObjProperty(spawnpoint, "monsters", monsters);
endfunction

function FillQueue(spawnpoint)
   var config      := spawnpoint.GetProps();
   var monsters    := config.monsters;

   // Remove monster from array
   // Set types and levels that is already spawned
   var types     := dictionary{};
   var levels    := dictionary{};
   var index_rem := array{};
   foreach monster_obj in monsters
      var monster := SystemFindObjectBySerial(monster_obj.serial);
      if (!monster || monster.master)
         index_rem += _monster_obj_iter;
      endif

      types[monster_obj.type]   := cint(types[monster_obj.type]) + 1;
      levels[monster_obj.level] := cint(levels[monster_obj.level]) + 1;
      sleepms(2);
   endforeach

   var spawn_queue := config.spawnqueue;
   foreach item in spawn_queue
      types[item.type]   := cint(types[item.type]) + 1;
      levels[item.level] := cint(levels[item.level]) + 1;
      sleepms(2);
   endforeach

   removeIndexes(monsters, index_rem);
   SetObjProperty(spawnpoint, "monsters", monsters);

   var spawn_size := monsters.size() + spawn_queue.size();

   // count recent died monsters
   var spot := SystemFindObjectBySerial(config.spot_serial);
   var spot_time := cint(GetObjProperty(spot, "timeinterval"));
   index_rem := array{};
   var diedmonsters := config.diedmonsters;
   foreach died in diedmonsters
      if (AddTimeTo(died.time, spot_time, "minutes") <= POLCore().systime)
         index_rem.append(_died_iter);
      else
         spawn_size += 1;
      endif
      sleepms(2);
   endforeach

   removeIndexes(diedmonsters, index_rem);
   SetObjProperty(spawnpoint, "diedmonsters", diedmonsters);

   // Spawn
   if (spawn_size >= config.mob_number)
      // print("Max Spawn size " + spawn_size);
      return; // Don't need to queue anything
   endif

   var spotgroup  := GetSpotGroupData(config.spotgroup, 0);
   if (!spotgroup.activegroup)
      SysLog(spawnpoint.serial+" no active spotgroup");
      return; //no active group
   endif
   var spawngroup := GetSpawnGroupData(spotgroup.activegroup, 0);
   if (!spawngroup)
      SysLog(spawnpoint.serial+" no active group");
      return; //no active group
   endif

   var daytime  := lower(GetGlobalProperty("dia"));
   var moonfase := lower(GetGlobalProperty("moonfase"));
   var n := 0;
   var spawn_number := cint(config.mob_number) - cint(spawn_size);
   for (n := 0; n < spawn_number; n := n + 1)
      var spawn_list := array{};
      spawn_queue := GetObjProperty(spawnpoint, "spawnqueue");

      foreach mob_config in (spawngroup.mob_list)
         if (mob_config.isBoss)
            mob_config.type := "boss";
         endif
         
         if ((!(mob_config.type in config.type)) || mob_config.power > config.level)
            continue;
         elseif (!(mob_config.daytime in array{ "always", "lua nova"}) && daytime != mob_config.daytime)
            continue;
         elseif (mob_config.daytime == "lua nova" && (moonfase != "nova" || daytime != "noite"))
            continue;
         elseif( !(cint(mob_config.mobgroup) in config.groups))
            continue;
         endif

         spawn_list.append(mob_config);
         sleepms(2);
      endforeach

      if (spawn_list.size() < 1)
         return; 
      endif
      
      if (config.type.size() > 1)
         if ("boss" in config.type && 1 < types["boss"])
            spawn_list := FindAllElemInArray(spawn_list, struct{ "type" := "boss" });
         else
            var preference := "melee";
            var z_diff := spot.z - spawnpoint.z;
            if (spot.z < 0 && z_diff < 0)
               z_diff := z_diff * -1;
            endif
            if (z_diff >= 10 && "ranged" in config.type)
               preference := "ranged";
            endif

            var calc_pref := Ceil(config.mob_number * 0.6);
            if (calc_pref < types[preference])
               spawn_list := FindAllElemInArray(spawn_list, struct{ "type" := preference });
            // elseif ("spellcaster" in config.type && types["spellcaster"] < Ceil(config.mob_number * 0.2))
            //    print("here2");
            //    spawn_list := FindAllElemInArray(spawn_list, struct{ "type" := "spellcaster" });
            endif
         endif
      endif

      var mob := spawn_list.RandomEntry();
      if (!mob) continue; endif;
      var to_spawn := struct{
         "template"   := mob.name,
         "type"       := mob.type,
         "level"      := mob.power,
         "daytime"    := mob.daytime,
         "bossParams" := mob.bossParams
      };

      if (mob.isBoss)
         to_spawn.+isBoss := mob.isBoss;
      endif
      
      spawn_queue += to_spawn;
      SetObjProperty(spawnpoint, "spawnqueue", spawn_queue);
      spawnpoint.SetProps(struct{ "spawnqueue" := spawn_queue });
      types[mob.type]   := cint(types[mob.type]) + 1;
      levels[mob.power] := cint(levels[mob.power]) + 1;
      sleepms(5);
   endfor
endfunction

function DoSpawnQueue(spawnpoint, pass := 0)
   var config  := spawnpoint.GetProps();
   var monster := config.spawnqueue[1];
   if (!monster)
      return 1;
   elseif (!pass && HasPlayerNear(spawnpoint.x, spawnpoint.y, spawnpoint.realm))
      return 1;
   endif

   var spot := SystemFindObjectBySerial(config.spot_serial);
   if (!spot) return; endif

   var x, y, z, spawned;
   var loscheck := (config.loscheck == 1);
   config.range := cint(config.range);
   if (config.range < 0) config.range := 0; endif

	for i := 1 to 3
		sleepms(5);
		if( config.range == 0)
			x := spawnpoint.x;
			y := spawnpoint.y;
			z := spawnpoint.z;
		elseif(spawnpoint.z > spot.z)
			x := spawnpoint.x + RandomIntMinMax(config.range * -1, config.range+1);
			y := spawnpoint.y + RandomIntMinMax(config.range * -1, config.range+1);
			z := GetStandingHeight(x, y, spawnpoint.z-5, spawnpoint.realm);
		elseif(spawnpoint.z <= spot.z)
			x := spawnpoint.x + RandomIntMinMax(config.range * -1, config.range+1);
			y := spawnpoint.y + RandomIntMinMax(config.range * -1, config.range+1);
			z := spawnpoint.z+5;
		endif

		if(loscheck && !CheckLosAt(spawnpoint, x, y, z))
         continue;
      endif

      spawned := CreateNpcFromTemplate(monster.template, x, y, z, 0, spawnpoint.realm);
      if (spawned != error)
         SetObjProperty(spawned, "spawnpoint", spawnpoint.serial);
         SetObjProperty(spawned, "spawnspot", config.spot_serial);
         SetObjProperty(spawned, "spawngroup", GetObjProperty(spot, "activegroup"));
         SetObjProperty(spawned, "range", config.range);
         
         if (monster.bossParams && monster.bossParams.Keys().size() >= 1)
            SetBoss(spawned, monster.bossParams);
         endif

         break;
      endif
	endfor

   if (spawned == error || !spawned.serial)
      return 0;
   endif

   monster.+serial := spawned.serial;
   config.spawnqueue.erase(1);
   config.monsters.append(monster);

   var update_conf := struct{
      "monsters" := config.monsters,
      "spawnqueue" := config.spawnqueue
   };

   spawnpoint.SetProps(update_conf);
   return 1;
endfunction