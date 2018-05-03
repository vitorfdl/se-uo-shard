/* $Id: client.inc 1343 2006-05-12 07:56:34Z austin $
 *
 * Purpose
 * This include file is designed to hold all the client lowlevel constants.
 * Not all constants might be usable as of now, but in the future, they
 * might be.
 *
 */

use uo;

enum GUMPID_LIST
    GUMPID_SKILLLIST   := 0x000010,
    GUMPID_EMAIL_INDEX := 0x000011,
    GUMPID_TNGUMPS     := 0x000015,
    GUMPID_CREATECHAR  := 0x000016,
    GUMPID_CRAFTING    := 0x000017,
    GUMPID_FICHARP     := 0x000018,
    GUMPID_RECIPES     := 0x000019,
	 GUMPID_CLERICBUY     := 0x00001A,
	 GUMPID_SPELLRECIPE   := 0x00001B,
	 GUMPID_CLERICSPELL   := 0x00001C,
	 GUMPID_MERCHANTBUY   := 0x00001D,
	 GUMPID_MERCHANTSELL  := 0x00001E,
	 GUMPID_GRIMORIO  := 0x000020,
	 GUMPID_RUNEBOOK  := 0x000021,
	 GUMPID_WORLDSAVE  := 0x000022,
	 GUMPID_ALCHEMYBOOk := 0x000023,
    GUMPID_BARD := 0x000024,
    GUMPID_DISGUISE := 0x000025,
    GUMPID_EXPBAR := 0x000026
endenum

enum HUMAN_ANIMATIONS
	ANIM_WALK_UNARMED      :=  0x0, // Walk unarmed.
	ANIM_WALK_ARMED        :=  0x1, // Walk armed.
	ANIM_RUN_UNARMED       :=  0x2, // Run unarmed.
	ANIM_RUN_ARMED         :=  0x3, // Run armed.
	ANIM_STAND             :=  0x4, // Stand.
	ANIM_FIDGET_1          :=  0x5, // Twist a little (look around?).
	ANIM_FIDGET_2          :=  0x6, // Look down.
	ANIM_STAND_1HAND       :=  0x7, // Stand 1 handed attack.
	ANIM_STAND_2HAND       :=  0x8, // Stand 2 handed attack.
	ANIM_ATTACK_1HAND_WIDE :=  0x9, // Attack 1 handed wide (sword/any).
	ANIM_ATTACK_1HAND_JAB  :=  0xA, // Attack 1 handed jab (fencing).
	ANIM_ATTACK_1HAND_DOWN :=  0xB, // Attack 1 handed down (mace).
	ANIM_ATTACK_2HAND_DOWN :=  0xC, // Attack 2 handed jab (mace).
	ANIM_ATTACK_2HAND_WIDE :=  0xD, // Attack 2 handed wide.
	ANIM_ATTACK_2HAND_JAB  :=  0xE, // Attack 2 handed jab (spear).
	ANIM_ATTACK_BOW        :=  0x12, // Attack bow.
	ANIM_ATTACK_XBOW       :=  0x13, // Attack xbow.
	ANIM_WALK_WARMODE      :=  0xF, // Walk in warmode.
	ANIM_CAST_DIR          :=  0x10, // Spellcasting direction.
	ANIM_CAST_AREA         :=  0x11, // Spellcasting area.
	ANIM_HIT               :=  0x14, // Get hit.
	ANIM_DIE_BACK          :=  0x15, // Die backward.
	ANIM_DIE_FOR           :=  0x16, // Die forward.
	ANIM_BOW               :=  0x20, // Bow.
	ANIM_SALUTE            :=  0x21, // Salute.
	ANIM_EAT               :=  0x22  // Eat.
endenum


enum LAYERS
	LAYER_NONE        :=  0x0,   // Not used?
	LAYER_HAND1       :=  0x1,   // One handed weapon.
	LAYER_HAND2       :=  0x2,   // Two handed weapon, shield or misc.
	LAYER_SHOES       :=  0x3,   // Shoes.
	LAYER_PANTS       :=  0x4,   // Pants.
	LAYER_SHIRT       :=  0x5,   // Shirt.
	LAYER_HELM        :=  0x6,   // Helm or Hat.
	LAYER_GLOVES      :=  0x7,   // Gloves.
	LAYER_RING        :=  0x8,   // Ring.
	LAYER_UNUSED9     :=  0x9,   // Unused.
	LAYER_NECK        :=  0xA,   // Neck.
	LAYER_HAIR        :=  0xB,   // Hair
	LAYER_WAIST       :=  0xC,   // Waist (half apron).
	LAYER_CHEST       :=  0xD,   // Torso (inner) (chest armor).
	LAYER_WRIST       :=  0xE,   // Bracelet.
	LAYER_PACK2       :=  0xF,   // Unused (backpack, but ord. bp is 0x15).
	LAYER_BEARD       :=  0x10,   // Facial hair.
	LAYER_TUNIC       :=  0x11,   // Torso (middle) (tunic, sash etc.).
	LAYER_EARS        :=  0x12,   // Earrings.
	LAYER_ARMS        :=  0x13,   // Arms.
	LAYER_CAPE        :=  0x14,   // Back (cloak).
	LAYER_PACK        :=  0x15,   // Backpack
	LAYER_ROBE        :=  0x16,   // Torso (outer) (robe)
	LAYER_SKIRT       :=  0x17,   // Legs (outer) (skirt/robe).
	LAYER_LEGS        :=  0x18,   // Legs (inner) (leg armor).
	LAYER_MOUNT       :=  0x19,   // Mount (horse, ostard etc.).
	LAYER_VENDORSTOCK :=  0x1A,   // This vendor will sell and restock.
	LAYER_VENDOREXTRA :=  0x1B,   // This vendor will resell to players but not restock.
	LAYER_VENDORBUY   :=  0x1C,   // This vendor can buy from players but does not stock.
	LAYER_BANKBOX     :=  0x1D,   // Contents of bankbox
	LAYER_TRADE       :=  0x1E    // Can be multiple of these, do not use directly.
endenum


enum SPEECH_TYPES
	SPEECH_REGULAR      := 0x0,
	SPEECH_BROADCAST    := 0x1,
	SPEECH_EMOTE        := 0x2,
	SPEECH_LABEL        := 0x6,
	SPEECH_EMPHASIS     := 0x7,
	SPEECH_WHISPER      := 0x8,
	SPEECH_YELL         := 0x9,
	SPEECH_SPELL        := 0xA
endenum


enum FONTS
	FONT_MAX          :=  0x9,   // Highest font.
	FONT_MIN          :=  0x0,   // Lowest font.
	
	FONT_BOLD        :=  0x0,   // Bold text.
	FONT_SHADOW      :=  0x1,   // Text with shadow.
	FONT_BOLD_SHAD   :=  0x2,   // Bold text with shadow.
	FONT_NORMAL      :=  0x3,   // Normal (default).
	FONT_GOTHIC      :=  0x4,   // Gothic.
	FONT_ITALIC      :=  0x5,   // Italic
	FONT_SMALL_DARK  :=  0x6,   // Small dark letters.
	FONT_COLOR       :=  0x7,   // Colorful font.
	FONT_RUNE        :=  0x8,   // Rune font (only capitals).
	FONT_SMALL_LIGHT :=  0x9    // Small light letters.
endenum


enum DIRECTIONS
	MIN_DIR	:=  0x0,
	MAX_DIR :=  0x7,

	DIR_N	:=  0x0,   // North.
	DIR_NE	:=  0x1,   // Northeast.
	DIR_E	:=  0x2,   // East.
	DIR_SE	:=  0x3,   // Southeast.
	DIR_S	:=  0x4,   // South.
	DIR_SW	:=  0x5,   // Southwest.
	DIR_W	:=  0x6,   // West.
	DIR_NW	:=  0x7    // Northwest.
endenum

enum GRAPHIC_EFFECTS
	GFX_DEATH_VORTEX_BIRTH := 0x3789,
	GFX_DEATH_VORTEX       := 0x37CC,
	GFX_FIELD_OF_BLADES    := 0x37A0,
	GFX_GLOW               := 0x37B9,
	GFX_BLACK_GLOW         := 0x37C3,
	GFX_SMOKE              := 0x3728,
	GFX_FIZZLE             := 0x3735
endenum

enum GRAPHIC_FIELDS
	GFX_POISON_GAS         := 0x113A,
	GFX_POISON_FIELD_EW    := 0x3915,
	GFX_POISON_FIELD_NS    := 0x3920,
	GFX_ENERGY_FIELD_EW    := 0x3946,
	GFX_ENERGY_FIELD_NS    := 0x3956,
	GFX_PARALYSIS_FIELD_EW := 0x3967,
	GFX_PARALYSIS_FIELD_NS := 0x3979,
	GFX_FIRE_FIELD_EW      := 0x398C,
	GFX_FIRE_FIELD_NS      := 0x3996,
	GFX_ENERGY             := 0x3818
endenum

enum GRAPHIC_SPARKLES
	GFX_MAGICAL_SPARKLES_EW := 0x1153,
	GFX_MAGICAL_SPARKLES_NS := 0x1159,
	GFX_BLUE_SPARKLES       := 0x373A,
	GFX_RED_SPARKLES        := 0x374A,
	GFX_BLUE_SPARKLE_SWIRL  := 0x375A,
	GFX_BLUE_SPARKLE_BORDER := 0x376A,
	GFX_BLUE_SPARKLE_GROUND := 0x3779
endenum

enum GRAPHIC_FIRE
	GFX_SMALL_FIREBALL	:= 0x36E4,
	GFX_LARGE_FIREBALL	:= 0x36D4,
	GFX_WAVY_FIREBALL	:= 0x36F4,
	GFX_EXPLOSION_BALL	:= 0x36FE,
	GFX_FIRE_COLUMN		:= 0x3709,
	GFX_EXPLODE_1		:= 0x36CB,
	GFX_EXPLODE_2		:= 0x36BD,
	GFX_EXPLODE_3		:= 0x36B0
endenum

////
// Creature IDs ( hardcoded graphic IDs )
/// 
Const CID_ZERO                := 0x000;  // Not used.
Const CID_OGRE                := 0x001;
Const CID_ETTIN               := 0x002;
Const CID_ZOMBIE              := 0x003;
Const CID_GARGOYLE            := 0x004;
Const CID_EAGLE               := 0x005;
Const CID_BIRD                := 0x006;
Const CID_ORC_LORD            := 0x007;
Const CID_CORPSER             := 0x008;
Const CID_DAEMON              := 0x009;
Const CID_DAEMON_SWORD        := 0x00a;
Const CID_DRAGON_GREY         := 0x00c;
Const CID_AIR_ELEMENTAL       := 0x00d;
Const CID_EARTH_ELEMENTAL     := 0x00e;
Const CID_FIRE_ELEMENTAL      := 0x00f;
Const CID_WATER_ELEMENTAL     := 0x010;
Const CID_ORC                 := 0x011;
Const CID_ETTIN_AXE           := 0x012;
Const CID_SHADE               := 0x014;
Const CID_GIANT_SNAKE         := 0x015;
Const CID_GAZER               := 0x016;
Const CID_LICH                := 0x018;
Const CID_GIANT_SPIDER        := 0x01c;
Const CID_APE                 := 0x01d;
Const CID_HARPY               := 0x01e;
Const CID_HEADLESS            := 0x01f;
Const CID_LIZARDMAN           := 0x021;
Const CID_LIZARDMAN_W_SPEAR   := 0x023;
Const CID_LIZARDMAN_W_MACE    := 0x024;
Const CID_MONGBAT             := 0x027;
Const CID_ORC_W_CLUB          := 0x029;
Const CID_RATMAN              := 0x02a;
Const CID_RATMAN_W_AXE        := 0x02c;
Const CID_RATMAN_W_SWORD      := 0x02d;
Const CID_REAPER              := 0x02f;
Const CID_SCORPION            := 0x030;
Const CID_SKELETON            := 0x032;
Const CID_SLIME               := 0x033;
Const CID_SNAKE               := 0x034;
Const CID_TROLL_W_AXE         := 0x035;
Const CID_TROLL               := 0x036;
Const CID_TROLL_W_MAUL        := 0x037;
Const CID_SKELETON_W_AXE      := 0x038;
Const CID_SKELETON_W_SCIMITAR := 0x039;
Const CID_WISP                := 0x03a;
Const CID_DRAGON_RED          := 0x03b;
Const CID_DRAKE_GREY          := 0x03c;
Const CID_DRAKE_RED           := 0x03d;
Const CID_TERATHEN_WARRIOR    := 0x046;
Const CID_TERATHEN_DRONE      := 0x047;
Const CID_TERATHEN_MATRIARCH  := 0x048;
Const CID_CYCLOPS             := 0x04b;
Const CID_TITAN               := 0x04c;
Const CID_GIANT_TOAD          := 0x050;
Const CID_BULLFROG            := 0x051;
Const CID_OPHIDIAN_MAGE       := 0x055;
Const CID_OPHIDIAN_WARRIOR    := 0x056;
Const CID_OPHIDIAN_QUEEN      := 0x057;
Const CID_SEA_CREATURE        := 0x05f;
Const CID_SEA_SERPENT         := 0x096;
Const CID_DOLPHIN             := 0x097;
Const CID_TAN_HORSE			:= 0x0c8;
Const CID_CAT				:= 0x0c9;
Const CID_ALLIGATOR			:= 0x0ca;
Const CID_PIG				:= 0x0cb;
Const CID_BROWN_HORSE		:= 0x0cc;
Const CID_BUNNY			:= 0x0cd;
Const CID_LAVA_LIZARD		:= 0x0ce;
Const CID_SHEEP			:= 0x0cf;
Const CID_CHICKEN			:= 0x0d0;
Const CID_GOAT				:= 0x0d1;
Const CID_DESERT_OSTARD		:= 0x0d2;
Const CID_BROWN_BEAR		:= 0x0d3;
Const CID_GRIZZLY_BEAR		:= 0x0d4;
Const CID_POLAR_BEAR		:= 0x0d5;
Const CID_PANTHER			:= 0x0d6;
Const CID_GIANT_RAT			:= 0x0d7;
Const CID_COW				:= 0x0d8;
Const CID_DOG				:= 0x0d9;
Const CID_FRENZIED_OSTARD	:= 0x0da;
Const CID_FOREST_OSTARD		:= 0x0db;
Const CID_LLAMA			:= 0x0dc;
Const CID_WALRUS			:= 0x0dd;
Const CID_SHEERED_SHEEP		:= 0x0df;
Const CID_WOLF				:= 0x0e1;
Const CID_LIGHT_HORSE		:= 0x0e2;
Const CID_GREY_HORSE		:= 0x0e4;
Const CID_COW2				:= 0x0e7;
Const CID_BULL				:= 0x0e8;
Const CID_BULL2			    := 0x0e9;
Const CID_GREAT_HART		:= 0x0ea;
Const CID_DEER				:= 0x0ed;
Const CID_RAT				:= 0x0ee;
Const CID_BOAR				:= 0x122;
Const CID_PACK_HORSE		:= 0x123;
Const CID_PACK_LLAMA		:= 0x124;

const CID_UNICORN           := 0x07a;
const CID_KIRIN             := 0x084;
const CID_FIRE_BEETLE       := 0x0a9;
const CID_RIDGEBACK         := 0x0bb;
const CID_SAVAGE_RIDGEBACK  := 0x0bc;
const CID_FIRESTEED         := 0x0be;
const CID_HIRYU             := 0x0f3;
const CID_REPTALON          := 0x114;
const CID_CU_SIDHE          := 0x115;
const CID_CHARGER_HORSE     := 0x11c;
const CID_ARMORED_BOURA     := 0x1b0;
const CID_GIANT_BEETLE      := 0x317;
const CID_HELLSTEED         := 0x319;
const CID_SWAMP_DRAGON      := 0x31a;
const CID_ARMORED_SWAMP_DRAGON := 0x31f;

const CID_HUMAN_MALE        := 0x190;
const CID_HUMAN_FEMALE      := 0x191;
const CID_ELF_MALE          := 0x25d;
const CID_ELF_FEMALE        := 0x25e;
const CID_GARGOYLE_MALE          := 0x29a;
const CID_GARGOYLE_FEMALE        := 0x29b;

const CID_HIGH_PLAINS_BOURA        := 0x2cb;
const CID_DRAGON_RUST := 0x2e;
const CID_DRAGON_FIRE := 0xac;
const CID_DRAGON_CRIMSON := 0xc5;
const CID_DRAGON_BLACK := 0xc6;


function LightningEffect(point, timing := 50)
	PlayMovingEffectXYZ(point.x, point.y, point.z+15, point.x-1, point.y, point.z+15, GFX_ENERGY, 5, timing);
	PlayMovingEffectXYZ(point.x, point.y, point.z+15, point.x+1, point.y, point.z+15, GFX_ENERGY, 5, timing);
	PlayMovingEffectXYZ(point.x, point.y, point.z+15, point.x, point.y+1, point.z+15, GFX_ENERGY, 5, timing);
	PlayMovingEffectXYZ(point.x, point.y, point.z+15, point.x, point.y-1, point.z+15, GFX_ENERGY, 5, timing);
endfunction

function SelectRandomExplosion()
   var explodes := array{GFX_EXPLODE_2, GFX_EXPLODE_3, GFX_EXPLODE_1 };
   return explodes.randomentry();
endfunction

function PlayAction(who, action, repeatcount := 1, repeatflag := 0, delay := 1)
   case (action)
      1: PerformAction(who, 0x00, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
      2: PerformAction(who, 0x01, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
      3: PerformAction(who, 0x0f, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
      4: PerformAction(who, 0x02, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
      5: PerformAction(who, 0x03, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
      6: PerformAction(who, 0x05, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
      7: PerformAction(who, 0x06, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
      8: PerformAction(who, 0x10, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
      9: PerformAction(who, 0x11, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
      10: PerformAction(who, 0x20, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
      11: PerformAction(who, 0x21, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;	
      12: PerformAction(who, 0x22, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
      13: PerformAction(who, 0x1e, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
      14: PerformAction(who, 0x1f, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
      15: PerformAction(who, 0x15, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
      16: PerformAction(who, 0x16, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
      17: PerformAction(who, 0x14, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
      18: PerformAction(who, 0x12, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
      19: PerformAction(who, 0x13, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
      20: PerformAction(who, 0x19, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
      21: PerformAction(who, 0x1a, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
      22: PerformAction(who, 0x1b, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
      23: PerformAction(who, 0x1c, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
      24: PerformAction(who, 0x17, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
      25: PerformAction(who, 0x18, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
      26: PerformAction(who, 0x1d, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
      27: PerformAction(who, 0x07, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
      28: PerformAction(who, 0x08, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
      29: PerformAction(who, 0x09, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
      30: PerformAction(who, 0x0a, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;

      31: PerformAction(who, 0xB, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;

      31: PerformAction(who, 0x0b, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;

      32: PerformAction(who, 0x0d, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
      33: PerformAction(who, 0x0c, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
      34: PerformAction(who, 0x0e, 5, repeatcount, ACTION_DIR_FORWARD , repeatflag, delay); break;
   endcase
endfunction