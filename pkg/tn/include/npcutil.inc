use uo;
use cfgfile;

function getnpccategory(template)

	var npccfgfile := ReadConfigFile(":ghaia:npcdesc");
	
	var category := npccfgfile[template].Category;	
	
	if (!category)
		return "sem categoria";
	else
		return category;
	endif
	
	unloadConfigFile(":ghaia:npcdesc");

endfunction

function getnpcname(who, template)

	var npccfgfile := ReadConfigFile(":ghaia:npcdesc");
	
	var name := CStr(npccfgfile[template].Name);	
	
	return name;

	
	unloadConfigFile(":ghaia:npcdesc");

endfunction