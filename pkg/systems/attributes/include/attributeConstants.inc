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

enum DIFFICULTIES
	DIFF_VERYEASY   := 8,
	DIFF_EASY       := 10,
	DIFF_MEDIUM     := 14,
	DIFF_HARD       := 17,
	DIFF_VERYHARD   := 20,
	DIFF_IMPOSSIBLE := 23,
endenum

enum SKILLS
	ALQUIMIA           := "Alquimia",
	ARMA_SIMPLES       := "Arma_Simples",
	ARMA_MARCIAL       := "Arma_Marcial",
	ARMADURA_LEVE      := "Armadura_Leve",
	ARMADURA_MEDIA     := "Armadura_Média",
	ARMADURA_PESADA    := "Armadura_Pesada",
	ARCO               := "Arco",
	ARCANISMO          := "Arcanismo",
	BESTA              := "Besta",
	DESARMADO          := "Desarmado",
	ESCUDO             := "Escudo",
	MEDICINA           := "Medicina",
	VENEFICIO          := "Veneício",
	ESCRIBA           := "Escriba",
	PERCEPCAO          := "Percepção",
	SOBREVIVENCIA      := "Sobrevivência",
	PLANTACAO          := "Plantação",
	MARCENARIA         := "Marcenaria",
	COZINHA            := "Cozinha",
	COSTURA            := "Costura",
	FERRARIA           := "Ferraria",
	EXTRACAO           := "Extração",
	TRATO_ANIMAIS      := "Trato_de_Animais",
	MONTARIA           := "Montaria",
	INSCRICAO_RUNICA   := "Inscrição_Rúnica",
	MILAGRES           := "Milagres",
	PRESTIDIGITACAO    := "Prestidigitação",
	FURTIVIDADE        := "Furtividade",
	SABOTAGEM 			 := "Sabotagem",
	TOCAR_INSTRUMENTO  := "Tocar_Instrumento",
endenum
