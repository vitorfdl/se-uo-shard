use polsys;
use datafile;
use cfgfile;

include ":gumps:yesno";
include ":gumps:requestgump";
include ":timedscripts:timedScripts";
include "include/say";
include "include/objtype";
include ":attributes:attributes";

function HaveRecipe(who, objtype)
   var items := GetObjProperty(who,  "recipes");

   if (objtype in items.keys())
      return 1;
   endif

   return 0;
endfunction

function RecipeCharges(difficulty)
   if (difficulty >= 25)
      return 5;
   elseif (difficulty >= 20)
      return 4;
   elseif (difficulty >= 18)
      return 3;
   elseif (difficulty >= 15)
      return 2;
   else
      return 1;
   endif
endfunction

function checkifHasLevel(objtype)
   var cfg := ReadConfigFile(":*:itemdesc");
   var obj := FindConfigElem(cfg, hex(objtype));
   if (!obj) FindConfigElem(cfg, cint(objtype)); endif

   if (obj.Damage || obj.hasLevel || obj.equipscript)
      return 1;
   endif

   return 0;
endfunction

function AddRecipe(who, objtype, from_smelt := 0)
   objtype := cint(objtype);
   var recipes := GetObjProperty(who, "recipes");
   if (!recipes)
      recipes := dictionary{};
   endif

   var item_has_level := checkifHasLevel(objtype);

   if (recipes[objtype])
      if (recipes[objtype].level >= 10)
         return 0;
      endif
      
      if (!item_has_level)
         return 0;
      endif

      var max_exp := recipes[objtype].level * 100;
      recipes[objtype].exp += _ifElse(from_smelt, 4, 10);
      if (recipes[objtype].exp >= max_exp)
         recipes[objtype].level += 1;
         recipes[objtype].exp -= max_exp;
      endif
   else
      recipes[objtype] := struct{ level := item_has_level, exp := 0 };
   endif

   SetObjProperty(who, "recipes", recipes);
   return 1;
endfunction

function CheckCharges(who, recipe, charges_to_learn)
   var charge_list := GetObjProperty(recipe, "learning");
   if (!charge_list)
      charge_list := array{};
   endif

   var already_learning := FindInArray(charge_list, struct{ person := who.serial });

   if (already_learning)
      var charges := charge_list[already_learning].charges;
      if (charges+1 >= charges_to_learn)
         return 1;
      endif

      charge_list[already_learning].charges += charges;
      SendSysMessageEx(who, "Aprendeu mais sobre a receita. Leia mais vezes para domina-la.", SSM_INFO);
   else
      var charges := 0;
      if (charges+1 >= charges_to_learn)
         return 1;
      endif
      
      var me_learning := struct{ "person" := who.serial, "charges" := 1};
      charge_list.append(me_learning);
      SendSysMessageEx(who, "Aprendeu mais sobre a receita. Leia mais vezes para domina-la.", SSM_INFO);
   endif

   SetObjProperty(recipe, "learning", charge_list);
   TS_StartCooldown(who, "learning_recipe", 60*5);
   return 0;
endfunction

function LearnRecipe(who, recipe)
   var recipe_obj := GetObjProperty(recipe, "objtype");
   if (hex(recipe.objtype) == error)
      SendSysMessageEx(who, "Esta receita deve ser aprendida de outra forma.", SSM_INFO);
      return 0;
   elseif (TS_GetCooldown(who, "learning_recipe") && !who.cmdlevel)
      SendSysMessageEx(who, "Sua mente ainda esta absorvendo a ultima receita que leu.", SSM_FAIL);
      return 0;
   endif

   var cfg := ReadConfigFile(":*:recipes");
   var obj := FindConfigElem(cfg, recipe_obj);

   var difficulty       := GetConfigInt(obj, "Difficulty");
   var type             := GetConfigString(obj, "Type");
   var charges_to_learn := RecipeCharges(difficulty);

   if (CheckCharges(who, recipe, charges_to_learn))
      if (type == "group" || type == "kit")
         var recipes := GetConfigStringArray(obj, "Recipe");
         foreach obj_type in recipes
            AddRecipe(who, obj_type);
            sleepms(2);
         endforeach
      else
         AddRecipe(who, recipe_obj);
      endif
      SendSysMessageEx(who, "A receita foi aprendida.", SSM_INFO);
      DestroyItem(recipe);
      return 1;
   endif
   SendLearnMessage(who);
endfunction

function SendLearnMessage(who)
   SendSysMessageEx(who, "Será possivel ler uma receita novamente em 5 minutos.", SSM_INFO);
endfunction
