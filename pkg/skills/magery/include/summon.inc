
function summon(template, targ,  mage)

	var undead := CreateNpcFromTemplate(template, targ.x, targ.y, targ.z, 0, targ.realm);
	undead.SetMaster(mage);
				
	SetObjProperty(undead, "GuardKill", 1);	
  SetObjProperty(undead, "summoned", 1);	
	SetObjProperty(undead, "owner", mage.serial);
	SetObjProperty(undead, "name", targ.name);
	SetObjproperty(undead, "script", targ.script);
	SetObjProperty(undead, "AntigoTreinadorID", mage.serial);
		
	undead.script := ":ghaia:tamed";
	undead.facing := mage.facing;
	RestartScript(undead);
				

return undead;
endfunction
