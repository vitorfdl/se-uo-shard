
function summon(template, targ,  mage)

	var summoned := CreateNpcFromTemplate(template, targ.x, targ.y, targ.z, 0, targ.realm);
	summoned.SetMaster(mage);
				
	SetObjProperty(summoned, "GuardKill", 1);	
  	SetObjProperty(summoned, "summoned", 1);	
	SetObjProperty(summoned, "owner", mage.serial);
	SetObjProperty(summoned, "name", targ.name);
	SetObjproperty(summoned, "script", targ.script);
	SetObjProperty(summoned, "AntigoTreinadorID", mage.serial);
		
	summoned.script := ":ghaia:tamed";
	summoned.facing := mage.facing;
	RestartScript(summoned);
	return summoned;
endfunction
