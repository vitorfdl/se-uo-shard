/* $Id: attributeConstants.inc 834 2005-11-02 14:09:50Z austin $
 *
 * Purpose
 * Provide various functions and constants for the attribute system.
 *
 */
use uo;

enum VITALS
	HITS			    := "Hits",
	MANA			    := "Mana",
	STAMINA			 := "Stamina"
endenum

enum STATS
	STRENGTH		    := "Strength",
	DEXTERITY		 := "Dexterity",
	INTELLIGENCE	 := "Intelligence",
	CONSTITUTION	 := "Constitution",
	WISDOM	       := "Wisdom",
endenum

enum SKILLS
	ALQUIMIA           := "Alquimia",
	DESARMADO          := "Desarmado",
	ARMADURA_LEVE      := "Armadura Leve",
	ARMADURA_MEDIA     := "Armadura Média",
	ARMADURA_PESADA    := "Armadura Pesada",
	ESCUDO             := "Escudo",
	ARMA_SIMPLES       := "Arma Simples",
	ARMA_MARCIAL       := "Arma Marcial",
	ARCO               := "Arco",
	BESTA              := "Besta",
	ARCANISMO          := "Arcanismo",
	MEDICINA           := "Medicina",
	VENEFICIO          := "Venefício",
	PESQUISA           := "Pesquisa",
	PERCEPCAO          := "Percepção",
	SOBREVIVENCIA      := "Sobrevivência",
	PLANTACAO          := "Plantação",
	MARCENARIA         := "Marcenaria",
	COZINHA            := "Cozinha",
	COSTURA            := "Costura",
	EXTRACAO           := "Extração",
	TRATO_ANIMAIS      := "Trato de Animais",
	MONTARIA           := "Montaria",
	RUNAS              := "Runas",
	CURA               := "Milagre de Cura",
	NECROMANCIA        := "Milagre de Necromância",
	PRESTIDIGITACAO    := "Prestidigitação",
	FURTIVIDADE        := "Furtividade",
	SABOTAGEM 			 := "Sabotagem",
	TOCAR_INSTRUMENTO  := "Tocar Instrumento",
endenum