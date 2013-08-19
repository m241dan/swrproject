/***************************************************************************
*                           STAR WARS REALITY 1.0                          *
*--------------------------------------------------------------------------*
* Star Wars Reality Code Additions and changes from the Smaug Code         *
* copyright (c) 1997 by Sean Cooper                                        *
* -------------------------------------------------------------------------*
* Starwars and Starwars Names copyright(c) Lucas Film Ltd.                 *
*--------------------------------------------------------------------------*
* SMAUG 1.0 (C) 1994, 1995, 1996 by Derek Snider                           *
* SMAUG code team: Thoric, Altrag, Blodkai, Narn, Haus,                    *
* Scryn, Rennard, Swordbearer, Gorog, Grishnakh and Tricops                *
* ------------------------------------------------------------------------ *
* Merc 2.1 Diku Mud improvments copyright (C) 1992, 1993 by Michael        *
* Chastain, Michael Quan, and Mitchell Tse.                                *
* Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,          *
* Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.     *
* ------------------------------------------------------------------------ *
*		       Online Building and Editing Module		   *
****************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "mud.h"
#include "sha256.h"

extern int top_affect;
extern int top_reset;
extern int top_ed;
extern bool fBootDb;

REL_DATA *first_relation = NULL;
REL_DATA *last_relation = NULL;
char *sprint_reset( RESET_DATA *pReset, short *num );
bool check_area_conflict( AREA_DATA *carea, int low_range, int hi_range );
void fix_exits( void );
bool validate_spec_fun( const char *name );

/*
 * AI Frames of Mind -Davenge
 */

const char *const frames_of_mind[MAX_FOM] = {
   "idle", "fighting", "hunting"
};

/*
 * Quest Types -Davenge
 */

const char *const quest_types[MAX_QUESTTYPE] = {
   "one time", "repeatable"
};
/* planet constants for vip and wanted flags */

const char *const l_type[MAX_LOOTTYPE] = {
   "set", "random", "gold"
};

const char *const d_type[MAX_DAMTYPE] = {
   "chaos", "physical", "elemental", "fire", "water",
   "earth", "electricity", "wind", "energy", "dark_energy",
   "blunt", "piercing", "slashing"
};

const char *const q_type[MAX_QUALITYTYPE] = {
   "common", "uncommon", "rare", "crafted", "high quality"
};

const char *const d_type_score[MAX_DAMTYPE] = {
   "&WAll Damage", "&zPhysical", "&zElemental",
   "&RFire", "&BWater", "&OEarth", "&YElectricity", "&GWind",
   "&WEnergy", "&zDark Energy", "&pBlunt", "&rPiercing", "&rSlashing"
};

const char *const planet_flags[] = {
   "coruscant", "kashyyyk", "ryloth", "rodia", "nal_hutta", "mon_calamari",
   "honoghr", "gamorr", "tatooine", "adari", "byss", "endor", "roche", "af'el", "trandosh",
   "chad", "", "p16", "p17", "p18", "p19", "p20", "p21", "p22", "p23", "p24",
   "p25", "p26", "p27", "p28", "p29", "p30", "p31"
};

const char *const weapon_table[13] = {
   "none",
   "vibro-axe", "vibro-blade", "lightsaber", "whip", "claw",
   "blaster", "w7", "bludgeon", "bowcaster", "w10",
   "force pike", "w12"
};

const char *const spice_table[] = {
   "glitterstim", "carsanum", "ryll", "andris", "s4", "s5", "s6", "s7", "s8", "s9"
};

const char *const crystal_table[8] = {
   "non-adegan", "kathracite", "relacite", "danite", "mephite", "ponite", "illum", "corusca"
};


const char *const ex_flags[] = {
   "isdoor", "closed", "locked", "secret", "swim", "pickproof", "fly", "climb",
   "dig", "r1", "nopassdoor", "hidden", "passage", "portal", "r2", "r3",
   "can_climb", "can_enter", "can_leave", "auto", "r4", "searchable",
   "bashed", "bashproof", "nomob", "window", "can_look"
};

const char *const r_flags[] = {
   "dark", "reserved", "nomob", "indoors", "can_land", "can_fly", "no_drive",
   "nomagic", "bank", "private", "safe", "remove_this_flag", "petshop", "norecall",
   "donation", "nodropall", "silence", "logspeech", "nodrop", "clanstoreroom",
   "plr_home", "empty_home", "teleport", "hotel", "nofloor", "refinery", "factory",
   "republic_recruit", "empire_recruit", "spacecraft", "prototype", "auction"
};

const char *const o_flags[] = {
   "glow", "hum", "dark", "hutt_size", "contraband", "invis", "magic", "nodrop", "bless",
   "antigood", "antievil", "antineutral", "noremove", "inventory",
   "antisoldier", "antithief", "antihunter", "antijedi", "small_size", "large_size",
   "donation", "clanobject", "anticitizen", "antisith", "antipilot",
   "hidden", "poisoned", "covering", "deathrot", "burried", "prototype", "human_size"
};

const char *const mag_flags[] = {
   "returning", "backstabber", "bane", "loyal", "haste", "drain",
   "lightning_blade"
};

const char *const w_flags[] = {
   "take", "finger", "neck", "body", "head", "legs", "feet", "hands", "arms",
   "shield", "about", "waist", "wrist", "wield", "hold", "_dual_", "ears", "eyes",
   "_missile_", "r1", "r2", "r3", "r4", "r5", "r6",
   "r7", "r8", "r9", "r10", "r11", "r12", "r13"
};

const char *const area_flags[] = {
   "nopkill", "prototype", "r2", "r3", "r4", "r5", "r6", "r7", "r8",
   "r9", "r10", "r11", "r12", "r13", "r14", "r15", "r16", "r17",
   "r18", "r19", "r20", "r21", "r22", "r23", "r24",
   "r25", "r26", "r27", "r28", "r29", "r30", "r31"
};

const char *const o_types[] = {
   "none", "light", "_scroll", "_wand", "staff", "weapon", "_fireweapon", "missile",
   "treasure", "armor", "potion", "_worn", "furniture", "trash", "_oldtrap",
   "container", "_note", "drinkcon", "key", "food", "money", "pen", "_boat",
   "corpse", "corpse_pc", "fountain", "pill", "_blood", "_bloodstain",
   "scraps", "_pipe", "_herbcon", "_herb", "_incense", "fire", "book", "switch",
   "lever", "_pullchain", "button", "dial", "_rune", "_runepouch", "_match", "trap",
   "map", "_portal", "paper", "_tinder", "lockpick", "_spike", "_disease", "_oil",
   "fuel", "_shortbow", "_longbow", "_crossbow", "ammo", "_quiver", "shovel",
   "salve", "rawspice", "lens", "crystal", "duraplast", "battery",
   "toolkit", "durasteel", "oven", "mirror", "circuit", "superconductor", "comlink", "medpac",
   "fabric", "rare_metal", "magnet", "thread", "spice", "smut", "device", "spacecraft",
   "grenade", "landmine", "government", "droid_corpse", "bolt", "chemical",
   "bacta"
};

const char *const a_types[MAX_APPLY_TYPE] = {
   "none", "strength", "dexterity", "intelligence", "wisdom", "constitution", "agility",
   "sex", "null", "level", "age", "height", "weight", "force", "hit", "move",
   "credits", "experience", "evasion", "hitroll", "damroll", "save_poison", "save_rod",
   "save_para", "save_breath", "save_spell", "charisma", "affected", "resistant",
   "immune", "susceptible", "weaponspell", "luck", "backstab", "pick", "track",
   "steal", "sneak", "hide", "palm", "detrap", "dodge", "peek", "scan", "gouge",
   "search", "mount", "disarm", "kick", "parry", "bash", "stun", "punch", "climb",
   "grip", "scribe", "brew", "wearspell", "removespell", "emotion", "mentalstate",
   "stripsn", "remove", "dig", "full", "thirst", "drunk", "blood", "armor", "threat",
   "pen_all", "pen_physical", "pen_elemental", "pen_fire", "pen_water", "pen_earth",
   "pen_electricity", "pen_wind", "pen_energy", "pen_darkenergy", "pen_blunt", "pen_piercing", "pen_slashing",
   "res_all", "res_physical", "res_elemental", "res_fire", "res_water", "res_earth",
   "res_electricity", "res_wind", "res_enregy", "res_darkenergy", "res_blunt", "res_piercing", "res_slashing",
   "dtype_all", "dtype_physical", "dtype_elemental", "dtype_fire", "dtype_water", "dtype_earth",
   "dtype_electricity", "dtype_wind", "dtype_energy", "dtype_darkenergy", "dtype_blunt", "dtype_piercing", "dtype_slashing",
   "haste_from_eq", "haste_from_magic", "haste_from_skill", "dbl_attack", "speed",
   "barenumdie", "baresizedie", "damplus", "wepnumdie", "wepsizedie", "wepplus", "damtype"
};

const char *const a_types_pretty[MAX_APPLY_TYPE] = {
   "none", "strength", "dexterity", "intelligence", "wisdom", "constitution", "agility",
   "sex", "null", "level", "age", "height", "weight", "force", "hit points", "movement",
   "credits", "experience", "evasion", "hitroll", "damroll", "save_poison", "save_rod",
   "save_para", "save_breath", "save_spell", "charisma", "affected", "resistant",
   "immune", "susceptible", "weaponspell", "luck", "backstab", "pick", "track",
   "steal", "sneak", "hide", "palm", "detrap", "dodge", "peek", "scan", "gouge",
   "search", "mount", "disarm", "kick", "parry", "bash", "stun", "punch", "climb",
   "grip", "scribe", "brew", "wearspell", "removespell", "emotion", "mentalstate",
   "stripsn", "remove", "dig", "full", "thirst", "drunk", "blood", "armor", "threat",
   "penetrate all damage", "penetrate all physical damage", "penetrate all elemental", "penetrate fire", "penetrate water", "penetrate earth",
   "penetrate electricity", "penetrate wind", "penetrate energy", "penetrate darkenergy", "penetrate blunt", "penetrate piercing", "penetrate slashing",
   "resist all", "resist physical", "resist elemental", "resist fire", "resist water", "resist earth",
   "resist electricity", "resist wind", "resist enregy", "resist darkenergy", "resist blunt", "resist piercing", "resist slashing",
   "damage type potency all", "damage type potency physical", "damage type potency elemental", "damage type potency fire", "damage type potency water", "damage type potency earth",
   "damage type potency electricity", "damage type potency wind", "damage type potency energy", "damage type potency darkenergy", "damage type potency blunt", "damage type potency piercing", "damage type potency slashing",
   "haste", "haste", "haste", "double attack", "speed",
   "barehand die number", "barehand die size", "damage plus", "weapon die number", "weapon die size", "weapon damage plus", "damage type"
};

const char *const a_flags[MAX_AFF] = {
   "none", "blind", "invisible", "detect_evil", "detect_invis", "detect_magic",
   "detect_hidden", "weaken", "sanctuary", "faerie_fire", "infrared", "curse",
   "_flaming", "poison", "protect", "paralysis", "sneak", "hide", "sleep",
   "charm", "flying", "pass_door", "floating", "truesight", "detect_traps",
   "scrying", "fireshield", "shockshield", "r1", "iceshield", "possess",
   "berserk", "aqua_breath"
};

const char *const act_flags[] = {
   "npc", "sentinel", "scavenger", "r3", "r4", "aggressive", "stayarea",
   "wimpy", "pet", "train", "practice", "immortal", "deadly", "polyself",
   "meta_aggr", "guardian", "running", "nowander", "mountable", "mounted", "scholar",
   "secretive", "polymorphed", "mobinvis", "noassist", "nokill", "droid", "nocorpse",
   "r28", "r29", "prototype", "r31"
};

const char *const pc_flags[] = {
   "r0", "r1", "unauthed", "norecall", "nointro", "gag", "retired", "guest",
   "nosummon", "pageron", "notitled", "room", "r12", "r13", "r14", "r15", "r16", "r17", "r18", "r19",
   "r20", "r21", "r22", "r23", "r24", "r25", "r26", "r27", "r28", "r29", "r30",
   "r31"
};

const char *const plr_flags[] = {
   "npc", "boughtpet", "shovedrag", "autoexits", "autoloot", "autosac", "blank",
   "outcast", "brief", "combine", "prompt", "telnet_ga", "holylight",
   "wizinvis", "roomvnum", "silence", "noemote", "attacker", "notell", "log",
   "deny", "freeze", "killer", "pf_3", "litterbug", "ansi", "rip", "nice",
   "flee", "autocred", "automap", "afk"
};

const char *const trap_flags[] = {
   "room", "obj", "enter", "leave", "open", "close", "get", "put", "pick",
   "unlock", "north", "south", "east", "west", "up", "down", "examine",
   "northeast", "northwest", "southeast", "southwest", "r6", "r7", "r8",
   "r9", "r10", "r11", "r12", "r13", "r14", "r15"
};

const char *const wear_locs[] = {
   "light", "finger1", "finger2", "neck1", "neck2", "body", "head", "legs",
   "feet", "hands", "arms", "shield", "about", "waist", "wrist1", "wrist2",
   "wield", "hold", "dual_wield", "ears", "eyes", "missile_wield"
};

const char *const ris_flags[] = {
   "fire", "cold", "electricity", "energy", "blunt", "pierce", "slash", "acid",
   "poison", "drain", "sleep", "charm", "hold", "nonmagic", "plus1", "plus2",
   "plus3", "plus4", "plus5", "plus6", "magic", "paralysis", "r1", "r2", "r3",
   "r4", "r5", "r6", "r7", "r8", "r9", "r10"
};

const char *const trig_flags[] = {
   "up", "unlock", "lock", "d_north", "d_south", "d_east", "d_west", "d_up",
   "d_down", "door", "container", "open", "close", "passage", "oload", "mload",
   "teleport", "teleportall", "teleportplus", "death", "cast", "fakeblade",
   "rand4", "rand6", "trapdoor", "anotherroom", "usedial", "absolutevnum",
   "showroomdesc", "autoreturn", "r2", "r3"
};

const char *const part_flags[] = {
   "head", "arms", "legs", "heart", "brains", "guts", "hands", "feet", "fingers",
   "ear", "eye", "long_tongue", "eyestalks", "tentacles", "fins", "wings",
   "tail", "scales", "claws", "fangs", "horns", "tusks", "tailattack",
   "sharpscales", "beak", "haunches", "hooves", "paws", "forelegs", "feathers",
   "r30", "r31"
};

const char *const attack_flags[] = {
   "bite", "claws", "tail", "sting", "punch", "kick",
   "trip", "r7", "r8", "r9", "backstab", "r11", "r12", "r13", "r14", "r15", "r16", "r17",
   "r18", "r19", "r20", "r21", "r22", "r23", "r24", "r25", "r26", "r27", "r28", "r29",
   "r30", "r31"
};

const char *const defense_flags[] = {
   "parry", "dodge", "r2", "r3", "r4", "r5",
   "r6", "r7", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15", "r16", "r17",
   "r18", "disarm", "r20", "grip", "r22", "r23", "r24", "r25", "r26", "r27", "r28", "r29",
   "r30", "r31"
};

const char *const npc_position[] = {
   "dead", "mortal", "incapacitated", "stunned", "sleeping",
   "resting", "sitting", "berserk", "aggressive", "fighting", "defensive",
   "evasive", "standing", "mounted", "shove", "drag"
};

const char *const npc_sex[] = {
   "neuter", "male", "female"
};

const char *sector_name[SECT_MAX] = {
   "inside", "city", "field", "forest", "hills", "mountain", "water swim", "water noswim",
   "underwater", "air", "desert", "unknown", "ocean floor", "underground"
};

/*
 * Note: I put them all in one big set of flags since almost all of these
 * can be shared between mobs, objs and rooms for the exception of
 * bribe and hitprcnt, which will probably only be used on mobs.
 * ie: drop -- for an object, it would be triggered when that object is
 * dropped; -- for a room, it would be triggered when anything is dropped
 *          -- for a mob, it would be triggered when anything is dropped
 *
 * Something to consider: some of these triggers can be grouped together,
 * and differentiated by different arguments... for example:
 *  hour and time, rand and randiw, speech and speechiw
 * 
 */
const char *const mprog_flags[] = {
   "act", "speech", "rand", "fight", "death", "hitprcnt", "entry", "greet",
   "allgreet", "give", "bribe", "hour", "time", "wear", "remove", "sac",
   "look", "exa", "zap", "get", "drop", "damage", "repair", "randiw",
   "speechiw", "pull", "push", "sleep", "rest", "leave", "script", "use",
   "quest"
};


const char *flag_string( int bitvector, const char *const flagarray[] )
{
   static char buf[MAX_STRING_LENGTH];
   int x;

   buf[0] = '\0';
   for( x = 0; x < 32; x++ )
      if( IS_SET( bitvector, 1 << x ) )
      {
         strcat( buf, flagarray[x] );
         strcat( buf, " " );
      }
   if( ( x = strlen( buf ) ) > 0 )
      buf[--x] = '\0';

   return buf;
}

const char *ext_flag_string( EXT_BV * bitvector, const char *const flagarray[] )
{
   static char buf[MAX_STRING_LENGTH];
   int x;

   buf[0] = '\0';
   for( x = 0; x < MAX_BITS; ++x )
      if( xIS_SET( *bitvector, x ) )
      {
         mudstrlcat( buf, flagarray[x], MAX_STRING_LENGTH );
         mudstrlcat( buf, " ", MAX_STRING_LENGTH );
      }
   if( ( x = strlen( buf ) ) > 0 )
      buf[--x] = '\0';

   return buf;
}


bool can_rmodify( CHAR_DATA * ch, ROOM_INDEX_DATA * room )
{
   int vnum = room->vnum;
   AREA_DATA *pArea;

   if( IS_NPC( ch ) )
      return FALSE;
   if( get_trust( ch ) >= sysdata.level_modify_proto )
      return TRUE;
   if( !ch->pcdata || !( pArea = ch->pcdata->area ) )
   {
      send_to_char( "You must have an assigned area to modify this room.\r\n", ch );
      return FALSE;
   }
   if( vnum >= pArea->low_r_vnum && vnum <= pArea->hi_r_vnum )
      return TRUE;

   send_to_char( "That room is not in your allocated range.\r\n", ch );
   return FALSE;
}

bool can_omodify( CHAR_DATA * ch, OBJ_DATA * obj )
{
   int vnum = obj->pIndexData->vnum;
   AREA_DATA *pArea;

   if( IS_NPC( ch ) )
      return FALSE;
   if( get_trust( ch ) >= sysdata.level_modify_proto )
      return TRUE;
   if( !ch->pcdata || !( pArea = ch->pcdata->area ) )
   {
      send_to_char( "You must have an assigned area to modify this object.\r\n", ch );
      return FALSE;
   }
   if( vnum >= pArea->low_o_vnum && vnum <= pArea->hi_o_vnum )
      return TRUE;

   send_to_char( "That object is not in your allocated range.\r\n", ch );
   return FALSE;
}

bool can_oedit( CHAR_DATA * ch, OBJ_INDEX_DATA * obj )
{
   int vnum = obj->vnum;
   AREA_DATA *pArea;

   if( IS_NPC( ch ) )
      return FALSE;
   if( get_trust( ch ) >= LEVEL_GOD )
      return TRUE;
   if( !ch->pcdata || !( pArea = ch->pcdata->area ) )
   {
      send_to_char( "You must have an assigned area to modify this object.\r\n", ch );
      return FALSE;
   }
   if( vnum >= pArea->low_o_vnum && vnum <= pArea->hi_o_vnum )
      return TRUE;

   send_to_char( "That object is not in your allocated range.\r\n", ch );
   return FALSE;
}


bool can_mmodify( CHAR_DATA * ch, CHAR_DATA * mob )
{
   int vnum;
   AREA_DATA *pArea;

   if( mob == ch )
      return TRUE;

   if( !IS_NPC( mob ) )
   {
      if( get_trust( ch ) >= sysdata.level_modify_proto && get_trust( ch ) > get_trust( mob ) )
         return TRUE;
      else
         send_to_char( "You can't do that.\r\n", ch );
      return FALSE;
   }

   vnum = mob->pIndexData->vnum;

   if( IS_NPC( ch ) )
      return FALSE;
   if( get_trust( ch ) >= sysdata.level_modify_proto )
      return TRUE;
   if( !ch->pcdata || !( pArea = ch->pcdata->area ) )
   {
      send_to_char( "You must have an assigned area to modify this mobile.\r\n", ch );
      return FALSE;
   }
   if( vnum >= pArea->low_m_vnum && vnum <= pArea->hi_m_vnum )
      return TRUE;

   send_to_char( "That mobile is not in your allocated range.\r\n", ch );
   return FALSE;
}

bool can_medit( CHAR_DATA * ch, MOB_INDEX_DATA * mob )
{
   int vnum = mob->vnum;
   AREA_DATA *pArea;

   if( IS_NPC( ch ) )
      return FALSE;
   if( get_trust( ch ) >= LEVEL_GOD )
      return TRUE;
   if( !ch->pcdata || !( pArea = ch->pcdata->area ) )
   {
      send_to_char( "You must have an assigned area to modify this mobile.\r\n", ch );
      return FALSE;
   }
   if( vnum >= pArea->low_m_vnum && vnum <= pArea->hi_m_vnum )
      return TRUE;

   send_to_char( "That mobile is not in your allocated range.\r\n", ch );
   return FALSE;
}

int get_otype( const char *type )
{
  size_t x;

  for( x = 0; x < ( sizeof( o_types ) / sizeof( o_types[0] ) ); x++ )
    if( !str_cmp( type, o_types[x] ) )
      return x;
  return -1;
}

int get_aflag( const char *flag )
{
   int x;

   for( x = 0; x < MAX_AFF; x++ )
      if( !str_cmp( flag, a_flags[x] ) )
         return x;
   return -1;
}

int get_damtype( const char *type )
{
   int x;

   for( x = 0; x < MAX_DAMTYPE; x++ )
      if( !str_cmp( type, d_type[x] ) )
         return x;
   return -1;
}

int get_qualitytype( const char *type )
{
   int x;

   for( x = 0; x < MAX_QUALITYTYPE; x++ )
      if( !str_cmp( type, q_type[x] ) )
         return x;
   return -1;
}

int get_loottype( const char *type )
{
   int x;

   for( x = 0; x < MAX_LOOTTYPE; x++ )
      if( !str_cmp( type, l_type[x] ) )
         return x;
   return -1;
}

int get_trapflag( const char *flag )
{
   int x;

   for( x = 0; x < 32; x++ )
      if( !str_cmp( flag, trap_flags[x] ) )
         return x;
   return -1;
}

int get_quest_type( const char *type )
{
   int x;

   for( x = 0; x < MAX_QUESTTYPE; x++ )
      if( !str_cmp( type, quest_types[x] ) )
         return x;
   return -1;
}

int get_atype( const char *type )
{
   int x;
   for( x = 0; x < MAX_APPLY_TYPE; x++ )
      if( !str_cmp( type, a_types[x] ) )
         return x;
   return -1;
}

int get_npc_race( const char *type )
{
   int x;

   for( x = 0; x < MAX_NPC_RACE; x++ )
      if( !str_cmp( type, npc_race[x] ) )
         return x;
   return -1;
}

int get_wearloc( const char *type )
{
   int x;

   for( x = 0; x < MAX_WEAR; x++ )
      if( !str_cmp( type, wear_locs[x] ) )
         return x;
   return -1;
}

int get_exflag( const char *flag )
{
   int x;

   for( x = 0; x <= MAX_EXFLAG; x++ )
      if( !str_cmp( flag, ex_flags[x] ) )
         return x;
   return -1;
}

int get_rflag( const char *flag )
{
   int x;

   for( x = 0; x < 32; x++ )
      if( !str_cmp( flag, r_flags[x] ) )
         return x;
   return -1;
}

int get_mpflag( const char *flag )
{
   int x;

   for( x = 0; x < MAX_PROGTYPE; x++ )
      if( !str_cmp( flag, mprog_flags[x] ) )
         return x;
   return -1;
}

int get_oflag( const char *flag )
{
   int x;

   for( x = 0; x < 32; x++ )
      if( !str_cmp( flag, o_flags[x] ) )
         return x;
   return -1;
}

int get_areaflag( const char *flag )
{
   int x;

   for( x = 0; x < 32; x++ )
      if( !str_cmp( flag, area_flags[x] ) )
         return x;
   return -1;
}

int get_wflag( const char *flag )
{
   int x;

   for( x = 0; x < 32; x++ )
      if( !str_cmp( flag, w_flags[x] ) )
         return x;
   return -1;
}

int get_actflag( const char *flag )
{
   int x;

   for( x = 0; x < 32; x++ )
      if( !str_cmp( flag, act_flags[x] ) )
         return x;
   return -1;
}

int get_vip_flag( const char *flag )
{
   int x;

   for( x = 0; x < 32; x++ )
      if( !str_cmp( flag, planet_flags[x] ) )
         return x;
   return -1;
}

int get_wanted_flag( const char *flag )
{
   int x;

   for( x = 0; x < 32; x++ )
      if( !str_cmp( flag, planet_flags[x] ) )
         return x;
   return -1;
}

int get_pcflag( const char *flag )
{
   int x;

   for( x = 0; x < 32; x++ )
      if( !str_cmp( flag, pc_flags[x] ) )
         return x;
   return -1;
}

int get_plrflag( const char *flag )
{
   int x;

   for( x = 0; x < 32; x++ )
      if( !str_cmp( flag, plr_flags[x] ) )
         return x;
   return -1;
}

int get_risflag( const char *flag )
{
   int x;

   for( x = 0; x < 32; x++ )
      if( !str_cmp( flag, ris_flags[x] ) )
         return x;
   return -1;
}

int get_trigflag( const char *flag )
{
   int x;

   for( x = 0; x < 32; x++ )
      if( !str_cmp( flag, trig_flags[x] ) )
         return x;
   return -1;
}

int get_partflag( const char *flag )
{
   int x;

   for( x = 0; x < 32; x++ )
      if( !str_cmp( flag, part_flags[x] ) )
         return x;
   return -1;
}

int get_attackflag( const char *flag )
{
   int x;

   for( x = 0; x < 32; x++ )
      if( !str_cmp( flag, attack_flags[x] ) )
         return x;
   return -1;
}

int get_defenseflag( const char *flag )
{
   int x;

   for( x = 0; x < 32; x++ )
      if( !str_cmp( flag, defense_flags[x] ) )
         return x;
   return -1;
}

int get_langflag( const char *flag )
{
   int x;

   for( x = 0; lang_array[x] != LANG_UNKNOWN; x++ )
      if( !str_cmp( flag, lang_names[x] ) )
         return lang_array[x];
   return LANG_UNKNOWN;
}

int get_langnum( const char *flag )
{
   unsigned int x;

   for( x = 0; lang_array[x] != LANG_UNKNOWN; x++ )
      if( !str_cmp( flag, lang_names[x] ) )
         return x;
   return -1;
}

int get_langnum_save( const char *flag )
{
   unsigned int x;

   for( x = 0; lang_array[x] != LANG_UNKNOWN; x++ )
      if( !str_cmp( flag, lang_names_save[x] ) )
         return x;
   return -1;
}
int get_secflag( const char *flag )
{
   unsigned int x;

   for( x = 0; x < ( sizeof( sector_name ) / sizeof( sector_name[0] ) ); x++ )
      if( !str_cmp( flag, sector_name[x] ) )
         return x;
   return -1;
}

int get_npc_position( const char *position )
{
   size_t x;

   for( x = 0; x <= POS_DRAG; x++ )
      if( !str_cmp( position, npc_position[x] ) )
         return x;
   return -1;
}

int get_npc_sex( const char *sex )
{
   size_t x;

   for( x = 0; x <= SEX_FEMALE; x++ )
      if( !str_cmp( sex, npc_sex[x] ) )
         return x;
   return -1;
}

int get_frame_of_mind( const char *fom )
{
   int x;

   for( x = 0; x < MAX_FOM; x++ )
      if( !str_cmp( fom, frames_of_mind[x] ) )
         return x;
   return -1;
}


/*
 * Remove carriage returns from a line
 */
char *strip_cr( const char *str )
{
  static char newstr[MAX_STRING_LENGTH];
  int i, j;

  if( !str || str[0] == '\0' )
    {
      newstr[0] = '\0';
      return newstr;
    }

  for( i = j = 0; str[i] != '\0'; i++ )
    if( str[i] != '\r' )
      {
	newstr[j++] = str[i];
      }

  newstr[j] = '\0';
  return newstr;
}

/*
 * Removes the tildes from a line, except if it's the last character.
 */
void smush_tilde( char *str )
{
   int len;
   char last;
   char *strptr;

   strptr = str;

   len = strlen( str );
   if( len )
      last = strptr[len - 1];
   else
      last = '\0';

   for( ; *str != '\0'; str++ )
   {
      if( *str == '~' )
         *str = '-';
   }
   if( len )
      strptr[len - 1] = last;

   return;
}


void do_goto( CHAR_DATA * ch, const char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   ROOM_INDEX_DATA *location;
   CHAR_DATA *fch;
   CHAR_DATA *fch_next;
   ROOM_INDEX_DATA *in_room;
   AREA_DATA *pArea;
   int vnum;

   one_argument( argument, arg );
   if( arg[0] == '\0' )
   {
      send_to_char( "Goto where?\r\n", ch );
      return;
   }

   if( ( location = find_location( ch, arg ) ) == NULL )
   {
      vnum = atoi( arg );
      if( vnum < 0 || get_room_index( vnum ) )
      {
         send_to_char( "You cannot find that...\r\n", ch );
         return;
      }

      if( vnum < 1 || IS_NPC( ch ) || !ch->pcdata->area )
      {
         send_to_char( "No such location.\r\n", ch );
         return;
      }
      if( get_trust( ch ) < sysdata.level_modify_proto )
      {
         if( !ch->pcdata || !( pArea = ch->pcdata->area ) )
         {
            send_to_char( "You must have an assigned area to create rooms.\r\n", ch );
            return;
         }
         if( vnum < pArea->low_r_vnum || vnum > pArea->hi_r_vnum )
         {
            send_to_char( "That room is not within your assigned range.\r\n", ch );
            return;
         }
      }
      if( !is_valid_vnum( vnum, VCHECK_ROOM ) )
      {
         ch_printf( ch, "Sorry, %d IS NOT a valid vnum!\r\n", vnum );
         return;
      }

      location = make_room( vnum, ch->pcdata->area );
      if( !location )
      {
         bug( "Goto: make_room failed", 0 );
         return;
      }
      location->area = ch->pcdata->area;
      set_char_color( AT_WHITE, ch );
      send_to_char( "Waving your hand, you form order from swirling chaos,\r\nand step into a new reality...\r\n", ch );
   }

   if( room_is_private( ch, location ) )
   {
      if( get_trust( ch ) < sysdata.level_override_private )
      {
         send_to_char( "That room is private right now.\r\n", ch );
         return;
      }
      else
         send_to_char( "Overriding private flag!\r\n", ch );
   }

   if( get_trust( ch ) < LEVEL_IMMORTAL )
   {
      vnum = atoi( arg );

      if( !ch->pcdata || !( pArea = ch->pcdata->area ) )
      {
         send_to_char( "You must have an assigned area to goto.\r\n", ch );
         return;
      }

      if( vnum < pArea->low_r_vnum || vnum > pArea->hi_r_vnum )
      {
         send_to_char( "That room is not within your assigned range.\r\n", ch );
         return;
      }

      if( ( ch->in_room->vnum < pArea->low_r_vnum
            || ch->in_room->vnum > pArea->hi_r_vnum ) && !IS_SET( ch->in_room->room_flags, ROOM_HOTEL ) )
      {
         send_to_char( "Builders can only use goto from a hotel or in their zone.\r\n", ch );
         return;
      }

   }

   in_room = ch->in_room;
   if( ch->fighting )
      stop_fighting( ch, TRUE );

   if( !IS_SET( ch->act, PLR_WIZINVIS ) )
   {
      if( ch->pcdata && ch->pcdata->bamfout[0] != '\0' )
         act( AT_IMMORT, "$T", ch, NULL, ch->pcdata->bamfout, TO_ROOM );
      else
         act( AT_IMMORT, "$n $T", ch, NULL, "leaves in a swirl of the force.", TO_ROOM );
   }


   ch->regoto = ch->in_room->vnum;
   char_from_room( ch );
   if( ch->mount )
   {
      char_from_room( ch->mount );
      char_to_room( ch->mount, location );
   }
   char_to_room( ch, location );

   if( !IS_SET( ch->act, PLR_WIZINVIS ) )
   {
      if( ch->pcdata && ch->pcdata->bamfin[0] != '\0' )
         act( AT_IMMORT, "$T", ch, NULL, ch->pcdata->bamfin, TO_ROOM );
      else
         act( AT_IMMORT, "$n $T", ch, NULL, "enters in a swirl of the Force.", TO_ROOM );
   }

   do_look( ch, "auto" );

   if( ch->in_room == in_room )
      return;
   for( fch = in_room->first_person; fch; fch = fch_next )
   {
      fch_next = fch->next_in_room;
      if( fch->master == ch && IS_IMMORTAL( fch ) )
      {
         act( AT_ACTION, "You follow $N.", fch, NULL, ch, TO_CHAR );
         do_goto( fch, argument );
      }
   }
   return;
}

void do_mset( CHAR_DATA * ch, const char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char arg3[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   char outbuf[MAX_STRING_LENGTH];
   int num, size, plus;
   char char1, char2;
   CHAR_DATA *victim;
   int value;
   int minattr, maxattr;
   bool lockvictim;
   const char *origarg = argument;

   if( IS_NPC( ch ) )
   {
      send_to_char( "Mob's can't mset\r\n", ch );
      return;
   }

   if( !ch->desc )
   {
      send_to_char( "You have no descriptor\r\n", ch );
      return;
   }

   switch ( ch->substate )
   {
      default:
         break;
      case SUB_MOB_DESC:
         if( !ch->dest_buf )
         {
            send_to_char( "Fatal error: report to Thoric.\r\n", ch );
            bug( "do_mset: sub_mob_desc: NULL ch->dest_buf", 0 );
            ch->substate = SUB_NONE;
            return;
         }
         victim = ( CHAR_DATA* ) ch->dest_buf;
         if( char_died( victim ) )
         {
            send_to_char( "Your victim died!\r\n", ch );
            stop_editing( ch );
            return;
         }
         STRFREE( victim->description );
         victim->description = copy_buffer( ch );
         if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         {
            STRFREE( victim->pIndexData->description );
            victim->pIndexData->description = QUICKLINK( victim->description );
         }
         stop_editing( ch );
         ch->substate = ch->tempnum;
         return;
   }

   victim = NULL;
   lockvictim = FALSE;
   argument = smash_tilde_static( argument );

   if( ch->substate == SUB_REPEATCMD )
   {
     victim = ( CHAR_DATA* ) ch->dest_buf;
      if( !victim )
      {
         send_to_char( "Your victim died!\r\n", ch );
         argument = "done";
      }
      if( argument[0] == '\0' || !str_cmp( argument, " " ) || !str_cmp( argument, "stat" ) )
      {
         if( victim )
            do_mstat( ch, victim->name );
         else
            send_to_char( "No victim selected.  Type '?' for help.\r\n", ch );
         return;
      }
      if( !str_cmp( argument, "done" ) || !str_cmp( argument, "off" ) )
      {
         if( ch->dest_buf )
            RelDestroy( relMSET_ON, ch, ch->dest_buf );
         send_to_char( "Mset mode off.\r\n", ch );
         ch->substate = SUB_NONE;
         ch->dest_buf = NULL;
         if( ch->pcdata && ch->pcdata->subprompt )
            STRFREE( ch->pcdata->subprompt );
         return;
      }
   }
   if( victim )
   {
      lockvictim = TRUE;
      strcpy( arg1, victim->name );
      argument = one_argument( argument, arg2 );
      strcpy( arg3, argument );
   }
   else
   {
      lockvictim = FALSE;
      argument = one_argument( argument, arg1 );
      argument = one_argument( argument, arg2 );
      strcpy( arg3, argument );
   }

   if( !str_cmp( arg1, "on" ) )
   {
      send_to_char( "Syntax: mset <victim|vnum> on.\r\n", ch );
      return;
   }

   if( arg1[0] == '\0' || ( arg2[0] == '\0' && ch->substate != SUB_REPEATCMD ) || !str_cmp( arg1, "?" ) )
   {
      if( ch->substate == SUB_REPEATCMD )
      {
         if( victim )
            send_to_char( "Syntax: <field>  <value>\r\n", ch );
         else
            send_to_char( "Syntax: <victim> <field>  <value>\r\n", ch );
      }
      else
         send_to_char( "Syntax: mset <victim> <field>  <value>\r\n", ch );
      send_to_char( "\r\n", ch );
      send_to_char( "Field being one of:\r\n", ch );
      send_to_char( "  str int wis dex con agi cha lck frc sex\r\n", ch );
      send_to_char( "  credits hp force move align race\r\n", ch );
      send_to_char( "  hitroll damroll armor affected level\r\n", ch );
      send_to_char( "  thirst drunk full blood flags\r\n", ch );
      send_to_char( "  pos defpos part (see BODYPARTS)\r\n", ch );
      send_to_char( "  sav1 sav2 sav4 sav4 sav5 (see SAVINGTHROWS)\r\n", ch );
      send_to_char( "  resistant immune susceptible (see RIS)\r\n", ch );
      send_to_char( "  penetration resistance damtype_potency\r\n", ch );
      send_to_char( "  attack defense numattacks addskill remskill\r\n", ch );
      send_to_char( "  discipline speaking speaks (see LANGUAGES)\r\n", ch );
      send_to_char( "  name short long description title spec spec2\r\n", ch );
      send_to_char( "  clan vip wanted addteach remteach addquest\r\n", ch );
      send_to_char( "  addthought remthought                    \r\n", ch );
      send_to_char( "\r\n", ch );
      send_to_char( "For editing index/prototype mobiles:\r\n", ch );
      send_to_char( "  hitnumdie hitsizedie hitplus (hit points)\r\n", ch );
      send_to_char( "  damnumdie damsizedie damplus (damage roll)\r\n", ch );
      send_to_char( "To toggle area flag: aloaded\r\n", ch );
      return;
   }

   if( !victim && get_trust( ch ) <= LEVEL_IMMORTAL )
   {
      if( ( victim = get_char_room( ch, arg1 ) ) == NULL )
      {
         send_to_char( "They aren't here.\r\n", ch );
         return;
      }
   }
   else if( !victim )
   {
      if( ( victim = get_char_world( ch, arg1 ) ) == NULL )
      {
         send_to_char( "No one like that in all the realms.\r\n", ch );
         return;
      }
   }

   if( get_trust( ch ) <= LEVEL_IMMORTAL && !IS_NPC( victim ) )
   {
      send_to_char( "You can't do that!\r\n", ch );
      ch->dest_buf = NULL;
      return;
   }
   if( get_trust( ch ) < get_trust( victim ) && !IS_NPC( victim ) )
   {
      send_to_char( "You can't do that!\r\n", ch );
      ch->dest_buf = NULL;
      return;
   }
   if( lockvictim )
      ch->dest_buf = victim;

   if( IS_NPC( victim ) )
   {
      minattr = -9999;
      maxattr = 9999;
   }
   else
   {
      minattr = -9999;
      maxattr = 9999;
   }

   if( !str_cmp( arg2, "on" ) )
   {
      CHECK_SUBRESTRICTED( ch );
      ch_printf( ch, "Mset mode on. (Editing %s).\r\n", victim->name );
      ch->substate = SUB_REPEATCMD;
      ch->dest_buf = victim;
      if( ch->pcdata )
      {
         if( ch->pcdata->subprompt )
            STRFREE( ch->pcdata->subprompt );
         if( IS_NPC( victim ) )
            sprintf( buf, "<&CMset &W#%d&w> %%i", victim->pIndexData->vnum );
         else
            sprintf( buf, "<&CMset &W%s&w> %%i", victim->name );
         ch->pcdata->subprompt = STRALLOC( buf );
      }
      RelCreate( relMSET_ON, ch, victim );
      return;
   }

   value = is_number( arg3 ) ? atoi( arg3 ) : -1;

   if( atoi( arg3 ) < -1 && value == -1 )
      value = atoi( arg3 );

   if( !str_cmp( arg2, "str" ) )
   {
      if( !can_mmodify( ch, victim ) )
         return;
      if( value < minattr || value > maxattr )
      {
         ch_printf( ch, "Strength range is %d to %d.\r\n", minattr, maxattr );
         return;
      }
      victim->perm_str = value;
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->perm_str = value;
      return;
   }

   if( !str_cmp( arg2, "int" ) )
   {
      if( !can_mmodify( ch, victim ) )
         return;
      if( value < minattr || value > maxattr )
      {
         ch_printf( ch, "Intelligence range is %d to %d.\r\n", minattr, maxattr );
         return;
      }
      victim->perm_int = value;
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->perm_int = value;
      return;
   }

   if( !str_cmp( arg2, "wis" ) )
   {
      if( !can_mmodify( ch, victim ) )
         return;
      if( value < minattr || value > maxattr )
      {
         ch_printf( ch, "Wisdom range is %d to %d.\r\n", minattr, maxattr );
         return;
      }
      victim->perm_wis = value;
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->perm_wis = value;
      return;
   }

   if( !str_cmp( arg2, "dex" ) )
   {
      if( !can_mmodify( ch, victim ) )
         return;
      if( value < minattr || value > maxattr )
      {
         ch_printf( ch, "Dexterity range is %d to %d.\r\n", minattr, maxattr );
         return;
      }
      victim->perm_dex = value;
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->perm_dex = value;
      return;
   }

   if( !str_cmp( arg2, "con" ) )
   {
      if( !can_mmodify( ch, victim ) )
         return;
      if( value < minattr || value > maxattr )
      {
         ch_printf( ch, "Constitution range is %d to %d.\r\n", minattr, maxattr );
         return;
      }
      victim->perm_con = value;
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->perm_con = value;
      return;
   }

   if( !str_cmp( arg2, "agi" ) )
   {
      if( !can_mmodify( ch, victim ) )
         return;
      if( value < minattr || value > maxattr )
      {
         ch_printf( ch, "Constitution range is %d to %d.\r\n", minattr, maxattr );
         return;
      }
      victim->perm_agi = value;
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->perm_agi = value;
      return;
   }

   if( !str_cmp( arg2, "cha" ) )
   {
      if( !can_mmodify( ch, victim ) )
         return;
      if( value < minattr || value > maxattr )
      {
         ch_printf( ch, "Charisma range is %d to %d.\r\n", minattr, maxattr );
         return;
      }
      victim->perm_cha = value;
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->perm_cha = value;
      return;
   }

   if( !str_cmp( arg2, "lck" ) )
   {
      if( !can_mmodify( ch, victim ) )
         return;
      if( value < minattr || value > maxattr )
      {
         ch_printf( ch, "Luck range is %d to %d.\r\n", minattr, maxattr );
         return;
      }
      victim->perm_lck = value;
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->perm_lck = value;
      return;
   }

   if( !str_cmp( arg2, "frc" ) )
   {
      if( !can_mmodify( ch, victim ) )
         return;
      if( value < 0 || value > 20 )
      {
         ch_printf( ch, "Frc range is %d to %d.\r\n", minattr, maxattr );
         return;
      }
      victim->perm_frc = value;
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->perm_frc = value;
      return;
   }

   if( !str_cmp( arg2, "addskill" ) )
   {
      int x;

      if( !can_mmodify( ch, victim ) )
         return;

      if( !IS_NPC( victim ) )
      {
         send_to_char( "Only on NPCs!\r\n", ch );
         return;
      }

      if( victim->pIndexData->npc_skills[MAX_NPC_SKILL-1] != -1 )
      {
         send_to_char( "Mob has too many skills, try removing some.\r\n", ch );
         return;
      }

      if( value == -1 )
         value = skill_lookup( arg3 );

      if( !IS_VALID_SN( value ) )
      {
         send_to_char( "Not a valid skill.\r\n", ch );
         return;
      }
      if( skill_table[value]->type != SKILL_SKILL && skill_table[value]->type != SKILL_SPELL )
      {
         send_to_char( "Not a valid skill.\r\n", ch );
         return;
      }
      for( x = 0; x < MAX_NPC_SKILL; x++ )
         if( victim->pIndexData->npc_skills[x] == -1 )
            break;
      victim->pIndexData->npc_skills[x] = value;
      send_to_char( "Skill set.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "remskill" ) )
   {
      if( !can_mmodify( ch, victim ) )
         return;

      if( !IS_NPC( victim ) )
      {
         send_to_char( "Only on NPCs!\r\n", ch );
         return;
      }
      if( value == -1 || value > ( MAX_NPC_SKILL - 1 ) )
      {
         send_to_char( "Out of range.\r\n", ch );
         return;
      }
      victim->pIndexData->npc_skills[value] = -1;
      sort_mob_skills( victim );
      send_to_char( "Skill Slot Reset.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "discipline" ) )
   {
      DISC_DATA *disc;

      if( !can_mmodify( ch, victim ) )
         return;

      if( IS_NPC( victim ) )
      {
         send_to_char( "Not on Mobiles.\r\n", ch );
         return;
      }

      if( !has_empty_discipline_slot( victim ) )
      {
         send_to_char( "They have too many disciplines.\r\n", ch );
         return;
      }

      if( ( disc = get_discipline( arg3 ) ) == NULL )
      {
         ch_printf( ch, "%s is not a discipline.\r\n", arg3 );
         return;
      }
      add_discipline( victim, disc );
      send_to_char( "Ok.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "remdiscipline" ) )
   {
      DISC_DATA *disc;

      if( !can_mmodify( ch, victim ) )
         return;

      if( IS_NPC( victim ) )
      {
         send_to_char( "Not on Mobiles.\r\n", ch );
         return;
      }

      if( ( disc = get_discipline( arg3 ) ) == NULL )
      {
         ch_printf( ch, "%s is not a discipline.\r\n", arg3 );
         return;
      }

      if( !player_has_discipline( victim, disc ) )
      {
         ch_printf( ch, "%s does not have %s learned.\r\n", victim->name, disc->name );
         return;
      }
      rem_discipline( victim, disc );
      send_to_char( "Ok.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "remloot" ) )
   {
      LOOT_DATA *loot;
      bool found = FALSE;
      int count = 0;

      if( !can_mmodify( ch, victim ) )
         return;

      if( value == 0 )
      {
         send_to_char( "Propse usage: mset <mob> remloot <slot>\r\n", ch );
         return;
      }

      for( loot = victim->pIndexData->first_loot; loot; loot = loot->next )
         if( ++count == value )
         {
            found = TRUE;
            break;
         }

      if( found )
      {
         UNLINK( loot, victim->pIndexData->first_loot, victim->pIndexData->last_loot, next, prev );
         DISPOSE( loot );
         send_to_char( "Ok.\r\n", ch );
         return;
      }
      send_to_char( "No loot at that slot to remove.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "addteach" ) )
   {
      DISC_DATA *disc;
      TEACH_DATA *teach;

      if( !can_mmodify( ch, victim ) )
         return;

      if( !IS_NPC( victim ) )
      {
         send_to_char( "Only on Mobs.\r\n", ch );
         return;
      }

      if( argument[0] == '\0' )
      {
         send_to_char( "Proper Usage: mset <mob> addteach 'discipline name' <credit cost>\r\n", ch );
         return;
      }

      argument = one_argument( argument, arg3 );
      if( ( disc = get_discipline( arg3 ) ) == NULL )
      {
         send_to_char( "No such discipline exists.\r\n", ch );
         return;
      }

      argument = one_argument( argument, arg3 );
      if( !is_number( arg3 ) )
      {
         send_to_char( "Enter a credit value for the teacher to charge.\r\n", ch );
         return;
      }

      CREATE( teach, TEACH_DATA, 1 );
      teach->disc_id = disc->id;
      teach->credits = atoi( arg3 );
      LINK( teach, victim->pIndexData->first_teach, victim->pIndexData->last_teach, next, prev );
      send_to_char( "Ok.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "remteach" ) )
   {
      TEACH_DATA *teach;
      int count = 0;

      if( !can_mmodify( ch, victim ) )
         return;

      if( !IS_NPC( victim ) )
      {
         send_to_char( "Only on Mobs.\r\n", ch );
         return;
      }

      if( argument[0] == '\0' )
      {
         send_to_char( "Proper Usage: mset <mob> remteach <index num>\r\n", ch );
         return;
      }

      if( value < 0 )
      {
         send_to_char( "There are no teach datas with that low an index.\r\n", ch );
         return;
      }

      for( teach = victim->pIndexData->first_teach; teach; teach = teach->next )
      {
         if( value == count++ )
         {
            UNLINK( teach, victim->pIndexData->first_teach, victim->pIndexData->last_teach, next, prev );
            DISPOSE( teach );
            send_to_char( "Ok.\r\n", ch );
            return;
         }
      }
      send_to_char(" There are no teach datas with that high an index.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "penetration" ) )
   {
      int value2;
      if( !can_mmodify( ch, victim ) )
         return;

      argument = one_argument( argument, arg3 );

      if( ( value = get_damtype( arg3 ) ) == -1 )
      {
         send_to_char( "&PProper Usage: mset <target> penetration <dam_type> <amount>\r\nNot a valid damage type.&w\r\n", ch );
         return;
      }
      if( !is_number( argument ) )
      {
         ch_printf( ch, "&PProper Usage: mset <target> penetration %s <amount>\r\nNot a valid amount.&w\r\n", d_type[value] );
         return;
      }
      value2 = atoi( argument );
      victim->penetration[value] = value2;
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->penetration[value] = value2;
      send_to_char( "Ok.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "resistance" ) )
   {
      int value2;
      if( !can_mmodify( ch, victim ) )
         return;

      argument = one_argument( argument, arg3 );

      if( ( value = get_damtype( arg3 ) ) == -1 ) 
      {
         send_to_char( "&PProper Usage: mset <target> resistance <dam_type> <amount>\r\nNot a valid damage type.&w\r\n", ch );
         return;
      }
      if( !is_number( argument ) )
      {
         ch_printf( ch, "&PProper Usage: mset <target> resistance %s <amount>\r\nNot a valid amount.&w\r\n", d_type[value] );
         return;
      }
      value2 = atoi( argument );
      victim->resistance[value] = value2;
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->resistance[value] = value2;
      send_to_char( "Ok.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "damtype_potency" ) )
   {
      int value2;
      if( !can_mmodify( ch, victim ) )
         return;

      argument = one_argument( argument, arg3 );

      if( ( value = get_damtype( arg3 ) ) == -1 ) 
      {
         send_to_char( "&PProper Usage: mset <target> damtype_potency <dam_type> <amount>\r\nNot a valid damage type.&w\r\n", ch );
         return;
      }
      if( !is_number( argument ) )
      {
         ch_printf( ch, "&PProper Usage: mset <target> damtype_potency %s <amount>\r\nNot a valid amount.&w\r\n", d_type[value] );
         return;
      }
      value2 = atoi( argument );
      victim->damtype_potency[value] = value2;
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->damtype_potency[value] = value2;
      send_to_char( "Ok.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "addthought" ) )
   {
      AI_THOUGHT *thought;

      if( !can_mmodify( ch, victim ) )
         return;

      if( !IS_NPC( victim ) )
      {
         send_to_char( "Not on Players.\r\n", ch );
         return;
      }

      if( ( thought = get_thought( arg3 ) ) == NULL )
      {
         send_to_char( "No such thought exists.\r\n", ch );
         return;
      }

      add_mob_thought( victim, thought );
      if( IS_SET( victim->act, ACT_PROTOTYPE ) )
         LINK( copy_thought( thought ), victim->pIndexData->first_thought, victim->pIndexData->last_thought, next, prev );
      send_to_char( "Ok.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "remthought" ) )
   {
      AI_THOUGHT *thought;
      int count;

      if( !can_mmodify( ch, victim ) )
         return;

      if( !IS_NPC( victim ) )
      {
         send_to_char( "Not on Players.\r\n", ch );
         return;
      }

      if( !IS_SET( victim->act, ACT_PROTOTYPE ) )
      {
         send_to_char( "Only on Prototype Mobs.\r\n", ch );
         return;
      }

      for( count = 0, thought = victim->pIndexData->first_thought; thought; thought = thought->next )
         if( ++count == value )
         {
            UNLINK( thought, victim->pIndexData->first_thought, victim->pIndexData->last_thought, next, prev );
            free_thought( thought );
         }

      send_to_char( "Removed.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "sav1" ) )
   {
      if( !can_mmodify( ch, victim ) )
         return;
      if( value < -30 || value > 30 )
      {
         send_to_char( "Saving throw range vs poison is -30 to 30.\r\n", ch );
         return;
      }
      victim->saving_poison_death = value;
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->saving_poison_death = value;
      return;
   }

   if( !str_cmp( arg2, "sav2" ) )
   {
      if( !can_mmodify( ch, victim ) )
         return;
      if( value < -30 || value > 30 )
      {
         send_to_char( "Saving throw range vs wands is -30 to 30.\r\n", ch );
         return;
      }
      victim->saving_wand = value;
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->saving_wand = value;
      return;
   }

   if( !str_cmp( arg2, "addquest" ) )
   {
      QUEST_DATA *quest;

      if( !can_mmodify( ch, victim ) )
         return;

      if( !IS_NPC( victim ) )
      {
         send_to_char( "Not on players.\r\n", ch );
         return;
      }

      if( ( quest = get_quest_from_id( value ) ) == NULL && ( quest = get_quest_from_name( arg3 ) ) == NULL )
      {
         send_to_char( "No such quest exists.\r\n", ch );
         return;
      }

      create_available_quest( ch, victim, quest );
      send_to_char( "Ok.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "sav3" ) )
   {
      if( !can_mmodify( ch, victim ) )
         return;
      if( value < -30 || value > 30 )
      {
         send_to_char( "Saving throw range vs para is -30 to 30.\r\n", ch );
         return;
      }
      victim->saving_para_petri = value;
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->saving_para_petri = value;
      return;
   }

   if( !str_cmp( arg2, "sav4" ) )
   {
      if( !can_mmodify( ch, victim ) )
         return;
      if( value < -30 || value > 30 )
      {
         send_to_char( "Saving throw range vs bad breath is -30 to 30.\r\n", ch );
         return;
      }
      victim->saving_breath = value;
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->saving_breath = value;
      return;
   }

   if( !str_cmp( arg2, "sav5" ) )
   {
      if( !can_mmodify( ch, victim ) )
         return;
      if( value < -30 || value > 30 )
      {
         send_to_char( "Saving throw range vs force powers is -30 to 30.\r\n", ch );
         return;
      }
      victim->saving_spell_staff = value;
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->saving_spell_staff = value;
      return;
   }

   if( !str_cmp( arg2, "sex" ) )
   {
      if( !can_mmodify( ch, victim ) )
         return;
      if( value < 0 || value > 2 )
      {
         send_to_char( "Sex range is 0 to 2.\r\n", ch );
         return;
      }
      victim->sex = value;
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->sex = value;
      return;
   }

   if( !str_cmp( arg2, "race" ) )
   {
      if( !can_mmodify( ch, victim ) )
         return;
      value = get_npc_race( arg3 );
      if( value < 0 )
         value = atoi( arg3 );
      if( !IS_NPC( victim ) && ( value < 0 || value >= MAX_RACE ) )
      {
         ch_printf( ch, "Race range is 0 to %d.\n", MAX_RACE - 1 );
         return;
      }
      if( IS_NPC( victim ) && ( value < 0 || value >= MAX_NPC_RACE ) )
      {
         ch_printf( ch, "Race range is 0 to %d.\n", MAX_NPC_RACE - 1 );
         return;
      }
      victim->race = value;
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->race = value;
      return;
   }

   if( !str_cmp( arg2, "evasion" ) )
   {
      if( !can_mmodify( ch, victim ) )
         return;
      victim->evasion = value;
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->evasion = value;
      return;
   }

   if( !str_cmp( arg2, "level" ) )
   {
      if( !can_mmodify( ch, victim ) )
         return;
      if( !IS_NPC( victim ) )
      {
         send_to_char( "Not on PC's.\r\n", ch );
         return;
      }

      victim->moblevel = value;
      victim->top_level = UMIN( value, 100 );
      if( IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->level = value;

      return;
   }

   if( !str_cmp( arg2, "numattacks" ) )
   {
      if( !can_mmodify( ch, victim ) )
         return;
      if( !IS_NPC( victim ) )
      {
         send_to_char( "Not on PC's.\r\n", ch );
         return;
      }

      if( value < 0 || value > 20 )
      {
         send_to_char( "Attacks range is 0 to 20.\r\n", ch );
         return;
      }
      victim->numattacks = value;
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->numattacks = value;
      return;
   }

   if( !str_cmp( arg2, "credits" ) )
   {
      if( !can_mmodify( ch, victim ) )
         return;
      victim->gold = value;
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->gold = value;
      return;
   }

   if( !str_cmp( arg2, "hitroll" ) )
   {
      if( !can_mmodify( ch, victim ) )
         return;
      victim->hitroll = URANGE( 0, value, 85 );
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->hitroll = victim->hitroll;
      return;
   }

   if( !str_cmp( arg2, "damroll" ) )
   {
      if( !can_mmodify( ch, victim ) )
         return;
      victim->damroll = URANGE( 0, value, 65 );
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->damroll = victim->damroll;
      return;
   }

   if( !str_cmp( arg2, "damtype" ) )
   {
      if( !can_mmodify( ch, victim ) )
         return;

      while( argument[0] != '\0' )
      {
         argument = one_argument( argument, arg2 );
         if( ( value = get_damtype( arg2 ) ) == -1 || ( value >= DAM_ALL && value <= DAM_PHYSICAL ) )
         {
            ch_printf( ch, "%s is an invalid damtype.\r\n", arg2 );
            continue;
         }
         xTOGGLE_BIT( victim->damtype, value );
         if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
            xTOGGLE_BIT( victim->pIndexData->damtype, value );
      }
      send_to_char( "Ok.\r\n", ch );
      return;
   }


   if( !str_cmp( arg2, "hp" ) )
   {
      if( !can_mmodify( ch, victim ) )
         return;
      if( value < 1 || value > 32700 )
      {
         send_to_char( "Hp range is 1 to 32,700 hit points.\r\n", ch );
         return;
      }
      victim->max_hit = value;
      return;
   }

   if( !str_cmp( arg2, "force" ) )
   {
      if( !can_mmodify( ch, victim ) )
         return;
      if( value < 0 || value > 30000 )
      {
         send_to_char( "Force range is 0 to 30,000 force points.\r\n", ch );
         return;
      }
      victim->max_mana = value;
      return;
   }

   if( !str_cmp( arg2, "move" ) )
   {
      if( !can_mmodify( ch, victim ) )
         return;
      if( value < 0 || value > 30000 )
      {
         send_to_char( "Move range is 0 to 30,000 move points.\r\n", ch );
         return;
      }
      victim->max_move = value;
      return;
   }

   if( !str_cmp( arg2, "align" ) )
   {
      if( !can_mmodify( ch, victim ) )
         return;
      if( value < -1000 || value > 1000 )
      {
         send_to_char( "Alignment range is -1000 to 1000.\r\n", ch );
         return;
      }
      victim->alignment = value;
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->alignment = value;
      return;
   }

   if( !str_cmp( arg2, "password" ) )
   {
      char *pwdnew;

      if( get_trust( ch ) < LEVEL_SUB_IMPLEM )
      {
         send_to_char( "You can't do that.\r\n", ch );
         return;
      }
      if( IS_NPC( victim ) )
      {
         send_to_char( "Mobs don't have passwords.\r\n", ch );
         return;
      }

      if( strlen( arg3 ) < 5 )
      {
         send_to_char( "New password must be at least five characters long.\r\n", ch );
         return;
      }

      if( arg3[0] == '!' )
      {
         send_to_char( "New password cannot begin with the '!' character.", ch );
         return;
      }

      pwdnew = sha256_crypt( arg3 );

      DISPOSE( victim->pcdata->pwd );
      victim->pcdata->pwd = str_dup( pwdnew );
      if( IS_SET( sysdata.save_flags, SV_PASSCHG ) )
         save_char_obj( victim );
      send_to_char( "Ok.\r\n", ch );
      ch_printf( victim, "Your password has been changed by %s.\r\n", ch->name );
      return;
   }

   if( !str_cmp( arg2, "quest" ) )
   {
      if( IS_NPC( victim ) )
      {
         send_to_char( "Not on NPC's.\r\n", ch );
         return;
      }

      if( value < 0 || value > 500 )
      {
         send_to_char( "The current quest range is 0 to 500.\r\n", ch );
         return;
      }

      victim->pcdata->quest_number = value;
      return;
   }

   if( !str_cmp( arg2, "qpa" ) )
   {
      if( IS_NPC( victim ) )
      {
         send_to_char( "Not on NPC's.\r\n", ch );
         return;
      }

      victim->pcdata->quest_accum = value;
      return;
   }

   if( !str_cmp( arg2, "qp" ) )
   {
      if( IS_NPC( victim ) )
      {
         send_to_char( "Not on NPC's.\r\n", ch );
         return;
      }

      if( value < 0 || value > 5000 )
      {
         send_to_char( "The current quest point range is 0 to 5000.\r\n", ch );
         return;
      }

      victim->pcdata->quest_curr = value;
      return;
   }

   if( !str_cmp( arg2, "mentalstate" ) )
   {
      if( value < -100 || value > 100 )
      {
         send_to_char( "Value must be in range -100 to +100.\r\n", ch );
         return;
      }
      victim->mental_state = value;
      return;
   }

   if( !str_cmp( arg2, "emotion" ) )
   {
      if( value < -100 || value > 100 )
      {
         send_to_char( "Value must be in range -100 to +100.\r\n", ch );
         return;
      }
      victim->emotional_state = value;
      return;
   }

   if( !str_cmp( arg2, "thirst" ) )
   {
      if( IS_NPC( victim ) )
      {
         send_to_char( "Not on NPC's.\r\n", ch );
         return;
      }

      if( value < 0 || value > 100 )
      {
         send_to_char( "Thirst range is 0 to 100.\r\n", ch );
         return;
      }

      victim->pcdata->condition[COND_THIRST] = value;
      return;
   }


   if( !str_cmp( arg2, "drunk" ) )
   {
      if( IS_NPC( victim ) )
      {
         send_to_char( "Not on NPC's.\r\n", ch );
         return;
      }

      if( value < 0 || value > 100 )
      {
         send_to_char( "Drunk range is 0 to 100.\r\n", ch );
         return;
      }

      victim->pcdata->condition[COND_DRUNK] = value;
      return;
   }

   if( !str_cmp( arg2, "full" ) )
   {
      if( IS_NPC( victim ) )
      {
         send_to_char( "Not on NPC's.\r\n", ch );
         return;
      }

      if( value < 0 || value > 100 )
      {
         send_to_char( "Full range is 0 to 100.\r\n", ch );
         return;
      }

      victim->pcdata->condition[COND_FULL] = value;
      return;
   }

   if( !str_cmp( arg2, "blood" ) )
   {
      if( IS_NPC( victim ) )
      {
         send_to_char( "Not on NPC's.\r\n", ch );
         return;
      }

      if( value < 0 || value > MAX_LEVEL + 10 )
      {
         ch_printf( ch, "Blood range is 0 to %d.\r\n", MAX_LEVEL + 10 );
         return;
      }

      victim->pcdata->condition[COND_BLOODTHIRST] = value;
      return;
   }

   if( !str_cmp( arg2, "name" ) )
   {
      if( !can_mmodify( ch, victim ) )
         return;
      if( !IS_NPC( victim ) )
      {
         send_to_char( "Not on PC's.\r\n", ch );
         return;
      }

      if( arg3[0] == '\0' )
      {
         send_to_char( "Names can not be set to an empty string.\r\n", ch );
         return;
      }

      STRFREE( victim->name );
      victim->name = STRALLOC( arg3 );
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
      {
         STRFREE( victim->pIndexData->player_name );
         victim->pIndexData->player_name = QUICKLINK( victim->name );
      }
      return;
   }

   if( !str_cmp( arg2, "minsnoop" ) )
   {
      if( get_trust( ch ) < LEVEL_SUB_IMPLEM )
      {
         send_to_char( "You can't do that.\r\n", ch );
         return;
      }
      if( IS_NPC( victim ) )
      {
         send_to_char( "Not on NPC's.\r\n", ch );
         return;
      }
      if( victim->pcdata )
      {
         victim->pcdata->min_snoop = value;
         return;
      }
   }

   if( !str_cmp( arg2, "clan" ) )
   {
      CLAN_DATA *clan;

      if( get_trust( ch ) < LEVEL_GREATER )
      {
         send_to_char( "You can't do that.\r\n", ch );
         return;
      }
      if( IS_NPC( victim ) )
      {
         send_to_char( "Not on NPC's.\r\n", ch );
         return;
      }

      if( arg3[0] == '\0' )
      {
         /*
          * Crash bug fix, oops guess I should have caught this one :)
          * * But it was early in the morning :P --Shaddai 
          */
         if( victim->pcdata->clan == NULL )
            return;
         /*
          * Added a check on immortals so immortals don't take up
          * * any membership space. --Shaddai
          */
	   if( !IS_IMMORTAL( victim ) ) 
         {
            --victim->pcdata->clan->members;
            if( victim->pcdata->clan->members < 0 )
               victim->pcdata->clan->members = 0;
            save_clan( victim->pcdata->clan );
         }
         STRFREE( victim->pcdata->clan_name );
         victim->pcdata->clan_name = STRALLOC( "" );
         victim->pcdata->clan = NULL;
         return;
      }
      clan = get_clan( arg3 );
      if( !clan )
      {
         send_to_char( "No such clan.\r\n", ch );
         return;
      }
      if( victim->pcdata->clan != NULL && !IS_IMMORTAL( victim ) )
      {
         --victim->pcdata->clan->members;
         if( victim->pcdata->clan->members < 0 )
            victim->pcdata->clan->members = 0;
         save_clan( victim->pcdata->clan );
      }
      STRFREE( victim->pcdata->clan_name );
      victim->pcdata->clan_name = QUICKLINK( clan->name );
      victim->pcdata->clan = clan;
      if( !IS_IMMORTAL( victim ) )
      {
         ++victim->pcdata->clan->members;
         save_clan( victim->pcdata->clan );
      }
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "short" ) )
   {
      STRFREE( victim->short_descr );
      victim->short_descr = STRALLOC( arg3 );
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
      {
         STRFREE( victim->pIndexData->short_descr );
         victim->pIndexData->short_descr = QUICKLINK( victim->short_descr );
      }
      return;
   }

   if( !str_cmp( arg2, "long" ) )
   {
      STRFREE( victim->long_descr );
      strcpy( buf, arg3 );
      strcat( buf, "\r\n" );
      victim->long_descr = STRALLOC( buf );
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
      {
         STRFREE( victim->pIndexData->long_descr );
         victim->pIndexData->long_descr = QUICKLINK( victim->long_descr );
      }
      return;
   }

   if( !str_cmp( arg2, "description" ) )
   {
      if( arg3[0] )
      {
         STRFREE( victim->description );
         victim->description = STRALLOC( arg3 );
         if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         {
            STRFREE( victim->pIndexData->description );
            victim->pIndexData->description = QUICKLINK( victim->description );
         }
         return;
      }
      CHECK_SUBRESTRICTED( ch );
      if( ch->substate == SUB_REPEATCMD )
         ch->tempnum = SUB_REPEATCMD;
      else
         ch->tempnum = SUB_NONE;
      ch->substate = SUB_MOB_DESC;
      ch->dest_buf = victim;
      start_editing( ch, (char *)victim->description );
      return;
   }

   if( !str_cmp( arg2, "title" ) )
   {
      if( IS_NPC( victim ) )
      {
         send_to_char( "Not on NPC's.\r\n", ch );
         return;
      }

      set_title( victim, arg3 );
      return;
   }

   if ( !str_cmp( arg2, "spec" ) || !str_cmp( arg2, "spec_fun" ) )
   {
	SPEC_FUN *specfun;

	if( !can_mmodify( ch, victim ) )
	   return;

	if( !IS_NPC(victim) )
	{
	   send_to_char( "Not on PC's.\r\n", ch );
	   return;
	}

      if( !str_cmp( arg3, "none" ) )
      {
         victim->spec_fun = NULL;
	   STRFREE( victim->spec_funname );
	   send_to_char( "Special function removed.\r\n", ch );
	   if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
	   {
	      victim->pIndexData->spec_fun = NULL;
		STRFREE( victim->pIndexData->spec_funname );
	   }
	   return;
      }

	if( ( specfun = spec_lookup( arg3 ) ) == NULL )
	{
	   send_to_char( "No such function.\r\n", ch );
	   return;
	}

	if( !validate_spec_fun( arg3 ) )
	{
	   ch_printf( ch, "%s is not a valid spec_fun for mobiles.\r\n", arg3 );
	   return;
	}

	victim->spec_fun = specfun;
	STRFREE( victim->spec_funname );
	victim->spec_funname = STRALLOC( arg3 );
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
	{
         victim->pIndexData->spec_fun = victim->spec_fun;
	   STRFREE( victim->pIndexData->spec_funname );
	   victim->pIndexData->spec_funname = STRALLOC( arg3 );
	}
	send_to_char( "Victim special function set.\r\n", ch );
	return;
   }

   if ( !str_cmp( arg2, "spec2" ) || !str_cmp( arg2, "spec_fun2" ) )
   {
	SPEC_FUN *specfun;

	if( !can_mmodify( ch, victim ) )
	   return;

	if( !IS_NPC(victim) )
	{
	   send_to_char( "Not on PC's.\r\n", ch );
	   return;
	}

      if( !str_cmp( arg3, "none" ) )
      {
         victim->spec_2 = NULL;
	   STRFREE( victim->spec_funname2 );
	   send_to_char( "Special function 2 removed.\r\n", ch );
	   if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
	   {
	      victim->pIndexData->spec_2 = NULL;
		STRFREE( victim->pIndexData->spec_funname2 );
	   }
	   return;
      }

	if( ( specfun = spec_lookup( arg3 ) ) == NULL )
	{
	   send_to_char( "No such function.\r\n", ch );
	   return;
	}

	if( !validate_spec_fun( arg3 ) )
	{
	   ch_printf( ch, "%s is not a valid spec_fun for mobiles.\r\n", arg3 );
	   return;
	}

	victim->spec_2 = specfun;
	STRFREE( victim->spec_funname2 );
	victim->spec_funname2 = STRALLOC( arg3 );
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
	{
         victim->pIndexData->spec_2 = victim->spec_2;
	   STRFREE( victim->pIndexData->spec_funname2 );
	   victim->pIndexData->spec_funname2 = STRALLOC( arg3 );
	}
	send_to_char( "Victim special function set.\r\n", ch );
	return;
   }

   if( !str_cmp( arg2, "flags" ) )
   {
      bool protoflag = FALSE, ftoggle = FALSE;

      if( !IS_NPC( victim ) && get_trust( ch ) < LEVEL_GREATER )
      {
         send_to_char( "You can only modify a mobile's flags.\r\n", ch );
         return;
      }

      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "Usage: mset <vic> flags <flag> [flag]...\r\n", ch );
         return;
      }

      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         protoflag = TRUE;

      while( argument[0] != '\0' )
      {
         argument = one_argument( argument, arg3 );

         if( IS_NPC( victim ) )
         {
            value = get_actflag( arg3 );

            if( value < 0 || value >= 31 )
               ch_printf( ch, "Unknown flag: %s\r\n", arg3 );
            else if( 1 << value == ACT_PROTOTYPE && ch->top_level < sysdata.level_modify_proto )
               send_to_char( "You cannot change the prototype flag.\r\n", ch );
            else if( 1 << value == ACT_IS_NPC )
               send_to_char( "If the NPC flag could be changed, it would cause many problems.\r\n", ch );
            else
            {
               ftoggle = TRUE;
               TOGGLE_BIT( victim->act, 1 << value );
            }
         }
         else
         {
            value = get_plrflag( arg3 );

            if( value < 0 || value >= 31 )
               ch_printf( ch, "Unknown flag: %s\r\n", arg3 );
            else if( 1 << value == ACT_IS_NPC )
               send_to_char( "If the NPC flag could be changed, it would cause many problems.\r\n", ch );
            else
            {
               ftoggle = TRUE;
               TOGGLE_BIT( victim->act, 1 << value );
            }
         }
      }
      if( ftoggle )
         send_to_char( "Flags set.\r\n", ch );
      if( IS_NPC( victim ) && ( IS_SET( victim->act, ACT_PROTOTYPE ) || ( 1 << value == ACT_PROTOTYPE && protoflag ) ) )
         victim->pIndexData->act = victim->act;
      return;
   }

   if( !str_cmp( arg2, "wanted" ) )
   {
      if( IS_NPC( victim ) )
      {
         send_to_char( "Wanted flags are for players only.\r\n", ch );
         return;
      }

      if( get_trust( ch ) < LEVEL_GREATER )
      {
         send_to_char( "You are not a high enough level to do that.\r\n", ch );
         return;
      }

      if( !can_mmodify( ch, victim ) )
         return;
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "Usage: mset <victim> wanted <planet> [planet]...\r\n", ch );
         return;
      }

      while( argument[0] != '\0' )
      {
         argument = one_argument( argument, arg3 );
         value = get_wanted_flag( arg3 );
         if( value < 0 || value > 31 )
            ch_printf( ch, "Unknown flag: %s\r\n", arg3 );
         else
            TOGGLE_BIT( victim->pcdata->wanted_flags, 1 << value );
      }
      return;
   }

   if( !str_cmp( arg2, "vip" ) )
   {
      if( !IS_NPC( victim ) )
      {
         send_to_char( "VIP flags are for mobs only.\r\n", ch );
         return;
      }

      if( !can_mmodify( ch, victim ) )
         return;

      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "Usage: mset <victim> vip <planet> [planet]...\r\n", ch );
         return;
      }

      while( argument[0] != '\0' )
      {
         argument = one_argument( argument, arg3 );
         value = get_vip_flag( arg3 );
         if( value < 0 || value > 31 )
            ch_printf( ch, "Unknown flag: %s\r\n", arg3 );
         else
            TOGGLE_BIT( victim->vip_flags, 1 << value );
      }
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->vip_flags = victim->vip_flags;
      return;
   }


   if( !str_cmp( arg2, "affected" ) )
   {
      if( !IS_NPC( victim ) && get_trust( ch ) < LEVEL_LESSER )
      {
         send_to_char( "You can only modify a mobile's flags.\r\n", ch );
         return;
      }

      if( !can_mmodify( ch, victim ) )
         return;
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "Usage: mset <victim> affected <flag> [flag]...\r\n", ch );
         return;
      }
      while( argument[0] != '\0' )
      {
         argument = one_argument( argument, arg3 );
         value = get_aflag( arg3 );
         if( value < 0 || value > MAX_BITS )
            ch_printf( ch, "Unknown flag: %s\r\n", arg3 );
         else
            xTOGGLE_BIT( victim->affected_by, value );
      }
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->affected_by = victim->affected_by;
      return;
   }

   /*
    * save some more finger-leather for setting RIS stuff
    */
   if( !str_cmp( arg2, "r" ) )
   {
      if( !IS_NPC( victim ) && get_trust( ch ) < LEVEL_LESSER )
      {
         send_to_char( "You can only modify a mobile's ris.\r\n", ch );
         return;
      }
      if( !can_mmodify( ch, victim ) )
         return;

      sprintf( outbuf, "%s resistant %s", arg1, arg3 );
      do_mset( ch, outbuf );
      return;
   }
   if( !str_cmp( arg2, "i" ) )
   {
      if( !IS_NPC( victim ) && get_trust( ch ) < LEVEL_LESSER )
      {
         send_to_char( "You can only modify a mobile's ris.\r\n", ch );
         return;
      }
      if( !can_mmodify( ch, victim ) )
         return;


      sprintf( outbuf, "%s immune %s", arg1, arg3 );
      do_mset( ch, outbuf );
      return;
   }
   if( !str_cmp( arg2, "s" ) )
   {
      if( !IS_NPC( victim ) && get_trust( ch ) < LEVEL_LESSER )
      {
         send_to_char( "You can only modify a mobile's ris.\r\n", ch );
         return;
      }
      if( !can_mmodify( ch, victim ) )
         return;

      sprintf( outbuf, "%s susceptible %s", arg1, arg3 );
      do_mset( ch, outbuf );
      return;
   }
   if( !str_cmp( arg2, "ri" ) )
   {
      if( !IS_NPC( victim ) && get_trust( ch ) < LEVEL_LESSER )
      {
         send_to_char( "You can only modify a mobile's ris.\r\n", ch );
         return;
      }
      if( !can_mmodify( ch, victim ) )
         return;

      sprintf( outbuf, "%s resistant %s", arg1, arg3 );
      do_mset( ch, outbuf );
      sprintf( outbuf, "%s immune %s", arg1, arg3 );
      do_mset( ch, outbuf );
      return;
   }

   if( !str_cmp( arg2, "rs" ) )
   {
      if( !IS_NPC( victim ) && get_trust( ch ) < LEVEL_LESSER )
      {
         send_to_char( "You can only modify a mobile's ris.\r\n", ch );
         return;
      }
      if( !can_mmodify( ch, victim ) )
         return;

      sprintf( outbuf, "%s resistant %s", arg1, arg3 );
      do_mset( ch, outbuf );
      sprintf( outbuf, "%s susceptible %s", arg1, arg3 );
      do_mset( ch, outbuf );
      return;
   }
   if( !str_cmp( arg2, "is" ) )
   {
      if( !IS_NPC( victim ) && get_trust( ch ) < LEVEL_LESSER )
      {
         send_to_char( "You can only modify a mobile's ris.\r\n", ch );
         return;
      }
      if( !can_mmodify( ch, victim ) )
         return;

      sprintf( outbuf, "%s immune %s", arg1, arg3 );
      do_mset( ch, outbuf );
      sprintf( outbuf, "%s susceptible %s", arg1, arg3 );
      do_mset( ch, outbuf );
      return;
   }
   if( !str_cmp( arg2, "ris" ) )
   {
      if( !IS_NPC( victim ) && get_trust( ch ) < LEVEL_LESSER )
      {
         send_to_char( "You can only modify a mobile's ris.\r\n", ch );
         return;
      }
      if( !can_mmodify( ch, victim ) )
         return;

      sprintf( outbuf, "%s resistant %s", arg1, arg3 );
      do_mset( ch, outbuf );
      sprintf( outbuf, "%s immune %s", arg1, arg3 );
      do_mset( ch, outbuf );
      sprintf( outbuf, "%s susceptible %s", arg1, arg3 );
      do_mset( ch, outbuf );
      return;
   }

   if( !str_cmp( arg2, "resistant" ) )
   {
      if( !IS_NPC( victim ) && get_trust( ch ) < LEVEL_LESSER )
      {
         send_to_char( "You can only modify a mobile's resistancies.\r\n", ch );
         return;
      }
      if( !can_mmodify( ch, victim ) )
         return;
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "Usage: mset <victim> resistant <flag> [flag]...\r\n", ch );
         return;
      }
      while( argument[0] != '\0' )
      {
         argument = one_argument( argument, arg3 );
         value = get_risflag( arg3 );
         if( value < 0 || value > 31 )
            ch_printf( ch, "Unknown flag: %s\r\n", arg3 );
         else
            TOGGLE_BIT( victim->resistant, 1 << value );
      }
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->resistant = victim->resistant;
      return;
   }

   if( !str_cmp( arg2, "immune" ) )
   {
      if( !IS_NPC( victim ) && get_trust( ch ) < LEVEL_LESSER )
      {
         send_to_char( "You can only modify a mobile's immunities.\r\n", ch );
         return;
      }
      if( !can_mmodify( ch, victim ) )
         return;
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "Usage: mset <victim> immune <flag> [flag]...\r\n", ch );
         return;
      }
      while( argument[0] != '\0' )
      {
         argument = one_argument( argument, arg3 );
         value = get_risflag( arg3 );
         if( value < 0 || value > 31 )
            ch_printf( ch, "Unknown flag: %s\r\n", arg3 );
         else
            TOGGLE_BIT( victim->immune, 1 << value );
      }
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->immune = victim->immune;
      return;
   }

   if( !str_cmp( arg2, "susceptible" ) )
   {
      if( !IS_NPC( victim ) && get_trust( ch ) < LEVEL_LESSER )
      {
         send_to_char( "You can only modify a mobile's susceptibilities.\r\n", ch );
         return;
      }
      if( !can_mmodify( ch, victim ) )
         return;
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "Usage: mset <victim> susceptible <flag> [flag]...\r\n", ch );
         return;
      }
      while( argument[0] != '\0' )
      {
         argument = one_argument( argument, arg3 );
         value = get_risflag( arg3 );
         if( value < 0 || value > 31 )
            ch_printf( ch, "Unknown flag: %s\r\n", arg3 );
         else
            TOGGLE_BIT( victim->susceptible, 1 << value );
      }
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->susceptible = victim->susceptible;
      return;
   }

   if( !str_cmp( arg2, "part" ) )
   {
      if( !IS_NPC( victim ) && get_trust( ch ) < LEVEL_LESSER )
      {
         send_to_char( "You can only modify a mobile's parts.\r\n", ch );
         return;
      }
      if( !can_mmodify( ch, victim ) )
         return;
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "Usage: mset <victim> part <flag> [flag]...\r\n", ch );
         return;
      }
      while( argument[0] != '\0' )
      {
         argument = one_argument( argument, arg3 );
         value = get_partflag( arg3 );
         if( value < 0 || value > 31 )
            ch_printf( ch, "Unknown flag: %s\r\n", arg3 );
         else
            TOGGLE_BIT( victim->xflags, 1 << value );
      }
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->xflags = victim->xflags;
      return;
   }

   if( !str_cmp( arg2, "attack" ) )
   {
      if( !IS_NPC( victim ) )
      {
         send_to_char( "You can only modify a mobile's attacks.\r\n", ch );
         return;
      }
      if( !can_mmodify( ch, victim ) )
         return;
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "Usage: mset <victim> attack <flag> [flag]...\r\n", ch );
         send_to_char( "bite          claws        tail        sting      punch        kick\r\n", ch );
         send_to_char( "trip          bash         stun        gouge      backstab\r\n", ch );
         return;
      }
      while( argument[0] != '\0' )
      {
         argument = one_argument( argument, arg3 );
         value = get_attackflag( arg3 );
         if( value < 0 || value > 31 )
            ch_printf( ch, "Unknown flag: %s\r\n", arg3 );
         else
            TOGGLE_BIT( victim->attacks, 1 << value );
      }
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->attacks = victim->attacks;
      return;
   }

   if( !str_cmp( arg2, "defense" ) )
   {
      if( !IS_NPC( victim ) )
      {
         send_to_char( "You can only modify a mobile's defenses.\r\n", ch );
         return;
      }
      if( !can_mmodify( ch, victim ) )
         return;
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "Usage: mset <victim> defense <flag> [flag]...\r\n", ch );
         send_to_char( "parry        dodge\r\n", ch );
         return;
      }
      while( argument[0] != '\0' )
      {
         argument = one_argument( argument, arg3 );
         value = get_defenseflag( arg3 );
         if( value < 0 || value > 31 )
            ch_printf( ch, "Unknown flag: %s\r\n", arg3 );
         else
            TOGGLE_BIT( victim->defenses, 1 << value );
      }
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->defenses = victim->defenses;
      return;
   }

   if( !str_cmp( arg2, "pos" ) )
   {
      if( !IS_NPC( victim ) )
      {
         send_to_char( "Mobiles only.\r\n", ch );
         return;
      }
      if( !can_mmodify( ch, victim ) )
         return;
      if( value < 0 || value > POS_STANDING )
      {
         ch_printf( ch, "Position range is 0 to %d.\r\n", POS_STANDING );
         return;
      }
      victim->position = value;
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->position = victim->position;
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "defpos" ) )
   {
      if( !IS_NPC( victim ) )
      {
         send_to_char( "Mobiles only.\r\n", ch );
         return;
      }
      if( !can_mmodify( ch, victim ) )
         return;
      if( value < 0 || value > POS_STANDING )
      {
         ch_printf( ch, "Position range is 0 to %d.\r\n", POS_STANDING );
         return;
      }
      victim->defposition = value;
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->defposition = victim->defposition;
      send_to_char( "Done.\r\n", ch );
      return;
   }

   /*
    * save some finger-leather
    */
   if( !str_cmp( arg2, "hitdie" ) )
   {
      if( !IS_NPC( victim ) )
      {
         send_to_char( "Mobiles only.\r\n", ch );
         return;
      }
      if( !can_mmodify( ch, victim ) )
         return;

      sscanf( arg3, "%d %c %d %c %d", &num, &char1, &size, &char2, &plus );
      sprintf( outbuf, "%s hitnumdie %d", arg1, num );
      do_mset( ch, outbuf );

      sprintf( outbuf, "%s hitsizedie %d", arg1, size );
      do_mset( ch, outbuf );

      sprintf( outbuf, "%s hitplus %d", arg1, plus );
      do_mset( ch, outbuf );
      return;
   }
   /*
    * save some more finger-leather
    */
   if( !str_cmp( arg2, "damdie" ) )
   {
      if( !IS_NPC( victim ) )
      {
         send_to_char( "Mobiles only.\r\n", ch );
         return;
      }
      if( !can_mmodify( ch, victim ) )
         return;

      sscanf( arg3, "%d %c %d %c %d", &num, &char1, &size, &char2, &plus );
      sprintf( outbuf, "%s damnumdie %d", arg1, num );
      do_mset( ch, outbuf );
      sprintf( outbuf, "%s damsizedie %d", arg1, size );
      do_mset( ch, outbuf );
      sprintf( outbuf, "%s damplus %d", arg1, plus );
      do_mset( ch, outbuf );
      return;
   }

   if( !str_cmp( arg2, "hitnumdie" ) )
   {
      if( !IS_NPC( victim ) )
      {
         send_to_char( "Mobiles only.\r\n", ch );
         return;
      }
      if( !can_mmodify( ch, victim ) )
         return;
      if( value < 0 || value > 32767 )
      {
         send_to_char( "Number of hitpoint dice range is 0 to 30000.\r\n", ch );
         return;
      }
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->hitnodice = value;
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "hitsizedie" ) )
   {
      if( !IS_NPC( victim ) )
      {
         send_to_char( "Mobiles only.\r\n", ch );
         return;
      }
      if( !can_mmodify( ch, victim ) )
         return;
      if( value < 0 || value > 32767 )
      {
         send_to_char( "Hitpoint dice size range is 0 to 30000.\r\n", ch );
         return;
      }
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->hitsizedice = value;
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "hitplus" ) )
   {
      if( !IS_NPC( victim ) )
      {
         send_to_char( "Mobiles only.\r\n", ch );
         return;
      }
      if( !can_mmodify( ch, victim ) )
         return;
      if( value < 0 || value > 32767 )
      {
         send_to_char( "Hitpoint bonus range is 0 to 30000.\r\n", ch );
         return;
      }
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->hitplus = value;
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "damnumdie" ) )
   {
      if( !IS_NPC( victim ) )
      {
         send_to_char( "Mobiles only.\r\n", ch );
         return;
      }
      if( !can_mmodify( ch, victim ) )
         return;
      if( value < 0 || value > 100 )
      {
         send_to_char( "Number of damage dice range is 0 to 100.\r\n", ch );
         return;
      }
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->damnodice = value;
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "damsizedie" ) )
   {
      if( !IS_NPC( victim ) )
      {
         send_to_char( "Mobiles only.\r\n", ch );
         return;
      }
      if( !can_mmodify( ch, victim ) )
         return;
      if( value < 0 || value > 100 )
      {
         send_to_char( "Damage dice size range is 0 to 100.\r\n", ch );
         return;
      }
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->damsizedice = value;
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "damplus" ) )
   {
      if( !IS_NPC( victim ) )
      {
         send_to_char( "Mobiles only.\r\n", ch );
         return;
      }
      if( !can_mmodify( ch, victim ) )
         return;
      if( value < 0 || value > 1000 )
      {
         send_to_char( "Damage bonus range is 0 to 1000.\r\n", ch );
         return;
      }

      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->damplus = value;
      send_to_char( "Done.\r\n", ch );
      return;

   }


   if( !str_cmp( arg2, "aloaded" ) )
   {
      if( IS_NPC( victim ) )
      {
         send_to_char( "Player Characters only.\r\n", ch );
         return;
      }

      /*
       * Make sure they have an area assigned -Druid 
       */
      if( !victim->pcdata->area )
      {
         send_to_char( "Player does not have an area assigned to them.\r\n", ch );
         return;
      }

      if( !can_mmodify( ch, victim ) )
         return;

      if( !IS_SET( victim->pcdata->area->status, AREA_LOADED ) )
      {
         SET_BIT( victim->pcdata->area->status, AREA_LOADED );
         send_to_char( "Your area set to LOADED!\r\n", victim );
         if( ch != victim )
            send_to_char( "Area set to LOADED!\r\n", ch );
         return;
      }
      else
      {
         REMOVE_BIT( victim->pcdata->area->status, AREA_LOADED );
         send_to_char( "Your area set to NOT-LOADED!\r\n", victim );
         if( ch != victim )
            send_to_char( "Area set to NON-LOADED!\r\n", ch );
         return;
      }
   }

   if( !str_cmp( arg2, "speaks" ) )
   {
      if( !can_mmodify( ch, victim ) )
         return;
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "Usage: mset <victim> speaks <language> [language] ...\r\n", ch );
         return;
      }
      while( argument[0] != '\0' )
      {
         argument = one_argument( argument, arg3 );
         value = get_langflag( arg3 );
         if( value == LANG_UNKNOWN )
            ch_printf( ch, "Unknown language: %s\r\n", arg3 );
         else if( !IS_NPC( victim ) )
         {
            if( !( value &= VALID_LANGS ) )
            {
               ch_printf( ch, "Players may not know %s.\r\n", arg3 );
               continue;
            }
         }
         TOGGLE_BIT( victim->speaks, value );
      }
      if( !IS_NPC( victim ) )
      {
         REMOVE_BIT( victim->speaks, race_table[victim->race].language );
         if( !knows_language( victim, victim->speaking, victim ) )
            victim->speaking = race_table[victim->race].language;
      }
      else if( IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->speaks = victim->speaks;
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "speaking" ) )
   {
      if( !IS_NPC( victim ) )
      {
         send_to_char( "Players must choose the language they speak themselves.\r\n", ch );
         return;
      }
      if( !can_mmodify( ch, victim ) )
         return;
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "Usage: mset <victim> speaking <language> [language]...\r\n", ch );
         return;
      }
      while( argument[0] != '\0' )
      {
         argument = one_argument( argument, arg3 );
         value = get_langflag( arg3 );
         if( value == LANG_UNKNOWN )
            ch_printf( ch, "Unknown language: %s\r\n", arg3 );
         else
            TOGGLE_BIT( victim->speaking, value );
      }
      if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
         victim->pIndexData->speaking = victim->speaking;
      send_to_char( "Done.\r\n", ch );
      return;
   }

   /*
    * Generate usage message.
    */
   if( ch->substate == SUB_REPEATCMD )
   {
      ch->substate = SUB_RESTRICTED;
      interpret( ch, origarg );
      ch->substate = SUB_REPEATCMD;
      ch->last_cmd = do_mset;
   }
   else
      do_mset( ch, "" );
   return;
}

void do_oset( CHAR_DATA * ch, const char *argument )
{
   char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH], arg3[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH], outbuf[MAX_STRING_LENGTH];
   OBJ_DATA *obj, *tmpobj;
   EXTRA_DESCR_DATA *ed;
   bool lockobj;
   int value, tmp;
   const char *origarg = argument;

   if( IS_NPC( ch ) )
   {
      send_to_char( "Mob's can't oset\r\n", ch );
      return;
   }

   if( !ch->desc )
   {
      send_to_char( "You have no descriptor!\r\n", ch );
      return;
   }

   switch ( ch->substate )
   {
      default:
         break;

      case SUB_OBJ_EXTRA:
         if( !ch->dest_buf )
         {
            send_to_char( "Fatal error: report to www.fusproject.org\r\n", ch );
            bug( "do_oset: sub_obj_extra: NULL ch->dest_buf", 0 );
            ch->substate = SUB_NONE;
            return;
         }
         /*
          * hopefully the object didn't get extracted...
          * if you're REALLY paranoid, you could always go through
          * the object and index-object lists, searching through the
          * extra_descr lists for a matching pointer...
          */
         ed = ( EXTRA_DESCR_DATA* ) ch->dest_buf;
         STRFREE( ed->description );
         ed->description = copy_buffer( ch );
         tmpobj = ( OBJ_DATA* ) ch->spare_ptr;
         stop_editing( ch );
         ch->dest_buf = tmpobj;
         ch->substate = ch->tempnum;
         return;

      case SUB_OBJ_LONG:
         if( !ch->dest_buf )
         {
            send_to_char( "Fatal error: report to www.smaugmuds.org\r\n", ch );
            bug( "do_oset: sub_obj_long: NULL ch->dest_buf", 0 );
            ch->substate = SUB_NONE;
            return;
         }
         obj = ( OBJ_DATA* ) ch->dest_buf;
         if( obj && obj_extracted( obj ) )
         {
            send_to_char( "Your object was extracted!\r\n", ch );
            stop_editing( ch );
            return;
         }
         STRFREE( obj->description );
         obj->description = copy_buffer( ch );
         if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
         {
            STRFREE( obj->pIndexData->description );
            obj->pIndexData->description = QUICKLINK( obj->description );
         }
         tmpobj = ( OBJ_DATA* ) ch->spare_ptr;
         stop_editing( ch );
         ch->substate = ch->tempnum;
         ch->dest_buf = tmpobj;
         return;
   }

   obj = NULL;
   argument = smash_tilde_static( argument );

   if( ch->substate == SUB_REPEATCMD )
   {
     obj = ( OBJ_DATA* ) ch->dest_buf;
      if( !obj )
      {
         send_to_char( "Your object was extracted!\r\n", ch );
         argument = "done";
      }
      if( argument[0] == '\0' || !str_cmp( argument, " " ) || !str_cmp( argument, "stat" ) )
      {
         if( obj )
            do_ostat( ch, obj->name );
         else
            send_to_char( "No object selected.  Type '?' for help.\r\n", ch );
         return;
      }
      if( !str_cmp( argument, "done" ) || !str_cmp( argument, "off" ) )
      {
         if( ch->dest_buf )
            RelDestroy( relOSET_ON, ch, ch->dest_buf );

         send_to_char( "Oset mode off.\r\n", ch );
         ch->substate = SUB_NONE;
         ch->dest_buf = NULL;
         if( ch->pcdata && ch->pcdata->subprompt )
            STRFREE( ch->pcdata->subprompt );
         return;
      }
   }
   if( obj )
   {
      lockobj = TRUE;
      strcpy( arg1, obj->name );
      argument = one_argument( argument, arg2 );
      strcpy( arg3, argument );
   }
   else
   {
      lockobj = FALSE;
      argument = one_argument( argument, arg1 );
      argument = one_argument( argument, arg2 );
      strcpy( arg3, argument );
   }

   if( !str_cmp( arg1, "on" ) )
   {
      send_to_char( "Syntax: oset <object|vnum> on.\r\n", ch );
      return;
   }

   if( arg1[0] == '\0' || arg2[0] == '\0' || !str_cmp( arg1, "?" ) )
   {
      if( ch->substate == SUB_REPEATCMD )
      {
         if( obj )
            send_to_char( "Syntax: <field>  <value>\r\n", ch );
         else
            send_to_char( "Syntax: <object> <field>  <value>\r\n", ch );
      }
      else
         send_to_char( "Syntax: oset <object> <field>  <value>\r\n", ch );
      send_to_char( "\r\n", ch );
      send_to_char( "Field being one of:\r\n", ch );
      send_to_char( "  flags wear level weight cost rent timer\r\n", ch );
      send_to_char( "  name short long desc ed rmed actiondesc\r\n", ch );
      send_to_char( "  type value0 value1 value2 value3 value4 value5\r\n", ch );
      send_to_char( "  affect rmaffect layers addmaterial remmaterial\r\n", ch );
      send_to_char( "For weapons:             For armor:\r\n", ch );
      send_to_char( "  weapontype condition     evasion armor-class\r\n", ch );
      send_to_char( "  numdamdie sizedamdie     temper       \r\n", ch );
      send_to_char( "  charges   maxcharges                  \r\n", ch );
      send_to_char( "  speed                                 \r\n", ch );
      send_to_char( "For potions, pills:\r\n", ch );
      send_to_char( "  slevel spell1 spell2 spell3\r\n", ch );
      send_to_char( "For devices:\r\n", ch );
      send_to_char( "  slevel spell maxcharges charges\r\n", ch );
      send_to_char( "For salves:\r\n", ch );
      send_to_char( "  slevel spell1 spell2 maxdoses delay (keep low - delay is anoying)\r\n", ch );
      send_to_char( "For containers:          For levers and switches:\r\n", ch );
      send_to_char( "  cflags key capacity      tflags\r\n", ch );
      send_to_char( "For rawspice:            For ammo and batteries:\r\n", ch );
      send_to_char( "  spicetype  grade         charges (at least 1000 for ammo)\r\n", ch );
      send_to_char( "For crystals:\r\n", ch );
      send_to_char( "  gemtype\r\n", ch );
      return;
   }

   if( !obj && get_trust( ch ) <= LEVEL_IMMORTAL )
   {
      if( ( obj = get_obj_here( ch, arg1 ) ) == NULL )
      {
         send_to_char( "You can't find that here.\r\n", ch );
         return;
      }
   }
   else if( !obj )
   {
      if( ( obj = get_obj_world( ch, arg1 ) ) == NULL )
      {
         send_to_char( "There is nothing like that in all the realms.\r\n", ch );
         return;
      }
   }
   if( lockobj )
      ch->dest_buf = obj;

   separate_obj( obj );
   value = atoi( arg3 );

   if( !str_cmp( arg2, "on" ) )
   {
      CHECK_SUBRESTRICTED( ch );
      ch_printf( ch, "Oset mode on. (Editing '%s' vnum %d).\r\n", obj->name, obj->pIndexData->vnum );
      ch->substate = SUB_REPEATCMD;
      ch->dest_buf = obj;
      if( ch->pcdata )
      {
         if( ch->pcdata->subprompt )
            STRFREE( ch->pcdata->subprompt );
         sprintf( buf, "<&COset &W#%d&w> %%i", obj->pIndexData->vnum );
         ch->pcdata->subprompt = STRALLOC( buf );
      }
      RelCreate( relOSET_ON, ch, obj );
      return;
   }

   if( !str_cmp( arg2, "value0" ) || !str_cmp( arg2, "v0" ) )
   {
      if( !can_omodify( ch, obj ) )
         return;
      obj->value[0] = value;
      if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
         obj->pIndexData->value[0] = value;
      return;
   }

   if( !str_cmp( arg2, "value1" ) || !str_cmp( arg2, "v1" ) )
   {
      if( !can_omodify( ch, obj ) )
         return;
      obj->value[1] = value;
      if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
         obj->pIndexData->value[1] = value;
      return;
   }

   if( !str_cmp( arg2, "value2" ) || !str_cmp( arg2, "v2" ) )
   {
      if( !can_omodify( ch, obj ) )
         return;
      obj->value[2] = value;
      if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
      {
         obj->pIndexData->value[2] = value;
         if( obj->item_type == ITEM_WEAPON && value != 0 )
            obj->value[2] = obj->pIndexData->value[1] * obj->pIndexData->value[2];
      }
      return;
   }

   if( !str_cmp( arg2, "value3" ) || !str_cmp( arg2, "v3" ) )
   {
      if( !can_omodify( ch, obj ) )
         return;
      obj->value[3] = value;
      if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
         obj->pIndexData->value[3] = value;
      return;
   }

   if( !str_cmp( arg2, "value4" ) || !str_cmp( arg2, "v4" ) )
   {
      if( !can_omodify( ch, obj ) )
         return;
      obj->value[4] = value;
      if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
         obj->pIndexData->value[4] = value;
      return;
   }

   if( !str_cmp( arg2, "value5" ) || !str_cmp( arg2, "v5" ) )
   {
      if( !can_omodify( ch, obj ) )
         return;
      obj->value[5] = value;
      if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
         obj->pIndexData->value[5] = value;
      return;
   }

   if( !str_cmp( arg2, "damtype" ) )
   {
      if( !can_omodify( ch, obj ) )
         return;
      if( obj->item_type != ITEM_WEAPON )
      {
         send_to_char( "Non-weapons cannot have damage types.\r\n", ch );
         return;
      }
      while( argument[0] != '\0' )
      {
         argument = one_argument( argument, arg3 );
         if( ( value = get_damtype( arg3 ) ) == -1 || ( value >= DAM_ALL && value <= DAM_ELEMENTAL ) )
         {
            ch_printf( ch, "%s is an invalid damtype.\r\n", arg3 );
            continue;
         }
         xTOGGLE_BIT( obj->damtype, value );
         if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
            xTOGGLE_BIT( obj->pIndexData->damtype, value );
      }
      send_to_char( "Ok.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "quality" ) )
   {
      if( !can_omodify( ch, obj ) )
         return;
      while( argument[0] != '\0' )
      {
         argument = one_argument( argument, arg3 );
         if( ( value = get_qualitytype( arg3 ) ) == -1 )
         {
            ch_printf( ch, "%s is an invalid quality type.\r\n", arg3 );
            continue;
         }
         xTOGGLE_BIT( obj->quality, value );
         if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
            xTOGGLE_BIT( obj->quality, value );
      }
      send_to_char( "Ok.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "temper" ) )
   {
      if( !can_omodify( ch, obj ) )
         return;
      if( obj->item_type != ITEM_ARMOR )
      {
         send_to_char( "Non-armors cannot have tempers.\r\n", ch );
         return;
      }
      while( argument[0] != '\0' )
      {
         argument = one_argument( argument, arg3 );
         if( ( value = get_damtype( arg3 ) ) == -1 || ( value >= DAM_ALL && value <= DAM_ELEMENTAL ) )
         {
            ch_printf( ch, "%s is an invalid temper damtype.\r\n", arg3 );
            continue;
         }
         xTOGGLE_BIT( obj->temper, value );
         if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
            xTOGGLE_BIT( obj->pIndexData->temper, value );
      }
      send_to_char( "Ok.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "speed" ) )
   {
      if( !can_omodify( ch, obj ) )
         return;
      if( obj->item_type != ITEM_WEAPON )
      {
         send_to_char( "Non-weapons cannot have speeds.\r\n", ch );
         return;
      }
      obj->speed = atof( argument );
      send_to_char( "Ok.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "type" ) )
   {
      if( !can_omodify( ch, obj ) )
         return;
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "Usage: oset <object> type <type>\r\n", ch );
         send_to_char( "Possible Types:\r\n", ch );
         send_to_char( "None        Light\r\n", ch );
         send_to_char( "Treasure    Armor      Comlink    Fabric      Grenade\r\n", ch );
         send_to_char( "Furniture   Trash      Container  Drink_con   Landmine\r\n", ch );
         send_to_char( "Key         Food       Money      Pen         Fuel\r\n", ch );
         send_to_char( "Fountain    Pill       Weapon     Medpac      Missile\r\n", ch );
         send_to_char( "Fire        Book       Superconductor         Rare_metal\r\n", ch );
         send_to_char( "Switch      Lever      Button     Dial        Government\r\n", ch );
         send_to_char( "Trap        Map        Portal     Paper       Magnet\r\n", ch );
         send_to_char( "Lockpick    Shovel     Thread     Smut        Ammo\r\n", ch );
         send_to_char( "Rawspice    Lens       Crystal    Duraplast   Battery\r\n", ch );
         send_to_char( "Toolkit     Durasteel  Oven       Mirror      Circuit\r\n", ch );
         send_to_char( "Potion      Salve      Pill       Device      Spacecraft\r\n", ch );
         send_to_char( "Bolt        Chemical\r\n", ch );
         return;
      }
      value = get_otype( argument );
      if( value < 1 )
      {
         ch_printf( ch, "Unknown type: %s\r\n", arg3 );
         return;
      }
      obj->item_type = ( short )value;
      if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
         obj->pIndexData->item_type = obj->item_type;
      return;
   }

   if( !str_cmp( arg2, "flags" ) )
   {
      if( !can_omodify( ch, obj ) )
         return;
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "Usage: oset <object> flags <flag> [flag]...\r\n", ch );
         send_to_char( "glow, dark, magic, bless, antievil, noremove, antisith, antisoldier,\r\n", ch );
         send_to_char( "donation, covering, hum, invis, nodrop, antigood, antipilot, anticitizen\r\n", ch );
         send_to_char( "antineutral, inventory, antithief, antijedi, clanobject, antihunter\r\n", ch );
         send_to_char( "small_size, human_size, large_size, hutt_size, contraband\r\n", ch );
         return;
      }
      while( argument[0] != '\0' )
      {
         argument = one_argument( argument, arg3 );
         value = get_oflag( arg3 );
         if( value < 0 || value > 31 )
            ch_printf( ch, "Unknown flag: %s\r\n", arg3 );
         else
         {
            TOGGLE_BIT( obj->extra_flags, 1 << value );
            if( 1 << value == ITEM_PROTOTYPE )
               obj->pIndexData->extra_flags = obj->extra_flags;
         }
      }
      if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
         obj->pIndexData->extra_flags = obj->extra_flags;
      return;
   }

   if( !str_cmp( arg2, "addmaterial" ) )
   {
      ITEM_MATERIAL *material;
      OBJ_INDEX_DATA *object;
      int amount;
      if( !can_omodify( ch, obj ) )
         return;

      if( argument[0] == '\0' )
      {
         send_to_char( "Proper usage: oset <object> material <vnum> <amount>\r\n", ch );
         return;
      }
      argument = one_argument( argument, arg3 );

      if( !is_number( arg3 ) )
      {
         send_to_char( "Enter a vnum.\r\n", ch );
         return;
      }
      value = atoi( arg3 );

      if( !is_number( argument ) )
      {
         send_to_char( "Enter an amount.\r\n", ch );
         return;
      }
      amount = atoi( argument );

      if( ( object = get_obj_index( value ) ) == NULL )
      {
         send_to_char( "No item with that vnum exists.\r\n", ch );
         return;
      }
      CREATE( material, ITEM_MATERIAL, 1 );
      material->object = object;
      material->amount = amount;
      LINK( material, obj->first_material, obj->last_material, next, prev );
      if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
         LINK( copy_material( material ), obj->pIndexData->first_material, obj->pIndexData->last_material, next, prev );
      send_to_char( "Ok.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "remmaterial" ) )
   {
      ITEM_MATERIAL *material;
      int count;
      if( value < 1 )
      {
         send_to_char( "Invalid value entered.\r\n", ch );
         return;
      }

      if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
      {
         for( count = 0, material = obj->pIndexData->first_material; material; material = material->next )
         {
            if( ++count == value )
            {
               UNLINK( material, obj->pIndexData->first_material, obj->pIndexData->last_material, next, prev );
               free_material( material );
               send_to_char( "Ok.\r\n", ch );
               return;
            }
         }
      }
      else
      {
         for( count = 0, material = obj->first_material; material; material = material->next )
         {
            if( ++count == value )
            {
               UNLINK( material, obj->first_material, obj->last_material, next, prev );
               free_material( material );
               send_to_char( "Ok.\r\n", ch );
               return;
            }
         }
      }
      send_to_char( "Item does not have that many materials.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "wear" ) )
   {
      if( !can_omodify( ch, obj ) )
         return;
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "Usage: oset <object> wear <flag> [flag]...\r\n", ch );
         send_to_char( "Possible locations:\r\n", ch );
         send_to_char( "take   finger   neck    body    head   legs\r\n", ch );
         send_to_char( "feet   hands    arms    shield  about  waist\r\n", ch );
         send_to_char( "wrist  wield    hold    ears    eyes\r\n", ch );
         return;
      }
      while( argument[0] != '\0' )
      {
         argument = one_argument( argument, arg3 );
         value = get_wflag( arg3 );
         if( value < 0 || value > 31 )
            ch_printf( ch, "Unknown flag: %s\r\n", arg3 );
         else
            TOGGLE_BIT( obj->wear_flags, 1 << value );
      }

      if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
         obj->pIndexData->wear_flags = obj->wear_flags;
      return;
   }

   if( !str_cmp( arg2, "level" ) )
   {
      if( !can_omodify( ch, obj ) )
         return;
      obj->level = value;
      if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
         obj->pIndexData->level = value;
      return;
   }

   if( !str_cmp( arg2, "weight" ) )
   {
      if( !can_omodify( ch, obj ) )
         return;
      obj->weight = value;
      if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
         obj->pIndexData->weight = value;
      return;
   }

   if( !str_cmp( arg2, "cost" ) )
   {
      if( !can_omodify( ch, obj ) )
         return;
      obj->cost = value;
      if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
         obj->pIndexData->cost = value;
      return;
   }

   if( !str_cmp( arg2, "rent" ) )
   {
      if( !can_omodify( ch, obj ) )
         return;
      if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
         obj->pIndexData->rent = value;
      else
         send_to_char( "Item must have prototype flag to set this value.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "layers" ) )
   {
      if( !can_omodify( ch, obj ) )
         return;
      if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
         obj->pIndexData->layers = value;
      else
         send_to_char( "Item must have prototype flag to set this value.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "timer" ) )
   {
      if( !can_omodify( ch, obj ) )
         return;
      obj->timer = value;
      return;
   }

   if( !str_cmp( arg2, "name" ) )
   {
      if( !can_omodify( ch, obj ) )
         return;

      if( arg3[0] == '\0' )
      {
         send_to_char( "You MUST choose a new name.\n\r", ch );
         return;
      }
      STRFREE( obj->name );
      obj->name = STRALLOC( arg3 );
      if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
      {
         STRFREE( obj->pIndexData->name );
         obj->pIndexData->name = QUICKLINK( obj->name );
      }
      return;
   }


   if( !str_cmp( arg2, "short" ) )
   {
      STRFREE( obj->short_descr );
      obj->short_descr = STRALLOC( arg3 );
      if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
      {
         STRFREE( obj->pIndexData->short_descr );
         obj->pIndexData->short_descr = QUICKLINK( obj->short_descr );
      }
      else
         /*
          * Feature added by Narn, Apr/96 
          * * If the item is not proto, add the word 'rename' to the keywords
          * * if it is not already there.
          */
      {
         if( str_infix( "rename", obj->name ) )
         {
            sprintf( buf, "%s %s", obj->name, "rename" );
            STRFREE( obj->name );
            obj->name = STRALLOC( buf );
         }
      }
      return;
   }

   if( !str_cmp( arg2, "actiondesc" ) )
   {
      if( strstr( arg3, "%n" ) || strstr( arg3, "%d" ) || strstr( arg3, "%l" ) )
      {
         send_to_char( "Illegal characters!\r\n", ch );
         return;
      }
      STRFREE( obj->action_desc );
      obj->action_desc = STRALLOC( arg3 );
      if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
      {
         STRFREE( obj->pIndexData->action_desc );
         obj->pIndexData->action_desc = QUICKLINK( obj->action_desc );
      }
      return;
   }

   if( !str_cmp( arg2, "long" ) )
   {
      if( arg3[0] )
      {
         STRFREE( obj->description );
         obj->description = STRALLOC( arg3 );
         if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
         {
            STRFREE( obj->pIndexData->description );
            obj->pIndexData->description = QUICKLINK( obj->description );
         }
         return;
      }
      CHECK_SUBRESTRICTED( ch );
      if( ch->substate == SUB_REPEATCMD )
         ch->tempnum = SUB_REPEATCMD;
      else
         ch->tempnum = SUB_NONE;
      if( lockobj )
         ch->spare_ptr = obj;
      else
         ch->spare_ptr = NULL;
      ch->substate = SUB_OBJ_LONG;
      ch->dest_buf = obj;
      start_editing( ch, (char *)obj->description );
      return;
   }

   if( !str_cmp( arg2, "affect" ) )
   {
      AFFECT_DATA *paf;
      short loc;
      EXT_BV bitv;

      argument = one_argument( argument, arg2 );
      if( arg2[0] == '\0' || !argument || argument[0] == '\0' )
      {
         int x, col;
         send_to_char( "Possible Affects:\r\n", ch );
         for( x = 0, col = 0; x < MAX_APPLY_TYPE; x++ )
         {
            ch_printf( ch, "%-18.18s", a_types[x] ? a_types[x] : "null" );
            if( 4 == col++ )
            {
               send_to_char( "\r\n", ch );
               col = 0;
            }
         }
         if( col != 0 )
            send_to_char( "\r\n", ch );
         return;
      }
      loc = get_atype( arg2 );
      if( loc < 1 )
      {
         ch_printf( ch, "Unknown field: %s\r\n", arg2 );
         return;
      }
      if( loc >= APPLY_AFFECT && loc < APPLY_WEAPONSPELL )
      {
         xCLEAR_BITS( bitv );
         while( argument[0] != '\0' )
         {
            argument = one_argument( argument, arg3 );
            if( loc == APPLY_AFFECT )
               value = get_aflag( arg3 );
            else
               value = get_risflag( arg3 );
            if( value > MAX_AFF )
               ch_printf( ch, "Unknown flag: %s\r\n", arg3 );
            else
               xSET_BIT( bitv, value );
         }
         if( xIS_EMPTY( bitv ) )
            return;
         value = 0;
      }
      else if( loc == APPLY_DAMTYPE )
      {
         argument = one_argument( argument, arg3 );
         if( ( value = get_damtype( arg3 ) ) < DAM_ALL )
         {
            int x;
            send_to_char( "Valid damtypes:", ch );
            for( x = 0; x < MAX_DAMTYPE; x++ )
               ch_printf( ch, " %s,", d_type[x] );
            send_to_char( "\r\n", ch );
         }
      }
      else
      {
         argument = one_argument( argument, arg3 );
         value = atoi( arg3 );
      }
      CREATE( paf, AFFECT_DATA, 1 );
      paf->type = -1;
      paf->duration = -1;
      paf->location = loc;
      paf->modifier = value;
      xCLEAR_BITS( paf->bitvector );
      xSET_BITS( paf->bitvector, bitv );
      paf->next = NULL;
      if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
      {
         if( loc != APPLY_WEARSPELL && loc != APPLY_REMOVESPELL && loc != APPLY_STRIPSN && loc != APPLY_WEAPONSPELL )
         {
            CHAR_DATA *vch;
            OBJ_DATA *eq;

            for( vch = first_char; vch; vch = vch->next )
            {
               for( eq = vch->first_carrying; eq; eq = eq->next_content )
               {
                  if( eq->pIndexData == obj->pIndexData && eq->wear_loc != WEAR_NONE )
                     affect_modify( vch, paf, TRUE );
               }
            }
         }
         LINK( paf, obj->pIndexData->first_affect, obj->pIndexData->last_affect, next, prev );
      }
      else
         LINK( paf, obj->first_affect, obj->last_affect, next, prev );
      ++top_affect;
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "rmaffect" ) )
   {
      AFFECT_DATA *paf;
      short loc, count;

      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "Usage: oset <object> rmaffect <affect#>\r\n", ch );
         return;
      }
      loc = atoi( argument );
      if( loc < 1 )
      {
         send_to_char( "Invalid number.\r\n", ch );
         return;
      }

      count = 0;

      if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
      {
         OBJ_INDEX_DATA *pObjIndex;

         pObjIndex = obj->pIndexData;
         for( paf = pObjIndex->first_affect; paf; paf = paf->next )
         {
            if( ++count == loc )
            {
               UNLINK( paf, pObjIndex->first_affect, pObjIndex->last_affect, next, prev );
               if( paf->location != APPLY_WEARSPELL && paf->location != APPLY_REMOVESPELL && paf->location != APPLY_STRIPSN
                && paf->location != APPLY_WEAPONSPELL )
               {
                  CHAR_DATA *vch;
                  OBJ_DATA *eq;

                  for( vch = first_char; vch; vch = vch->next )
                  {
                     for( eq = vch->first_carrying; eq; eq = eq->next_content )
                     {
                        if( eq->pIndexData == pObjIndex && eq->wear_loc != WEAR_NONE )
                           affect_modify( vch, paf, FALSE );
                     }
                  }
               }
               free_affect( paf );
               send_to_char( "Removed.\n\r", ch );
               return;
            }
         }
      }
      else
      {
         for( paf = obj->first_affect; paf; paf = paf->next )
         {
            if( ++count == loc )
            {
               UNLINK( paf, obj->first_affect, obj->last_affect, next, prev );
               free_affect( paf );
               send_to_char( "Removed.\r\n", ch );
               return;
            }
         }
         send_to_char( "Not found.\r\n", ch );
         return;
      }
   }

   if( !str_cmp( arg2, "ed" ) )
   {
      if( arg3[0] == '\0' )
      {
         send_to_char( "Syntax: oset <object> ed <keywords>\r\n", ch );
         return;
      }
      CHECK_SUBRESTRICTED( ch );
      if( obj->timer )
      {
         send_to_char( "It's not safe to edit an extra description on an object with a timer.\r\nTurn it off first.\r\n",
                       ch );
         return;
      }
      if( obj->item_type == ITEM_PAPER )
      {
         send_to_char( "You can not add an extra description to a note paper at the moment.\r\n", ch );
         return;
      }
      if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
         ed = SetOExtraProto( obj->pIndexData, arg3 );
      else
         ed = SetOExtra( obj, arg3 );
      if( ch->substate == SUB_REPEATCMD )
         ch->tempnum = SUB_REPEATCMD;
      else
         ch->tempnum = SUB_NONE;
      if( lockobj )
         ch->spare_ptr = obj;
      else
         ch->spare_ptr = NULL;
      ch->substate = SUB_OBJ_EXTRA;
      ch->dest_buf = ed;
      start_editing( ch, (char *)ed->description );
      return;
   }

   if( !str_cmp( arg2, "desc" ) )
   {
      CHECK_SUBRESTRICTED( ch );
      if( obj->timer )
      {
         send_to_char( "It's not safe to edit a description on an object with a timer.\r\nTurn it off first.\r\n", ch );
         return;
      }
      if( obj->item_type == ITEM_PAPER )
      {
         send_to_char( "You can not add a description to a note paper at the moment.\r\n", ch );
         return;
      }
      if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
         ed = SetOExtraProto( obj->pIndexData, obj->name );
      else
         ed = SetOExtra( obj, obj->name );
      if( ch->substate == SUB_REPEATCMD )
         ch->tempnum = SUB_REPEATCMD;
      else
         ch->tempnum = SUB_NONE;
      if( lockobj )
         ch->spare_ptr = obj;
      else
         ch->spare_ptr = NULL;
      ch->substate = SUB_OBJ_EXTRA;
      ch->dest_buf = ed;
      start_editing( ch, (char *)ed->description );
      return;
   }

   if( !str_cmp( arg2, "rmed" ) )
   {
      if( arg3[0] == '\0' )
      {
         send_to_char( "Syntax: oset <object> rmed <keywords>\r\n", ch );
         return;
      }
      if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
      {
         if( DelOExtraProto( obj->pIndexData, arg3 ) )
            send_to_char( "Deleted.\r\n", ch );
         else
            send_to_char( "Not found.\r\n", ch );
         return;
      }
      if( DelOExtra( obj, arg3 ) )
         send_to_char( "Deleted.\r\n", ch );
      else
         send_to_char( "Not found.\r\n", ch );
      return;
   }
   /*
    * save some finger-leather
    */
   if( !str_cmp( arg2, "ris" ) )
   {
      sprintf( outbuf, "%s affect resistant %s", arg1, arg3 );
      do_oset( ch, outbuf );
      sprintf( outbuf, "%s affect immune %s", arg1, arg3 );
      do_oset( ch, outbuf );
      sprintf( outbuf, "%s affect susceptible %s", arg1, arg3 );
      do_oset( ch, outbuf );
      return;
   }

   if( !str_cmp( arg2, "r" ) )
   {
      sprintf( outbuf, "%s affect resistant %s", arg1, arg3 );
      do_oset( ch, outbuf );
      return;
   }

   if( !str_cmp( arg2, "i" ) )
   {
      sprintf( outbuf, "%s affect immune %s", arg1, arg3 );
      do_oset( ch, outbuf );
      return;
   }
   if( !str_cmp( arg2, "s" ) )
   {
      sprintf( outbuf, "%s affect susceptible %s", arg1, arg3 );
      do_oset( ch, outbuf );
      return;
   }

   if( !str_cmp( arg2, "ri" ) )
   {
      sprintf( outbuf, "%s affect resistant %s", arg1, arg3 );
      do_oset( ch, outbuf );
      sprintf( outbuf, "%s affect immune %s", arg1, arg3 );
      do_oset( ch, outbuf );
      return;
   }

   if( !str_cmp( arg2, "rs" ) )
   {
      sprintf( outbuf, "%s affect resistant %s", arg1, arg3 );
      do_oset( ch, outbuf );
      sprintf( outbuf, "%s affect susceptible %s", arg1, arg3 );
      do_oset( ch, outbuf );
      return;
   }

   if( !str_cmp( arg2, "is" ) )
   {
      sprintf( outbuf, "%s affect immune %s", arg1, arg3 );
      do_oset( ch, outbuf );
      sprintf( outbuf, "%s affect susceptible %s", arg1, arg3 );
      do_oset( ch, outbuf );
      return;
   }

   /*
    * Make it easier to set special object values by name than number
    *                  -Thoric
    */
   tmp = -1;
   switch ( obj->item_type )
   {
      case ITEM_WEAPON:
         if( !str_cmp( arg2, "weapontype" ) )
         {
	   size_t x;

            value = -1;
            for( x = 0; x < sizeof( weapon_table ) / sizeof( weapon_table[0] ); x++ )
               if( !str_cmp( arg3, weapon_table[x] ) )
                  value = x;
            if( value < 0 )
            {
               send_to_char( "Unknown weapon type.\r\n", ch );
               send_to_char( "\r\nChoices:\r\n", ch );
               send_to_char( "   none, lightsaber, vibro-blade, blaster, force pike, bowcaster, bludgeon\r\n", ch );
               return;
            }
            tmp = 3;
            break;
         }
         if( !str_cmp( arg2, "condition" ) )
            tmp = 0;
         if( !str_cmp( arg2, "numdamdie" ) )
            tmp = 1;
         if( !str_cmp( arg2, "sizedamdie" ) )
            tmp = 2;
         if( !str_cmp( arg2, "charges" ) )
            tmp = 4;
         if( !str_cmp( arg2, "maxcharges" ) )
            tmp = 5;
         if( !str_cmp( arg2, "charge" ) )
            tmp = 4;
         if( !str_cmp( arg2, "maxcharge" ) )
            tmp = 5;
         break;
      case ITEM_BOLT:
      case ITEM_AMMO:
         if( !str_cmp( arg2, "charges" ) )
            tmp = 0;
         if( !str_cmp( arg2, "charge" ) )
            tmp = 0;
         break;
      case ITEM_BATTERY:
         if( !str_cmp( arg2, "charges" ) )
            tmp = 0;
         if( !str_cmp( arg2, "charge" ) )
            tmp = 0;
         break;
      case ITEM_RAWSPICE:
      case ITEM_SPICE:
         if( !str_cmp( arg2, "grade" ) )
            tmp = 1;
         if( !str_cmp( arg2, "spicetype" ) )
         {
	   size_t x;

            value = -1;
            for( x = 0; x < sizeof( spice_table ) / sizeof( spice_table[0] ); x++ )
               if( !str_cmp( arg3, spice_table[x] ) )
                  value = x;
            if( value < 0 )
            {
               send_to_char( "Unknown spice type.\r\n", ch );
               send_to_char( "\r\nChoices:\r\n", ch );
               send_to_char( "   glitterstim, carsanum, ryll, andris\r\n", ch );
               return;
            }
            tmp = 0;
            break;
         }
         break;
      case ITEM_CRYSTAL:
         if( !str_cmp( arg2, "gemtype" ) )
         {
	   size_t x;

            value = -1;
            for( x = 0; x < sizeof( crystal_table ) / sizeof( crystal_table[0] ); x++ )
               if( !str_cmp( arg3, crystal_table[x] ) )
                  value = x;
            if( value < 0 )
            {
               send_to_char( "Unknown gem type.\r\n", ch );
               send_to_char( "\r\nChoices:\r\n", ch );
               send_to_char( "   non-adegan, kathracite, relacite, danite, mephite, ponite, illum, corusca\r\n", ch );
               return;
            }
            tmp = 0;
            break;
         }
         break;
      case ITEM_ARMOR:
         if( !str_cmp( arg2, "evasion" ) )
            tmp = 0;
         if( !str_cmp( arg2, "armor-class" ) )
            tmp = 2;
         break;
      case ITEM_SALVE:
         if( !str_cmp( arg2, "slevel" ) )
            tmp = 0;
         if( !str_cmp( arg2, "maxdoses" ) )
            tmp = 1;
         if( !str_cmp( arg2, "doses" ) )
            tmp = 2;
         if( !str_cmp( arg2, "delay" ) )
            tmp = 3;
         if( !str_cmp( arg2, "spell1" ) )
            tmp = 4;
         if( !str_cmp( arg2, "spell2" ) )
            tmp = 5;
         if( tmp >= 4 && tmp <= 5 )
            value = skill_lookup( arg3 );
         break;
      case ITEM_POTION:
      case ITEM_PILL:
         if( !str_cmp( arg2, "slevel" ) )
            tmp = 0;
         if( !str_cmp( arg2, "spell1" ) )
            tmp = 1;
         if( !str_cmp( arg2, "spell2" ) )
            tmp = 2;
         if( !str_cmp( arg2, "spell3" ) )
            tmp = 3;
         if( tmp >= 1 && tmp <= 3 )
            value = skill_lookup( arg3 );
         break;
      case ITEM_DEVICE:
         if( !str_cmp( arg2, "slevel" ) )
            tmp = 0;
         if( !str_cmp( arg2, "spell" ) )
         {
            tmp = 3;
            value = skill_lookup( arg3 );
         }
         if( !str_cmp( arg2, "maxcharges" ) )
            tmp = 1;
         if( !str_cmp( arg2, "charges" ) )
            tmp = 2;
         break;
      case ITEM_CONTAINER:
         if( !str_cmp( arg2, "capacity" ) )
            tmp = 0;
         if( !str_cmp( arg2, "cflags" ) )
            tmp = 1;
         if( !str_cmp( arg2, "key" ) )
            tmp = 2;
         break;
      case ITEM_SWITCH:
      case ITEM_LEVER:
      case ITEM_BUTTON:
         if( !str_cmp( arg2, "tflags" ) )
         {
            tmp = 0;
            value = get_trigflag( arg3 );
         }
         break;
   }
   if( tmp >= 0 && tmp <= 5 )
   {
      if( !can_omodify( ch, obj ) )
         return;
      obj->value[tmp] = value;
      if( IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
         obj->pIndexData->value[tmp] = value;
      return;
   }

   /*
    * Generate usage message.
    */
   if( ch->substate == SUB_REPEATCMD )
   {
      ch->substate = SUB_RESTRICTED;
      interpret( ch, origarg );
      ch->substate = SUB_REPEATCMD;
      ch->last_cmd = do_oset;
   }
   else
      do_oset( ch, "" );
   return;
}


/*
 * Obsolete Merc room editing routine
 */
void do_rset( CHAR_DATA * ch, const char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char arg3[MAX_INPUT_LENGTH];
   ROOM_INDEX_DATA *location;
   int value;
   bool proto;

   argument = smash_tilde_static( argument );
   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   strcpy( arg3, argument );

   if( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
   {
      send_to_char( "Syntax: rset <location> <field> value\r\n", ch );
      send_to_char( "\r\n", ch );
      send_to_char( "Field being one of:\r\n", ch );
      send_to_char( "  flags sector\r\n", ch );
      return;
   }

   if( ( location = find_location( ch, arg1 ) ) == NULL )
   {
      send_to_char( "No such location.\r\n", ch );
      return;
   }

   if( !can_rmodify( ch, location ) )
      return;

   if( !is_number( arg3 ) )
   {
      send_to_char( "Value must be numeric.\r\n", ch );
      return;
   }
   value = atoi( arg3 );

   /*
    * Set something.
    */
   if( !str_cmp( arg2, "flags" ) )
   {
      /*
       * Protect from messing up prototype flag
       */
      if( IS_SET( location->room_flags, ROOM_PROTOTYPE ) )
         proto = TRUE;
      else
         proto = FALSE;
      location->room_flags = value;
      if( proto )
         SET_BIT( location->room_flags, ROOM_PROTOTYPE );
      return;
   }

   if( !str_cmp( arg2, "sector" ) )
   {
      location->sector_type = value;
      return;
   }

   /*
    * Generate usage message.
    */
   do_rset( ch, "" );
   return;
}

/*
 * Returns value 0 - 9 based on directional text.
 */
int get_dir( const char *txt )
{
   int edir;
   char c1, c2;

   if( !str_cmp( txt, "northeast" ) )
      return DIR_NORTHEAST;
   if( !str_cmp( txt, "northwest" ) )
      return DIR_NORTHWEST;
   if( !str_cmp( txt, "southeast" ) )
      return DIR_SOUTHEAST;
   if( !str_cmp( txt, "southwest" ) )
      return DIR_SOUTHWEST;
   if( !str_cmp( txt, "somewhere" ) )
      return 10;

   c1 = txt[0];
   if( c1 == '\0' )
      return 0;
   c2 = txt[1];
   edir = 0;
   switch ( c1 )
   {
      case 'n':
         switch ( c2 )
         {
            default:
               edir = 0;
               break;   /* north */
            case 'e':
               edir = 6;
               break;   /* ne   */
            case 'w':
               edir = 7;
               break;   /* nw   */
         }
         break;
      case '0':
         edir = 0;
         break;   /* north */
      case 'e':
      case '1':
         edir = 1;
         break;   /* east  */
      case 's':
         switch ( c2 )
         {
            default:
               edir = 2;
               break;   /* south */
            case 'e':
               edir = 8;
               break;   /* se   */
            case 'w':
               edir = 9;
               break;   /* sw   */
         }
         break;
      case '2':
         edir = 2;
         break;   /* south */
      case 'w':
      case '3':
         edir = 3;
         break;   /* west  */
      case 'u':
      case '4':
         edir = 4;
         break;   /* up    */
      case 'd':
      case '5':
         edir = 5;
         break;   /* down  */
      case '6':
         edir = 6;
         break;   /* ne   */
      case '7':
         edir = 7;
         break;   /* nw   */
      case '8':
         edir = 8;
         break;   /* se   */
      case '9':
         edir = 9;
         break;   /* sw   */
      case '?':
         edir = 10;
         break;   /* somewhere */
   }
   return edir;
}

void do_redit( CHAR_DATA * ch, const char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char arg3[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   ROOM_INDEX_DATA *location, *tmp;
   EXTRA_DESCR_DATA *ed;
   EXIT_DATA *xit, *texit;
   int value;
   int edir = 0, ekey, evnum;
   const char *origarg = argument;

   if( !ch->desc )
   {
      send_to_char( "You have no descriptor.\r\n", ch );
      return;
   }

   switch ( ch->substate )
   {
      default:
         break;
      case SUB_ROOM_DESC:
	location = ( ROOM_INDEX_DATA* ) ch->dest_buf;
         if( !location )
         {
            bug( "redit: sub_room_desc: NULL ch->dest_buf", 0 );
            location = ch->in_room;
         }
         STRFREE( location->description );
         location->description = copy_buffer( ch );
         stop_editing( ch );
         ch->substate = ch->tempnum;
         return;
      case SUB_ROOM_EXTRA:
	ed = ( EXTRA_DESCR_DATA* ) ch->dest_buf;
         if( !ed )
         {
            bug( "redit: sub_room_extra: NULL ch->dest_buf", 0 );
            stop_editing( ch );
            return;
         }
         STRFREE( ed->description );
         ed->description = copy_buffer( ch );
         stop_editing( ch );
         ch->substate = ch->tempnum;
         return;
   }

   location = ch->in_room;

   argument = smash_tilde_static( argument );
   argument = one_argument( argument, arg );
   if( ch->substate == SUB_REPEATCMD )
   {
      if( arg[0] == '\0' )
      {
         do_rstat( ch, "" );
         return;
      }
      if( !str_cmp( arg, "done" ) || !str_cmp( arg, "off" ) )
      {
         send_to_char( "Redit mode off.\r\n", ch );
         if( ch->pcdata && ch->pcdata->subprompt )
            STRFREE( ch->pcdata->subprompt );
         ch->substate = SUB_NONE;
         return;
      }
   }
   if( arg[0] == '\0' || !str_cmp( arg, "?" ) )
   {
      if( ch->substate == SUB_REPEATCMD )
         send_to_char( "Syntax: <field> value\r\n", ch );
      else
         send_to_char( "Syntax: redit <field> value\r\n", ch );
      send_to_char( "\r\n", ch );
      send_to_char( "Field being one of:\r\n", ch );
      send_to_char( "  name desc ed rmed\r\n", ch );
      send_to_char( "  exit bexit exdesc exflags exname exkey\r\n", ch );
      send_to_char( "  flags sector teledelay televnum tunnel\r\n", ch );
      send_to_char( "  rlist exdistance\r\n", ch );
      return;
   }

   if( !can_rmodify( ch, location ) )
      return;

   if( !str_cmp( arg, "on" ) )
   {
      CHECK_SUBRESTRICTED( ch );
      send_to_char( "Redit mode on.\r\n", ch );
      ch->substate = SUB_REPEATCMD;
      if( ch->pcdata )
      {
         if( ch->pcdata->subprompt )
            STRFREE( ch->pcdata->subprompt );
         ch->pcdata->subprompt = STRALLOC( "<&CRedit &W#%r&w> %i" );
      }
      return;
   }

   if( !str_cmp( arg, "name" ) )
   {
      if( argument[0] == '\0' )
      {
         send_to_char( "Set the room name.  A very brief single line room description.\r\n", ch );
         send_to_char( "Usage: redit name <Room summary>\r\n", ch );
         return;
      }
      STRFREE( location->name );
      location->name = STRALLOC( argument );
      return;
   }

   if( !str_cmp( arg, "desc" ) )
   {
      if( ch->substate == SUB_REPEATCMD )
         ch->tempnum = SUB_REPEATCMD;
      else
         ch->tempnum = SUB_NONE;
      ch->substate = SUB_ROOM_DESC;
      ch->dest_buf = location;
      start_editing( ch, (char *)location->description );
      return;
   }

   if( !str_cmp( arg, "tunnel" ) )
   {
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "Set the maximum characters allowed in the room at one time. (0 = unlimited).\r\n", ch );
         send_to_char( "Usage: redit tunnel <value>\r\n", ch );
         return;
      }
      location->tunnel = URANGE( 0, atoi( argument ), 1000 );
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg, "ed" ) )
   {
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "Create an extra description.\r\n", ch );
         send_to_char( "You must supply keyword(s).\r\n", ch );
         return;
      }
      CHECK_SUBRESTRICTED( ch );
      ed = SetRExtra( location, argument );
      if( ch->substate == SUB_REPEATCMD )
         ch->tempnum = SUB_REPEATCMD;
      else
         ch->tempnum = SUB_NONE;
      ch->substate = SUB_ROOM_EXTRA;
      ch->dest_buf = ed;
      start_editing( ch, (char *)ed->description );
      return;
   }

   if( !str_cmp( arg, "rmed" ) )
   {
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "Remove an extra description.\r\n", ch );
         send_to_char( "You must supply keyword(s).\r\n", ch );
         return;
      }
      if( DelRExtra( location, argument ) )
         send_to_char( "Deleted.\r\n", ch );
      else
         send_to_char( "Not found.\r\n", ch );
      return;
   }

   if( !str_cmp( arg, "rlist" ) )
   {
      RESET_DATA *pReset;
      char *rbuf;
      short num;

      if( !location->first_reset )
      {
         send_to_char( "This room has no resets to list.\r\n", ch );
         return;
      }
      num = 0;
      for( pReset = location->first_reset; pReset; pReset = pReset->next )
      {
         num++;
         if( !( rbuf = sprint_reset( pReset, &num ) ) )
            continue;
         send_to_char( rbuf, ch );
      }
      return;
   }

   if( !str_cmp( arg, "flags" ) )
   {
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "Toggle the room flags.\r\n", ch );
         send_to_char( "Usage: redit flags <flag> [flag]...\r\n", ch );
         send_to_char( "\r\nPossible Flags: \r\n", ch );
         send_to_char( "dark, nomob, indoors, nomagic, bank,\r\n", ch );
         send_to_char( "private, safe, petshop, norecall, donation, nodropall, silence,\r\n", ch );
         send_to_char( "logspeach, nodrop, clanstoreroom, plr_home, empty_home, teleport\r\n", ch );
         send_to_char( "nofloor, prototype, refinery, factory, republic_recruit, empire_recruit\r\n", ch );
         send_to_char( "spacecraft, auction, no_drive, can_land, can_fly, hotel\r\n", ch );
         return;
      }
      while( argument[0] != '\0' )
      {
         argument = one_argument( argument, arg2 );
         value = get_rflag( arg2 );
         if( value < 0 || value > 31 )
            ch_printf( ch, "Unknown flag: %s\r\n", arg2 );
         else if( 1 << value == ROOM_PLR_HOME && get_trust( ch ) < LEVEL_SUPREME )
            send_to_char( "If you want to build a player home use the 'empty_home' flag instead.\r\n", ch );
         else
         {
            TOGGLE_BIT( location->room_flags, 1 << value );
         }
      }
      return;
   }

   if( !str_cmp( arg, "teledelay" ) )
   {
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "Set the delay of the teleport. (0 = off).\r\n", ch );
         send_to_char( "Usage: redit teledelay <value>\r\n", ch );
         return;
      }
      location->tele_delay = atoi( argument );
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg, "televnum" ) )
   {
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "Set the vnum of the room to teleport to.\r\n", ch );
         send_to_char( "Usage: redit televnum <vnum>\r\n", ch );
         return;
      }
      location->tele_vnum = atoi( argument );
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg, "sector" ) )
   {
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "Set the sector type.\r\n", ch );
         send_to_char( "Usage: redit sector <value>\r\n", ch );
         send_to_char( "\r\nSector Values:\r\n", ch );
         send_to_char( "0:dark, 1:city, 2:field, 3:forest, 4:hills, 5:mountain, 6:water_swim\r\n", ch );
         send_to_char( "7:water_noswim, 8:underwater, 9:air, 10:desert, 11:unkown, 12:oceanfloor, 13:underground\r\n", ch );

         return;
      }
      location->sector_type = atoi( argument );
      if( location->sector_type < 0 || location->sector_type >= SECT_MAX )
      {
         location->sector_type = 1;
         send_to_char( "Out of range\r\n.", ch );
      }
      else
         send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg, "exkey" ) )
   {
      argument = one_argument( argument, arg2 );
      argument = one_argument( argument, arg3 );
      if( arg2[0] == '\0' || arg3[0] == '\0' )
      {
         send_to_char( "Usage: redit exkey <dir> <key vnum>\r\n", ch );
         return;
      }
      if( arg2[0] == '#' )
      {
         edir = atoi( arg2 + 1 );
         xit = get_exit_num( location, edir );
      }
      else
      {
         edir = get_dir( arg2 );
         xit = get_exit( location, edir );
      }
      value = atoi( arg3 );
      if( !xit )
      {
         send_to_char( "No exit in that direction.  Use 'redit exit ...' first.\r\n", ch );
         return;
      }
      xit->key = value;
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg, "exname" ) )
   {
      argument = one_argument( argument, arg2 );
      if( arg2[0] == '\0' )
      {
         send_to_char( "Change or clear exit keywords.\r\n", ch );
         send_to_char( "Usage: redit exname <dir> [keywords]\r\n", ch );
         return;
      }
      if( arg2[0] == '#' )
      {
         edir = atoi( arg2 + 1 );
         xit = get_exit_num( location, edir );
      }
      else
      {
         edir = get_dir( arg2 );
         xit = get_exit( location, edir );
      }
      if( !xit )
      {
         send_to_char( "No exit in that direction.  Use 'redit exit ...' first.\r\n", ch );
         return;
      }
      STRFREE( xit->keyword );
      xit->keyword = STRALLOC( argument );
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg, "exflags" ) )
   {
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "Toggle or display exit flags.\r\n", ch );
         send_to_char( "Usage: redit exflags <dir> <flag> [flag]...\r\n", ch );
         send_to_char( "\r\nExit flags:\r\n", ch );
         send_to_char( "isdoor, closed, locked, can_look, searchable, can_leave, can_climb,\r\n", ch );
         send_to_char( "nopassdoor, secret, pickproof, fly, climb, dig, window, auto, can_enter\r\n", ch );
         send_to_char( "hidden, no_mob, bashproof, bashed\r\n", ch );

         return;
      }
      argument = one_argument( argument, arg2 );
      if( arg2[0] == '#' )
      {
         edir = atoi( arg2 + 1 );
         xit = get_exit_num( location, edir );
      }
      else
      {
         edir = get_dir( arg2 );
         xit = get_exit( location, edir );
      }
      if( !xit )
      {
         send_to_char( "No exit in that direction.  Use 'redit exit ...' first.\r\n", ch );
         return;
      }
      if( argument[0] == '\0' )
      {
         sprintf( buf, "Flags for exit direction: %d  Keywords: %s  Key: %d\r\n[ ", xit->vdir, xit->keyword, xit->key );
         for( value = 0; value <= MAX_EXFLAG; value++ )
         {
            if( IS_SET( xit->exit_info, 1 << value ) )
            {
               strcat( buf, ex_flags[value] );
               strcat( buf, " " );
            }
         }
         strcat( buf, "]\r\n" );
         send_to_char( buf, ch );
         return;
      }
      while( argument[0] != '\0' )
      {
         argument = one_argument( argument, arg2 );
         value = get_exflag( arg2 );
         if( value < 0 || value > MAX_EXFLAG )
            ch_printf( ch, "Unknown flag: %s\r\n", arg2 );
         else
            TOGGLE_BIT( xit->exit_info, 1 << value );
      }
      return;
   }

   if( !str_cmp( arg, "exit" ) )
   {
      bool addexit, numnotdir;

      argument = one_argument( argument, arg2 );
      argument = one_argument( argument, arg3 );
      if( arg2[0] == '\0' )
      {
         send_to_char( "Create, change or remove an exit.\r\n", ch );
         send_to_char( "Usage: redit exit <dir> [room] [flags] [key] [keywords]\r\n", ch );
         return;
      }
      addexit = numnotdir = FALSE;
      switch ( arg2[0] )
      {
         default:
            edir = get_dir( arg2 );
            break;
         case '+':
            edir = get_dir( arg2 + 1 );
            addexit = TRUE;
            break;
         case '#':
            edir = atoi( arg2 + 1 );
            numnotdir = TRUE;
            break;
      }
      if( arg3[0] == '\0' )
         evnum = 0;
      else
         evnum = atoi( arg3 );
      if( numnotdir )
      {
         if( ( xit = get_exit_num( location, edir ) ) != NULL )
            edir = xit->vdir;
      }
      else
         xit = get_exit( location, edir );
      if( !evnum )
      {
         if( xit )
         {
            extract_exit( location, xit );
            send_to_char( "Exit removed.\r\n", ch );
            return;
         }
         send_to_char( "No exit in that direction.\r\n", ch );
         return;
      }
      if( evnum < 1 || evnum > 2147483647 )
      {
         send_to_char( "Invalid room number.\r\n", ch );
         return;
      }
      if( ( tmp = get_room_index( evnum ) ) == NULL )
      {
         send_to_char( "Non-existant room.\r\n", ch );
         return;
      }
      if( get_trust( ch ) <= LEVEL_IMMORTAL && tmp->area != location->area )
      {
         send_to_char( "You can't make an exit to that room.\r\n", ch );
         return;
      }
      if( addexit || !xit )
      {
         if( numnotdir )
         {
            send_to_char( "Cannot add an exit by number, sorry.\r\n", ch );
            return;
         }
         if( addexit && xit && get_exit_to( location, edir, tmp->vnum ) )
         {
            send_to_char( "There is already an exit in that direction leading to that location.\r\n", ch );
            return;
         }
         xit = make_exit( location, tmp, edir );
         xit->keyword = STRALLOC( "" );
         xit->description = STRALLOC( "" );
         xit->key = -1;
         xit->exit_info = 0;
         act( AT_IMMORT, "$n reveals a hidden passage!", ch, NULL, NULL, TO_ROOM );
      }
      else
         act( AT_IMMORT, "Something is different...", ch, NULL, NULL, TO_ROOM );
      if( xit->to_room != tmp )
      {
         xit->to_room = tmp;
         xit->vnum = evnum;
         texit = get_exit_to( xit->to_room, rev_dir[edir], location->vnum );
         if( texit )
         {
            texit->rexit = xit;
            xit->rexit = texit;
         }
      }
      argument = one_argument( argument, arg3 );
      if( arg3[0] != '\0' )
         xit->exit_info = atoi( arg3 );
      if( argument && argument[0] != '\0' )
      {
         one_argument( argument, arg3 );
         ekey = atoi( arg3 );
         if( ekey != 0 || arg3[0] == '0' )
         {
            argument = one_argument( argument, arg3 );
            xit->key = ekey;
         }
         if( argument && argument[0] != '\0' )
         {
            STRFREE( xit->keyword );
            xit->keyword = STRALLOC( argument );
         }
      }
      send_to_char( "Done.\r\n", ch );
      return;
   }

   /*
    * Twisted and evil, but works           -Thoric
    * Makes an exit, and the reverse in one shot.
    */
   if( !str_cmp( arg, "bexit" ) )
   {
      EXIT_DATA *nxit, *rxit;
      char tmpcmd[MAX_INPUT_LENGTH];
      ROOM_INDEX_DATA *tmploc;
      int vnum, exnum;
      char rvnum[MAX_INPUT_LENGTH];
      bool numnotdir;

      argument = one_argument( argument, arg2 );
      argument = one_argument( argument, arg3 );
      if( arg2[0] == '\0' )
      {
         send_to_char( "Create, change or remove a two-way exit.\r\n", ch );
         send_to_char( "Usage: redit bexit <dir> [room] [flags] [key] [keywords]\r\n", ch );
         return;
      }
      numnotdir = FALSE;
      switch ( arg2[0] )
      {
         default:
            edir = get_dir( arg2 );
            break;
         case '#':
            numnotdir = TRUE;
            edir = atoi( arg2 + 1 );
            break;
         case '+':
            edir = get_dir( arg2 + 1 );
            break;
      }
      tmploc = location;
      exnum = edir;
      if( numnotdir )
      {
         if( ( nxit = get_exit_num( tmploc, edir ) ) != NULL )
            edir = nxit->vdir;
      }
      else
         nxit = get_exit( tmploc, edir );
      rxit = NULL;
      vnum = 0;
      rvnum[0] = '\0';
      if( nxit )
      {
         vnum = nxit->vnum;
         if( arg3[0] != '\0' )
            sprintf( rvnum, "%d", tmploc->vnum );
         if( nxit->to_room )
            rxit = get_exit( nxit->to_room, rev_dir[edir] );
         else
            rxit = NULL;
      }
      sprintf( tmpcmd, "exit %s %s %s", arg2, arg3, argument );
      do_redit( ch, tmpcmd );
      if( numnotdir )
         nxit = get_exit_num( tmploc, exnum );
      else
         nxit = get_exit( tmploc, edir );
      if( !rxit && nxit )
      {
         vnum = nxit->vnum;
         if( arg3[0] != '\0' )
            sprintf( rvnum, "%d", tmploc->vnum );
         if( nxit->to_room )
            rxit = get_exit( nxit->to_room, rev_dir[edir] );
         else
            rxit = NULL;
      }
      if( vnum )
      {
         sprintf( tmpcmd, "%d redit exit %d %s %s", vnum, rev_dir[edir], rvnum, argument );
         do_at( ch, tmpcmd );
      }
      return;
   }

   if( !str_cmp( arg, "exdistance" ) )
   {
      argument = one_argument( argument, arg2 );
      if( arg2[0] == '\0' )
      {
         send_to_char( "Set the distance (in rooms) between this room, and the destination room.\r\n", ch );
         send_to_char( "Usage: redit exdistance <dir> [distance]\r\n", ch );
         return;
      }
      if( arg2[0] == '#' )
      {
         edir = atoi( arg2 + 1 );
         xit = get_exit_num( location, edir );
      }
      else
      {
         edir = get_dir( arg2 );
         xit = get_exit( location, edir );
      }
      if( xit )
      {
         xit->distance = URANGE( 1, atoi( argument ), 50 );
         send_to_char( "Done.\r\n", ch );
         return;
      }
      send_to_char( "No exit in that direction.  Use 'redit exit ...' first.\r\n", ch );
      return;
   }

   if( !str_cmp( arg, "exdesc" ) )
   {
      argument = one_argument( argument, arg2 );
      if( arg2[0] == '\0' )
      {
         send_to_char( "Create or clear a description for an exit.\r\n", ch );
         send_to_char( "Usage: redit exdesc <dir> [description]\r\n", ch );
         return;
      }
      if( arg2[0] == '#' )
      {
         edir = atoi( arg2 + 1 );
         xit = get_exit_num( location, edir );
      }
      else
      {
         edir = get_dir( arg2 );
         xit = get_exit( location, edir );
      }
      if( xit )
      {
         STRFREE( xit->description );
         if( !argument || argument[0] == '\0' )
            xit->description = STRALLOC( "" );
         else
         {
            sprintf( buf, "%s\r\n", argument );
            xit->description = STRALLOC( buf );
         }
         send_to_char( "Done.\r\n", ch );
         return;
      }
      send_to_char( "No exit in that direction.  Use 'redit exit ...' first.\r\n", ch );
      return;
   }

   /*
    * Generate usage message.
    */
   if( ch->substate == SUB_REPEATCMD )
   {
      ch->substate = SUB_RESTRICTED;
      interpret( ch, origarg );
      ch->substate = SUB_REPEATCMD;
      ch->last_cmd = do_redit;
   }
   else
      do_redit( ch, "" );
   return;
}

void do_ocreate( CHAR_DATA * ch, const char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   OBJ_INDEX_DATA *pObjIndex;
   OBJ_DATA *obj;
   int vnum, cvnum;

   if( IS_NPC( ch ) )
   {
      send_to_char( "Mobiles cannot create.\r\n", ch );
      return;
   }

   argument = one_argument( argument, arg );

   vnum = is_number( arg ) ? atoi( arg ) : -1;

   if( vnum == -1 || !argument || argument[0] == '\0' )
   {
      send_to_char( "Usage: ocreate <vnum> [copy vnum] <item name>\r\n", ch );
      return;
   }

   if( vnum < 1 || vnum > 2147483647 )
   {
      send_to_char( "Bad number.\r\n", ch );
      return;
   }

   one_argument( argument, arg2 );
   cvnum = atoi( arg2 );
   if( cvnum != 0 )
      argument = one_argument( argument, arg2 );
   if( cvnum < 1 )
      cvnum = 0;

   if( get_obj_index( vnum ) )
   {
      send_to_char( "An object with that number already exists.\r\n", ch );
      return;
   }

   if( IS_NPC( ch ) )
      return;
   if( get_trust( ch ) <= LEVEL_IMMORTAL )
   {
      AREA_DATA *pArea;

      if( !ch->pcdata || !( pArea = ch->pcdata->area ) )
      {
         send_to_char( "You must have an assigned area to create objects.\r\n", ch );
         return;
      }
      if( vnum < pArea->low_o_vnum || vnum > pArea->hi_o_vnum )
      {
         send_to_char( "That number is not in your allocated range.\r\n", ch );
         return;
      }
   }
   if( !is_valid_vnum( vnum, VCHECK_OBJ ) )
   {
      ch_printf( ch, "Sorry, %d IS NOT a valid vnum!\r\n", vnum );
      return;
   }

   pObjIndex = make_object( vnum, cvnum, argument );
   if( !pObjIndex )
   {
      send_to_char( "Error.\r\n", ch );
      log_string( "do_ocreate: make_object failed." );
      return;
   }
   obj = create_object( pObjIndex, get_trust( ch ) );
   obj_to_char( obj, ch );
   act( AT_IMMORT, "$n makes some ancient arcane gestures, and opens $s hands to reveal $p!", ch, obj, NULL, TO_ROOM );
   act( AT_IMMORT, "You make some ancient arcane gestures, and open your hands to reveal $p!", ch, obj, NULL, TO_CHAR );
}

void do_mcreate( CHAR_DATA * ch, const char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   MOB_INDEX_DATA *pMobIndex;
   CHAR_DATA *mob;
   int vnum, cvnum;

   if( IS_NPC( ch ) )
   {
      send_to_char( "Mobiles cannot create.\r\n", ch );
      return;
   }

   argument = one_argument( argument, arg );

   vnum = is_number( arg ) ? atoi( arg ) : -1;

   if( vnum == -1 || !argument || argument[0] == '\0' )
   {
      send_to_char( "Usage: mcreate <vnum> [cvnum] <mobile name>\r\n", ch );
      return;
   }

   if( vnum < 1 || vnum > 2147483647 )
   {
      send_to_char( "Bad number.\r\n", ch );
      return;
   }

   one_argument( argument, arg2 );
   cvnum = atoi( arg2 );
   if( cvnum != 0 )
      argument = one_argument( argument, arg2 );
   if( cvnum < 1 )
      cvnum = 0;

   if( get_mob_index( vnum ) )
   {
      send_to_char( "A mobile with that number already exists.\r\n", ch );
      return;
   }

   if( IS_NPC( ch ) )
      return;
   if( get_trust( ch ) <= LEVEL_IMMORTAL )
   {
      AREA_DATA *pArea;

      if( !ch->pcdata || !( pArea = ch->pcdata->area ) )
      {
         send_to_char( "You must have an assigned area to create mobiles.\r\n", ch );
         return;
      }
      if( vnum < pArea->low_m_vnum || vnum > pArea->hi_m_vnum )
      {
         send_to_char( "That number is not in your allocated range.\r\n", ch );
         return;
      }
   }
   if( !is_valid_vnum( vnum, VCHECK_MOB ) )
   {
      ch_printf( ch, "Sorry, %d IS NOT a valid vnum!\r\n", vnum );
      return;
   }

   pMobIndex = make_mobile( vnum, cvnum, argument );
   if( !pMobIndex )
   {
      send_to_char( "Error.\r\n", ch );
      log_string( "do_mcreate: make_mobile failed." );
      return;
   }
   mob = create_mobile( pMobIndex );
   char_to_room( mob, ch->in_room );
   act( AT_IMMORT, "$n waves $s arms about, and $N appears at $s command!", ch, NULL, mob, TO_ROOM );
   act( AT_IMMORT, "You wave your arms about, and $N appears at your command!", ch, NULL, mob, TO_CHAR );
}

void assign_area( CHAR_DATA * ch )
{
   char buf[MAX_STRING_LENGTH];
   char buf2[MAX_STRING_LENGTH];
   char taf[1024];
   AREA_DATA *tarea, *tmp;
   bool created = FALSE;

   if( IS_NPC( ch ) )
      return;
   if( get_trust( ch ) >= LEVEL_AVATAR && ch->pcdata->r_range_lo && ch->pcdata->r_range_hi )
   {
      tarea = ch->pcdata->area;
      sprintf( taf, "%s.are", capitalize( ch->name ) );
      if( !tarea )
      {
         for( tmp = first_build; tmp; tmp = tmp->next )
            if( !str_cmp( taf, tmp->filename ) )
            {
               tarea = tmp;
               break;
            }
      }
      if( !tarea )
      {
         sprintf( buf, "Creating area entry for %s", ch->name );
         log_string_plus( buf, LOG_NORMAL, ch->top_level );
         CREATE( tarea, AREA_DATA, 1 );
         LINK( tarea, first_build, last_build, next, prev );
         tarea->first_room = tarea->last_room = NULL;
         sprintf( buf, "{PROTO} %s's area in progress", ch->name );
         tarea->name = str_dup( buf );
         tarea->filename = str_dup( taf );
         sprintf( buf2, "%s", ch->name );
         tarea->author = STRALLOC( buf2 );
         tarea->age = 0;
         tarea->nplayer = 0;
         created = TRUE;
      }
      else
      {
         sprintf( buf, "Updating area entry for %s", ch->name );
         log_string_plus( buf, LOG_NORMAL, ch->top_level );
      }
      tarea->low_r_vnum = ch->pcdata->r_range_lo;
      tarea->low_o_vnum = ch->pcdata->o_range_lo;
      tarea->low_m_vnum = ch->pcdata->m_range_lo;
      tarea->hi_r_vnum = ch->pcdata->r_range_hi;
      tarea->hi_o_vnum = ch->pcdata->o_range_hi;
      tarea->hi_m_vnum = ch->pcdata->m_range_hi;
      ch->pcdata->area = tarea;
      if( created )
         sort_area( tarea, TRUE );
   }
}

void do_aassign( CHAR_DATA * ch, const char *argument )
{
   char buf[MAX_STRING_LENGTH];
   AREA_DATA *tarea, *tmp;

   if( IS_NPC( ch ) )
      return;

   if( argument[0] == '\0' )
   {
      send_to_char( "Syntax: aassign <filename.are>\r\n", ch );
      return;
   }

   if( !str_cmp( "none", argument ) || !str_cmp( "null", argument ) || !str_cmp( "clear", argument ) )
   {
      ch->pcdata->area = NULL;
      assign_area( ch );
      if( !ch->pcdata->area )
         send_to_char( "Area pointer cleared.\r\n", ch );
      else
         send_to_char( "Originally assigned area restored.\r\n", ch );
      return;
   }

   sprintf( buf, "%s", argument );
   tarea = NULL;

/*	if ( get_trust(ch) >= sysdata.level_modify_proto )   */

   if( get_trust( ch ) >= LEVEL_GREATER
       || ( is_name( buf, ch->pcdata->bestowments ) && get_trust( ch ) >= sysdata.level_modify_proto ) )
      for( tmp = first_area; tmp; tmp = tmp->next )
         if( !str_cmp( buf, tmp->filename ) )
         {
            tarea = tmp;
            break;
         }

   if( !tarea )
      for( tmp = first_build; tmp; tmp = tmp->next )
         if( !str_cmp( buf, tmp->filename ) )
         {
/*		if ( get_trust(ch) >= sysdata.level_modify_proto  */
            if( get_trust( ch ) >= LEVEL_GREATER || is_name( tmp->filename, ch->pcdata->bestowments ) )
            {
               tarea = tmp;
               break;
            }
            else
            {
               send_to_char( "You do not have permission to use that area.\r\n", ch );
               return;
            }
         }

   if( !tarea )
   {
      if( get_trust( ch ) >= sysdata.level_modify_proto )
         send_to_char( "No such area.  Use 'zones'.\r\n", ch );
      else
         send_to_char( "No such area.  Use 'newzones'.\r\n", ch );
      return;
   }
   ch->pcdata->area = tarea;
   ch_printf( ch, "Assigning you: %s\r\n", tarea->name );
   return;
}


EXTRA_DESCR_DATA *SetRExtra( ROOM_INDEX_DATA * room, const char *keywords )
{
   EXTRA_DESCR_DATA *ed;

   for( ed = room->first_extradesc; ed; ed = ed->next )
   {
      if( is_name( keywords, ed->keyword ) )
         break;
   }
   if( !ed )
   {
      CREATE( ed, EXTRA_DESCR_DATA, 1 );
      LINK( ed, room->first_extradesc, room->last_extradesc, next, prev );
      ed->keyword = STRALLOC( keywords );
      ed->description = STRALLOC( "" );
      top_ed++;
   }
   return ed;
}

bool DelRExtra( ROOM_INDEX_DATA * room, const char *keywords )
{
   EXTRA_DESCR_DATA *rmed;

   for( rmed = room->first_extradesc; rmed; rmed = rmed->next )
   {
      if( is_name( keywords, rmed->keyword ) )
         break;
   }
   if( !rmed )
      return FALSE;
   UNLINK( rmed, room->first_extradesc, room->last_extradesc, next, prev );
   STRFREE( rmed->keyword );
   STRFREE( rmed->description );
   DISPOSE( rmed );
   top_ed--;
   return TRUE;
}

EXTRA_DESCR_DATA *SetOExtra( OBJ_DATA * obj, const char *keywords )
{
   EXTRA_DESCR_DATA *ed;

   for( ed = obj->first_extradesc; ed; ed = ed->next )
   {
      if( is_name( keywords, ed->keyword ) )
         break;
   }
   if( !ed )
   {
      CREATE( ed, EXTRA_DESCR_DATA, 1 );
      LINK( ed, obj->first_extradesc, obj->last_extradesc, next, prev );
      ed->keyword = STRALLOC( keywords );
      ed->description = STRALLOC( "" );
      top_ed++;
   }
   return ed;
}

bool DelOExtra( OBJ_DATA * obj, const char *keywords )
{
   EXTRA_DESCR_DATA *rmed;

   for( rmed = obj->first_extradesc; rmed; rmed = rmed->next )
   {
      if( is_name( keywords, rmed->keyword ) )
         break;
   }
   if( !rmed )
      return FALSE;
   UNLINK( rmed, obj->first_extradesc, obj->last_extradesc, next, prev );
   STRFREE( rmed->keyword );
   STRFREE( rmed->description );
   DISPOSE( rmed );
   top_ed--;
   return TRUE;
}

EXTRA_DESCR_DATA *SetOExtraProto( OBJ_INDEX_DATA * obj, const char *keywords )
{
   EXTRA_DESCR_DATA *ed;

   for( ed = obj->first_extradesc; ed; ed = ed->next )
   {
      if( is_name( keywords, ed->keyword ) )
         break;
   }
   if( !ed )
   {
      CREATE( ed, EXTRA_DESCR_DATA, 1 );
      LINK( ed, obj->first_extradesc, obj->last_extradesc, next, prev );
      ed->keyword = STRALLOC( keywords );
      ed->description = STRALLOC( "" );
      top_ed++;
   }
   return ed;
}

bool DelOExtraProto( OBJ_INDEX_DATA * obj, const char *keywords )
{
   EXTRA_DESCR_DATA *rmed;

   for( rmed = obj->first_extradesc; rmed; rmed = rmed->next )
   {
      if( is_name( keywords, rmed->keyword ) )
         break;
   }
   if( !rmed )
      return FALSE;
   UNLINK( rmed, obj->first_extradesc, obj->last_extradesc, next, prev );
   STRFREE( rmed->keyword );
   STRFREE( rmed->description );
   DISPOSE( rmed );
   top_ed--;
   return TRUE;
}

void fwrite_fuss_exdesc( FILE * fpout, EXTRA_DESCR_DATA * ed )
{
   fprintf( fpout, "%s", "#EXDESC\n" );
   fprintf( fpout, "ExDescKey    %s~\n", ed->keyword );
   if( ed->description && ed->description[0] != '\0' )
      fprintf( fpout, "ExDesc       %s~\n", strip_cr( ed->description ) );
   fprintf( fpout, "%s", "#ENDEXDESC\n\n" );
}

void fwrite_fuss_exit( FILE * fpout, EXIT_DATA * pexit )
{
   fprintf( fpout, "%s", "#EXIT\n" );
   fprintf( fpout, "Direction %s~\n", strip_cr( dir_name[pexit->vdir] ) );
   fprintf( fpout, "ToRoom    %d\n", pexit->vnum );
   if( pexit->key != -1 && pexit->key > 0 )
      fprintf( fpout, "Key       %d\n", pexit->key );
   if( pexit->distance > 1 )
      fprintf( fpout, "Distance  %d\n", pexit->distance );
   if( pexit->description && pexit->description[0] != '\0' )
      fprintf( fpout, "Desc      %s~\n", strip_cr( pexit->description ) );
   if( pexit->keyword && pexit->keyword[0] != '\0' )
      fprintf( fpout, "Keywords  %s~\n", strip_cr( pexit->keyword ) );
   if( pexit->exit_info )
      fprintf( fpout, "Flags     %s~\n", flag_string( pexit->exit_info, ex_flags ) );
   fprintf( fpout, "%s", "#ENDEXIT\n\n" );
}

void fwrite_fuss_affect( FILE * fp, AFFECT_DATA * paf )
{
   bool npc = FALSE;
   int top_skill;

   if( paf->from )
      if( IS_NPC( paf->from ) )
         npc = TRUE;

   if( npc )
      top_skill = top_sn;
   else
      top_skill = paf->from ? paf->from->top_sn : -1;

   if( paf->type < 0 || paf->type >= top_skill )
   {
      fprintf( fp, "Affect       %d %f %d %d %d %d %d '%s' %s\n",
               paf->type,
               paf->duration,
               paf->modifier, paf->location, paf->factor_id, paf->affect_type, paf->from_pool ? paf->from_pool->id : 0, paf->from ? paf->from->name : "null", print_bitvector( &paf->bitvector ) );
   }
   else
   {
      fprintf( fp, "AffectData   '%s' %f %d %d %d %d %d '%s' %s\n",
               npc ? skill_table[paf->type]->name : paf->from->pc_skills[paf->type]->name,
               paf->duration,
               paf->modifier, paf->location, paf->factor_id, paf->affect_type, paf->from_pool ? paf->from_pool->id : 0, paf->from ? paf->from->name : saving_char->name, print_bitvector( &paf->bitvector ) );
   }
}

// Write a prog
bool mprog_write_prog( FILE * fpout, MPROG_DATA * mprog )
{
   if( ( mprog->arglist && mprog->arglist[0] != '\0' ) )
   {
      fprintf( fpout, "%s", "#MUDPROG\n" );
      fprintf( fpout, "Progtype  %s~\n", mprog_type_to_name( mprog->type ) );
      fprintf( fpout, "Arglist   %s~\n", mprog->arglist );

      if( mprog->comlist && mprog->comlist[0] != '\0' && !mprog->fileprog )
         fprintf( fpout, "Comlist   %s~\n", strip_cr( mprog->comlist ) );

      fprintf( fpout, "%s", "#ENDPROG\n\n" );
      return true;
   }
   return false;
}

void save_reset_level( FILE * fpout, RESET_DATA * start_reset, const int level )
{
   int spaces = level * 2;

   RESET_DATA *reset;

   for( reset = start_reset; reset; )
   {
      switch ( UPPER( reset->command ) )  /* extra arg1 arg2 arg3 */
      {
         case '*':
            break;

         default:
            fprintf( fpout, "%*.sReset %c %d %d %d %d\n", spaces, "",
                     UPPER( reset->command ), reset->extra, reset->arg1, reset->arg2, reset->arg3 );
            break;

         case 'G':
         case 'R':
            fprintf( fpout, "%*.sReset %c %d %d %d\n", spaces, "",
                     UPPER( reset->command ), reset->extra, reset->arg1, reset->arg2 );
            break;
      }  /* end of switch on command */

      /*
       * recurse to save nested resets 
       */
      save_reset_level( fpout, reset->first_reset, level + 1 );

      /*
       * where we go next depends on if this is a top-level reset or not - for some reason 
       */
      if( level == 0 )
         reset = reset->next;
      else
         reset = reset->next_reset;
   }  /* end of looping through resets */
}  /* end of save_reset_level */

void fwrite_fuss_room( FILE * fpout, ROOM_INDEX_DATA * room, bool install )
{
   EXIT_DATA *xit;
   //AFFECT_DATA *paf;
   EXTRA_DESCR_DATA *ed;
   MPROG_DATA *mprog;

   if( install )
   {
      CHAR_DATA *victim, *vnext;
      OBJ_DATA *obj, *obj_next;

      // remove prototype flag from room 
      REMOVE_BIT( room->room_flags, ROOM_PROTOTYPE );

      // purge room of (prototyped) mobiles 
      for( victim = room->first_person; victim; victim = vnext )
      {
         vnext = victim->next_in_room;
         if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
            extract_char( victim, true );
      }

      // purge room of (prototyped) objects 
      for( obj = room->first_content; obj; obj = obj_next )
      {
         obj_next = obj->next_content;
         if( IS_SET( obj->extra_flags, ITEM_PROTOTYPE ) )
            extract_obj( obj );
      }
   }

   // Get rid of the track markers before saving.
   //REMOVE_BIT( room->room_flags, ROOM_BFS_MARK );

   fprintf( fpout, "%s", "#ROOM\n" );
   fprintf( fpout, "Vnum     %d\n", room->vnum );
   fprintf( fpout, "Name     %s~\n", room->name );
   fprintf( fpout, "Sector   %s~\n", strip_cr( sector_name[room->sector_type] ) );
   if( room->room_flags )
      fprintf( fpout, "Flags    %s~\n", flag_string( room->room_flags, r_flags ) );
   if( room->tele_delay > 0 || room->tele_vnum > 0 || room->tunnel > 0 )
      fprintf( fpout, "Stats    %d %d %d\n", room->tele_delay, room->tele_vnum, room->tunnel );
   if( room->description && room->description[0] != '\0' )
      fprintf( fpout, "Desc     %s~\n", strip_cr( room->description ) );

   for( xit = room->first_exit; xit; xit = xit->next )
   {
      if( IS_SET( xit->exit_info, EX_PORTAL ) ) /* don't fold portals */
         continue;

      fwrite_fuss_exit( fpout, xit );
   }

   save_reset_level( fpout, room->first_reset, 0 );

  // for( paf = room->first_permaffect; paf; paf = paf->next )
   //   fwrite_fuss_affect( fpout, paf );

   for( ed = room->first_extradesc; ed; ed = ed->next )
      fwrite_fuss_exdesc( fpout, ed );

   if( room->mudprogs )
   {
      for( mprog = room->mudprogs; mprog; mprog = mprog->next )
         mprog_write_prog( fpout, mprog );
   }
   fprintf( fpout, "%s", "#ENDROOM\n\n" );
}

void fwrite_fuss_object( FILE * fpout, OBJ_INDEX_DATA * pObjIndex, bool install )
{
   ITEM_MATERIAL *material;
   AFFECT_DATA *paf;
   EXTRA_DESCR_DATA *ed;
   MPROG_DATA *mprog;
   int val0, val1, val2, val3, val4, val5;

   if( install )
      REMOVE_BIT( pObjIndex->extra_flags, ITEM_PROTOTYPE );

   fprintf( fpout, "%s", "#OBJECT\n" );
   fprintf( fpout, "Vnum     %d\n", pObjIndex->vnum );
   fprintf( fpout, "Keywords %s~\n", pObjIndex->name );
   fprintf( fpout, "Type     %s~\n", o_types[pObjIndex->item_type] );
   fprintf( fpout, "Short    %s~\n", pObjIndex->short_descr );
   if( pObjIndex->description && pObjIndex->description[0] != '\0' )
      fprintf( fpout, "Long     %s~\n", pObjIndex->description );
   if( pObjIndex->action_desc && pObjIndex->action_desc[0] != '\0' )
      fprintf( fpout, "Action   %s~\n", pObjIndex->action_desc );
   if( pObjIndex->extra_flags )
      fprintf( fpout, "Flags    %s~\n", flag_string( pObjIndex->extra_flags, o_flags ) );
   if( pObjIndex->wear_flags )
      fprintf( fpout, "WFlags   %s~\n", flag_string( pObjIndex->wear_flags, w_flags ) );

   val0 = pObjIndex->value[0];
   val1 = pObjIndex->value[1];
   val2 = pObjIndex->value[2];
   val3 = pObjIndex->value[3];
   val4 = pObjIndex->value[4];
   val5 = pObjIndex->value[5];

   if( !xIS_EMPTY( pObjIndex->quality ) )
      fprintf( fpout, "Quality   %s~\n", print_bitvector( &pObjIndex->quality ) );

   switch ( pObjIndex->item_type )
   {
      case ITEM_ARMOR:
         fprintf( fpout, "Temper     %s\n", print_bitvector( &pObjIndex->temper ) );
         break;
      case ITEM_WEAPON:
         fprintf( fpout, "Damtype    %s\n", print_bitvector( &pObjIndex->damtype ) );
         fprintf( fpout, "Speed      %f\n", pObjIndex->speed );
         break;
      case ITEM_PILL:
      case ITEM_POTION:
      case ITEM_SCROLL:
         if( IS_VALID_SN( val1 ) )
         {
            val1 = HAS_SPELL_INDEX;
         }
         if( IS_VALID_SN( val2 ) )
         {
            val2 = HAS_SPELL_INDEX;
         }
         if( IS_VALID_SN( val3 ) )
         {
            val3 = HAS_SPELL_INDEX;
         }
         break;

      case ITEM_STAFF:
      case ITEM_WAND:
         if( IS_VALID_SN( val3 ) )
         {
            val3 = HAS_SPELL_INDEX;
         }
         break;
      case ITEM_SALVE:
         if( IS_VALID_SN( val4 ) )
         {
            val4 = HAS_SPELL_INDEX;
         }
         if( IS_VALID_SN( val5 ) )
         {
            val5 = HAS_SPELL_INDEX;
         }
         break;
   }
   fprintf( fpout, "Values   %d %d %d %d %d %d\n", val0, val1, val2, val3, val4, val5 );
   fprintf( fpout, "Stats    %d %d %d %d %d %d\n", pObjIndex->weight,
            pObjIndex->cost, pObjIndex->rent ? pObjIndex->rent : ( int )( pObjIndex->cost / 10 ),
            pObjIndex->level, pObjIndex->layers, pObjIndex->max_pool );

   for( paf = pObjIndex->first_affect; paf; paf = paf->next )
      fwrite_fuss_affect( fpout, paf );

   switch ( pObjIndex->item_type )
   {
      case ITEM_PILL:
      case ITEM_POTION:
      case ITEM_SCROLL:
         fprintf( fpout, "Spells   '%s' '%s' '%s'\n",
                  IS_VALID_SN( pObjIndex->value[1] ) ?
                  skill_table[pObjIndex->value[1]]->name : "NONE",
                  IS_VALID_SN( pObjIndex->value[2] ) ?
                  skill_table[pObjIndex->value[2]]->name : "NONE",
                  IS_VALID_SN( pObjIndex->value[3] ) ? skill_table[pObjIndex->value[3]]->name : "NONE" );
         break;
      case ITEM_STAFF:
      case ITEM_WAND:
         fprintf( fpout, "Spells   '%s'\n",
                  IS_VALID_SN( pObjIndex->value[3] ) ? skill_table[pObjIndex->value[3]]->name : "NONE" );

         break;
      case ITEM_SALVE:
         fprintf( fpout, "Spells   '%s' '%s'\n",
                  IS_VALID_SN( pObjIndex->value[4] ) ?
                  skill_table[pObjIndex->value[4]]->name : "NONE",
                  IS_VALID_SN( pObjIndex->value[5] ) ? skill_table[pObjIndex->value[5]]->name : "NONE" );
         break;
   }

   for( ed = pObjIndex->first_extradesc; ed; ed = ed->next )
      fwrite_fuss_exdesc( fpout, ed );

   for( material = pObjIndex->first_material; material; material = material->next )
      fprintf( fpout, "Material     %d %d\n", material->object->vnum, material->amount );

   if( pObjIndex->mudprogs )
   {
      for( mprog = pObjIndex->mudprogs; mprog; mprog = mprog->next )
         mprog_write_prog( fpout, mprog );
   }

   fprintf( fpout, "%s", "#ENDOBJECT\n\n" );
}

void fwrite_fuss_mobile( FILE * fpout, MOB_INDEX_DATA * pMobIndex, bool install )
{
   AV_QUEST *available_quest;
   TEACH_DATA *teach;
   SHOP_DATA *pShop;
   REPAIR_DATA *pRepair;
   MPROG_DATA *mprog;
   AI_THOUGHT *thought;

   if( install )
      REMOVE_BIT( pMobIndex->act, ACT_PROTOTYPE );

   fprintf( fpout, "%s", "#MOBILE\n" );

   fprintf( fpout, "Vnum       %d\n", pMobIndex->vnum );
   fprintf( fpout, "Keywords   %s~\n", pMobIndex->player_name );
   fprintf( fpout, "Short      %s~\n", pMobIndex->short_descr );
   if( pMobIndex->long_descr && pMobIndex->long_descr[0] != '\0' )
      fprintf( fpout, "Long       %s~\n", strip_cr( pMobIndex->long_descr ) );
   if( pMobIndex->description && pMobIndex->description[0] != '\0' )
      fprintf( fpout, "Desc       %s~\n", strip_cr( pMobIndex->description ) );
   fprintf( fpout, "Race       %s~\n", npc_race[pMobIndex->race] );
   fprintf( fpout, "Position   %s~\n", npc_position[pMobIndex->position] );
   fprintf( fpout, "DefPos     %s~\n", npc_position[pMobIndex->defposition] );
   if( pMobIndex->spec_fun && pMobIndex->spec_funname && pMobIndex->spec_funname[0] != '\0' )
      fprintf( fpout, "Specfun    %s~\n", pMobIndex->spec_funname );
   if( pMobIndex->spec_2 && pMobIndex->spec_funname2 && pMobIndex->spec_funname2[0] != '\0' )
      fprintf( fpout, "Specfun2    %s~\n", pMobIndex->spec_funname2 );
   fprintf( fpout, "Gender     %s~\n", npc_sex[pMobIndex->sex] );
   fprintf( fpout, "Actflags   %s~\n", flag_string( pMobIndex->act, act_flags ) );
   if( !xIS_EMPTY( pMobIndex->affected_by ) )
      fprintf( fpout, "Affected   %s~\n", ext_flag_string( &pMobIndex->affected_by, a_flags ) );
   fprintf( fpout, "Stats1     %d %d %d %d %d %d %d\n", pMobIndex->alignment, pMobIndex->level, pMobIndex->mobthac0,
            pMobIndex->evasion, pMobIndex->armor, pMobIndex->gold, pMobIndex->exp );
   fprintf( fpout, "Stats2     %d %d %d\n", pMobIndex->hitnodice, pMobIndex->hitsizedice, pMobIndex->hitplus );
   fprintf( fpout, "Stats3     %d %d %d\n", pMobIndex->damnodice, pMobIndex->damsizedice, pMobIndex->damplus );
   fprintf( fpout, "Stats4     %d %d %d %d %d\n",
            pMobIndex->height, pMobIndex->weight, pMobIndex->numattacks, pMobIndex->hitroll, pMobIndex->damroll );
   fprintf( fpout, "Stats5     %d %d %f %d\n", pMobIndex->dodge, pMobIndex->parry, pMobIndex->round, pMobIndex->haste );
   fprintf( fpout, "Damtype    %s\n", print_bitvector( &pMobIndex->damtype ) );
   fprintf( fpout, "Attribs    %d %d %d %d %d %d %d %d %d\n",
            pMobIndex->perm_str,
            pMobIndex->perm_int,
            pMobIndex->perm_wis, pMobIndex->perm_dex, pMobIndex->perm_con, pMobIndex->perm_agi, pMobIndex->perm_cha, pMobIndex->perm_lck, pMobIndex->perm_frc );
   {
      int count;
      fprintf( fpout, "Resistance  " );
      for( count = 0; count < MAX_DAMTYPE; count++ )
         fprintf( fpout, " %d", pMobIndex->resistance[count] );
      fprintf( fpout, "\n" );
      fprintf( fpout, "Penetration " );
      for( count = 0; count < MAX_DAMTYPE; count++ )
         fprintf( fpout, " %d", pMobIndex->penetration[count] );
      fprintf( fpout, "\n" );
      fprintf( fpout, "DamtypePotency " );
      for( count = 0; count < MAX_DAMTYPE; count++ )
         fprintf( fpout, " %d", pMobIndex->damtype_potency[count] );
      fprintf( fpout, "\n" );
   }
   fprintf( fpout, "AIStuff    %f\n", pMobIndex->tspeed );
   for( thought = pMobIndex->first_thought; thought; thought = thought->next )
      fprintf( fpout, "Thought    %d\n", thought->id );
   fprintf( fpout, "Saves      %d %d %d %d %d\n",
            pMobIndex->saving_poison_death,
            pMobIndex->saving_wand, pMobIndex->saving_para_petri, pMobIndex->saving_breath, pMobIndex->saving_spell_staff );
   if( pMobIndex->speaks )
      fprintf( fpout, "Speaks     %s~\n", flag_string( pMobIndex->speaks, lang_names_save ) );
   if( pMobIndex->speaking )
      fprintf( fpout, "Speaking   %s~\n", flag_string( pMobIndex->speaking, lang_names_save ) );
   if( pMobIndex->xflags )
      fprintf( fpout, "Bodyparts  %s~\n", flag_string( pMobIndex->xflags, part_flags ) );
   if( pMobIndex->resistant )
      fprintf( fpout, "Resist     %s~\n", flag_string( pMobIndex->resistant, ris_flags ) );
   if( pMobIndex->immune )
      fprintf( fpout, "Immune     %s~\n", flag_string( pMobIndex->immune, ris_flags ) );
   if( pMobIndex->susceptible )
      fprintf( fpout, "Suscept    %s~\n", flag_string( pMobIndex->susceptible, ris_flags ) );
   if( pMobIndex->attacks )
      fprintf( fpout, "Attacks    %s~\n", flag_string( pMobIndex->attacks, attack_flags ) );
   if( pMobIndex->defenses )
      fprintf( fpout, "Defenses   %s~\n", flag_string( pMobIndex->defenses, defense_flags ) );
   if( pMobIndex->vip_flags )
      fprintf( fpout, "VIPFlags   %s~\n", flag_string( pMobIndex->vip_flags, planet_flags ) );

   // Mob has a shop? Add that data to the mob index.
   if( ( pShop = pMobIndex->pShop ) != NULL )
   {
      fprintf( fpout, "ShopData   %d %d %d %d %d %d %d %d %d\n",
               pShop->buy_type[0], pShop->buy_type[1], pShop->buy_type[2], pShop->buy_type[3], pShop->buy_type[4],
               pShop->profit_buy, pShop->profit_sell, pShop->open_hour, pShop->close_hour );
   }

   // Mob is a repair shop? Add that data to the mob index.
   if( ( pRepair = pMobIndex->rShop ) != NULL )
   {
      fprintf( fpout, "RepairData %d %d %d %d %d %d %d\n",
               pRepair->fix_type[0], pRepair->fix_type[1], pRepair->fix_type[2], pRepair->profit_fix, pRepair->shop_type,
               pRepair->open_hour, pRepair->close_hour );
   }

   for( teach = pMobIndex->first_teach; teach; teach = teach->next )
      fprintf( fpout, "TeachData  %d %d\n", teach->disc_id, teach->credits );

   for( available_quest = pMobIndex->first_available_quest; available_quest; available_quest = available_quest->next )
      fprintf( fpout, "AvailableQuest   %d\n", available_quest->quest->id );

   if( pMobIndex->mudprogs )
   {
      for( mprog = pMobIndex->mudprogs; mprog; mprog = mprog->next )
         mprog_write_prog( fpout, mprog );
   }
   if( pMobIndex->first_loot )
   {
      LOOT_DATA *loot;

      for( loot = pMobIndex->first_loot; loot; loot = loot->next )
         fwrite_loot_data( fpout, loot );
   }

   fwrite_skill_data( fpout, pMobIndex );

   fprintf( fpout, "%s", "#ENDMOBILE\n\n" );
}

void fwrite_loot_data( FILE *fpout, LOOT_DATA * loot )
{
   fprintf( fpout, "%s", "#LOOTDATA\n\n" );
   fprintf( fpout, "Vnum        %d\n", loot->vnum );
   fprintf( fpout, "Type        %d\n", loot->type );
   fprintf( fpout, "Percent     %d\n", loot->percent );
   fprintf( fpout, "Amount      %d\n", loot->amount );
   fprintf( fpout, "%s", "#ENDLOOTDATA\n\n" );
   return;
}

void fwrite_skill_data( FILE *fpout, MOB_INDEX_DATA *pMobIndex )
{
   int x;
   fprintf( fpout, "#NPCSKILLS\n" );
   for( x = 0; x < MAX_NPC_SKILL; x++ )
   {
      if( pMobIndex->npc_skills[x] == -1 )
         break;
      fprintf( fpout, "'%s'\n", skill_table[pMobIndex->npc_skills[x]]->name );
   }
   fprintf( fpout, "#ENDNPCSKILLS\n" );

}

void fwrite_area_header( FILE * fpout, AREA_DATA * tarea, bool install )
{
   if( install )
      REMOVE_BIT( tarea->flags, AFLAG_PROTOTYPE );

   fprintf( fpout, "%s", "#AREADATA\n" );
   fprintf( fpout, "Version      %d\n", tarea->version );
   fprintf( fpout, "Name         %s~\n", tarea->name );
   fprintf( fpout, "Author       %s~\n", tarea->author );
   fprintf( fpout, "Ranges       %d %d %d %d\n",
            tarea->low_soft_range, tarea->hi_soft_range, tarea->low_hard_range, tarea->hi_hard_range );
   if( tarea->high_economy || tarea->low_economy )
      fprintf( fpout, "Economy      %d %d\n", tarea->high_economy, tarea->low_economy );
   if( tarea->resetmsg )
      fprintf( fpout, "ResetMsg     %s~\n", tarea->resetmsg );
   if( tarea->reset_frequency )
      fprintf( fpout, "ResetFreq    %d\n", tarea->reset_frequency );
   if( tarea->flags )
      fprintf( fpout, "Flags        %s~\n", flag_string( tarea->flags, area_flags ) );

   fprintf( fpout, "%s", "#ENDAREADATA\n\n" );
}

void fold_area( AREA_DATA * tarea, const char *fname, bool install )
{
   char buf[256];
   FILE *fpout;
   MOB_INDEX_DATA *pMobIndex;
   OBJ_INDEX_DATA *pObjIndex;
   ROOM_INDEX_DATA *pRoomIndex;
   LOOT_DATA *loot;
   int vnum;

   snprintf( buf, 256, "%s.bak", fname );
   rename( fname, buf );
   if( !( fpout = fopen( fname, "w" ) ) )
   {
      bug( "%s: fopen", __FUNCTION__ );
      perror( fname );
      return;
   }

   tarea->version = AREA_VERSION_WRITE;

   fprintf( fpout, "%s", "#FUSSAREA\n" );

   fwrite_area_header( fpout, tarea, install );

   for( vnum = tarea->low_m_vnum; vnum <= tarea->hi_m_vnum; ++vnum )
   {
      if( !( pMobIndex = get_mob_index( vnum ) ) )
         continue;
      fwrite_fuss_mobile( fpout, pMobIndex, install );
   }

   for( vnum = tarea->low_o_vnum; vnum <= tarea->hi_o_vnum; ++vnum )
   {
      if( !( pObjIndex = get_obj_index( vnum ) ) )
         continue;
      fwrite_fuss_object( fpout, pObjIndex, install );
   }

   for( vnum = tarea->low_r_vnum; vnum <= tarea->hi_r_vnum; ++vnum )
   {
      if( !( pRoomIndex = get_room_index( vnum ) ) )
         continue;
      fwrite_fuss_room( fpout, pRoomIndex, install );
   }

   for( loot = tarea->first_loot; loot; loot = loot->next )
      fwrite_loot_data( fpout, loot );

   fprintf( fpout, "%s", "#ENDAREA\n" );
   fclose( fpout );
   fpout = NULL;
   return;
}

void old_fold_area( AREA_DATA * tarea, const char *filename, bool install )
{
   RESET_DATA *pReset, *tReset, *gReset;
   ROOM_INDEX_DATA *room;
   MOB_INDEX_DATA *pMobIndex;
   OBJ_INDEX_DATA *pObjIndex;
   MPROG_DATA *mprog;
   EXIT_DATA *xit;
   EXTRA_DESCR_DATA *ed;
   AFFECT_DATA *paf;
   SHOP_DATA *pShop;
   REPAIR_DATA *pRepair;
   char buf[MAX_STRING_LENGTH];
   FILE *fpout;
   int vnum;
   int val0, val1, val2, val3, val4, val5;
   bool complexmob;

   sprintf( buf, "Saving %s...", tarea->filename );
   log_string_plus( buf, LOG_NORMAL, LEVEL_GREATER );

   sprintf( buf, "%s.bak", filename );
   rename( filename, buf );
   if( ( fpout = fopen( filename, "w" ) ) == NULL )
   {
      bug( "fold_area: fopen", 0 );
      perror( filename );
      return;
   }

   if( install )
      REMOVE_BIT( tarea->flags, AFLAG_PROTOTYPE );
   fprintf( fpout, "#AREA   %s~\n\n\n\n", tarea->name );

   fprintf( fpout, "#AUTHOR %s~\n\n", tarea->author );
   fprintf( fpout, "#RANGES\n" );
   fprintf( fpout, "%d %d %d %d\n", tarea->low_soft_range,
            tarea->hi_soft_range, tarea->low_hard_range, tarea->hi_hard_range );
   fprintf( fpout, "$\n\n" );
   if( tarea->resetmsg )   /* Rennard */
      fprintf( fpout, "#RESETMSG %s~\n\n", tarea->resetmsg );
   if( tarea->reset_frequency )
      fprintf( fpout, "#FLAGS\n%d %d\n\n", tarea->flags, tarea->reset_frequency );
   else
      fprintf( fpout, "#FLAGS\n%d\n\n", tarea->flags );

   fprintf( fpout, "#ECONOMY %d %d\n\n", tarea->high_economy, tarea->low_economy );

   /*
    * save mobiles 
    */
   fprintf( fpout, "#MOBILES\n" );
   for( vnum = tarea->low_m_vnum; vnum <= tarea->hi_m_vnum; vnum++ )
   {
      if( ( pMobIndex = get_mob_index( vnum ) ) == NULL )
         continue;
      if( install )
         REMOVE_BIT( pMobIndex->act, ACT_PROTOTYPE );
      if( pMobIndex->perm_str != 13 || pMobIndex->perm_int != 13
          || pMobIndex->perm_wis != 13 || pMobIndex->perm_dex != 13
          || pMobIndex->perm_con != 13 || pMobIndex->perm_cha != 13
          || pMobIndex->perm_lck != 13
          || pMobIndex->hitroll != 0 || pMobIndex->damroll != 0
          || pMobIndex->race != 0
          || pMobIndex->attacks != 0 || pMobIndex->defenses != 0
          || pMobIndex->height != 0 || pMobIndex->weight != 0
          || pMobIndex->speaks != 0 || pMobIndex->speaking != 0
          || pMobIndex->xflags != 0 || pMobIndex->numattacks != 1 || pMobIndex->vip_flags != 0 )
         complexmob = TRUE;
      else
         complexmob = FALSE;
      fprintf( fpout, "#%d\n", vnum );
      fprintf( fpout, "%s~\n", pMobIndex->player_name );
      fprintf( fpout, "%s~\n", pMobIndex->short_descr );
      fprintf( fpout, "%s~\n", strip_cr( pMobIndex->long_descr ) );
      fprintf( fpout, "%s~\n", strip_cr( pMobIndex->description ) );
      fprintf( fpout, "%d %s %d %c\n", pMobIndex->act,
               print_bitvector( &pMobIndex->affected_by ), pMobIndex->alignment, complexmob ? 'Z' : 'S' );
      /*
       * C changed to Z for swreality vip_flags  
       */

      fprintf( fpout, "%d %d %d %d", pMobIndex->level, pMobIndex->mobthac0, pMobIndex->evasion, pMobIndex->armor );
      fprintf( fpout, "%dd%d+%d ", pMobIndex->hitnodice, pMobIndex->hitsizedice, pMobIndex->hitplus );
      fprintf( fpout, "%dd%d+%d\n", pMobIndex->damnodice, pMobIndex->damsizedice, pMobIndex->damplus );
      fprintf( fpout, "%d 0\n", pMobIndex->gold );
      fprintf( fpout, "%d %d %d\n", pMobIndex->position, pMobIndex->defposition, pMobIndex->sex );
      if( complexmob )
      {
         fprintf( fpout, "%d %d %d %d %d %d %d\n",
                  pMobIndex->perm_str,
                  pMobIndex->perm_int,
                  pMobIndex->perm_wis, pMobIndex->perm_dex, pMobIndex->perm_con, pMobIndex->perm_cha, pMobIndex->perm_lck );
         fprintf( fpout, "%d %d %d %d %d\n",
                  pMobIndex->saving_poison_death,
                  pMobIndex->saving_wand,
                  pMobIndex->saving_para_petri, pMobIndex->saving_breath, pMobIndex->saving_spell_staff );
         fprintf( fpout, "%d 0 %d %d %d %d %d\n",
                  pMobIndex->race,
                  pMobIndex->height, pMobIndex->weight, pMobIndex->speaks, pMobIndex->speaking, pMobIndex->numattacks );
         fprintf( fpout, "%d %d %d %d %d %d %d %d\n",
                  pMobIndex->hitroll,
                  pMobIndex->damroll,
                  pMobIndex->xflags,
                  pMobIndex->resistant, pMobIndex->immune, pMobIndex->susceptible, pMobIndex->attacks, pMobIndex->defenses );
         fprintf( fpout, "%d 0 0 0 0 0 0 0\n", pMobIndex->vip_flags );
      }
      if( pMobIndex->mudprogs )
      {
         int count = 0;
         for( mprog = pMobIndex->mudprogs; mprog; mprog = mprog->next )
         {
            if( ( mprog->arglist && mprog->arglist[0] != '\0' ) )
            {
               if( mprog->type == IN_FILE_PROG )
               {
                  fprintf( fpout, "> %s %s~\n", mprog_type_to_name( mprog->type ), mprog->arglist );
                  count++;
               }
               // Don't let it save progs which came from files. That would be silly.
               else if( mprog->comlist && mprog->comlist[0] != '\0' && !mprog->fileprog )
               {
                  fprintf( fpout, "> %s %s~\n%s~\n", mprog_type_to_name( mprog->type ),
                     mprog->arglist, strip_cr( mprog->comlist ) );
                  count++;
               }
            }
         }
         if( count > 0 )
            fprintf( fpout, "%s", "|\n" );
      }
   }
   fprintf( fpout, "#0\n\n\n" );
   if( install && vnum < tarea->hi_m_vnum )
      tarea->hi_m_vnum = vnum - 1;

   /*
    * save objects 
    */
   fprintf( fpout, "#OBJECTS\n" );
   for( vnum = tarea->low_o_vnum; vnum <= tarea->hi_o_vnum; vnum++ )
   {
      if( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
         continue;
      if( install )
         REMOVE_BIT( pObjIndex->extra_flags, ITEM_PROTOTYPE );
      fprintf( fpout, "#%d\n", vnum );
      fprintf( fpout, "%s~\n", pObjIndex->name );
      fprintf( fpout, "%s~\n", pObjIndex->short_descr );
      fprintf( fpout, "%s~\n", pObjIndex->description );
      fprintf( fpout, "%s~\n", pObjIndex->action_desc );
      if( pObjIndex->layers )
         fprintf( fpout, "%d %d %d %d\n", pObjIndex->item_type,
                  pObjIndex->extra_flags, pObjIndex->wear_flags, pObjIndex->layers );
      else
         fprintf( fpout, "%d %d %d\n", pObjIndex->item_type, pObjIndex->extra_flags, pObjIndex->wear_flags );

      val0 = pObjIndex->value[0];
      val1 = pObjIndex->value[1];
      val2 = pObjIndex->value[2];
      val3 = pObjIndex->value[3];
      val4 = pObjIndex->value[4];
      val5 = pObjIndex->value[5];
      switch ( pObjIndex->item_type )
      {
         case ITEM_PILL:
         case ITEM_POTION:
         case ITEM_SCROLL:
            if( IS_VALID_SN( val1 ) )
               val1 = skill_table[val1]->slot;
            if( IS_VALID_SN( val2 ) )
               val2 = skill_table[val2]->slot;
            if( IS_VALID_SN( val3 ) )
               val3 = skill_table[val3]->slot;
            break;
         case ITEM_DEVICE:
            if( IS_VALID_SN( val3 ) )
               val3 = skill_table[val3]->slot;
            break;
         case ITEM_SALVE:
            if( IS_VALID_SN( val4 ) )
               val4 = skill_table[val4]->slot;
            if( IS_VALID_SN( val5 ) )
               val5 = skill_table[val5]->slot;
            break;
      }
      if( val4 || val5 )
         fprintf( fpout, "%d %d %d %d %d %d\n", val0, val1, val2, val3, val4, val5 );
      else
         fprintf( fpout, "%d %d %d %d\n", val0, val1, val2, val3 );

      fprintf( fpout, "%d %d %d\n", pObjIndex->weight,
               pObjIndex->cost, pObjIndex->rent ? pObjIndex->rent : ( int )( pObjIndex->cost / 10 ) );

      for( ed = pObjIndex->first_extradesc; ed; ed = ed->next )
         fprintf( fpout, "E\n%s~\n%s~\n", ed->keyword, strip_cr( ed->description ) );

      for( paf = pObjIndex->first_affect; paf; paf = paf->next )
         fprintf( fpout, "A\n%d %d\n", paf->location,
                  ( ( paf->location == APPLY_WEAPONSPELL
                      || paf->location == APPLY_WEARSPELL
                      || paf->location == APPLY_REMOVESPELL
                      || paf->location == APPLY_STRIPSN )
                    && IS_VALID_SN( paf->modifier ) ) ? skill_table[paf->modifier]->slot : paf->modifier );

      if( pObjIndex->mudprogs )
      {
         int count = 0;
         for( mprog = pObjIndex->mudprogs; mprog; mprog = mprog->next )
         {
            if( ( mprog->arglist && mprog->arglist[0] != '\0' ) )
            {
               if( mprog->type == IN_FILE_PROG )
               {
                  fprintf( fpout, "> %s %s~\n", mprog_type_to_name( mprog->type ), mprog->arglist );
                  count++;
               }
               // Don't let it save progs which came from files. That would be silly.
               else if( mprog->comlist && mprog->comlist[0] != '\0' && !mprog->fileprog )
               {
                  fprintf( fpout, "> %s %s~\n%s~\n", mprog_type_to_name( mprog->type ),
                     mprog->arglist, strip_cr( mprog->comlist ) );
                  count++;
               }
            }
         }
         if( count > 0 )
            fprintf( fpout, "%s", "|\n" );
      }
   }
   fprintf( fpout, "#0\n\n\n" );
   if( install && vnum < tarea->hi_o_vnum )
      tarea->hi_o_vnum = vnum - 1;

   /*
    * save rooms   
    */
   fprintf( fpout, "#ROOMS\n" );
   for( vnum = tarea->low_r_vnum; vnum <= tarea->hi_r_vnum; vnum++ )
   {
      if( ( room = get_room_index( vnum ) ) == NULL )
         continue;
      if( install )
      {
         CHAR_DATA *victim, *vnext;
         OBJ_DATA *obj, *obj_next;

         /*
          * remove prototype flag from room 
          */
         REMOVE_BIT( room->room_flags, ROOM_PROTOTYPE );
         /*
          * purge room of (prototyped) mobiles 
          */
         for( victim = room->first_person; victim; victim = vnext )
         {
            vnext = victim->next_in_room;
            if( IS_NPC( victim ) && IS_SET( victim->act, ACT_PROTOTYPE ) )
               extract_char( victim, TRUE );
         }
         /*
          * purge room of (prototyped) objects 
          */
         for( obj = room->first_content; obj; obj = obj_next )
         {
            obj_next = obj->next_content;
            if( IS_SET( obj->extra_flags, ITEM_PROTOTYPE ) )
               extract_obj( obj );
         }
      }
      fprintf( fpout, "#%d\n", vnum );
      fprintf( fpout, "%s~\n", room->name );
      fprintf( fpout, "%s~\n", strip_cr( room->description ) );
      if( ( room->tele_delay > 0 && room->tele_vnum > 0 ) || room->tunnel > 0 )
         fprintf( fpout, "0 %d %d %d %d %d\n", room->room_flags,
                  room->sector_type, room->tele_delay, room->tele_vnum, room->tunnel );
      else
         fprintf( fpout, "0 %d %d\n", room->room_flags, room->sector_type );
      for( xit = room->first_exit; xit; xit = xit->next )
      {
         if( IS_SET( xit->exit_info, EX_PORTAL ) ) /* don't fold portals */
            continue;
         fprintf( fpout, "D%d\n", xit->vdir );
         fprintf( fpout, "%s~\n", strip_cr( xit->description ) );
         fprintf( fpout, "%s~\n", strip_cr( xit->keyword ) );
         if( xit->distance > 1 )
            fprintf( fpout, "%d %d %d %d\n", xit->exit_info & ~EX_BASHED, xit->key, xit->vnum, xit->distance );
         else
            fprintf( fpout, "%d %d %d\n", xit->exit_info & ~EX_BASHED, xit->key, xit->vnum );
      }

      for( pReset = room->first_reset; pReset; pReset = pReset->next )
      {
	   switch( pReset->command ) /* extra arg1 arg2 arg3 */
	   {
	      default:  case '*': break;
	      case 'm': case 'M':
	      case 'o': case 'O':
		   fprintf( fpout, "R %c %d %d %d %d\n", UPPER( pReset->command ),
		      pReset->extra, pReset->arg1, pReset->arg2, pReset->arg3 );
            
               for( tReset = pReset->first_reset; tReset; tReset = tReset->next_reset )
               {
                     switch( tReset->command )
                     {
                        case 'p':
                        case 'P':
                        case 'e':
                        case 'E':
                           fprintf( fpout, "  R %c %d %d %d %d\n", UPPER( tReset->command ),
                              tReset->extra, tReset->arg1, tReset->arg2, tReset->arg3 );
                           if( tReset->first_reset )
                           {
                              for( gReset = tReset->first_reset; gReset; gReset = gReset->next_reset )
                              {
                                 if( gReset->command != 'p' && gReset->command != 'P' )
                                    continue;
                                 fprintf( fpout, "    R %c %d %d %d %d\n", UPPER( gReset->command ),
                                    gReset->extra, gReset->arg1, gReset->arg2, gReset->arg3 );
                              }
                           }
                           break;

                        case 'g':
                        case 'G':
                           fprintf( fpout, "  R %c %d %d %d\n", UPPER( tReset->command ),
                              tReset->extra, tReset->arg1, tReset->arg2 );
                           if( tReset->first_reset )
                           {
                              for( gReset = tReset->first_reset; gReset; gReset = gReset->next_reset )
                              {
                                 if( gReset->command != 'p' && gReset->command != 'P' )
                                    continue;
                                 fprintf( fpout, "    R %c %d %d %d %d\n", UPPER( gReset->command ),
                                    gReset->extra, gReset->arg1, gReset->arg2, gReset->arg3 );
                              }
                           }
                        break;

                        case 't':
                        case 'T':
                        case 'h':
                        case 'H':
                           fprintf( fpout, "  R %c %d %d %d %d\n", UPPER( tReset->command ),
                              tReset->extra, tReset->arg1, tReset->arg2, tReset->arg3 );
                           break;
                     }
               }
		   break;

	      case 'd': case 'D':
	      case 't': case 'T':
	  	case 'h': case 'H':
		   fprintf( fpout, "R %c %d %d %d %d\n", UPPER( pReset->command ),
		      pReset->extra, pReset->arg1, pReset->arg2, pReset->arg3 );
		   break;

	      case 'r': case 'R':
		   fprintf( fpout, "R %c %d %d %d\n", UPPER( pReset->command ), pReset->extra, pReset->arg1, pReset->arg2 );
		   break;
	   }
      }

      for( ed = room->first_extradesc; ed; ed = ed->next )
         fprintf( fpout, "E\n%s~\n%s~\n", ed->keyword, strip_cr( ed->description ) );

      if( room->mudprogs )
      {
         int count = 0;
         for( mprog = room->mudprogs; mprog; mprog = mprog->next )
         {
            if( ( mprog->arglist && mprog->arglist[0] != '\0' ) )
            {
               if( mprog->type == IN_FILE_PROG )
               {
                  fprintf( fpout, "> %s %s~\n", mprog_type_to_name( mprog->type ), mprog->arglist );
                  count++;
               }
               // Don't let it save progs which came from files. That would be silly.
               else if( mprog->comlist && mprog->comlist[0] != '\0' && !mprog->fileprog )
               {
                  fprintf( fpout, "> %s %s~\n%s~\n", mprog_type_to_name( mprog->type ),
                     mprog->arglist, strip_cr( mprog->comlist ) );
                  count++;
               }
            }
         }
         if( count > 0 )
            fprintf( fpout, "%s", "|\n" );
      }
      fprintf( fpout, "S\n" );
   }
   fprintf( fpout, "#0\n\n\n" );
   if( install && vnum < tarea->hi_r_vnum )
      tarea->hi_r_vnum = vnum - 1;

   /*
    * save shops 
    */
   fprintf( fpout, "#SHOPS\n" );
   for( vnum = tarea->low_m_vnum; vnum <= tarea->hi_m_vnum; vnum++ )
   {
      if( ( pMobIndex = get_mob_index( vnum ) ) == NULL )
         continue;
      if( ( pShop = pMobIndex->pShop ) == NULL )
         continue;
      fprintf( fpout, " %d   %2d %2d %2d %2d %2d   %3d %3d",
               pShop->keeper,
               pShop->buy_type[0],
               pShop->buy_type[1],
               pShop->buy_type[2], pShop->buy_type[3], pShop->buy_type[4], pShop->profit_buy, pShop->profit_sell );
      fprintf( fpout, "        %2d %2d    ; %s\n", pShop->open_hour, pShop->close_hour, pMobIndex->short_descr );
   }
   fprintf( fpout, "0\n\n\n" );

   /*
    * save repair shops 
    */
   fprintf( fpout, "#REPAIRS\n" );
   for( vnum = tarea->low_m_vnum; vnum <= tarea->hi_m_vnum; vnum++ )
   {
      if( ( pMobIndex = get_mob_index( vnum ) ) == NULL )
         continue;
      if( ( pRepair = pMobIndex->rShop ) == NULL )
         continue;
      fprintf( fpout, " %d   %2d %2d %2d         %3d %3d",
               pRepair->keeper,
               pRepair->fix_type[0], pRepair->fix_type[1], pRepair->fix_type[2], pRepair->profit_fix, pRepair->shop_type );
      fprintf( fpout, "        %2d %2d    ; %s\n", pRepair->open_hour, pRepair->close_hour, pMobIndex->short_descr );
   }
   fprintf( fpout, "0\n\n\n" );

   /*
    * save specials 
    */
   fprintf( fpout, "%s", "#SPECIALS\n" );
   for( vnum = tarea->low_m_vnum; vnum <= tarea->hi_m_vnum; vnum++ )
   {
      if( ( pMobIndex = get_mob_index( vnum ) ) != NULL )
      {
         if( pMobIndex->spec_fun )
	      fprintf( fpout, "M  %d %s\n", pMobIndex->vnum, pMobIndex->spec_funname );
         if( pMobIndex->spec_2 )
	      fprintf( fpout, "M  %d %s\n", pMobIndex->vnum, pMobIndex->spec_funname2 );
      }
   }
   fprintf( fpout, "S\n\n\n" );

   /*
    * END 
    */
   fprintf( fpout, "#$\n" );
   fclose( fpout );
   return;
}

void do_savearea( CHAR_DATA * ch, const char *argument )
{
   AREA_DATA *tarea;
   char filename[256];

   if( IS_NPC( ch ) || get_trust( ch ) < LEVEL_AVATAR || !ch->pcdata || ( argument[0] == '\0' && !ch->pcdata->area ) )
   {
      send_to_char( "You don't have an assigned area to save.\r\n", ch );
      return;
   }

   if( argument[0] == '\0' )
      tarea = ch->pcdata->area;
   else
   {
      bool found;

      if( get_trust( ch ) < LEVEL_GOD )
      {
         send_to_char( "You can only save your own area.\r\n", ch );
         return;
      }
      for( found = FALSE, tarea = first_build; tarea; tarea = tarea->next )
         if( !str_cmp( tarea->filename, argument ) )
         {
            found = TRUE;
            break;
         }
      if( !found )
      {
         send_to_char( "Area not found.\r\n", ch );
         return;
      }
   }

   if( !tarea )
   {
      send_to_char( "No area to save.\r\n", ch );
      return;
   }

/* Ensure not wiping out their area with save before load - Scryn 8/11 */
   if( !IS_SET( tarea->status, AREA_LOADED ) )
   {
      send_to_char( "Your area is not loaded!\r\n", ch );
      return;
   }

   sprintf( filename, "%s%s", BUILD_DIR, tarea->filename );
   fold_area( tarea, filename, FALSE );
   send_to_char( "Done.\r\n", ch );
}

void do_loadarea( CHAR_DATA * ch, const char *argument )
{
   AREA_DATA *tarea;
   char filename[256];
   int tmp;

   if( IS_NPC( ch ) || get_trust( ch ) < LEVEL_AVATAR || !ch->pcdata || ( argument[0] == '\0' && !ch->pcdata->area ) )
   {
      send_to_char( "You don't have an assigned area to load.\r\n", ch );
      return;
   }

   if( argument[0] == '\0' )
      tarea = ch->pcdata->area;
   else
   {
      bool found;

      if( get_trust( ch ) < LEVEL_GOD )
      {
         send_to_char( "You can only load your own area.\r\n", ch );
         return;
      }
      for( found = FALSE, tarea = first_build; tarea; tarea = tarea->next )
         if( !str_cmp( tarea->filename, argument ) )
         {
            found = TRUE;
            break;
         }
      if( !found )
      {
         send_to_char( "Area not found.\r\n", ch );
         return;
      }
   }

   if( !tarea )
   {
      send_to_char( "No area to load.\r\n", ch );
      return;
   }

/* Stops char from loading when already loaded - Scryn 8/11 */
   if( IS_SET( tarea->status, AREA_LOADED ) )
   {
      send_to_char( "Your area is already loaded.\r\n", ch );
      return;
   }
   sprintf( filename, "%s%s", BUILD_DIR, tarea->filename );
   send_to_char( "Loading...\r\n", ch );
   load_area_file( tarea, filename );
   send_to_char( "Linking exits...\r\n", ch );
   fix_area_exits( tarea );
   if( tarea->first_room )
   {
      tmp = tarea->nplayer;
      tarea->nplayer = 0;
      send_to_char( "Resetting area...\r\n", ch );
      reset_area( tarea );
      tarea->nplayer = tmp;
   }
   send_to_char( "Done.\r\n", ch );
}

/*
 * Dangerous command.  Can be used to install an area that was either:
 *   (a) already installed but removed from area.lst
 *   (b) designed offline
 * The mud will likely crash if:
 *   (a) this area is already loaded
 *   (b) it contains vnums that exist
 *   (c) the area has errors
 *
 * NOTE: Use of this command is not recommended.		-Thoric
 */
void do_unfoldarea( CHAR_DATA * ch, const char *argument )
{

   if( !argument || argument[0] == '\0' )
   {
      send_to_char( "Unfold what?\r\n", ch );
      return;
   }

   fBootDb = TRUE;
   load_area_file( last_area, argument );
   fBootDb = FALSE;
   return;
}


void do_foldarea( CHAR_DATA * ch, const char *argument )
{
   AREA_DATA *tarea;
   char arg[MAX_INPUT_LENGTH];

   argument = one_argument( argument, arg );
   if( arg[0] == '\0' )
   {
      send_to_char( "Usage: foldarea <filename> [remproto]\r\n", ch );
      return;
   }

   if( !str_cmp( arg, "all" ) )
   {
      for( tarea = first_area; tarea; tarea = tarea->next )
         fold_area( tarea, tarea->filename, FALSE );

      set_char_color( AT_IMMORT, ch );
      send_to_char( "Folding completed.\r\n", ch );
      return;
   }

   for( tarea = first_area; tarea; tarea = tarea->next )
   {
      if( !str_cmp( tarea->filename, arg ) )
      {
         send_to_char( "Folding...\r\n", ch );
         if( !strcmp( argument, "remproto" ) )
            fold_area( tarea, tarea->filename, TRUE );
         else
            fold_area( tarea, tarea->filename, FALSE );
         send_to_char( "Done.\r\n", ch );
         return;
      }
   }
   send_to_char( "No such area exists.\r\n", ch );
   return;
}

extern int top_area;

void write_area_list( void )
{
   AREA_DATA *tarea;
   FILE *fpout;

   fpout = fopen( AREA_LIST, "w" );
   if( !fpout )
   {
      bug( "FATAL: cannot open area.lst for writing!\r\n", 0 );
      return;
   }
   fprintf( fpout, "help.are\n" );
   for( tarea = first_area; tarea; tarea = tarea->next )
      fprintf( fpout, "%s\n", tarea->filename );
   fprintf( fpout, "$\n" );
   fclose( fpout );
}

/*
 * A complicated to use command as it currently exists.		-Thoric
 * Once area->author and area->name are cleaned up... it will be easier
 */
void do_installarea( CHAR_DATA * ch, const char *argument )
{
   AREA_DATA *tarea;
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   int num;
   DESCRIPTOR_DATA *d;

   argument = one_argument( argument, arg );
   if( arg[0] == '\0' )
   {
      send_to_char( "Syntax: installarea <filename> [Area title]\r\n", ch );
      return;
   }

   for( tarea = first_build; tarea; tarea = tarea->next )
   {
      if( !str_cmp( tarea->filename, arg ) )
      {
         if( argument && argument[0] != '\0' )
         {
            DISPOSE( tarea->name );
            tarea->name = str_dup( argument );
         }

         /*
          * Fold area with install flag -- auto-removes prototype flags 
          */
         send_to_char( "Saving and installing file...\r\n", ch );
         fold_area( tarea, tarea->filename, TRUE );

         /*
          * Remove from prototype area list 
          */
         UNLINK( tarea, first_build, last_build, next, prev );

         /*
          * Add to real area list 
          */
         LINK( tarea, first_area, last_area, next, prev );

         /*
          * Remove it from the prototype sort list. BUGFIX: Samson 4-15-03 
          */
         UNLINK( tarea, first_bsort, last_bsort, next_sort, prev_sort );

         /*
          * Sort the area into it's proper sort list. BUGFIX: Samson 4-15-03 
          */
         sort_area( tarea, FALSE );
         sort_area_by_name( tarea );

         /*
          * Fix up author if online 
          */
         for( d = first_descriptor; d; d = d->next )
            if( d->character && d->character->pcdata && d->character->pcdata->area == tarea )
            {
               /*
                * remove area from author 
                */
               d->character->pcdata->area = NULL;
               /*
                * clear out author vnums  
                */
               d->character->pcdata->r_range_lo = 0;
               d->character->pcdata->r_range_hi = 0;
               d->character->pcdata->o_range_lo = 0;
               d->character->pcdata->o_range_hi = 0;
               d->character->pcdata->m_range_lo = 0;
               d->character->pcdata->m_range_hi = 0;
            }

         top_area++;
         send_to_char( "Writing area.lst...\r\n", ch );
         write_area_list(  );
         send_to_char( "Resetting new area.\r\n", ch );
         num = tarea->nplayer;
         tarea->nplayer = 0;
         reset_area( tarea );
         tarea->nplayer = num;
         send_to_char( "Renaming author's building file.\r\n", ch );
         sprintf( buf, "%s%s.installed", BUILD_DIR, tarea->filename );
         sprintf( arg, "%s%s", BUILD_DIR, tarea->filename );
         rename( arg, buf );
         send_to_char( "Done.\r\n", ch );
         return;
      }
   }
   send_to_char( "No such area exists.\r\n", ch );
   return;
}

void do_astat( CHAR_DATA * ch, const char *argument )
{
   AREA_DATA *tarea;
   bool proto, found;


   found = FALSE;
   proto = FALSE;
   for( tarea = first_area; tarea; tarea = tarea->next )
      if( !str_cmp( tarea->filename, argument ) )
      {
         found = TRUE;
         break;
      }

   if( !found )
      for( tarea = first_build; tarea; tarea = tarea->next )
         if( !str_cmp( tarea->filename, argument ) )
         {
            found = TRUE;
            proto = TRUE;
            break;
         }

   if( !found )
   {
      if( argument && argument[0] != '\0' )
      {
         send_to_char( "Area not found.  Check 'zones'.\r\n", ch );
         return;
      }
      else
      {
         tarea = ch->in_room->area;
      }
   }

   ch_printf( ch, "Name: %s\r\nFilename: %-20s  Prototype: %s\r\n", tarea->name, tarea->filename, proto ? "yes" : "no" );
   if( !proto )
   {
      ch_printf( ch, "Max players: %d  IllegalPks: %d  Credits Looted: %d\r\n",
                 tarea->max_players, tarea->illegal_pk, tarea->gold_looted );
      if( tarea->high_economy )
         ch_printf( ch, "Area economy: %d billion and %d credits.\r\n", tarea->high_economy, tarea->low_economy );
      else
         ch_printf( ch, "Area economy: %d credits.\r\n", tarea->low_economy );
      if( tarea->planet )
         ch_printf( ch, "Planet: %s.\r\n", tarea->planet->name );
      ch_printf( ch, "Mdeaths: %d  Mkills: %d  Pdeaths: %d  Pkills: %d\r\n",
                 tarea->mdeaths, tarea->mkills, tarea->pdeaths, tarea->pkills );
   }
   ch_printf( ch, "Author: %s\r\nAge: %d   Number of players: %d\r\n", tarea->author, tarea->age, tarea->nplayer );
   ch_printf( ch, "Area flags: %s\r\n", flag_string( tarea->flags, area_flags ) );
   ch_printf( ch, "low_room: %5d  hi_room: %d\r\n", tarea->low_r_vnum, tarea->hi_r_vnum );
   ch_printf( ch, "low_obj : %5d  hi_obj : %d\r\n", tarea->low_o_vnum, tarea->hi_o_vnum );
   ch_printf( ch, "low_mob : %5d  hi_mob : %d\r\n", tarea->low_m_vnum, tarea->hi_m_vnum );
   ch_printf( ch, "soft range: %d - %d.  hard range: %d - %d.\r\n",
              tarea->low_soft_range, tarea->hi_soft_range, tarea->low_hard_range, tarea->hi_hard_range );
   ch_printf( ch, "Resetmsg: %s\r\n", tarea->resetmsg ? tarea->resetmsg : "(default)" );  /* Rennard */
   ch_printf( ch, "Reset frequency: %d minutes.\r\n", tarea->reset_frequency ? tarea->reset_frequency : 15 );
}

/* check other areas for a conflict while ignoring the current area */
bool check_for_area_conflicts( AREA_DATA *carea, int lo, int hi )
{
   AREA_DATA *area;

   for( area = first_area; area; area = area->next )
      if( area != carea && check_area_conflict( area, lo, hi ) )
         return TRUE;
   for( area = first_build; area; area = area->next )
      if( area != carea && check_area_conflict( area, lo, hi ) )
         return TRUE;

   return FALSE;
}

void do_aset( CHAR_DATA * ch, const char *argument )
{
   AREA_DATA *tarea;
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char arg3[MAX_INPUT_LENGTH];
   bool proto, found;
   int vnum, value;

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   vnum = atoi( argument );

   if( arg1[0] == '\0' || arg2[0] == '\0' )
   {
      send_to_char( "Usage: aset <area filename> <field> <value>\r\n", ch );
      send_to_char( "\r\nField being one of:\r\n", ch );
      send_to_char( "  low_room hi_room low_obj hi_obj low_mob hi_mob\r\n", ch );
      send_to_char( "  name filename low_soft hi_soft low_hard hi_hard\r\n", ch );
      send_to_char( "  author resetmsg resetfreq flags planet\r\n", ch );
      return;
   }

   found = FALSE;
   proto = FALSE;
   for( tarea = first_area; tarea; tarea = tarea->next )
      if( !str_cmp( tarea->filename, arg1 ) )
      {
         found = TRUE;
         break;
      }

   if( !found )
      for( tarea = first_build; tarea; tarea = tarea->next )
         if( !str_cmp( tarea->filename, arg1 ) )
         {
            found = TRUE;
            proto = TRUE;
            break;
         }

   if( !found )
   {
      send_to_char( "Area not found.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "name" ) )
   {
      AREA_DATA *uarea;

      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "You can't set an area's name to nothing.\r\n", ch );
         return;
      }

      for( uarea = first_area; uarea; uarea = uarea->next )
      {
         if( !str_cmp( uarea->name, argument ) )
         {
            send_to_char( "There is already an installed area with that name.\r\n", ch );
            return;
         }
      }

      for( uarea = first_build; uarea; uarea = uarea->next )
      {
         if( !str_cmp( uarea->name, argument ) )
         {
            send_to_char( "There is already a prototype area with that name.\r\n", ch );
            return;
         }
      }
      DISPOSE( tarea->name );
      tarea->name = str_dup( argument );
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "planet" ) )
   {
      PLANET_DATA *planet = get_planet( argument );
      if( planet )
      {
         if( tarea->planet )
         {
            PLANET_DATA *old_planet;

            old_planet = tarea->planet;
            UNLINK( tarea, old_planet->first_area, old_planet->last_area, next_on_planet, prev_on_planet );
         }
         tarea->planet = planet;
         LINK( tarea, planet->first_area, planet->last_area, next_on_planet, prev_on_planet );
         save_planet( planet );
      }
      return;
   }

   if( !str_cmp( arg2, "filename" ) )
   {
      char filename[256];

      if( proto )
      {
         send_to_char( "You should only change the filename of installed areas.\r\n", ch );
         return;
      }

      if( !is_valid_filename( ch, "", argument ) )
         return;

      strncpy( filename, tarea->filename, 256 );
      DISPOSE( tarea->filename );
      tarea->filename = str_dup( argument );
      rename( filename, tarea->filename );
      write_area_list(  );
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "low_economy" ) )
   {
      tarea->low_economy = vnum;
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "high_economy" ) )
   {
      tarea->high_economy = vnum;
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "low_room" ) )
   {
      if( check_for_area_conflicts( tarea, tarea->low_r_vnum, vnum ) )
      {
         send_to_char( "That would conflict with another area.\r\n", ch );
         return;
      }
      if( tarea->hi_r_vnum < vnum )
      {
         send_to_char( "Can't set low_vnum higher than the hi_vnum.\r\n", ch );
         return;
      }
      tarea->low_r_vnum = vnum;
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "hi_room" ) )
   {
      if( check_for_area_conflicts( tarea, tarea->hi_r_vnum, vnum ) )
      {
         send_to_char( "That would conflict with another area.\r\n", ch );
         return;
      }
      if( tarea->low_r_vnum > vnum )
      {
         send_to_char( "Can't set low_vnum lower than the low_vnum.\r\n", ch );
         return;
      }
      tarea->hi_r_vnum = vnum;
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "low_obj" ) )
   {
      if( check_for_area_conflicts( tarea, tarea->low_o_vnum, vnum ) )
      {
         send_to_char( "That would conflict with another area.\r\n", ch );
         return;
      }
      if( tarea->hi_o_vnum < vnum )
      {
         send_to_char( "Can't set low_obj higher than the hi_obj.\r\n", ch );
         return;
      }
      tarea->low_o_vnum = vnum;
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "hi_obj" ) )
   {
      if( check_for_area_conflicts( tarea, tarea->hi_o_vnum, vnum ) )
      {
         send_to_char( "That would conflict with another area.\r\n", ch );
         return;
      }
      if( tarea->low_o_vnum > vnum )
      {
         send_to_char( "Can't set hi_obj lower than the low_obj.\r\n", ch );
         return;
      }
      tarea->hi_o_vnum = vnum;
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "low_mob" ) )
   {
      if( check_for_area_conflicts( tarea, tarea->low_m_vnum, vnum ) )
      {
         send_to_char( "That would conflict with another area.\r\n", ch );
         return;
      }
      if( tarea->hi_m_vnum < vnum )
      {
         send_to_char( "Can't set low_mob higher than the hi_mob.\r\n", ch );
         return;
      }
      tarea->low_m_vnum = vnum;
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "hi_mob" ) )
   {
      if( check_for_area_conflicts( tarea, tarea->hi_m_vnum, vnum ) )
      {
         send_to_char( "That would conflict with another area.\r\n", ch );
         return;
      }
      if( tarea->low_m_vnum > vnum )
      {
         send_to_char( "Can't set hi_mob lower than the low_mob.\r\n", ch );
         return;
      }
      tarea->hi_m_vnum = vnum;
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "low_soft" ) )
   {
      if( vnum < 0 || vnum > MAX_LEVEL )
      {
         send_to_char( "That is not an acceptable value.\r\n", ch );
         return;
      }

      tarea->low_soft_range = vnum;
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "hi_soft" ) )
   {
      if( vnum < 0 || vnum > MAX_LEVEL )
      {
         send_to_char( "That is not an acceptable value.\r\n", ch );
         return;
      }

      tarea->hi_soft_range = vnum;
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "low_hard" ) )
   {
      if( vnum < 0 || vnum > MAX_LEVEL )
      {
         send_to_char( "That is not an acceptable value.\r\n", ch );
         return;
      }

      tarea->low_hard_range = vnum;
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "hi_hard" ) )
   {
      if( vnum < 0 || vnum > MAX_LEVEL )
      {
         send_to_char( "That is not an acceptable value.\r\n", ch );
         return;
      }

      tarea->hi_hard_range = vnum;
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "author" ) )
   {
      STRFREE( tarea->author );
      tarea->author = STRALLOC( argument );
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "resetmsg" ) )
   {
      if( tarea->resetmsg )
         DISPOSE( tarea->resetmsg );
      if( str_cmp( argument, "clear" ) )
         tarea->resetmsg = str_dup( argument );
      send_to_char( "Done.\r\n", ch );
      return;
   }  /* Rennard */

   if( !str_cmp( arg2, "resetfreq" ) )
   {
      tarea->reset_frequency = vnum;
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "flags" ) )
   {
      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "Usage: aset <filename> flags <flag> [flag]...\r\n", ch );
         return;
      }
      while( argument[0] != '\0' )
      {
         argument = one_argument( argument, arg3 );
         value = get_areaflag( arg3 );
         if( value < 0 || value > 31 )
            ch_printf( ch, "Unknown flag: %s\r\n", arg3 );
         else
         {
            if( IS_SET( tarea->flags, 1 << value ) )
               REMOVE_BIT( tarea->flags, 1 << value );
            else
               SET_BIT( tarea->flags, 1 << value );
         }
      }
      return;
   }

   do_aset( ch, "" );
   return;
}


void do_rlist( CHAR_DATA * ch, const char *argument )
{
   ROOM_INDEX_DATA *room;
   int vnum;
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   AREA_DATA *tarea;
   int lrange;
   int trange;

   if( IS_NPC( ch ) || get_trust( ch ) < LEVEL_AVATAR || !ch->pcdata
       || ( !ch->pcdata->area && get_trust( ch ) < LEVEL_GREATER ) )
   {
      send_to_char( "You don't have an assigned area.\r\n", ch );
      return;
   }

   tarea = ch->pcdata->area;
   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if( tarea )
   {
      if( arg1[0] == '\0' )   /* cleaned a big scary mess */
         lrange = tarea->low_r_vnum;   /* here.     -Thoric */
      else
         lrange = atoi( arg1 );
      if( arg2[0] == '\0' )
         trange = tarea->hi_r_vnum;
      else
         trange = atoi( arg2 );

      if( ( lrange < tarea->low_r_vnum || trange > tarea->hi_r_vnum ) && get_trust( ch ) < LEVEL_GREATER )
      {
         send_to_char( "That is out of your vnum range.\r\n", ch );
         return;
      }
   }
   else
   {
      lrange = ( is_number( arg1 ) ? atoi( arg1 ) : 1 );
      trange = ( is_number( arg2 ) ? atoi( arg2 ) : 1 );
   }

   for( vnum = lrange; vnum <= trange; vnum++ )
   {
      if( ( room = get_room_index( vnum ) ) == NULL )
         continue;
      ch_printf( ch, "%5d) %s\r\n", vnum, room->name );
   }
   return;
}

void do_olist( CHAR_DATA * ch, const char *argument )
{
   OBJ_INDEX_DATA *obj;
   int vnum;
   AREA_DATA *tarea;
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   int lrange;
   int trange;

   /*
    * Greater+ can list out of assigned range - Tri (mlist/rlist as well)
    */
   if( IS_NPC( ch ) || get_trust( ch ) < LEVEL_CREATOR || !ch->pcdata
       || ( !ch->pcdata->area && get_trust( ch ) < LEVEL_GREATER ) )
   {
      send_to_char( "You don't have an assigned area.\r\n", ch );
      return;
   }
   tarea = ch->pcdata->area;
   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if( tarea )
   {
      if( arg1[0] == '\0' )   /* cleaned a big scary mess */
         lrange = tarea->low_o_vnum;   /* here.     -Thoric */
      else
         lrange = atoi( arg1 );
      if( arg2[0] == '\0' )
         trange = tarea->hi_o_vnum;
      else
         trange = atoi( arg2 );

      if( ( lrange < tarea->low_o_vnum || trange > tarea->hi_o_vnum ) && get_trust( ch ) < LEVEL_GREATER )
      {
         send_to_char( "That is out of your vnum range.\r\n", ch );
         return;
      }
   }
   else
   {
      lrange = ( is_number( arg1 ) ? atoi( arg1 ) : 1 );
      trange = ( is_number( arg2 ) ? atoi( arg2 ) : 3 );
   }

   for( vnum = lrange; vnum <= trange; vnum++ )
   {
      if( ( obj = get_obj_index( vnum ) ) == NULL )
         continue;
      ch_printf( ch, "%5d) %-20s (%s)\r\n", vnum, obj->name, obj->short_descr );
   }
   return;
}

void do_mlist( CHAR_DATA * ch, const char *argument )
{
   MOB_INDEX_DATA *mob;
   int vnum;
   AREA_DATA *tarea;
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   int lrange;
   int trange;

   if( IS_NPC( ch ) || get_trust( ch ) < LEVEL_CREATOR || !ch->pcdata
       || ( !ch->pcdata->area && get_trust( ch ) < LEVEL_GREATER ) )
   {
      send_to_char( "You don't have an assigned area.\r\n", ch );
      return;
   }

   tarea = ch->pcdata->area;
   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if( tarea )
   {
      if( arg1[0] == '\0' )   /* cleaned a big scary mess */
         lrange = tarea->low_m_vnum;   /* here.     -Thoric */
      else
         lrange = atoi( arg1 );
      if( arg2[0] == '\0' )
         trange = tarea->hi_m_vnum;
      else
         trange = atoi( arg2 );

      if( ( lrange < tarea->low_m_vnum || trange > tarea->hi_m_vnum ) && get_trust( ch ) < LEVEL_GREATER )
      {
         send_to_char( "That is out of your vnum range.\r\n", ch );
         return;
      }
   }
   else
   {
      lrange = ( is_number( arg1 ) ? atoi( arg1 ) : 1 );
      trange = ( is_number( arg2 ) ? atoi( arg2 ) : 1 );
   }

   for( vnum = lrange; vnum <= trange; vnum++ )
   {
      if( ( mob = get_mob_index( vnum ) ) == NULL )
         continue;
      ch_printf( ch, "%5d) %-20s '%s'\r\n", vnum, mob->player_name, mob->short_descr );
   }
}

void mpedit( CHAR_DATA * ch, MPROG_DATA * mprg, int mptype,
	     const char *argument )
{
   if( mptype != -1 )
   {
      mprg->type = mptype;
      if( mprg->arglist )
         STRFREE( mprg->arglist );
      mprg->arglist = STRALLOC( argument );
   }
   ch->substate = SUB_MPROG_EDIT;
   ch->dest_buf = mprg;
   if( !mprg->comlist )
      mprg->comlist = STRALLOC( "" );
   start_editing( ch, (char *)mprg->comlist );
   return;
}

/*
 * Mobprogram editing - cumbersome				-Thoric
 */
void do_mpedit( CHAR_DATA * ch, const char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char arg3[MAX_INPUT_LENGTH];
   char arg4[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   MPROG_DATA *mprog, *mprg, *mprg_next = NULL;
   int value, mptype = 0, cnt;

   if( IS_NPC( ch ) )
   {
      send_to_char( "Mob's can't mpedit\r\n", ch );
      return;
   }

   if( !ch->desc )
   {
      send_to_char( "You have no descriptor\r\n", ch );
      return;
   }

   switch ( ch->substate )
   {
      default:
         break;
      case SUB_RESTRICTED:
         send_to_char( "You can't use this command from within another command.\r\n", ch );
         return;
      case SUB_MPROG_EDIT:
         if( !ch->dest_buf )
         {
            send_to_char( "Fatal error: report to Thoric.\r\n", ch );
            bug( "do_mpedit: sub_mprog_edit: NULL ch->dest_buf", 0 );
            ch->substate = SUB_NONE;
            return;
         }
         mprog = ( MPROG_DATA* ) ch->dest_buf;
         if( mprog->comlist )
            STRFREE( mprog->comlist );
         mprog->comlist = copy_buffer( ch );
         stop_editing( ch );
         return;
   }

   argument = smash_tilde_static( argument );
   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   argument = one_argument( argument, arg3 );
   value = atoi( arg3 );

   if( arg1[0] == '\0' || arg2[0] == '\0' )
   {
      send_to_char( "Syntax: mpedit <victim> <command> [number] <program> <value>\r\n", ch );
      send_to_char( "\r\n", ch );
      send_to_char( "Command being one of:\r\n", ch );
      send_to_char( "  add delete insert edit list\r\n", ch );
      send_to_char( "Program being one of:\r\n", ch );
      send_to_char( "  act speech rand fight hitprcnt greet allgreet\r\n", ch );
      send_to_char( "  entry give bribe death time hour script\r\n", ch );
      return;
   }

   if( get_trust( ch ) < LEVEL_GOD )
   {
      if( ( victim = get_char_room( ch, arg1 ) ) == NULL )
      {
         send_to_char( "They aren't here.\r\n", ch );
         return;
      }
   }
   else
   {
      if( ( victim = get_char_world( ch, arg1 ) ) == NULL )
      {
         send_to_char( "No one like that in all the realms.\r\n", ch );
         return;
      }
   }

   if( get_trust( ch ) < get_trust( victim ) || !IS_NPC( victim ) )
   {
      send_to_char( "You can't do that!\r\n", ch );
      return;
   }

   if( !can_mmodify( ch, victim ) )
      return;

   if( !IS_SET( victim->act, ACT_PROTOTYPE ) )
   {
      send_to_char( "A mobile must have a prototype flag to be mpset.\r\n", ch );
      return;
   }

   mprog = victim->pIndexData->mudprogs;

   set_char_color( AT_GREEN, ch );

   if( !str_cmp( arg2, "list" ) )
   {
      cnt = 0;
      if( !mprog )
      {
         send_to_char( "That mobile has no mob programs.\r\n", ch );
         return;
      }
      for( mprg = mprog; mprg; mprg = mprg->next )
         ch_printf( ch, "%d>%s %s\r\n%s\r\n", ++cnt, mprog_type_to_name( mprg->type ), mprg->arglist, mprg->comlist );
      return;
   }

   if( !str_cmp( arg2, "edit" ) )
   {
      if( !mprog )
      {
         send_to_char( "That mobile has no mob programs.\r\n", ch );
         return;
      }
      argument = one_argument( argument, arg4 );
      if( arg4[0] != '\0' )
      {
         mptype = get_mpflag( arg4 );
         if( mptype == -1 )
         {
            send_to_char( "Unknown program type.\r\n", ch );
            return;
         }
      }
      else
         mptype = -1;
      if( value < 1 )
      {
         send_to_char( "Program not found.\r\n", ch );
         return;
      }
      cnt = 0;
      for( mprg = mprog; mprg; mprg = mprg->next )
      {
         if( ++cnt == value )
         {
            mpedit( ch, mprg, mptype, argument );
            xCLEAR_BITS( victim->pIndexData->progtypes );
            for( mprg = mprog; mprg; mprg = mprg->next )
               xSET_BIT( victim->pIndexData->progtypes, mprg->type );
            return;
         }
      }
      send_to_char( "Program not found.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "delete" ) )
   {
      int num;
      bool found;

      if( !mprog )
      {
         send_to_char( "That mobile has no mob programs.\r\n", ch );
         return;
      }
      argument = one_argument( argument, arg4 );
      if( value < 1 )
      {
         send_to_char( "Program not found.\r\n", ch );
         return;
      }
      cnt = 0;
      found = FALSE;
      for( mprg = mprog; mprg; mprg = mprg->next )
      {
         if( ++cnt == value )
         {
            mptype = mprg->type;
            found = TRUE;
            break;
         }
      }
      if( !found )
      {
         send_to_char( "Program not found.\r\n", ch );
         return;
      }
      cnt = num = 0;
      for( mprg = mprog; mprg; mprg = mprg->next )
         if( IS_SET( mprg->type, mptype ) )
            num++;
      if( value == 1 )
      {
         mprg_next = victim->pIndexData->mudprogs;
         victim->pIndexData->mudprogs = mprg_next->next;
      }
      else
         for( mprg = mprog; mprg; mprg = mprg_next )
         {
            mprg_next = mprg->next;
            if( ++cnt == ( value - 1 ) )
            {
               mprg->next = mprg_next->next;
               break;
            }
         }
      STRFREE( mprg_next->arglist );
      STRFREE( mprg_next->comlist );
      DISPOSE( mprg_next );
      if( num <= 1 )
         xREMOVE_BIT( victim->pIndexData->progtypes, mptype );
      send_to_char( "Program removed.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "insert" ) )
   {
      if( !mprog )
      {
         send_to_char( "That mobile has no mob programs.\r\n", ch );
         return;
      }
      argument = one_argument( argument, arg4 );
      mptype = get_mpflag( arg4 );
      if( mptype == -1 )
      {
         send_to_char( "Unknown program type.\r\n", ch );
         return;
      }
      if( value < 1 )
      {
         send_to_char( "Program not found.\r\n", ch );
         return;
      }
      if( value == 1 )
      {
         CREATE( mprg, MPROG_DATA, 1 );
         xSET_BIT( victim->pIndexData->progtypes, mptype );
         mpedit( ch, mprg, mptype, argument );
         mprg->next = mprog;
         victim->pIndexData->mudprogs = mprg;
         return;
      }
      cnt = 1;
      for( mprg = mprog; mprg; mprg = mprg->next )
      {
         if( ++cnt == value && mprg->next )
         {
            CREATE( mprg_next, MPROG_DATA, 1 );
            xSET_BIT( victim->pIndexData->progtypes, mptype );
            mpedit( ch, mprg_next, mptype, argument );
            mprg_next->next = mprg->next;
            mprg->next = mprg_next;
            return;
         }
      }
      send_to_char( "Program not found.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "add" ) )
   {
      mptype = get_mpflag( arg3 );
      if( mptype == -1 )
      {
         send_to_char( "Unknown program type.\r\n", ch );
         return;
      }
      if( mprog != NULL )
         for( ; mprog->next; mprog = mprog->next );
      CREATE( mprg, MPROG_DATA, 1 );
      if( mprog )
         mprog->next = mprg;
      else
         victim->pIndexData->mudprogs = mprg;
      xSET_BIT( victim->pIndexData->progtypes, mptype );
      mpedit( ch, mprg, mptype, argument );
      mprg->next = NULL;
      return;
   }

   do_mpedit( ch, "" );
}

void do_opedit( CHAR_DATA * ch, const char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char arg3[MAX_INPUT_LENGTH];
   char arg4[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   MPROG_DATA *mprog, *mprg, *mprg_next = NULL;
   int value, mptype = 0, cnt;

   if( IS_NPC( ch ) )
   {
      send_to_char( "Mob's can't opedit\r\n", ch );
      return;
   }

   if( !ch->desc )
   {
      send_to_char( "You have no descriptor\r\n", ch );
      return;
   }

   switch ( ch->substate )
   {
      default:
         break;
      case SUB_RESTRICTED:
         send_to_char( "You can't use this command from within another command.\r\n", ch );
         return;
      case SUB_MPROG_EDIT:
         if( !ch->dest_buf )
         {
            send_to_char( "Fatal error: report to Thoric.\r\n", ch );
            bug( "do_opedit: sub_oprog_edit: NULL ch->dest_buf", 0 );
            ch->substate = SUB_NONE;
            return;
         }
         mprog = ( MPROG_DATA* ) ch->dest_buf;
         if( mprog->comlist )
            STRFREE( mprog->comlist );
         mprog->comlist = copy_buffer( ch );
         stop_editing( ch );
         return;
   }

   argument = smash_tilde_static( argument );
   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   argument = one_argument( argument, arg3 );
   value = atoi( arg3 );

   if( arg1[0] == '\0' || arg2[0] == '\0' )
   {
      send_to_char( "Syntax: opedit <object> <command> [number] <program> <value>\r\n", ch );
      send_to_char( "\r\n", ch );
      send_to_char( "Command being one of:\r\n", ch );
      send_to_char( "  add delete insert edit list\r\n", ch );
      send_to_char( "Program being one of:\r\n", ch );
      send_to_char( "  act speech rand wear remove sac zap get\r\n", ch );
      send_to_char( "  drop damage repair greet exa use\r\n", ch );
      send_to_char( "  pull push (for levers,pullchains,buttons)\r\n", ch );
      send_to_char( "\r\n", ch );
      send_to_char( "Object should be in your inventory to edit.\r\n", ch );
      return;
   }

   if( get_trust( ch ) < LEVEL_GOD )
   {
      if( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
      {
         send_to_char( "You aren't carrying that.\r\n", ch );
         return;
      }
   }
   else
   {
      if( ( obj = get_obj_world( ch, arg1 ) ) == NULL )
      {
         send_to_char( "Nothing like that in all the realms.\r\n", ch );
         return;
      }
   }

   if( !can_omodify( ch, obj ) )
      return;

   if( !IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) )
   {
      send_to_char( "An object must have a prototype flag to be opset.\r\n", ch );
      return;
   }

   mprog = obj->pIndexData->mudprogs;

   set_char_color( AT_GREEN, ch );

   if( !str_cmp( arg2, "list" ) )
   {
      cnt = 0;
      if( !mprog )
      {
         send_to_char( "That object has no obj programs.\r\n", ch );
         return;
      }
      for( mprg = mprog; mprg; mprg = mprg->next )
         ch_printf( ch, "%d>%s %s\r\n%s\r\n", ++cnt, mprog_type_to_name( mprg->type ), mprg->arglist, mprg->comlist );
      return;
   }

   if( !str_cmp( arg2, "edit" ) )
   {
      if( !mprog )
      {
         send_to_char( "That object has no obj programs.\r\n", ch );
         return;
      }
      argument = one_argument( argument, arg4 );
      if( arg4[0] != '\0' )
      {
         mptype = get_mpflag( arg4 );
         if( mptype == -1 )
         {
            send_to_char( "Unknown program type.\r\n", ch );
            return;
         }
      }
      else
         mptype = -1;
      if( value < 1 )
      {
         send_to_char( "Program not found.\r\n", ch );
         return;
      }
      cnt = 0;
      for( mprg = mprog; mprg; mprg = mprg->next )
      {
         if( ++cnt == value )
         {
            mpedit( ch, mprg, mptype, argument );
            xCLEAR_BITS( obj->pIndexData->progtypes );
            for( mprg = mprog; mprg; mprg = mprg->next )
               xSET_BIT( obj->pIndexData->progtypes, mprg->type );
            return;
         }
      }
      send_to_char( "Program not found.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "delete" ) )
   {
      int num;
      bool found;

      if( !mprog )
      {
         send_to_char( "That object has no obj programs.\r\n", ch );
         return;
      }
      argument = one_argument( argument, arg4 );
      if( value < 1 )
      {
         send_to_char( "Program not found.\r\n", ch );
         return;
      }
      cnt = 0;
      found = FALSE;
      for( mprg = mprog; mprg; mprg = mprg->next )
      {
         if( ++cnt == value )
         {
            mptype = mprg->type;
            found = TRUE;
            break;
         }
      }
      if( !found )
      {
         send_to_char( "Program not found.\r\n", ch );
         return;
      }
      cnt = num = 0;
      for( mprg = mprog; mprg; mprg = mprg->next )
         if( IS_SET( mprg->type, mptype ) )
            num++;
      if( value == 1 )
      {
         mprg_next = obj->pIndexData->mudprogs;
         obj->pIndexData->mudprogs = mprg_next->next;
      }
      else
         for( mprg = mprog; mprg; mprg = mprg_next )
         {
            mprg_next = mprg->next;
            if( ++cnt == ( value - 1 ) )
            {
               mprg->next = mprg_next->next;
               break;
            }
         }
      STRFREE( mprg_next->arglist );
      STRFREE( mprg_next->comlist );
      DISPOSE( mprg_next );
      if( num <= 1 )
         xREMOVE_BIT( obj->pIndexData->progtypes, mptype );
      send_to_char( "Program removed.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "insert" ) )
   {
      if( !mprog )
      {
         send_to_char( "That object has no obj programs.\r\n", ch );
         return;
      }
      argument = one_argument( argument, arg4 );
      mptype = get_mpflag( arg4 );
      if( mptype == -1 )
      {
         send_to_char( "Unknown program type.\r\n", ch );
         return;
      }
      if( value < 1 )
      {
         send_to_char( "Program not found.\r\n", ch );
         return;
      }
      if( value == 1 )
      {
         CREATE( mprg, MPROG_DATA, 1 );
         xSET_BIT( obj->pIndexData->progtypes, mptype );
         mpedit( ch, mprg, mptype, argument );
         mprg->next = mprog;
         obj->pIndexData->mudprogs = mprg;
         return;
      }
      cnt = 1;
      for( mprg = mprog; mprg; mprg = mprg->next )
      {
         if( ++cnt == value && mprg->next )
         {
            CREATE( mprg_next, MPROG_DATA, 1 );
            xSET_BIT( obj->pIndexData->progtypes, mptype );
            mpedit( ch, mprg_next, mptype, argument );
            mprg_next->next = mprg->next;
            mprg->next = mprg_next;
            return;
         }
      }
      send_to_char( "Program not found.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "add" ) )
   {
      mptype = get_mpflag( arg3 );
      if( mptype == -1 )
      {
         send_to_char( "Unknown program type.\r\n", ch );
         return;
      }
      if( mprog != NULL )
         for( ; mprog->next; mprog = mprog->next );
      CREATE( mprg, MPROG_DATA, 1 );
      if( mprog )
         mprog->next = mprg;
      else
         obj->pIndexData->mudprogs = mprg;
      xSET_BIT( obj->pIndexData->progtypes, mptype );
      mpedit( ch, mprg, mptype, argument );
      mprg->next = NULL;
      return;
   }

   do_opedit( ch, "" );
}



/*
 * RoomProg Support
 */
void rpedit( CHAR_DATA * ch, MPROG_DATA * mprg, int mptype,
	     const char *argument )
{
   if( mptype != -1 )
   {
      mprg->type = 1 << mptype;
      if( mprg->arglist )
         STRFREE( mprg->arglist );
      mprg->arglist = STRALLOC( argument );
   }
   ch->substate = SUB_MPROG_EDIT;
   ch->dest_buf = mprg;
   if( !mprg->comlist )
      mprg->comlist = STRALLOC( "" );
   start_editing( ch, (char *)mprg->comlist );
   return;
}

void do_rpedit( CHAR_DATA * ch, const char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char arg3[MAX_INPUT_LENGTH];
   MPROG_DATA *mprog, *mprg, *mprg_next = NULL;
   int value, mptype = 0, cnt;

   if( IS_NPC( ch ) )
   {
      send_to_char( "Mob's can't rpedit\r\n", ch );
      return;
   }

   if( !ch->desc )
   {
      send_to_char( "You have no descriptor\r\n", ch );
      return;
   }

   switch ( ch->substate )
   {
      default:
         break;
      case SUB_RESTRICTED:
         send_to_char( "You can't use this command from within another command.\r\n", ch );
         return;
      case SUB_MPROG_EDIT:
         if( !ch->dest_buf )
         {
            send_to_char( "Fatal error: report to Thoric.\r\n", ch );
            bug( "do_opedit: sub_oprog_edit: NULL ch->dest_buf", 0 );
            ch->substate = SUB_NONE;
            return;
         }
         mprog = ( MPROG_DATA* ) ch->dest_buf;
         if( mprog->comlist )
            STRFREE( mprog->comlist );
         mprog->comlist = copy_buffer( ch );
         stop_editing( ch );
         return;
   }

   argument = smash_tilde_static( argument );
   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   value = atoi( arg2 );
   /*
    * argument = one_argument( argument, arg3 ); 
    */

   if( arg1[0] == '\0' )
   {
      send_to_char( "Syntax: rpedit <command> [number] <program> <value>\r\n", ch );
      send_to_char( "\r\n", ch );
      send_to_char( "Command being one of:\r\n", ch );
      send_to_char( "  add delete insert edit list\r\n", ch );
      send_to_char( "Program being one of:\r\n", ch );
      send_to_char( "  act speech rand sleep rest rfight entry\r\n", ch );
      send_to_char( "  leave death\r\n", ch );
      send_to_char( "\r\n", ch );
      send_to_char( "You should be standing in room you wish to edit.\r\n", ch );
      return;
   }

   if( !can_rmodify( ch, ch->in_room ) )
      return;

   mprog = ch->in_room->mudprogs;

   set_char_color( AT_GREEN, ch );

   if( !str_cmp( arg1, "list" ) )
   {
      cnt = 0;
      if( !mprog )
      {
         send_to_char( "This room has no room programs.\r\n", ch );
         return;
      }
      for( mprg = mprog; mprg; mprg = mprg->next )
         ch_printf( ch, "%d>%s %s\r\n%s\r\n", ++cnt, mprog_type_to_name( mprg->type ), mprg->arglist, mprg->comlist );
      return;
   }

   if( !str_cmp( arg1, "edit" ) )
   {
      if( !mprog )
      {
         send_to_char( "This room has no room programs.\r\n", ch );
         return;
      }
      argument = one_argument( argument, arg3 );
      if( arg3[0] != '\0' )
      {
         mptype = get_mpflag( arg3 );
         if( mptype == -1 )
         {
            send_to_char( "Unknown program type.\r\n", ch );
            return;
         }
      }
      else
         mptype = -1;
      if( value < 1 )
      {
         send_to_char( "Program not found.\r\n", ch );
         return;
      }
      cnt = 0;
      for( mprg = mprog; mprg; mprg = mprg->next )
      {
         if( ++cnt == value )
         {
            mpedit( ch, mprg, mptype, argument );
            xCLEAR_BITS( ch->in_room->progtypes );
            for( mprg = mprog; mprg; mprg = mprg->next )
               xSET_BIT( ch->in_room->progtypes, mprg->type );
            return;
         }
      }
      send_to_char( "Program not found.\r\n", ch );
      return;
   }

   if( !str_cmp( arg1, "delete" ) )
   {
      int num;
      bool found;

      if( !mprog )
      {
         send_to_char( "That room has no room programs.\r\n", ch );
         return;
      }
      argument = one_argument( argument, arg3 );
      if( value < 1 )
      {
         send_to_char( "Program not found.\r\n", ch );
         return;
      }
      cnt = 0;
      found = FALSE;
      for( mprg = mprog; mprg; mprg = mprg->next )
      {
         if( ++cnt == value )
         {
            mptype = mprg->type;
            found = TRUE;
            break;
         }
      }
      if( !found )
      {
         send_to_char( "Program not found.\r\n", ch );
         return;
      }
      cnt = num = 0;
      for( mprg = mprog; mprg; mprg = mprg->next )
         if( IS_SET( mprg->type, mptype ) )
            num++;
      if( value == 1 )
      {
         mprg_next = ch->in_room->mudprogs;
         ch->in_room->mudprogs = mprg_next->next;
      }
      else
         for( mprg = mprog; mprg; mprg = mprg_next )
         {
            mprg_next = mprg->next;
            if( ++cnt == ( value - 1 ) )
            {
               mprg->next = mprg_next->next;
               break;
            }
         }
      STRFREE( mprg_next->arglist );
      STRFREE( mprg_next->comlist );
      DISPOSE( mprg_next );
      if( num <= 1 )
         xREMOVE_BIT( ch->in_room->progtypes, mptype );
      send_to_char( "Program removed.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "insert" ) )
   {
      if( !mprog )
      {
         send_to_char( "That room has no room programs.\r\n", ch );
         return;
      }
      argument = one_argument( argument, arg3 );
      mptype = get_mpflag( arg2 );
      if( mptype == -1 )
      {
         send_to_char( "Unknown program type.\r\n", ch );
         return;
      }
      if( value < 1 )
      {
         send_to_char( "Program not found.\r\n", ch );
         return;
      }
      if( value == 1 )
      {
         CREATE( mprg, MPROG_DATA, 1 );
         xSET_BIT( ch->in_room->progtypes, mptype );
         mpedit( ch, mprg, mptype, argument );
         mprg->next = mprog;
         ch->in_room->mudprogs = mprg;
         return;
      }
      cnt = 1;
      for( mprg = mprog; mprg; mprg = mprg->next )
      {
         if( ++cnt == value && mprg->next )
         {
            CREATE( mprg_next, MPROG_DATA, 1 );
            xSET_BIT( ch->in_room->progtypes, mptype );
            mpedit( ch, mprg_next, mptype, argument );
            mprg_next->next = mprg->next;
            mprg->next = mprg_next;
            return;
         }
      }
      send_to_char( "Program not found.\r\n", ch );
      return;
   }

   if( !str_cmp( arg1, "add" ) )
   {
      mptype = get_mpflag( arg2 );
      if( mptype == -1 )
      {
         send_to_char( "Unknown program type.\r\n", ch );
         return;
      }
      if( mprog )
         for( ; mprog->next; mprog = mprog->next );
      CREATE( mprg, MPROG_DATA, 1 );
      if( mprog )
         mprog->next = mprg;
      else
         ch->in_room->mudprogs = mprg;
      xSET_BIT( ch->in_room->progtypes, mptype );
      mpedit( ch, mprg, mptype, argument );
      mprg->next = NULL;
      return;
   }

   do_rpedit( ch, "" );
}

void do_rdelete( CHAR_DATA * ch, const char *argument )
{
   ROOM_INDEX_DATA *location;

   if( ch->substate == SUB_RESTRICTED )
   {
      send_to_char( "You can't do that while in a subprompt.\r\n", ch );
      return;
   }

   if( !argument || argument[0] == '\0' )
   {
      send_to_char( "Delete which room?\r\n", ch );
      return;
   }

   /*
    * Find the room. 
    */
   if( !( location = find_location( ch, argument ) ) )
   {
      send_to_char( "No such location.\r\n", ch );
      return;
   }

   /*
    * Does the player have the right to delete this room? 
    */
   if( get_trust( ch ) < sysdata.level_modify_proto
       && ( location->vnum < ch->pcdata->area->low_r_vnum || location->vnum > ch->pcdata->area->hi_r_vnum ) )
   {
      send_to_char( "That room is not in your assigned range.\r\n", ch );
      return;
   }
   delete_room( location );
   fix_exits(); /* Need to call this to solve a crash */
   ch_printf( ch, "Room %s has been deleted.\r\n", argument );
   return;
}

void do_odelete( CHAR_DATA * ch, const char *argument )
{
   OBJ_INDEX_DATA *obj;
   int vnum;

   if( ch->substate == SUB_RESTRICTED )
   {
      send_to_char( "You can't do that while in a subprompt.\r\n", ch );
      return;
   }

   if( !argument || argument[0] == '\0' )
   {
      send_to_char( "Delete which object?\r\n", ch );
      return;
   }

   if( !is_number( argument ) )
   {
      send_to_char( "You must specify the object's vnum to delete it.\r\n", ch );
      return;
   }

   vnum = atoi( argument );

   /*
    * Find the obj. 
    */
   if( !( obj = get_obj_index( vnum ) ) )
   {
      send_to_char( "No such object.\r\n", ch );
      return;
   }

   /*
    * Does the player have the right to delete this object? 
    */
   if( get_trust( ch ) < sysdata.level_modify_proto
       && ( obj->vnum < ch->pcdata->area->low_o_vnum || obj->vnum > ch->pcdata->area->hi_o_vnum ) )
   {
      send_to_char( "That object is not in your assigned range.\r\n", ch );
      return;
   }
   delete_obj( obj );
   ch_printf( ch, "Object %d has been deleted.\r\n", vnum );
   return;
}

void do_mdelete( CHAR_DATA * ch, const char *argument )
{
   MOB_INDEX_DATA *mob;
   int vnum;

   if( ch->substate == SUB_RESTRICTED )
   {
      send_to_char( "You can't do that while in a subprompt.\r\n", ch );
      return;
   }

   if( !argument || argument[0] == '\0' )
   {
      send_to_char( "Delete which mob?\r\n", ch );
      return;
   }

   if( !is_number( argument ) )
   {
      send_to_char( "You must specify the mob's vnum to delete it.\r\n", ch );
      return;
   }

   vnum = atoi( argument );

   /*
    * Find the mob. 
    */
   if( !( mob = get_mob_index( vnum ) ) )
   {
      send_to_char( "No such mob.\r\n", ch );
      return;
   }

   /*
    * Does the player have the right to delete this mob? 
    */
   if( get_trust( ch ) < sysdata.level_modify_proto
       && ( mob->vnum < ch->pcdata->area->low_m_vnum || mob->vnum > ch->pcdata->area->hi_m_vnum ) )
   {
      send_to_char( "That mob is not in your assigned range.\r\n", ch );
      return;
   }
   delete_mob( mob );
   ch_printf( ch, "Mob %d has been deleted.\r\n", vnum );
   return;
}

/*
 * Relations created to fix a crash bug with oset on and rset on
 * code by: gfinello@mail.karmanet.it
 */
void RelCreate( relation_type tp, void *actor, void *subject )
{
   REL_DATA *tmp;

   if( tp < relMSET_ON || tp > relOSET_ON )
   {
      bug( "RelCreate: invalid type (%d)", tp );
      return;
   }
   if( !actor )
   {
      bug( "RelCreate: NULL actor" );
      return;
   }
   if( !subject )
   {
      bug( "RelCreate: NULL subject" );
      return;
   }
   for( tmp = first_relation; tmp; tmp = tmp->next )
      if( tmp->Type == tp && tmp->Actor == actor && tmp->Subject == subject )
      {
         bug( "RelCreate: duplicated relation" );
         return;
      }
   CREATE( tmp, REL_DATA, 1 );
   tmp->Type = tp;
   tmp->Actor = actor;
   tmp->Subject = subject;
   LINK( tmp, first_relation, last_relation, next, prev );
}


/*
 * Relations created to fix a crash bug with oset on and rset on
 * code by: gfinello@mail.karmanet.it
 */
void RelDestroy( relation_type tp, void *actor, void *subject )
{
   REL_DATA *rq;

   if( tp < relMSET_ON || tp > relOSET_ON )
   {
      bug( "RelDestroy: invalid type (%d)", tp );
      return;
   }
   if( !actor )
   {
      bug( "RelDestroy: NULL actor" );
      return;
   }
   if( !subject )
   {
      bug( "RelDestroy: NULL subject" );
      return;
   }
   for( rq = first_relation; rq; rq = rq->next )
      if( rq->Type == tp && rq->Actor == actor && rq->Subject == subject )
      {
         UNLINK( rq, first_relation, last_relation, next, prev );
         /*
          * Dispose will also set to NULL the passed parameter 
          */
         DISPOSE( rq );
         break;
      }
}

/* Is valid vnum checks to make sure an area has the valid vnum for any type
   types: 0=room, 1=obj, 2=mob                     -->Keberus 12/03/08 */
bool is_valid_vnum( int vnum, short type )
{
   AREA_DATA *area;
   int low_value =-1, hi_value =-1;
   bool isValid = FALSE;

   if( (type < VCHECK_ROOM) || (type > VCHECK_MOB) )
   {
       bug( "is_valid_vnum: bad type %d", type );
       return FALSE;
   }
   for( area = first_area; area; area = area->next )
   {
        if( type == VCHECK_ROOM )
        {
            low_value = area->low_r_vnum;
            hi_value = area->hi_r_vnum;
        }
        else if( type == VCHECK_OBJ )
        {
            low_value = area->low_o_vnum;
            hi_value = area->hi_o_vnum;
        }
        else
        {
            low_value = area->low_m_vnum;
            hi_value = area->hi_m_vnum;
        }

        if( (vnum >= low_value) && (vnum <= hi_value ) )
        {
            isValid = TRUE;
            break;
        }
   }
   for( area = first_build; area; area = area->next )
   {
      if( type == VCHECK_ROOM )
      {
         low_value = area->low_r_vnum;
         hi_value = area->hi_r_vnum;
      }
      else if( type == VCHECK_OBJ )
      {
         low_value = area->low_o_vnum;
         hi_value = area->hi_o_vnum;
      }
      else
      {
         low_value = area->low_m_vnum;
         hi_value = area->hi_m_vnum;
      }

      if( ( vnum >= low_value ) && ( vnum <= hi_value ) )
      {
         isValid = TRUE;
         break;
      }
   }
   return isValid;
}

void do_dset( CHAR_DATA *ch, const char *argument )
{
   DISC_DATA *discipline;
   FACTOR_DATA *factor;
   int x, value, id;
   int selection, count;

   char arg[MAX_STRING_LENGTH];
   char arg2[MAX_STRING_LENGTH];
   char arg3[MAX_STRING_LENGTH];

   argument = one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      send_to_char( "Syntax: dset 'discipline name' <command> <etc>\r\n", ch );
      send_to_char( "Or:     dset create <discipline name>\r\n", ch );
      send_to_char( "Or:     dset delete <discipline name>(not implemented yet)\r\n\r\n", ch );
      send_to_char( "Commands:\r\n", ch );
      send_to_char( "  addfactor <factor_type> <location> <modifier> <duration> <apply_type>\r\n", ch );
      send_to_char( "  remfactor <factor_number>\r\n", ch );
      send_to_char( "  addaffect <factor_number> <affects...>\r\n", ch );
      send_to_char( "  show hit_gain move_gain mana_gain minlevel\r\n", ch );
      send_to_char( "  cost skill_type skill_style damtype target_type\r\n", ch );
      return;
   }

   if( !str_cmp( arg, "create" ) )
   {
      CREATE( discipline, DISC_DATA, 1 );
      discipline->name = str_dup( argument );
      for( ;; )
      {
         id = number_range( 1000, 9999 );
         if( !get_discipline_from_id( id ) )
            break;
      }
      discipline->id = id;
      LINK( discipline, first_discipline, last_discipline, next, prev );
      ch_printf( ch, "The %s discipline has been created.\r\n", discipline->name );
      save_disciplines( );
      return;
   }
   if( !str_cmp( arg, "delete" ) )
      return;

   argument = one_argument( argument, arg2 );

   if( ( discipline = get_discipline( arg ) ) == NULL )
   {
      send_to_char( "No such discipline.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "addaffect" ) )
   {
      argument = one_argument( argument, arg3 );

      if( !is_number( arg3 ) )
      {
         send_to_char( "Remove factor by number\r\n", ch );
         return;
      }

      if( ( selection = atoi( arg3 ) ) < 0 )
      {
         send_to_char( "No factor that low.\r\n", ch );
         return;
      }

      for( x = 0, factor = discipline->first_factor; factor; factor = factor->next )
      {
         if( selection == x++ )
            break;
      }

      if( !factor )
      {
         send_to_char( "No factor that high.\r\n", ch );
         return;
      }

      if( factor->factor_type != APPLY_FACTOR && factor->location != APPLY_AFFECT )
      {
         send_to_char( "You can't add flags to that factor.\r\n", ch );
         return;
      }

      if( argument[0] == '\0' )
      {
         send_to_char( "Valid affect flags:", ch );
         for( x = 0; x < MAX_AFF; x++ )
            ch_printf( ch, " %s", a_flags[x] );
         send_to_char( "\r\n", ch );
         return;
      }

      count = 0;
      while( argument[0] != '\0' )
      {
         if( ++count > (int)factor->modifier )
         {
            ch_printf( ch, "Factor can't have more than %d affects. These ones have been left off: %s\r\n", count, argument );
            return;
         }
         argument = one_argument( argument, arg3 );
         if( ( value = get_aflag( arg3 ) ) == -1 )
         {
            ch_printf( ch, "%s is an invalid affect flag.\r\n", arg3 );
            continue;
         }
         xSET_BIT( factor->affect, value );
      }
      update_disciplines( );
      save_disciplines( );
      send_to_char( "Affect(s) added.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "remfactor" ) )
   {
      if( !is_number( argument ) )
      {
         send_to_char( "Remove factor by number\r\n", ch );
         return;
      }

      if( ( selection = atoi( argument ) ) < 0 )
      {
         send_to_char( "There are no factors that low.\r\n", ch );
         return;
      }
      for( x = 0, factor = discipline->first_factor; factor; factor = factor->next )
      {
         if( x == selection )
         {
            UNLINK( factor, discipline->first_factor, discipline->last_factor, next, prev );
            free_factor( factor );
            update_disciplines( );
            save_disciplines( );
            send_to_char( "Factor removed.\r\n", ch );
            return;
         }
         x++;
      }
      send_to_char( "There are no factors that high on this discipline.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "addfactor" ) )
   {
      int factor_type, location, duration, apply_type;
      double modifier;

      argument = one_argument( argument, arg3 );
      if( ( factor_type = get_factor_type( arg3 ) ) == -1 )
      {
         ch_printf( ch, "'%s' not a valid factor type.\r\n", arg3[0] == '\0' ? "nothing" : arg3 );
         send_to_char( "Valid Options are:\r\n", ch );
         for( x = 0; x < MAX_FACTOR; x++ )
            ch_printf( ch, "%s ", factor_names[x] );
         send_to_char( "\r\n", ch );
         return;
      }

      argument = one_argument( argument, arg3 );
      if( ( location = get_atype( arg3 ) ) == -1 )
      {
         ch_printf( ch, "'%s' not a valid location.\r\n", arg3[0] == '\0' ? "nothing" : arg3 );
         send_to_char( "Valid Options are:\r\n", ch );
         for( x = 0; x < MAX_APPLY_TYPE; x++ )
            ch_printf( ch, "%s ", a_types[x] );
         send_to_char( "\r\n", ch );
         return;
      }

      argument = one_argument( argument, arg3 );
      if( arg3[0] == '\0' || ( modifier = atof( arg3 ) ) < 0 )
      {
         ch_printf( ch, "'%s' is not a valid modifier value.\r\n", arg3[0] == '\0' ? "nothing" : arg3 );
         return;
      }
      argument = one_argument( argument, arg3 );
      if( !is_number( arg3 ) )
      {
         ch_printf( ch, "'%s' is not a valid duration value.\r\n", arg3[0] == '\0' ? "nothing" : arg3 );
         return;
      }
      duration = atoi( arg3 );

      argument = one_argument( argument, arg3 );
      if( ( apply_type = get_apply_type( arg3 ) ) == -1 )
      {
         ch_printf( ch, "'%s' is not a valid apply type.\r\n", arg3[0] == '\0' ? "nothing" : arg3 );
         send_to_char( "Valid Options are:\r\n", ch );
         for( x = 0; x < MAX_APPLYTYPE; x++ )
            ch_printf( ch, "%s ", applytypes_type[x] );
         send_to_char( "\r\n", ch );

         return;
      }

      CREATE( factor, FACTOR_DATA, 1 );
      factor->owner = discipline;
      factor->factor_type = factor_type;
      factor->location = location;
      factor->duration = duration;
      factor->apply_type = apply_type;
      factor->modifier = modifier;
      for( ;; )
      {
         id = number_range( 1000, 9999 );
         if( !get_factor_from_id( id ) )
            break;
      }
      factor->id = id;
      LINK( factor, discipline->first_factor, discipline->last_factor, next, prev );
      send_to_char( "Ok.\r\n", ch );
      update_disciplines( );
      save_disciplines( );
      return;
   }

   if( !str_cmp( arg2, "hit_gain" ) || !str_cmp( arg2, "move_gain" ) || !str_cmp( arg2, "mana_gain" ) || !str_cmp( arg2, "minlevel" ) )
   {
      argument = one_argument( argument, arg3 );
      if( !is_number( arg3 ) )
      {
         ch_printf( ch, "%s must be a number.\r\n", arg2 );
         return;
      }

      if( !str_cmp( arg2, "hit_gain" ) )
         discipline->hit_gain = atoi( arg3 );
      else if( !str_cmp( arg2, "move_gain" ) )
         discipline->move_gain = atoi( arg3 );
      else if( !str_cmp( arg2, "mana_gain" ) )
         discipline->mana_gain = atoi( arg3 );
      else if( !str_cmp( arg2, "minlevel" ) )
         discipline->min_level = atoi( arg3 );

      send_to_char( "Ok.\r\n", ch );
      update_disciplines( );
      save_disciplines( );
      return;
   }

   if( !str_cmp( arg2, "cost" ) )
   {
      if( argument[0] == '\0' )
      {
         send_to_char( "Valid Cost Types:", ch );
         for( x = 0; x < MAX_COST; x++ )
            ch_printf( ch, " %s", cost_type[x] );
         send_to_char( "\r\n", ch );
         return;
      }
      while( argument[0] != '\0' )
      {
         argument = one_argument( argument, arg3 );
         if( ( value = get_cost_type( arg3 ) ) == -1 )
         {
            ch_printf( ch, "%s is an invalid cost type.\r\n", arg3 );
            continue;
         }
         xTOGGLE_BIT( discipline->cost, value );
      }
      update_disciplines( );
      save_disciplines( );
      send_to_char( "Ok.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "skill_type" ) )
   {
      if( argument[0] == '\0' )
      {
         send_to_char( "Valid Skill Types: Skill or Spell", ch );
         return;
      }

      while( argument[0] != '\0' )
      {
         argument = one_argument( argument, arg3 );
         if( ( value = get_skill( arg3 ) ) == 0 )
         {
            ch_printf( ch, "%s is an invalid skill type.\r\n", arg3 );
            continue;
         }
         xTOGGLE_BIT( discipline->skill_type, value );
      }
      update_disciplines( );
      save_disciplines( );
      send_to_char( "Ok.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "skill_style" ) )
   {
      if( argument[0] == '\0' )
      {
         send_to_char( "Valid Skill Styles:", ch );
         for( x = 0; x < STYLE_MAX; x++ )
            ch_printf( ch, " %s", style_type[x] );
         send_to_char( "\r\n", ch );
      }

      while( argument[0] != '\0' ) 
      {
         argument = one_argument( argument, arg3 );
         if( ( value = get_style_type( arg3 ) ) == -1 )
         { 
            ch_printf( ch, "%s is an invalid style type.\r\n", arg3 );
            continue;
         }
         xTOGGLE_BIT( discipline->skill_style, value );
      }
      update_disciplines( );
      save_disciplines( );
      send_to_char( "Ok.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "damtype" ) )
   {
      if( argument[0] == '\0' )
      {
         send_to_char( "Valid DamTypes:", ch );
         for( x = 0; x < MAX_DAMTYPE; x++ )
            ch_printf( ch, " %s", d_type[x] );
         send_to_char( "\r\n", ch );
      }

      while( argument[0] != '\0' )
      {
         argument = one_argument( argument, arg3 );
         if( ( value = get_damtype( arg3 ) ) == -1 )
         { 
            ch_printf( ch, "%s is an invalid damtype.\r\n", arg3 );
            continue;
         }
         xTOGGLE_BIT( discipline->damtype, value );
      }
      update_disciplines( );
      save_disciplines( );
      send_to_char( "Ok.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "target_type" ) )
   {
      if( argument[0] == '\0' )
      {
         send_to_char( "Valid Target Types:", ch );
         for( x = 0; x < TAR_CHAR_MAX; x++ )
            ch_printf( ch, " %s", target_type[x] );
         send_to_char( "\r\n", ch );
      }

      while( argument[0] != '\0' )
      {
         argument = one_argument( argument, arg3 );
         if( ( value = get_starget( arg3 ) ) == -1 )
         { 
            ch_printf( ch, "%s is an invalid target type.\r\n", arg3 );
            continue;
         }
         xTOGGLE_BIT( discipline->target_type, value );
      }
      update_disciplines( );
      save_disciplines( );
      send_to_char( "Ok.\r\n", ch );
      return;
   }

   if( !str_cmp( arg2, "show" ) )
   {
      ch_printf( ch, "*=- %s -=*\r\n", discipline->name );
      ch_printf( ch, "MinLevel: %d Hit Gains: %d Move Gains: %d Mana Gains: %d\r\n", discipline->min_level, discipline->hit_gain, discipline->move_gain, discipline->mana_gain );

      send_to_char( "Grants Skill Types: ", ch );
      if( xIS_EMPTY( discipline->skill_type ) )
         send_to_char( "none\r\n", ch );
      else
      {
         for( x = 0; x < MAX_SKILLTYPE; x++ )
            if( xIS_SET( discipline->skill_type, x ) )
               ch_printf( ch, "%s ", skill_tname[x] );
         send_to_char( "\r\n", ch );
      }

      send_to_char( "Grants Target Types: ", ch );
      if( xIS_EMPTY( discipline->target_type ) )
         send_to_char( "none\r\n", ch );
      else
      {
         for( x = 0; x < TAR_CHAR_MAX; x++ )
            if( xIS_SET( discipline->target_type, x ) )
               ch_printf( ch, "%s ", target_type[x] );
         send_to_char( "\r\n", ch );
      }

      send_to_char( "Grants Cost Types: ", ch );
      if( xIS_EMPTY( discipline->cost ) )
         send_to_char( "none\r\n", ch );
      else
      {
         for( x = 0; x < MAX_COST; x++ )
            if( xIS_SET( discipline->cost, x ) )
               ch_printf( ch, "%s ", cost_type[x] );
         send_to_char( "\r\n", ch );
      }

      send_to_char( "Grants Skill Styles: ", ch );
      if( xIS_EMPTY( discipline->skill_style ) )
         send_to_char( "none\r\n", ch );
      else
      {
         for( x = 0; x < STYLE_MAX; x++ )
            if( xIS_SET( discipline->skill_style, x ) )
               ch_printf( ch, "%s ", style_type[x] );
         send_to_char( "\r\n", ch );
      }

      send_to_char( "Grants Damtypes: ", ch );
      if( xIS_EMPTY( discipline->damtype ) )
         send_to_char( "none\r\n", ch );
      else
      {
         for( x = 0; x < MAX_DAMTYPE; x++ )
            if( xIS_SET( discipline->damtype, x ) )
               ch_printf( ch, "%s ", d_type[x] );
         send_to_char( "\r\n", ch );
      }

      send_to_char( "Factors\r\n---------------------------------------------------------------\r\n", ch );
      if( !discipline->first_factor )
         send_to_char( "No factors\r\n", ch );
      else
      {
         selection = 0;
         for( factor = discipline->first_factor; factor; factor = factor->next )
         {
            if( factor->factor_type == APPLY_FACTOR )
            {
               ch_printf( ch, "%-2d: Factor Type: %-10.10s | Location: %-10.10s | Apply Type: %-10.10s | Duration: %-10d |\r\n",
                          selection,
                          factor_names[factor->factor_type],
                          a_types[factor->location],
                          applytypes_type[factor->apply_type],
                          (int)factor->duration );

               if( factor->location == APPLY_AFFECT )
               {
                  ch_printf( ch, " Max Affects: %-2d Affects:", (int)factor->modifier );
                  for( x = 0; x < MAX_AFF; x++ )
                     if( xIS_SET( factor->affect, x ) )
                        ch_printf( ch, " %s,", a_flags[x] );
                  send_to_char( "\r\n", ch );
               }
               else
                  ch_printf( ch, " Modifier: %f\r\n", factor->modifier );
            }
            else if( factor->factor_type == STAT_FACTOR )
            {
               ch_printf( ch, "%-2d: Factor Type: %-10.10s | Add %d%% of %s to Base Roll\r\n",
                          selection,
                          factor_names[factor->factor_type],
                          (int)( factor->modifier * 100 ),
                          a_types[factor->location] );
            }
            else if( factor->factor_type == BASEROLL_FACTOR )
               ch_printf( ch, "%d: Factor Type: %-10.10s | Multiplies Base Roll of Weapon by %f\r\n",
                          selection,
                          factor_names[factor->factor_type],
                          factor->modifier );
            selection++;
         }
      }
      return;
   }
   do_dset( ch, "" );
   return;
}

void do_discipline( CHAR_DATA *ch, const char *argument )
{
   DISC_DATA *disc;
   char arg[MAX_STRING_LENGTH];
   int x;

   argument = one_argument( argument, arg );

   if( arg[0] == '\0' || argument[0] == '\0' )
   {
      if( IS_IMMORTAL( ch ) )
      {
         send_to_char( "Displaying All Disciplines:\r\n", ch );
         for( disc = first_discipline; disc; disc = disc->next )
            ch_printf( ch, "  %s", disc->name );
         send_to_char( "\r\n", ch );
      }

      send_to_char( "&wYour Disciplines: &zGrey = Set &WWhite = Unset&w\r\n", ch );
      for( x = 0; x < MAX_DISCIPLINE; x++ )
         if( ch->known_disciplines[x] != NULL )
            ch_printf( ch, " %s%s&w\r\n",
            is_discipline_set( ch, ch->known_disciplines[x] ) ? "&z" : "&W",
            ch->known_disciplines[x]->name );
      send_to_char( "Commands: set unset\r\n", ch );
   }

   if( !str_cmp( arg, "set" ) )
   {
      if( ( disc = get_discipline( argument ) ) == NULL )
      {
         ch_printf( ch, "%s is not a discipline.\r\n", argument );
         return;
      }
      if( !player_has_discipline( ch, disc ) )
      {
         ch_printf( ch, "You don't know %s.\r\n", disc->name );
         return;
      }
      if( is_discipline_set( ch, disc ) )
      {
         ch_printf( ch, "%s is already set.\r\n", disc->name );
         return;
      }
      if( !player_has_discipline_setslot( ch ) )
      {
         send_to_char( "You don't have a free slot, try unsetting one first.\r\n", ch );
         return;
      }
      set_discipline( ch, disc );
      send_to_char( "Discipline Set.\r\n", ch );
      return;
   }
   if( !str_cmp( arg, "unset" ) )
   {
      if( ( disc = get_discipline( argument ) ) == NULL )
      {
         ch_printf( ch, "%s is not a discipline.\r\n", ch );
         return;
      }
      if( !player_has_discipline( ch, disc ) )
      {
         ch_printf( ch, "You don't know %s.\r\n", disc->name );
         return;
      }
      if( !is_discipline_set( ch, disc ) )
      {
         ch_printf( ch, "%s is not set.\r\n", disc->name );
         return;
      }
      unset_discipline( ch, disc );
      send_to_char( "Discipline Unset.\r\n", ch );
      return;
   }
   return;
}

void save_quests( void )
{
   FILE *fp;
   QUEST_DATA *quest;

   if( ( fp = fopen( QUEST_FILE, "w" ) ) == NULL )
   {
      bug( "Cannot open quest.dat for writting", 0 );
      perror( QUEST_FILE );
      return;
   }

   for( quest = first_quest; quest; quest = quest->next )
   {
      fprintf( fp, "#QUEST\n" );
      fwrite_quest( fp, quest );
   }

   fprintf( fp, "#END\n" );
   fclose( fp );
}

void fwrite_quest( FILE *fp, QUEST_DATA *quest )
{
   PRE_QUEST *prequest;
   fprintf( fp, "ID           %d\n", quest->id );
   fprintf( fp, "Name         %s~\n", quest->name );
   fprintf( fp, "Description  %s~\n", quest->description );
   fprintf( fp, "LevelReq     %d\n", quest->level_req );
   fprintf( fp, "Type         %d\n", quest->type );
   for( prequest = quest->first_prequest; prequest; prequest = prequest->next )
      fprintf( fp, "Prequest     %d\n", prequest->quest->id );
   fprintf( fp, "End\n" );
   return;
}

void do_quest( CHAR_DATA *ch, const char *argument )
{
   CHAR_DATA *victim;
   char arg[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];

   argument = one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      send_to_char( "\r\nProper Usage: quest list <mob>\r\n", ch );
      send_to_char( "              quest show <mob> <quest#>\r\n", ch ); /* Spit out the description and perhaps requirements to accept the quest */
      send_to_char( "              quest accept <mob> <quest#>\r\n", ch );
      if( IS_IMMORTAL ( ch ) )
      {
         send_to_char( "Immortal Usage: quest create <name>\r\n", ch );
         send_to_char( "                quest delete <name> \r\n", ch );
         send_to_char( "                quest name <name/id> <new name>\r\n", ch ); /* to rename a quest */
         send_to_char( "                quest description <name/id> <description>\r\n", ch );
         send_to_char( "                quest level <name/id> <level>\r\n", ch );
         send_to_char( "                quest type <name/id> <type>\r\n", ch ); /* repeatable, one-time, etc */
         send_to_char( "                quest addprequest <prequest id/name> <name/id>\r\n", ch );
         send_to_char( "                quest remprequest <prequest id/name> <mame/id>\r\n", ch );
         send_to_char( "                quest all\r\n", ch );
      }
      return;
   }

   if( !str_cmp( arg, "list" ) || !str_cmp( arg, "show" ) || !str_cmp( arg, "accept" ) )
   {
      argument = one_argument( argument, arg2 );

      if( ( victim = get_char_room( ch, arg2 ) ) == NULL )
      {
         send_to_char( "That person is not here.\r\n", ch );
         return;
      }
      if( !IS_NPC( victim ) )
      {
         send_to_char( "Not on players.\r\n", ch );
         return;
      }
      if( !victim->pIndexData->first_available_quest )
      {
         send_to_char( "That mob has no quests.\r\n", ch );
         return;
      }
   }

   if( !str_cmp( arg, "list" ) )
   {
      list_mob_quest( ch, victim );
      return;
   }

   if( !str_cmp( arg, "show" ) )
   {
      show_mob_quest( ch, victim, argument );
      return;
   }

   if( !str_cmp( arg, "accept" ) )
   {
      accept_mob_quest( ch, victim, argument );
      return;
   }

   if( IS_IMMORTAL( ch ) )
   {
      QUEST_DATA *quest;

      if( !str_cmp( arg, "all" ) )
      {
         send_to_char( "Quests:\r\n", ch );

         for( quest = first_quest; quest; quest = quest->next )
            ch_printf( ch, "ID: %-4d Name: %s\r\n", quest->id, quest->name );
         return;
      }

      if( get_trust( ch ) < QUEST_COMMAND_LEVEL )
      {
         send_to_char( "You aren't high enough level to use those commands.\r\n", ch );
         return;
      }
      /* commands that take one argument */
      if( !str_cmp( arg, "create" ) )
      {
         create_quest( ch, argument );
         save_quests( );
         return;
      }

      /* all commands below this point take two arguments */
      argument = one_argument( argument, arg2 );

      if( ( quest = get_quest_from_id( atoi( arg2 ) ) ) == NULL && ( quest = get_quest_from_name( arg2 ) ) == NULL )
      {
         send_to_char( "No such quest exists.\r\n", ch );
         return;
      }

      if( !str_cmp( arg, "name" ) )
         change_quest_name( ch, quest, argument );
      if( !str_cmp( arg, "description" ) )
         change_quest_description( ch, quest, argument );
      if( !str_cmp( arg, "level" ) )
         change_quest_level( ch, quest, argument );
      if( !str_cmp( arg, "type" ) )
         change_quest_type( ch, quest, argument );
      if( !str_cmp( arg, "addprequest" ) )
         add_prequest( ch, quest, argument );
      if( !str_cmp( arg, "remprequest" ) )
         rem_prequest( ch, quest, argument );
      if( !str_cmp( arg, "delete" ) )
         delete_quest( ch, quest );

      if( !str_cmp( arg, "create" ) || !str_cmp( arg, "name" ) || !str_cmp( arg, "description" ) || !str_cmp( arg, "level" ) || !str_cmp( arg, "type" ) || !str_cmp( arg, "addprequest" ) || !str_cmp( arg, "remprequest" ) || !str_cmp( arg, "delete" ) )
      {
         save_quests( );
         return;
      }



   }
   do_quest( ch, "" );
   return;
}

void delete_quest( CHAR_DATA *ch, QUEST_DATA *quest )
{
   AREA_DATA *area;
   AV_QUEST *av_quest;
   PLAYER_QUEST *pquest;
   CHAR_DATA *dch, *dch_next;

   for( dch = last_char; dch; dch = dch_next )
   {
      dch_next = dch->prev;

      if( IS_NPC( dch ) )
      {
         while( ( av_quest = get_available_quest( dch, quest ) ) != NULL ) /* incase somehow has multiple of one quest on him */
         {
            UNLINK( av_quest, dch->pIndexData->first_available_quest, dch->pIndexData->last_available_quest, next, prev );
            free_avquest( av_quest );
            for( area = first_area; area; area = area->next )
            {
               if( dch->pIndexData->vnum >= area->low_m_vnum && dch->pIndexData->vnum <= area->hi_m_vnum )
               {
                  fold_area( area, area->filename, FALSE );
                  break;
               }
            }
         }
      }
      else
      {
         while( ( pquest = get_player_quest( dch, quest ) ) != NULL ) /* Incase a player has multiple of one quest on him */
         {
            UNLINK( pquest, dch->first_pquest, dch->last_pquest, next, prev );
            free_pquest( pquest );
         }
         save_char_obj( dch );
         saving_char = NULL;
      }
   }

   UNLINK( quest, first_quest, last_quest, next, prev );
   free_quest( quest );
   send_to_char( "Quest deleted.\r\n", ch );
   return;
}

void rem_prequest( CHAR_DATA *ch, QUEST_DATA *quest, const char *argument )
{
   PRE_QUEST *pquest, *next_prequest;
   int x, count;

   if( !is_number( argument ) )
   {
      send_to_char( "Enter a proper index number.\r\n", ch );
      return;
   }
   else if( ( count = atoi( argument ) ) < 0 )
   {
      send_to_char( "That number is too low.\r\n", ch );
      return;
   }

   for( x = 0, pquest = quest->first_prequest; pquest; pquest = next_prequest )
   {
      next_prequest = pquest->next;
      if( count == x++ )
         break;
   }
   if( !pquest )
   {
      send_to_char( "This quest doesn't have that many prequests.\r\n", ch );
      return;
   }

   UNLINK( pquest, quest->first_prequest, quest->last_prequest, next, prev );
   free_prequest( pquest );
   send_to_char( "Prequest removed.\r\n", ch );
   return;
}

void add_prequest( CHAR_DATA *ch, QUEST_DATA *quest, const char *argument )
{
   PRE_QUEST *pquest;
   QUEST_DATA *pre_quest_data;

   if( ( pre_quest_data = get_quest_from_id( atoi( argument ) ) ) == NULL && ( pre_quest_data = get_quest_from_name( argument ) ) == NULL )
   {
      ch_printf( ch, "No such quest '%s' exists.\r\n", argument );
      return;
   }

   CREATE( pquest, PRE_QUEST, 1 );
   pquest->quest = pre_quest_data;
   LINK( pquest, quest->first_prequest, quest->last_prequest, next, prev );
   ch_printf( ch, "Prequest '%s' added to quest %s.\r\n", pre_quest_data->name, quest->name );
   return;
}

void change_quest_type( CHAR_DATA *ch, QUEST_DATA *quest, const char *argument )
{
   int type;

   if( ( type = get_quest_type( argument ) ) == -1 )
   {
      send_to_char( "Invalid quest type.\r\n", ch );
      return;
   }
   quest->type = type;
   ch_printf( ch, "%s type changed to %s.\r\n", quest->name, quest_types[type] );
   return;
}

void change_quest_level( CHAR_DATA *ch, QUEST_DATA *quest, const char *argument )
{
   int level;

   if( !is_number( argument ) )
   {
      send_to_char( "You must input a number.\r\n", ch );
      return;
   }
   else if( ( level = atoi( argument ) ) > MAX_CLASS_LEVEL || level < 1 )
   {
      send_to_char( "Invalid level entered.\r\n", ch );
      return;
   }

   quest->level_req = level;
   ch_printf( ch, "%s changed to level %d.\r\n", quest->name, level );
   return;
}

void change_quest_description( CHAR_DATA *ch, QUEST_DATA *quest, const char *argument )
{
   STRFREE( quest->description );
   quest->description = str_dup( argument );
   send_to_char( "Description changed.\r\n", ch );
   return;
}

void change_quest_name( CHAR_DATA *ch, QUEST_DATA *quest, const char *argument )
{
   STRFREE( quest->name );
   quest->name = str_dup( argument );
   ch_printf( ch, "Quest name changed to '%s'.\r\n", quest->name );
   return;
}

void create_quest( CHAR_DATA *ch, const char *argument )
{
   QUEST_DATA *new_quest;
   int id;

   CREATE( new_quest, QUEST_DATA, 1 );
   new_quest->name = str_dup( argument );

   for( ;; )
   {
      id = number_range( 1000, 9999 );
      if( !(get_quest_from_id( id )) )
         break;
   }
   new_quest->id = id;
   LINK( new_quest, first_quest, last_quest, next, prev );
   ch_printf( ch, "You have created the quest '%s'\r\n", new_quest->name );
   return;
}

void accept_mob_quest( CHAR_DATA *ch, CHAR_DATA *victim, const char *argument )
{
   AV_QUEST *av_quest;
   QUEST_DATA *quest;
   PLAYER_QUEST *pquest;
   PRE_QUEST *pre_quest;

   if( ( av_quest = get_available_quest_from_list( victim, argument ) ) == NULL || !av_quest->quest )
   {
      send_to_char( "That Mob does not contain a quest with that high of an list number.\r\n", ch );
      return;
   }
   else
      quest = av_quest->quest;

   if( ch->skill_level[COMBAT_ABILITY] < quest->level_req )
   {
      send_to_char( "You aren't high enough level.\r\n", ch );
      return;
   }

   if( ( pquest = get_player_quest( ch, quest ) ) && ( pquest->stage == -1 || pquest->stage > 0 ) )
   {
      send_to_char( "You cannot accept that quest.\r\n", ch );
      return;
   }

   for( pre_quest = quest->first_prequest; pre_quest; pre_quest = pre_quest->next )
      if( !has_quest_completed( ch, pre_quest->quest ) )
      {
         send_to_char( "You have not completed all the prequests.\r\n", ch );
         return;
      }

   if( !pquest && ( pquest = create_player_quest( ch, quest ) ) != NULL )
      pquest->stage = QUEST_START;

   send_to_char( "You have accepted the quest!\r\n", ch );
   save_char_obj( ch );
   saving_char = NULL;
   mprog_quest_trigger( victim, ch );
   return;
}

PLAYER_QUEST *create_player_quest( CHAR_DATA *ch, QUEST_DATA *quest )
{
   PLAYER_QUEST *pquest;

   CREATE( pquest, PLAYER_QUEST, 1 );
   pquest->quest = quest;
   pquest->stage = 0;
   pquest->progress = "";
   LINK( pquest, ch->first_pquest, ch->last_pquest, next, prev );
   return pquest;
}

AV_QUEST *create_available_quest( CHAR_DATA *ch, CHAR_DATA *victim, QUEST_DATA *quest )
{
   AV_QUEST *av_quest;

   if( !IS_NPC( victim ) )
   {
      bug( "%s: attempting to create quest on non-npc named %s", __FUNCTION__, victim->name );
      return NULL;
   }

   if( !quest )
   {
      bug( "%s: somehow quest is NULL.", __FUNCTION__ );
      return NULL;
   }

   CREATE( av_quest, AV_QUEST, 1 );
   av_quest->quest = quest;
   LINK( av_quest, victim->pIndexData->first_available_quest, victim->pIndexData->last_available_quest, next, prev );
   return av_quest;
}

void show_mob_quest( CHAR_DATA *ch, CHAR_DATA *victim, const char *argument )
{
   QUEST_DATA *quest;
   AV_QUEST *av_quest;
   PRE_QUEST *prequest;

   if( ( av_quest = get_available_quest_from_list( victim, argument ) ) == NULL || !av_quest->quest )
   {
      send_to_char( "That Mob does not contain a quest with that high of an list number.\r\n", ch );
      return;
   }
   else
      quest = av_quest->quest;

   ch_printf( ch, "Name: %s\r\nLevel Required: %d\r\nType: %s\r\n", quest->name, quest->level_req, quest_types[quest->type] );
   if( quest->first_prequest )
   {
      send_to_char( "PreQuests:", ch );
      for( prequest = quest->first_prequest; prequest; prequest = prequest->next )
         if( prequest->quest )
            ch_printf( ch, " %15.15s,", prequest->quest->name );
      send_to_char( "\r\n", ch );
   }
   ch_printf( ch, "Description: %s\r\n", quest->description );
   return;
}

void list_mob_quest( CHAR_DATA *ch, CHAR_DATA *victim )
{
   AV_QUEST *av_quest, *av_quest_next;
   int x = 0;

   send_to_char( "This mob has these quests available:\r\n", ch );

   for( av_quest = victim->pIndexData->first_available_quest; av_quest; av_quest = av_quest_next )
   {
      av_quest_next = av_quest->next;
      if( !av_quest->quest )
      {
         UNLINK( av_quest, victim->pIndexData->first_available_quest, victim->pIndexData->last_available_quest, next, prev );
         free_avquest( av_quest );
         bug( "%s: something real fucked up. Removing one of Mob %d's av_quest", __FUNCTION__, victim->pIndexData->vnum );
         continue;
      }

      ch_printf( ch, "Quest %d: %-15.15s Level: %-3d Type %-10.10s Status: %s\r\n",
                 x++,
                 av_quest->quest->name,
                 av_quest->quest->level_req,
                 quest_types[av_quest->quest->type],
                 get_status( ch, av_quest->quest ) );
   }
   return;
}

const char *get_status( CHAR_DATA *ch, QUEST_DATA *quest )
{
   PLAYER_QUEST *pquest;
   const char *status = "Unavailable";

   if( ( pquest = get_player_quest( ch, quest ) ) == NULL )
      status = "Not Started";
   else if( pquest->stage == QUEST_COMPLETE )
      status = "Complete";
   else if( pquest->stage == QUEST_COMPLETE_REPEATABLE )
      status = "Repeatable";
   else if( pquest->stage == QUEST_UNSTARTED )
      status = "Not Started";
   else if( pquest->stage == QUEST_START )
      status = "Recently Started";
   else
      status = "Incomplete";

   return status;
}

void save_pools( void )
{
   FILE *fp;
   POOL_DATA *pool;

   if( ( fp = fopen( POOL_FILE, "w" ) ) == NULL )
   {
      bug( "Cannot open pool.dat for writting", 0 );
      perror( POOL_FILE );
      return;
   }

   for( pool = first_pool; pool; pool = pool->next )
   {
      fprintf( fp, "#POOL\n" );
      fwrite_pool( fp, pool );
   }

   fprintf( fp, "#END\n" );
   fclose( fp );
}

void fwrite_pool( FILE *fp, POOL_DATA *pool )
{
   int x;
   fprintf( fp, "ID           %d\n", pool->id );
   fprintf( fp, "Location     %d\n", pool->location );
   fprintf( fp, "MinStat      %d\n", pool->minstat );
   fprintf( fp, "MaxStat      %d\n", pool->maxstat );
   fprintf( fp, "MinLevel     %d\n", pool->minlevel );
   fprintf( fp, "MaxLevel     %d\n", pool->maxlevel );
   fprintf( fp, "Rules       " );
   for( x = 0; x < MAX_ITEM_WEAR; x++ )
      fprintf( fp, " %d", pool->rules[x] );
   fprintf( fp, "\n" );
   fprintf( fp, "End\n" );
   return;
}

void do_pool( CHAR_DATA *ch, const char *argument )
{
   char arg[MAX_INPUT_LENGTH];

   argument = one_argument( argument, arg );
   if( arg[0] == '\0' )
   {
      send_to_char( "Proper usage: pool create <location> <minstat> <maxstat> <minlevel> <maxlevel> <rules...etc>\r\n", ch );
      send_to_char( "              pool list no arg|id|location|minstat|maxstat|minlevel|maxlevel|rule <value>\r\n", ch );
      send_to_char( "              pool delete <id>\r\n", ch );
      return;
   }
   if( !str_cmp( arg, "create" ) )
   {
      create_pool( ch, argument );
      return;
   }
   if( !str_cmp( arg, "list" ) )
   {
      list_pools( ch, argument );
      return;
   }
   if( !str_cmp( arg, "delete" ) )
   {
      delete_pool( ch, get_pool_from_id( atoi( argument ) ) );
      return;
   }
   do_pool( ch, "" );
   return;
}

void delete_pool( CHAR_DATA *ch, POOL_DATA *pool )
{
   OBJ_DATA *obj;
   AFFECT_DATA *af, *af_next;

   if( !pool )
   {
      bug( "%s: bad pool id.\r\n", __FUNCTION__ );
      return;
   }

   for( obj = first_object; obj; obj = obj->next )
      for( af = obj->first_affect; af; af = af_next )
      {
         af_next = af->next;
         if( af->from_pool && af->from_pool->id == pool->id )
         {
            if( obj->wear_loc != -1 )
               affect_modify( obj->carried_by, af, FALSE );
            UNLINK( af, obj->first_affect, obj->last_affect, next, prev );
            free_affect( af );
         }
      }
   UNLINK( pool, first_pool, last_pool, next, prev );
   free_pool( pool );
   save_pools( );
   send_to_char( "Pool Deleted.\r\n", ch );
   return;
}

void list_pools( CHAR_DATA *ch, const char *argument )
{
   POOL_DATA *pool;
   char arg[MAX_INPUT_LENGTH];
   int sort;

   argument = one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      send_to_char( "Listing All Pools...\r\n", ch );
      for( pool = first_pool; pool; pool = pool->next )
         display_pool( ch, pool );
      return;
   }
   if( !str_cmp( arg, "id" ) )
   {
      if( !is_number( argument ) )
      {
         send_to_char( "You must enter a number.\r\n", ch );
         return;
      }
      display_pool( ch, get_pool_from_id( atoi( argument ) ) );
      return;
   }
   if( !str_cmp( arg, "location" ) )
   {
      if( ( sort = get_atype( argument ) ) == -1 )
      {
         send_to_char( "Invalid location used.\r\n", ch );
         return;
      }
      ch_printf( ch, "Pools with %s location...\r\n", a_types[sort] );
      for( pool = first_pool; pool; pool = pool->next )
         if( pool->location == sort )
            display_pool( ch, pool );
      return;
   }
   if( !str_cmp( arg, "minstat" ) )
   {
      if( !is_number( argument ) )
      {
         send_to_char( "Enter a number for minstat.\r\n", ch );
         return;
      }
      sort = atoi( argument );
      ch_printf( ch, "Pools with minstat value of %d...\r\n", sort );
      for( pool = first_pool; pool; pool = pool->next )
         if( pool->minstat == sort )
            display_pool( ch, pool );
      return;
   }
   if( !str_cmp( arg, "maxstat" ) )
   {
      if( !is_number( argument ) )
      {
         send_to_char( "Enter a number for maxstat.\r\n", ch );
         return;
      }
      sort = atoi( argument );
      ch_printf( ch, "Pools with maxstat value of %d...\r\n", sort );
      for( pool = first_pool; pool; pool = pool->next )
         if( pool->maxstat == sort )
            display_pool( ch, pool );
      return;
   }
   if( !str_cmp( arg, "minlevel" ) )
   {
      if( !is_number( argument ) )
      {
         send_to_char( "Enter a number for minlevel.\r\n", ch );
         return;
      }
      sort = atoi( argument );
      ch_printf( ch, "Pools with minlevel value of %d...\r\n", sort );
      for( pool = first_pool; pool; pool = pool->next )
         if( pool->minlevel == sort )
            display_pool( ch, pool );
      return;
   }
   if( !str_cmp( arg, "maxlevel" ) )
   {
      if( !is_number( argument ) )
      {
         send_to_char( "Enter a number for maxlevel.\r\n", ch );
         return;
      }
      sort = atoi( argument );
      ch_printf( ch, "Pools with maxlevel value of %d...\r\n", sort );
      for( pool = first_pool; pool; pool = pool->next )
         if( pool->maxlevel == sort )
            display_pool( ch, pool );
      return;
   }
   if( !str_cmp( arg, "rule" ) )
   {
      if( ( sort = get_wflag( argument ) ) == -1 )
      {
         send_to_char( "Invalid wearloc.\r\n", ch );
         return;
      }
      ch_printf( ch, "Pools with Rule location of %s...\r\n", w_flags[sort] );
      for( pool = first_pool; pool; pool = pool->next )
         if( pool->rules[sort] == 1 )
            display_pool( ch, pool );
      return;
   }
   return;
}

void display_pool( CHAR_DATA *ch, POOL_DATA *pool )
{
   int x;

   if( !pool )
   {
      bug( "%s: bad pool id", __FUNCTION__ );
      return;
   }

   ch_printf( ch, "ID %d: affects %s by %d to %d loads on mob from level %d to %d\r\n", 
              pool->id,
              a_types[pool->location],
              pool->minstat,
              pool->maxstat,
              pool->minlevel,
              pool->maxlevel );
   send_to_char( " - Rules:", ch );
   for( x = 0; x < MAX_ITEM_WEAR; x++ )
      if( pool->rules[x] == 1 )
         ch_printf( ch, " %s,", w_flags[x] );
   send_to_char( "\r\n", ch );
}

void create_pool( CHAR_DATA *ch, const char *argument )
{
   POOL_DATA *pool;
   char arg[MAX_INPUT_LENGTH];
   int location, minstat, maxstat, minlevel, maxlevel;
   int rules[MAX_ITEM_WEAR];
   int x, col;

   /* start with location */
   argument = one_argument( argument, arg );
   if( ( location = get_atype( arg ) ) == -1 )
   {
      send_to_char( "Valid Locations:\r\n", ch );
      for( x = 0, col = 0; x < MAX_APPLY_TYPE; x++ )
      {
         ch_printf( ch, "%-18.18s", a_types[x] ? a_types[x] : "null" );
         if( 4 == col++ )
         {
            send_to_char( "\r\n", ch );
            col = 0;
         }
      }
      if( col != 0 )
         send_to_char( "\r\n", ch );
      return;
   }

   /* now minstat */
   argument = one_argument( argument, arg );
   if( !is_number( arg ) )
   {
      send_to_char( "Minstat must be a number!\r\n", ch );
      return;
   }
   minstat = atoi( arg );

   /* now maxstat */
   argument = one_argument( argument, arg );
   if( !is_number( arg ) )
   {
      send_to_char( "Maxstat must be a number!\r\n", ch );
      return;
   }
   if( ( maxstat = atoi(arg ) ) < minstat )
   {
      send_to_char( "Maxstat must be higher than minstat!\r\n", ch );
      return;
   }

   /* now minlevel */
   argument = one_argument( argument, arg );
   if( !is_number( arg ) )
   {
      send_to_char( "Minlevel must be a number!\r\n", ch );
      return;
   }
   minlevel = atoi( arg );

   /* now maxlevel */
   argument = one_argument( argument, arg );
   if( !is_number( arg ) )
   {
      send_to_char( "Maxlevel must be a number!\r\n", ch );
      return;
   }
   if( ( maxlevel = atoi( arg ) ) < minlevel )
   {
      send_to_char( "Maxlevel must be higher than minlevel!\r\n", ch );
      return;
   }

   /* now to handle rules */
   for( x = 0; x < MAX_ITEM_WEAR; x++ ) /* init our array */
      rules[x] = 0;

   while( argument[0] != '\0' )
   {
      argument = one_argument( argument, arg );
      if( ( x = get_wflag( arg ) ) == -1 )
      {
         ch_printf( ch, "%s invalid wearlocation.\r\n", arg );
         continue;
      }
      rules[x] = 1;
   }
   int id;
   for( ;; )
   {
      id = number_range( 1000, 9999 );
      if( !(get_pool_from_id( id )) )
         break;
   }
   CREATE( pool, POOL_DATA, 1 );
   pool->id = id;
   pool->location = location;
   pool->minstat = minstat;
   pool->maxstat = maxstat;
   pool->minlevel = minlevel;
   pool->maxlevel = maxlevel;
   for( x = 0; x < MAX_ITEM_WEAR; x++ )
      pool->rules[x] = rules[x];
   LINK( pool, first_pool, last_pool, next, prev );
   save_pools( );
   send_to_char( "Ok.\r\n", ch );
};

void do_addloot( CHAR_DATA *ch, const char *argument )
{
   CHAR_DATA *victim = NULL;
   PLANET_DATA *planet = NULL;
   AREA_DATA *area = NULL;
   OBJ_INDEX_DATA *pObjIndex;
   LOOT_DATA *loot;
   char arg[MAX_INPUT_LENGTH];
   int type;
   int chance;
   int amount;

   /* Grab Target First */
   argument = one_argument( argument, arg );
   if( arg[0] == '\0' )
   {
      send_to_char( "Proper Syntax: addloot <target> <type> <vnum> <amount> <%chance>\r\n\r\n", ch );
      send_to_char( " Targets: mob, area filename, planet\r\n", ch );
      send_to_char( " Types: set, random, gold\r\n", ch );
      send_to_char( "\r\nEnter any vnum for gold\r\n", ch );
      return;
   }
   if( ( victim = get_char_world( ch, arg ) ) == NULL && ( area = get_area_from_filename( arg ) ) == NULL && ( planet = get_planet( arg ) ) == NULL )
   {
      send_to_char( "Invalid target.\r\n", ch );
      return;
   }

   /* Lets grab the type next */
   argument = one_argument( argument, arg );
   if( arg[0] == '\0' || ( type = get_loottype( arg ) ) == -1 )
   {
      send_to_char( "Invalid type.\r\n", ch );
      return;
   }

   /* Lets grab the obj_index next */
   argument = one_argument( argument, arg );
   if( arg[0] == '\0' || ( ( pObjIndex = get_obj_index( atoi( arg ) ) ) == NULL && type != LOOT_GOLD ) )
   {
      send_to_char( "Invalid vnum.\r\n", ch );
      return;
   }

   /* Time to grab amount */
   argument = one_argument( argument, arg );
   if( arg[0] == '\0' || !is_number( arg ) || ( amount = atoi( arg ) ) < 1 )
   {
      send_to_char( "Invalid amount.\r\n", ch );
      return;
   }

   /* Now to make sure we have a good "chance" */
   argument = one_argument( argument, arg );
   if( arg[0] == '\0' || !is_number( arg ) || ( chance = atoi( arg ) ) > 100 || chance < 0 )
   {
      send_to_char( "Invalid chance.\r\n", ch );
      return;
   }

   CREATE( loot, LOOT_DATA, 1 );
   loot->type = type;
   loot->vnum = type != LOOT_GOLD ? pObjIndex->vnum : 0;
   loot->amount = amount;
   loot->percent = chance;

   if( victim )
   {
      if( !IS_NPC( victim ) )
      {
         free_loot( loot );
         send_to_char( "Not on players.\r\n", ch );
         return;
      }
      LINK( loot, victim->pIndexData->first_loot, victim->pIndexData->last_loot, next, prev );
      send_to_char( "Loot Added to NPC.\r\n", ch );
      return;
   }
   else if( planet )
   {
      LINK( loot, planet->first_loot, planet->last_loot, next, prev );
      save_planet( planet );
      send_to_char( "Loot Added to Planet.\r\n", ch );
      return;
   }
   else if( area )
   {
      LINK( loot, area->first_loot, area->last_loot, next, prev );
      fold_area( area, area->filename, FALSE );
      send_to_char( "Loot Added to Area.\r\n", ch );
      return;
   }
   free_loot( loot );
   bug( "%s: got to end with invalid target...", __FUNCTION__ );
   return;
}

void do_remloot( CHAR_DATA *ch, const char *argument )
{
   CHAR_DATA *victim;
   PLANET_DATA *planet;
   AREA_DATA *area;
   LOOT_DATA *loot;
   char arg[MAX_INPUT_LENGTH];
   int number, count;

   /* Grab Target First */
   argument = one_argument( argument, arg );
   if( arg[0] == '\0' )
   {
      send_to_char( "Proper Syntax: remloot <target> <#>\r\n\r\n", ch );
      send_to_char( " Targets: mob, area filename, planet\r\n", ch );
      send_to_char( "\r\nNumber starts at 1\r\n", ch );
      return;
   }
   if( ( victim = get_char_world( ch, arg ) ) == NULL && ( area = get_area_from_filename( arg ) ) == NULL && ( planet = get_planet( arg ) ) == NULL )
   {
      send_to_char( "Invalid target.\r\n", ch );
      return;
   }

   if( !is_number( argument ) )
   {
      send_to_char( "Enter a number.\r\n", ch );
      return;
   }
   number = atoi( argument );

   if( ( number = atoi( argument ) ) < 1 )
   {
      send_to_char( "Number must be greater than one.\r\n", ch );
      return;
   }
   count = 0;

   if( victim )
   {
      if( !IS_NPC( victim ) )
      {
         send_to_char( "Not on players.\r\n", ch );
         return;
      }
      for( loot = victim->pIndexData->first_loot; loot; loot = loot->next )
         if( ++count == number )
         {
            UNLINK( loot, victim->pIndexData->first_loot, victim->pIndexData->last_loot, next, prev );
            free_loot( loot );
            send_to_char( "Loot removed.\r\n", ch );
            return;
         }
      ch_printf( ch, "%s doesn't have that many loot datas.\r\n", victim->name );
      return;
   }
   else if( area )
   {
      for( loot = area->first_loot; loot; loot = loot->next )
         if( ++count == number )
         {
            UNLINK( loot, area->first_loot, area->last_loot, next, prev );
            free_loot( loot );
            fold_area( area, area->filename, FALSE );
            send_to_char( "Loot removed.\r\n", ch );
            return;
         }
      ch_printf( ch, "%s doesn't have that many loot datas.\r\n", area->filename );
      return;
   }
   else if( planet )
   {
      for( loot = planet->first_loot; loot; loot = loot->next )
         if( ++count == number )
         {
            UNLINK( loot, planet->first_loot, planet->last_loot, next, prev );
            free_loot( loot );
            save_planet( planet );
            send_to_char( "Look removed.\r\n", ch );
            return;
         }
      ch_printf( ch, "%s doesn't have that many loot datas.\r\n", planet->name );
      return;
   }
   bug( "%s: got to end of function without resolution.", __FUNCTION__ );
   return;
}

void save_thoughts( void )
{
   FILE *fp;
   AI_THOUGHT *thought;

   if( ( fp = fopen( THOUGHT_FILE, "w" ) ) == NULL )
   {
      bug( "Cannot open thoughts.dat for writting", 0 );
      perror( THOUGHT_FILE );
      return;
   }

   for( thought = first_thought; thought; thought = thought->next )
   {
      fprintf( fp, "#THOUGHT\n" );
      fwrite_thought( fp, thought );
   }

   fprintf( fp, "#END\n" );
   fclose( fp );
}

void fwrite_thought( FILE *fp, AI_THOUGHT *thought )
{
   fprintf( fp, "ID           %d\n", thought->id );
   fprintf( fp, "Name         %s~\n", thought->name );
   fprintf( fp, "FoM          %d\n", thought->fom );
   fprintf( fp, "MinHp        %d\n", thought->minhp );
   fprintf( fp, "MaxHp        %d\n", thought->maxhp );
   fprintf( fp, "Script       %s~\n", strip_cr( thought->script ) );
   fprintf( fp, "End\n" );
   return;
}

void do_thought( CHAR_DATA *ch, const char *argument )
{
   AI_THOUGHT *thought;
   char arg[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char arg3[MAX_INPUT_LENGTH];

   switch ( ch->substate )
   {
      default:
         break;
      case SUB_THOUGHT_EDIT:
         thought = ( AI_THOUGHT* ) ch->dest_buf;
         if( !thought )
         {
            bug( "thought: sub_thought_edit: NULL ch->dest_buf", 0 );
            return;
         }
         STRFREE( thought->script );
         thought->script = copy_buffer( ch );
         stop_editing( ch );
         ch->substate = ch->tempnum;
         save_thoughts( );
         return;
   }

   if( argument[0] == '\0' )
   {
      send_to_char( "Proper usage: thought create <name>\r\n", ch );
      send_to_char( "              thought edit <name> <parameter> <value>\r\n", ch );
      send_to_char( "              thought list\r\n", ch );
      send_to_char( "Parameters: fom, minhp, maxhp, name, script\r\n", ch );
      return;
   }

   argument = one_argument( argument, arg );
   if( !str_cmp( arg, "create" ) )
   {
      if( argument[0] == '\0' )
      {
         send_to_char( "You must enter a name.\r\n", ch );
         return;
      }
      create_thought( ch, argument );
      save_thoughts( );
      send_to_char( "Thought Created.\r\n", ch );
      return;
   }
   if( !str_cmp( arg, "list" ) )
   {
      list_thoughts( ch );
      return;
   }
   if( !str_cmp( arg, "edit" ) )
   {
      argument = one_argument( argument, arg2 ); /* get the quest name */
      if( ( thought = get_thought( arg2 ) ) == NULL )
      {
         send_to_char( "No such thought exists.\r\n", ch );
         return;
      }

      argument = one_argument( argument, arg3 ); /* get the parameter */
      if( arg3[0] == '\0' )
      {
         send_to_char( "Valid parameters: fom, minhp, maxhp, name, script\r\n", ch );
         return;
      }
      if( !str_cmp( arg3, "script" ) )
      {
         if( ch->substate == SUB_REPEATCMD )
            ch->tempnum = SUB_REPEATCMD;
         else
            ch->tempnum = SUB_NONE;
         ch->substate = SUB_THOUGHT_EDIT;
         ch->dest_buf = thought;
         start_editing( ch, (char *)thought->script );
         return;
      }
      edit_thought( ch, thought, argument, arg3 );
      save_thoughts( );
      return;
   }
   do_thought( ch, "" );
   return;
}

void list_thoughts( CHAR_DATA *ch )
{
   AI_THOUGHT *thought;
   int alpha;

   for( alpha = 0; alpha < 27; alpha++ )
      for( thought = first_thought; thought; thought = thought->next )
         if( ( ( LOWER( thought->name[0] ) - 'a' ) + 1 ) == alpha )
            ch_printf( ch, "Thought: %-40.40s MinHP: %-3d MaxHP: %-3d FoM: %s\r\n",
                       thought->name,
                       thought->minhp,
                       thought->maxhp,
                       frames_of_mind[thought->fom] );

   return;
}

void create_thought( CHAR_DATA *ch, const char *argument )
{
   AI_THOUGHT *thought;

   if( argument[0] == '\0' )
   {
      bug( "%s: being called with null argument, ie the name", __FUNCTION__ );
      return;
   }

   int id;
   for( ;; )
   {
      id = number_range( 1000, 9999 );
      if( !(get_thought_from_id( id )) )
         break;
   }

   CREATE( thought, AI_THOUGHT, 1 );
   thought->id = id;
   thought->name = STRALLOC( argument );
   thought->script = STRALLOC( "" );
   thought->fom = 0;
   thought->minhp = 1;
   thought->maxhp = 100;
   LINK( thought, first_thought, last_thought, next, prev );
   return;
}


void edit_thought( CHAR_DATA *ch, AI_THOUGHT *thought, const char *argument, const char *parameter )
{
   int value;

   if( !str_cmp( parameter, "minhp" ) || !str_cmp( parameter, "maxhp" ) )
   {
      if( !is_number( argument ) )
      {
         send_to_char( "You must enter a number for value.\r\n", ch );
         return;
      }
      value = atoi( argument );

      if( value < 1 || value > 100 )
      {
         send_to_char( "MinHP or MaxHP must be between 1 and 100.\r\n", ch );
         return; 
      }

      if( !str_cmp( parameter, "minhp" ) )
      {
         if( value >= thought->maxhp )
         {
            send_to_char( "Minhp must be lower than Maxhp!\r\n", ch );
            return;
         }
         thought->minhp = value;
      }
      if( !str_cmp( parameter, "maxhp" ) )
      {
         if( value <= thought->minhp )
         {
            send_to_char( "Maxhp must be higher than Minhp!\r\n", ch );
            return;
         }
         thought->maxhp = value;
      }
      send_to_char( "Ok.\r\n", ch );
      return;
   }

   if( !str_cmp( parameter, "fom" ) )
   {
      if( ( value = get_frame_of_mind( argument ) ) == -1 )
      {
         int x;
         send_to_char( "Invalid frame of mind.\r\n Valid FoMs:", ch );
         for( x = 0; x < MAX_FOM; x++ )
            ch_printf( ch, " %s,", frames_of_mind[x] );
         send_to_char( "\r\n", ch );
         return;
      }
      thought->fom = value;
      send_to_char( "Ok.\r\n", ch );
      return;
   }

   if( !str_cmp( parameter, "name" ) )
   {
      if( thought->name[0] != '\0' )
         STRFREE( thought->name );
      thought->name = STRALLOC( argument );
      send_to_char( "Ok.\r\n", ch );
      return;
   }

   do_thought( ch, "" );
   return;
}
