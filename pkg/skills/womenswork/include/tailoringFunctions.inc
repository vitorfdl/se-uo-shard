
function process_wool(who, blade, sheep)
  if((!CheckLineOfSight(who, sheep)) or (Distance(who, sheep) > 2))
    SendSysMessage(who, "Voc� n�o pode usar isso.");
    return;
  endif
  if(sheep.graphic == 207)
    sheep.graphic := 223;
    CreateItemInContainer(who.backpack, 0xf125, 3);
  else
    SendSysMessage(who,"Essa ovelha n�o esta pronta para ser tosquiada.");
  endif
endfunction
