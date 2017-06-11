use uo;
use cfgfile;

include ":gumps:gumps";
include "include/arrays";
include ":fls_core:fls_chargumps_ex";
include ":attributes:attributes";
include ":gumps:yesno";
include ":charactercreation:characters";
include "include/say";

function KitGump(who)

	var gump := GFCreateGump();

  GFPage(gump, 0);
  GFDisposable(gump, 0);
	GFResizePic(gump, 0, 25, 3600, 600, 480);
	GFResizePic(gump, 15, 40, 0x13BE, 570, 450);
	GFTextMid(gump, 20, 50, 250, 1153, "Escolha seu kit");
  
  GFTextLine(gump, 50, 80, 1153, "Veterano" );
  GFTextLine(gump, 50, 100, 1153, "Criado por Lobos" );
  GFTextLine(gump, 50, 120, 1153, "Covarde" );
  GFTextLine(gump, 50, 140, 1153, "Valentão" );
  GFTextLine(gump, 50, 160, 1153, "Ex-Prisioneiro" );
  GFTextLine(gump, 50, 180, 1153, "Espírito Forte" );
  GFTextLine(gump, 50, 200, 1153, "Filho de Burgês" );
  GFTextLine(gump, 50, 220, 1153, "Treinado no Exército" );
  GFTextLine(gump, 50, 240, 1153, "Traça de Livro" );
  GFTextLine(gump, 50, 260, 1153, "Pacto Sombrio" );
  GFTextLine(gump, 50, 280, 1153, "Herança" );
  GFTextLine(gump, 50, 300, 1153, "Criado por Monges" );
  GFTextLine(gump, 50, 320, 1153, "Encantador de Cobras" );
  
  GFTextLine(gump, 300, 80, 1153, "Crescido nas Arenas" );
  GFTextLine(gump, 300, 100, 1153, "Viajante de Circo" );
  GFTextLine(gump, 300, 120, 1153, "Doentio" );
  GFTextLine(gump, 300, 140, 1153, "Pele Rija" );
  GFTextLine(gump, 300, 160, 1153, "Abascanto" );
  GFTextLine(gump, 300, 180, 1153, "Alquimista" );
  GFTextLine(gump, 300, 200, 1153, "Camponês" );
//  GFTextLine(gump, 300, 220, 1153, "Nobre" );
  GFTextLine(gump, 300, 240, 1153, "Filho de Heróis" );
  GFTextLine(gump, 300, 260, 1153, "Crescida Entre Garotos" );
  GFTextLine(gump, 300, 280, 1153, "Duro de Matar" );
  GFTextLine(gump, 300, 300, 1153, "Cria troll" );
  GFTextLine(gump, 300, 320, 1153, "Desbravador" );

  GFAddButton(gump, 30, 83, 2103, 2104, GF_PAGE_BTN, 2);
  GFAddButton(gump, 30, 103, 2103, 2104, GF_PAGE_BTN, 3);
  GFAddButton(gump, 30, 123, 2103, 2104, GF_PAGE_BTN, 4);
  GFAddButton(gump, 30, 143, 2103, 2104, GF_PAGE_BTN, 5);
  GFAddButton(gump, 30, 163, 2103, 2104, GF_PAGE_BTN, 6);
  GFAddButton(gump, 30, 183, 2103, 2104, GF_PAGE_BTN, 7);
  GFAddButton(gump, 30, 203, 2103, 2104, GF_PAGE_BTN, 8);
  GFAddButton(gump, 30, 223, 2103, 2104, GF_PAGE_BTN, 9);
  GFAddButton(gump, 30, 243, 2103, 2104, GF_PAGE_BTN, 10);
  GFAddButton(gump, 30, 263, 2103, 2104, GF_PAGE_BTN, 11);
  GFAddButton(gump, 30, 283, 2103, 2104, GF_PAGE_BTN, 12);
  GFAddButton(gump, 30, 303, 2103, 2104, GF_PAGE_BTN, 13);
  GFAddButton(gump, 30, 323, 2103, 2104, GF_PAGE_BTN, 14);

  GFAddButton(gump, 280, 83, 2103, 2104, GF_PAGE_BTN, 15);
  GFAddButton(gump, 280, 103, 2103, 2104, GF_PAGE_BTN, 16);
  GFAddButton(gump, 280, 123, 2103, 2104, GF_PAGE_BTN, 17);
  GFAddButton(gump, 280, 143, 2103, 2104, GF_PAGE_BTN, 18);
  GFAddButton(gump, 280, 163, 2103, 2104, GF_PAGE_BTN, 19);
  GFAddButton(gump, 280, 183, 2103, 2104, GF_PAGE_BTN, 20);
  GFAddButton(gump, 280, 203, 2103, 2104, GF_PAGE_BTN, 21);
//  GFAddButton(gump, 280, 223, 2103, 2104, GF_PAGE_BTN, 22);
  GFAddButton(gump, 280, 243, 2103, 2104, GF_PAGE_BTN, 23);
  GFAddButton(gump, 280, 263, 2103, 2104, GF_PAGE_BTN, 24);
  GFAddButton(gump, 280, 283, 2103, 2104, GF_PAGE_BTN, 25);
  GFAddButton(gump, 280, 303, 2103, 2104, GF_PAGE_BTN, 26);
  GFAddButton(gump, 280, 323, 2103, 2104, GF_PAGE_BTN, 27);

  GFAddButton(gump, 510, 450, 4502, 4502, 1, 99);
  GFTextLine(gump, 450, 460, 1153, "Proximo");
  GFSetRadioGroup(gump, 1);

  GFPage(gump, 2);
  GFHTMLArea( gump, 50, 360, 500, 90, "Você já participou de muitas guerras e batalhas antes, sabe como manejar armas e um pouco de estratégia, mas os ferimentos que sofreu o tornaram menos resistente aos rigores da guerra.", 0, 1);
  GFRadioButton (  gump, 50, 450, 210, 211, 0, 100);
  GFTextLine (gump, 80, 450, 1153, "Aceito este kit." );

  GFPage(gump, 3);
  GFHTMLArea( gump, 50, 360, 500, 90, "Você foi criado por uma alcatéia de lobos selvagens, a sobrevivência na natureza lhe deu um bom corpo mas uma dificuldade para as ferramentas e os livros.", 0, 1);
  GFRadioButton (  gump, 50, 450, 210, 211, 0, 101);
  GFTextLine (gump, 80, 450, 1153, "Aceito este kit." );

  GFPage(gump, 4);
  GFHTMLArea( gump, 50, 360, 500, 90, "O medo e a covardia fizeram de você bem esguio e bom em se esconder e passar despercebido, você aprendeu a fazer dessas habilidades suas armas.", 0, 1);
  GFRadioButton (  gump, 50, 450, 210, 211, 0, 102);
  GFTextLine (gump, 80, 450, 1153, "Aceito este kit." );

  GFPage(gump, 5);
  GFHTMLArea( gump, 50, 360, 500, 90, "Voce é grande, forte e estúpido.", 0, 1);
  GFRadioButton (  gump, 50, 450, 210, 211, 0, 103);
  GFTextLine (gump, 80, 450, 1153, "Aceito este kit." );

  GFPage(gump, 6);
  GFHTMLArea( gump, 50, 360, 500, 90, "Por algum motivo, você passou anos em uma masmorra e sessões de tortura o fizeram bem resistente, mas seu corpo carrega as seqüelas.", 0, 1);
  GFRadioButton (  gump, 50, 450, 210, 211, 0, 104);
  GFTextLine (gump, 80, 450, 1153, "Aceito este kit." );

  GFPage(gump, 7);
  GFHTMLArea( gump, 50, 360, 500, 90, "Você carrega uma Alma naturalmente mais poderosa, entretanto seu corpo sofre em contê-la.", 0, 1);
  GFRadioButton (  gump, 50, 450, 210, 211, 0, 105);
  GFTextLine (gump, 80, 450, 1153, "Aceito este kit." );

  GFPage(gump, 8);
  GFHTMLArea( gump, 50, 360, 500, 90, "Seu pai criou um negócio e deu certo, e você foi criado nas artes do comércio, não se exercitando mas aprendendo a fazer negócios e conseguir o que quer.", 0, 1);
  GFRadioButton (  gump, 50, 450, 210, 211, 0, 106);
  GFTextLine (gump, 80, 450, 1153, "Aceito este kit." );

  GFPage(gump, 9);
  GFHTMLArea( gump, 50, 360, 500, 90, "Você recebeu algum treinamento com armas no exército quando era jovem, mas acabou sendo dispensado por ter pé chato.", 0, 1);
  GFRadioButton (  gump, 50, 450, 210, 211, 0, 107);
  GFTextLine (gump, 80, 450, 1153, "Aceito este kit." );

  GFPage(gump, 10);
  GFHTMLArea( gump, 50, 360, 500, 90, "Você passou boa parte da vida lendo livros, mas por outro lado desenvolveu uma miopia de tanto ler no escuro, o que acaba atrapalhando-o em algumas tarefas.", 0, 1);
  GFRadioButton (  gump, 50, 450, 210, 211, 0, 108);
  GFTextLine (gump, 80, 450, 1153, "Aceito este kit." );

  GFPage(gump, 11);
  GFHTMLArea( gump, 50, 360, 500, 90, "Você trocou sua alma por poder, mas sua vida será curta.", 0, 1);
  GFRadioButton (  gump, 50, 450, 210, 211, 0, 109);
  GFTextLine (gump, 80, 450, 1153, "Aceito este kit." );

  GFPage(gump, 12);
  GFHTMLArea( gump, 50, 360, 500, 90, "Você recebeu alguma grande herança de alguem da sua família, a vida fácil resultou em poucos problemas pra você e isso fez com que levasse uma vida sem muitas preocupações ou aborrecimentos.", 0, 1);
  GFRadioButton (  gump, 50, 450, 210, 211, 0, 110);
  GFTextLine (gump, 80, 450, 1153, "Aceito este kit." );

  GFPage(gump, 13);
  GFHTMLArea( gump, 50, 360, 500, 90, "Você foi criado por monges e fez um voto de pobreza, nunca poderá acumular dinheiro, por outro lado você foi bem educado em várias áreas do conhecimento.", 0, 1);
  GFRadioButton (  gump, 50, 450, 210, 211, 0, 111);
  GFTextLine (gump, 80, 450, 1153, "Aceito este kit." );
  
  GFPage(gump, 14);
  GFHTMLArea( gump, 50, 360, 500, 90, "Você foi criado por gente que lida com serpentes e ganhou imunidade parcial a alguns venenos mas por outro lado várias mordidas debilitaram um pouco a sua saúde.", 0, 1);
  GFRadioButton (  gump, 50, 450, 210, 211, 0, 112);
  GFTextLine (gump, 80, 450, 1153, "Aceito este kit." );

  GFPage(gump, 15);
  GFHTMLArea( gump, 50, 360, 500, 90, "Você passou a vida toda lutando em arenas, é bastante apto para o combate, essa vida lhe deixou cansado e debilitado mentalmente.", 0, 1);
  GFRadioButton (  gump, 50, 450, 210, 211, 0, 113);
  GFTextLine (gump, 80, 450, 1153, "Aceito este kit." );

  GFPage(gump, 16);
  GFHTMLArea( gump, 50, 360, 500, 90, "Você passou muito tempo andando com uma trupe de artistas de circo. Possui um corpo atlético mas pouco dinheiro e pouca educaçao.", 0, 1);
  GFRadioButton (  gump, 50, 450, 210, 211, 0, 114);
  GFTextLine (gump, 80, 450, 1153, "Aceito este kit." );
  
  GFPage(gump, 17);
  GFHTMLArea( gump, 50, 360, 500, 90, "Você foi marcado pela natureza e tem um corpo debilitado e fraco por muitas doenças e infecções, por outro lado voce se dedicou muito ao estudo e a leitura.", 0, 1);
  GFRadioButton (  gump, 50, 450, 210, 211, 0, 115);
  GFTextLine (gump, 80, 450, 1153, "Aceito este kit." );
  
  GFPage(gump, 18);
  GFHTMLArea( gump, 50, 360, 500, 90, "Sua pele é mais rija que o normal, garantindo alguma armadura natural, mas devido a isso voce é um pouco lento e sua aparência pode ser assustadora para outras pessoas.", 0, 1);
  GFRadioButton (  gump, 50, 450, 210, 211, 0, 116);
  GFTextLine (gump, 80, 450, 1153, "Aceito este kit." );

  GFPage(gump, 19);
  GFHTMLArea( gump, 50, 360, 500, 90, "Magia não funciona direito em voce, seja para o bem ou para o mal.", 0, 1);
  GFRadioButton (  gump, 50, 450, 210, 211, 0, 117);
  GFTextLine (gump, 80, 450, 1153, "Aceito este kit." );

  GFPage(gump, 20);
  GFHTMLArea( gump, 50, 360, 500, 90, "Você passou sua vida mexendo com substâncias químicas, seus extratos e suas combinações. Sua aparência é doentia devido a exposição a substância tóxias, e sua compleição é doentia. Entretanto,você possui grande conhecimento.", 0, 1);
  GFRadioButton (  gump, 50, 450, 210, 211, 0, 118);
  GFTextLine (gump, 80, 450, 1153, "Aceito este kit." );

  GFPage(gump, 21);
  GFHTMLArea( gump, 50, 360, 500, 90, "Sua família sempre foi pobre e trabalhou nos campos, você cresceu forte e saudável com o trabalho, mas pobre e sem educação.", 0, 1);
  GFRadioButton (  gump, 50, 450, 210, 211, 0, 119);
  GFTextLine (gump, 80, 450, 1153, "Aceito este kit." );
  
  GFPage(gump, 22);
  GFHTMLArea( gump, 50, 360, 500, 90, "Voce é rico e decadente, sua vida foi sempre voltada a nobreza, e você manteve pouco contato com o resto do mundo, graças a isso voce se tornou inpato a realizar as tarefas mais simples.", 0, 1);
  GFRadioButton (  gump, 50, 450, 210, 211, 0, 120);
  GFTextLine (gump, 80, 450, 1153, "Aceito este kit." );

  GFPage(gump, 23);
  GFHTMLArea( gump, 50, 360, 500, 90, "Seu pai ou mãe eram famosos heróis. Você começa com a espada que seu pai usou e a fama de ser um filho de um herói, mas você tem um dever a cumprir e caso falhe todos cobrarão de você.", 0, 1); 
  GFRadioButton (  gump, 50, 450, 210, 211, 0, 121);
  GFTextLine (gump, 80, 450, 1153, "Aceito este kit." );

  GFPage(gump, 24);
  GFHTMLArea( gump, 50, 360, 500, 90, "Você sempre preferiu brincar com garotos ao invés de garotas. Você possui mais força, mas menos graciosidade.", 0, 1);
  GFRadioButton (  gump, 50, 450, 210, 211, 0, 122);
  GFTextLine (gump, 80, 450, 1153, "Aceito este kit." );

  GFPage(gump, 25);
  GFHTMLArea( gump, 50, 360, 500, 90, "Você já resistiu de tudo na sua vida, mas seu sofrimento foi tanto que lhe afetou em todos os sentidos possíveis.", 0, 1);
  GFRadioButton (  gump, 50, 450, 210, 211, 0, 123);
  GFTextLine (gump, 80, 450, 1153, "Aceito este kit." );

  GFPage(gump, 26);
  GFHTMLArea( gump, 50, 360, 500, 90, "O destino foi cruel com você, dotado de uma deformação horrenda, foi abandonado por seus pais, e como todos pensam que voce é a cria de algum monstro você se tornou um hábil combatente para se proteger de valentões, mas nunca pode estudar ou acumular riquezas.", 0, 1);
  GFRadioButton (  gump, 50, 450, 210, 211, 0, 124);
  GFTextLine (gump, 80, 450, 1153, "Aceito este kit." );

  GFPage(gump, 27);
  GFHTMLArea( gump, 50, 360, 500, 90, "Você é um aventureiro-nato, vive em busca de novas descobertas e tesouros. Mas você sempre foi muito dependente de um grupo de pessoas para fazer o trabalho pesado.", 0, 1);
  GFRadioButton (  gump, 50, 450, 210, 211, 0, 125);
  GFTextLine (gump, 80, 450, 1153, "Aceito este kit." );

  var input := GFSendGump(who, gump);
  
  //sendsysmessage(who, " " + input.keys);
  
  foreach key in (input.keys)
    if (key >= 100)
      if (key == 100)
        SetObjProperty(who, "kit", "veterano");
        AP_SetSkillMod(who, TACTICS, 15);
		AP_ModifySkillMod(who, SWORDSMANSHIP, 5);
		AP_ModifySkillMod(who, PARRY, 5);
		AP_ModifySkillMod(who, AXE, 5);
		AP_ModifySkillMod(who, STAFF, 5);
		AP_ModifySkillMod(who, WRESTLING, 5);
		AP_ModifySkillMod(who, ARCHERY, 5);
		AP_ModifySkillMod(who, FENCING, 5);
		AP_ModifySkillMod(who, MACE_FIGHTING, 5);
		AP_ModifySkillMod(who, POLE_ARMS, 5);
        AP_SetTrueStat(who, STRENGTH, AP_GetTrueStat(who, STRENGTH)-10);
      elseif ( key == 101)
        SetObjProperty(who, "kit", "criado por lobos");
        AP_SetTrueStat(who, INTELLIGENCE, AP_GetTrueStat(who, INTELLIGENCE)-10);
      elseif ( key == 102 )
        SetObjProperty(who, "kit", "covarde");
        AP_SetSkillMod(who, HIDING, 10);
        AP_SetSkillMod(who, STEALTH, 10);
        AP_SetTrueStat(who, STRENGTH, AP_GetTrueStat(who, STRENGTH)-10);
      elseif ( key == 103 )
        SetObjProperty(who, "kit", "valentao");
        AP_SetTrueStat(who, STRENGTH, AP_GetTrueStat(who, STRENGTH)+10);
        AP_SetTrueStat(who, INTELLIGENCE, AP_GetTrueStat(who, INTELLIGENCE)-10);
      elseif ( key == 104 )
        SetObjProperty(who, "kit", "exprisioneiro");
        AP_SetTrueStat(who, STRENGTH, AP_GetTrueStat(who, STRENGTH)-10);
        SetObjProperty(who, "hitsmod", CInt(AP_GetTrueStat(who, STRENGTH)/2.5));
      elseif ( key == 105 )
        SetObjProperty(who, "kit", "espirito forte"); //-10 hits +30 mana
        SetObjProperty(who, "hitsmod", -10);
        SetObjProperty(who, "manamod", 30);
      elseif ( key == 106 )
        SetObjProperty(who, "kit", "filho de burgues");
        AP_SetSkillMod(who, CARPENTRY, 10);
        AP_SetSkillMod(who, BLACKSMITHY, 10);
        AP_SetSkillMod(who, TINKERING, 10);
        AP_SetSkillMod(who, TAILORING, 10);
        AP_SetTrueStat(who, INTELLIGENCE, AP_GetTrueStat(who, INTELLIGENCE)+5);
        AP_SetTrueStat(who, STRENGTH, AP_GetTrueStat(who, STRENGTH)-15);
        AP_SetTrueStat(who, DEXTERITY, AP_GetTrueStat(who, DEXTERITY)-10);
      elseif ( key == 107 )
        SetObjProperty(who, "kit", "treinado no exercito");
        AP_SetSkillMod(who, TACTICS, 15);
        AP_SetTrueStat(who, DEXTERITY, AP_GetTrueStat(who, DEXTERITY)-10);
      elseif ( key == 108 )
        SetObjProperty(who, "kit", "traca de livro");
        AP_SetTrueStat(who, INTELLIGENCE, AP_GetTrueStat(who, INTELLIGENCE)+10);
        AP_SetTrueStat(who, DEXTERITY, AP_GetTrueStat(who, DEXTERITY)-10);
      elseif ( key == 109 )
        SetObjProperty(who, "kit", "pacto sombrio");
        AP_SetTrueStat(who, STRENGTH, AP_GetTrueStat(who, STRENGTH)+10);
        AP_SetTrueStat(who, DEXTERITY, AP_GetTrueStat(who, DEXTERITY)+10);
        AP_SetTrueStat(who, INTELLIGENCE, AP_GetTrueStat(who, INTELLIGENCE)+10);
	SetObjProperty(who, "dpmod", -30);
      elseif ( key == 110 )
        SetObjProperty(who, "kit", "herança");
        AP_SetTrueStat(who, INTELLIGENCE, AP_GetTrueStat(who, INTELLIGENCE)-10);
        var coin := CreateItemInBackpack(who, 0xba64, 200);
	SetObjProperty(coin, "marked", who.name+" - "+who.acctname);
        CreateItemInBackpack(who, 0xba63, 1000);
        return 500;
      elseif ( key == 111 )
        SetObjProperty(who, "kit", "criado por monges");
        AP_SetTrueStat(who, INTELLIGENCE, AP_GetTrueStat(who, INTELLIGENCE)+10);
        return -80;
      elseif ( key == 112 )
        SetObjProperty(who, "kit", "criado por encatadores de serpentes");
        AP_SetTrueStat(who, STRENGTH, AP_GetTrueStat(who, STRENGTH)-5);
      elseif ( key == 113 )
        SetObjProperty(who, "kit", "crescido nas arenas");
        AP_SetTrueStat(who, INTELLIGENCE, AP_GetTrueStat(who, INTELLIGENCE)-20);
        AP_SetTrueStat(who, STRENGTH, AP_GetTrueStat(who, STRENGTH)+10);
        AP_SetTrueStat(who, DEXTERITY, AP_GetTrueStat(who, DEXTERITY)+10);
	SetObjProperty(who, "stammod", -20);
      elseif ( key == 114 )
        SetObjProperty(who, "kit", "viajante de circo");
        AP_SetTrueStat(who, INTELLIGENCE, AP_GetTrueStat(who, INTELLIGENCE)-5);
        AP_SetTrueStat(who, DEXTERITY, AP_GetTrueStat(who, DEXTERITY)+10);
        return -50; //pobre
      elseif ( key == 115 )
        SetObjProperty(who, "kit", "doentio");
        AP_SetTrueStat(who, INTELLIGENCE, AP_GetTrueStat(who, INTELLIGENCE)+30);
        AP_SetTrueStat(who, STRENGTH, AP_GetTrueStat(who, STRENGTH)-10);
        AP_SetTrueStat(who, DEXTERITY, AP_GetTrueStat(who, DEXTERITY)-10);
      elseif ( key == 116 )
        SetObjProperty(who, "kit", "pele rija");
        AP_SetTrueStat(who, DEXTERITY, AP_GetTrueStat(who, DEXTERITY)-10);
        SetObjProperty(who, "PhysicalResistMod", 15);
      elseif ( key == 117 )
        SetObjProperty(who, "kit", "abascanto");
      elseif ( key == 118 )
        SetObjProperty(who, "kit", "alquimista");
        AP_SetTrueStat(who, STRENGTH, AP_GetTrueStat(who, STRENGTH)-10);
        AP_SetTrueStat(who, DEXTERITY, AP_GetTrueStat(who, DEXTERITY)-10);
        AP_SetSkillMod(who, ALCHEMY, 25);
      elseif ( key == 119 )
        SetObjProperty(who, "kit", "campones");
        AP_SetTrueStat(who, STRENGTH, AP_GetTrueStat(who, STRENGTH)+10);
        AP_SetTrueStat(who, INTELLIGENCE, AP_GetTrueStat(who, INTELLIGENCE)-5);
        return -80; //pobre
      elseif ( key == 120 )
        SetObjProperty(who, "kit", "nobre");
        AP_SetTrueStat(who, STRENGTH, AP_GetTrueStat(who, STRENGTH)-10);
        AP_SetTrueStat(who, DEXTERITY, AP_GetTrueStat(who, DEXTERITY)-10);
        AP_SetTrueStat(who, INTELLIGENCE, AP_GetTrueStat(who, INTELLIGENCE)-10);
        var coin := CreateItemInBackpack(who, 0xba65, 120);
	SetObjProperty(coin, "marked", who.name+" - "+who.acctname);
        coin := CreateItemInBackpack(who, 0xba64, 200);
	SetObjProperty(coin, "marked", who.name+" - "+who.acctname);
        CreateItemInBackpack(who, 0xba63, 1000);
        return 500;
      elseif ( key == 121 )
        SetObjProperty(who, "kit", "filho de heroi");
        var sword := CreateItemInBackpack(who, "vikingsword", 1);
        sword.dmg_mod := 5;
        sword.hp_mod := 50;
        SetName(sword, "espada magica");
	SetObjProperty(sword, "owner", who.acctname+" - "+who.name);
      elseif ( key == 122 )
        SetObjProperty(who, "kit", "crescida entre garotos");
        AP_SetTrueStat(who, STRENGTH, AP_GetTrueStat(who, STRENGTH)+10);
        AP_SetTrueStat(who, DEXTERITY, AP_GetTrueStat(who, DEXTERITY)-10);
      elseif ( key == 123 )
        SetObjProperty(who, "kit", "duro de matar");
	SetObjProperty(who, "dpmod", 30);
        AP_SetTrueStat(who, STRENGTH, AP_GetTrueStat(who, STRENGTH)-10);
        AP_SetTrueStat(who, DEXTERITY, AP_GetTrueStat(who, DEXTERITY)-10);
        AP_SetTrueStat(who, INTELLIGENCE, AP_GetTrueStat(who, INTELLIGENCE)-10);
      elseif ( key == 124 )
        SetObjProperty(who, "kit", "cria troll");
        AP_SetTrueStat(who, STRENGTH, AP_GetTrueStat(who, STRENGTH)+30);
        AP_SetTrueStat(who, INTELLIGENCE, AP_GetTrueStat(who, INTELLIGENCE)-30);
        return -80;
      elseif ( key == 125 )
        SetObjProperty(who, "kit", "desbravador");
        AP_SetTrueStat(who, STRENGTH, AP_GetTrueStat(who, STRENGTH)-15);
        AP_SetSkillMod(who, CARTOGRAPHY, 10);
        AP_SetSkillMod(who, REMOVE_TRAP, 10);
        AP_SetSkillMod(who, LOCKPICKING, 10);
        AP_SetSkillMod(who, DETECTING_HIDDEN, 10);
      endif
    endif
  endforeach

endfunction


function ItemPickerGump(who, byref chardata)

	var cfg := ReadConfigFile(":charactercreation:config/itempicker");
	var secoes := GetConfigStringKeys( cfg );
	var chargump := GFCreateGump();
	var input;

	GFClosable(chargump, 0);
	GFDisposable(chargump, 0);
	GFResizePic(chargump, 0, 25, 3600, 600, 480);
	GFResizePic(chargump, 15, 40, 0x13BE, 570, 450);
	GFTextMid(chargump, 15, 45, 600, 1153, "Defina seus Itens Iniciais");
	GFTextMid(chargump, 15, 60, 600, 1153, "Voce tem " + chardata.Credits + " creditos.");
	GFAddButton(chargump, 510, 450, 4502, 4502, 1, 999 );
	GFTextLine(chargump, 450, 460, 1153, "Termina");

	var x := 50;
	var y := 75;
	var page := 1;
	foreach entry in secoes
		if (entry != "DefaultAmount")
			GFTextLine(chargump, x, y, 1153, entry );
			GFAddButton(chargump, x-30, y+4, 9762, 9763, GF_PAGE_BTN, page);
			y := y + 20;
			page := page + 1;
		endif
    sleepms(2);
	endforeach

	page := 1;
	var itens;
	x := 170;
	y := 75;
	var itensmap := dictionary;
	var itemcusto := dictionary;
	var i := 1;
	var elem;
	var custo;
	foreach entry in secoes
		if (entry != "DefaultAmount")
			GFPage(chargump, page);
			elem := FindConfigElem(cfg, entry);
			itens := ListConfigElemProps(elem);
			foreach item in itens
				GFTextLine(chargump, x, y, 1153, item );
				custo := GetConfigInt(elem, item);
				GFTextLine(chargump, x+100, y, 1153, ""+custo );
				itemcusto[item] := custo;
				itensmap[i] := item;
				GFAddButton(chargump, x-20, y+4, 2103, 2104, 1, i);
				i := i + 1;
				y := y + 20;
				if ( y >= 450 )
					y := 75;
					x := 320;
				endif
			endforeach
			x := 170;
			y := 75;
			page := page + 1;
		endif
    sleepms(2);
	endforeach

	input := GFSendGump(who, chargump);
	input := input[0];

	if (input == 999)
		return;
	else
		var buyitem := itensmap[input];
		var custo := itemcusto[buyitem];
		var amt := 1;
		var amtelem := FindConfigElem(cfg, "DefaultAmount");
		amt := Cint(GetConfigInt(amtelem, buyitem));
		if ( (amt == 0) || (amt == error) )
			amt := 1;
		endif
		if (chardata.Credits >= custo)
			chardata.Credits := chardata.Credits - custo;
			CreateItemInBackpack( who, buyitem, amt );
			ItemPickerGump(who, chardata);
		else
			SendSysMessage(who, "Voce nao tem creditos para comprar isto.");
			ItemPickerGump(who, chardata);
		endif
	endif

endfunction
