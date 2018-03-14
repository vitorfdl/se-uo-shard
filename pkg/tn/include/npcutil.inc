use uo;
use cfgfile;

function getnpccategory(template)
   if (template.isA(POLCLASS_NPC))
      template := template.my_template();
   endif
	var npccfgfile := ReadConfigFile(":ghaia:npcdesc");
	
	var category := npccfgfile[template].Category;	
	
	if (!category)
		return "sem categoria";
	else
		return category;
	endif
endfunction

function getnpcname(who, template)

	var npccfgfile := ReadConfigFile(":ghaia:npcdesc");
	
	var name := CStr(npccfgfile[template].Name);	
	
	return name;
endfunction
