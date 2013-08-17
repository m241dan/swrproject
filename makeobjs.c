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
*			Specific object creation module			   *
****************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

/*
 * Make a fire.
 */
void make_fire( ROOM_INDEX_DATA * in_room, short timer )
{
   OBJ_DATA *fire;

   fire = create_object( get_obj_index( OBJ_VNUM_FIRE ), 0 );
   fire->timer = number_fuzzy( timer );
   obj_to_room( fire, in_room );
   return;
}

/*
 * Make a trap.
 */
OBJ_DATA *make_trap( int v0, int v1, int v2, int v3 )
{
   OBJ_DATA *trap;

   trap = create_object( get_obj_index( OBJ_VNUM_TRAP ), 0 );
   trap->timer = 0;
   trap->value[0] = v0;
   trap->value[1] = v1;
   trap->value[2] = v2;
   trap->value[3] = v3;
   return trap;
}

/*
 * Turn an object into scraps.		-Thoric
 */
void make_scraps( OBJ_DATA * obj )
{
   char buf[MAX_STRING_LENGTH];
   OBJ_DATA *scraps, *tmpobj;
   CHAR_DATA *ch = NULL;

   separate_obj( obj );
   scraps = create_object( get_obj_index( OBJ_VNUM_SCRAPS ), 0 );
   scraps->timer = number_range( 5, 15 );

   /*
    * don't make scraps of scraps of scraps of ... 
    */
   if( obj->pIndexData->vnum == OBJ_VNUM_SCRAPS )
   {
      STRFREE( scraps->short_descr );
      scraps->short_descr = STRALLOC( "some debris" );
      STRFREE( scraps->description );
      scraps->description = STRALLOC( "Bits of debris lie on the ground here." );
   }
   else
   {
      sprintf( buf, scraps->short_descr, obj->short_descr );
      STRFREE( scraps->short_descr );
      scraps->short_descr = STRALLOC( buf );
      sprintf( buf, scraps->description, obj->short_descr );
      STRFREE( scraps->description );
      scraps->description = STRALLOC( buf );
   }

   if( obj->carried_by )
   {
      act( AT_OBJECT, "$p falls to the ground in scraps!", obj->carried_by, obj, NULL, TO_CHAR );
      if( obj == get_eq_char( obj->carried_by, WEAR_WIELD )
          && ( tmpobj = get_eq_char( obj->carried_by, WEAR_DUAL_WIELD ) ) != NULL )
         tmpobj->wear_loc = WEAR_WIELD;

      obj_to_room( scraps, obj->carried_by->in_room );
   }
   else if( obj->in_room )
   {
      if( ( ch = obj->in_room->first_person ) != NULL )
      {
         act( AT_OBJECT, "$p is reduced to little more than scraps.", ch, obj, NULL, TO_ROOM );
         act( AT_OBJECT, "$p is reduced to little more than scraps.", ch, obj, NULL, TO_CHAR );
      }
      obj_to_room( scraps, obj->in_room );
   }
   if( ( obj->item_type == ITEM_CONTAINER || obj->item_type == ITEM_CORPSE_PC ) && obj->first_content )
   {
      if( ch && ch->in_room )
      {
         act( AT_OBJECT, "The contents of $p fall to the ground.", ch, obj, NULL, TO_ROOM );
         act( AT_OBJECT, "The contents of $p fall to the ground.", ch, obj, NULL, TO_CHAR );
      }
      if( obj->carried_by )
         empty_obj( obj, NULL, obj->carried_by->in_room );
      else if( obj->in_room )
         empty_obj( obj, NULL, obj->in_room );
      else if( obj->in_obj )
         empty_obj( obj, obj->in_obj, NULL );
   }
   extract_obj( obj );
}

/*
 * Make a corpse out of a character.
 */
OBJ_DATA *make_corpse( CHAR_DATA * ch, CHAR_DATA * killer )
{
   char buf[MAX_STRING_LENGTH];
   OBJ_DATA *corpse;
   OBJ_DATA *obj;
   OBJ_DATA *obj_next;
   const char *name;

   if( IS_NPC( ch ) )
   {
      name = ch->short_descr;
      if( IS_SET( ch->act, ACT_DROID ) )
         corpse = create_object( get_obj_index( OBJ_VNUM_DROID_CORPSE ), 0 );
      else
         corpse = create_object( get_obj_index( OBJ_VNUM_CORPSE_NPC ), 0 );
      corpse->timer = 6;
      if( ch->gold > 0 )
      {
         if( ch->in_room )
            ch->in_room->area->gold_looted += ch->gold;
         obj_to_obj( create_money( ch->gold ), corpse );
         ch->gold = 0;
      }

/* Cannot use these!  They are used.
	corpse->value[0] = (int)ch->pIndexData->vnum;
	corpse->value[1] = (int)ch->max_hit;
*/
/*	Using corpse cost to cheat, since corpses not sellable */
      corpse->cost = ( -( int )ch->pIndexData->vnum );
      corpse->value[2] = corpse->timer;
   }
   else
   {
      name = ch->name;
      corpse = create_object( get_obj_index( OBJ_VNUM_CORPSE_PC ), 0 );
      corpse->timer = 40;
      corpse->value[2] = ( int )( corpse->timer / 8 );
      corpse->value[3] = 0;
      if( ch->gold > 0 )
      {
         if( ch->in_room )
            ch->in_room->area->gold_looted += ch->gold;
         obj_to_obj( create_money( ch->gold ), corpse );
         ch->gold = 0;
      }
   }

   /*
    * Added corpse name - make locate easier , other skills 
    */
   sprintf( buf, "corpse %s", name );
   STRFREE( corpse->name );
   corpse->name = STRALLOC( buf );

   sprintf( buf, corpse->short_descr, name );
   STRFREE( corpse->short_descr );
   corpse->short_descr = STRALLOC( buf );

   sprintf( buf, corpse->description, name );
   STRFREE( corpse->description );
   corpse->description = STRALLOC( buf );

   handle_loot( corpse, ch ); // Handle loot

   for( obj = ch->first_carrying; obj; obj = obj_next )
   {
      obj_next = obj->next_content;
      obj_from_char( obj );
      if( IS_OBJ_STAT( obj, ITEM_INVENTORY ) || IS_OBJ_STAT( obj, ITEM_DEATHROT ) )
         extract_obj( obj );
      else
         obj_to_obj( obj, corpse );
   }
   return obj_to_room( corpse, ch->in_room );
}

void make_blood( CHAR_DATA * ch )
{
   OBJ_DATA *obj;

   obj = create_object( get_obj_index( OBJ_VNUM_BLOOD ), 0 );
   obj->timer = number_range( 2, 4 );
   obj->value[1] = number_range( 3, UMIN( 5, ch->top_level ) );
   obj_to_room( obj, ch->in_room );
}

void make_bloodstain( CHAR_DATA * ch )
{
   OBJ_DATA *obj;

   obj = create_object( get_obj_index( OBJ_VNUM_BLOODSTAIN ), 0 );
   obj->timer = number_range( 1, 2 );
   obj_to_room( obj, ch->in_room );
}


/*
 * make some coinage
 */
OBJ_DATA *create_money( int amount )
{
   char buf[MAX_STRING_LENGTH];
   OBJ_DATA *obj;

   if( amount <= 0 )
   {
      bug( "Create_money: zero or negative money %d.", amount );
      amount = 1;
   }

   if( amount == 1 )
   {
      obj = create_object( get_obj_index( OBJ_VNUM_MONEY_ONE ), 0 );
   }
   else
   {
      obj = create_object( get_obj_index( OBJ_VNUM_MONEY_SOME ), 0 );
      sprintf( buf, obj->short_descr, amount );
      STRFREE( obj->short_descr );
      obj->short_descr = STRALLOC( buf );
      obj->value[0] = amount;
   }

   return obj;
}

void handle_loot( OBJ_DATA *corpse, CHAR_DATA *victim )
{
   OBJ_INDEX_DATA *pObjIndex;
   LOOT_DATA *loot;

   if( !IS_NPC( victim ) )
      return;

   for( loot = victim->pIndexData->first_loot; loot; loot = loot->next )
      dispense_loot( corpse, loot );

   for( loot = victim->in_room->area->first_loot; loot; loot = loot->next )
   {
      if( ( pObjIndex = get_obj_index( loot->vnum ) ) == NULL && loot->type != LOOT_GOLD )
      {
         bug( "%s: loot with bad vnum %d", __FUNCTION__, loot->vnum );
         continue;
      }
      if( pObjIndex->level > victim->moblevel )
         continue;
      dispense_loot( corpse, loot );
   }

   if( !victim->in_room->area->planet )
      return;

   for( loot = victim->in_room->area->planet->first_loot; loot; loot = loot->next )
   {
      if( ( pObjIndex = get_obj_index( loot->vnum ) ) == NULL && loot->type != LOOT_GOLD )
      {
         bug( "%s: loot with bad vnum %d", __FUNCTION__, loot->vnum );
         continue;
      }
      if( pObjIndex->level > victim->moblevel )
         continue;
      dispense_loot( corpse, loot );
   }
   return;
}

void dispense_loot( OBJ_DATA *corpse, LOOT_DATA *loot )
{
   OBJ_INDEX_DATA *pObjIndex; // for set
   OBJ_DATA *obj; //for random
   int count;

   /* chance to get this loot */
   if( number_percent( ) > loot->percent )
      return;

   switch( loot->type )
   {
      case LOOT_GOLD:
         obj_to_obj( create_money( loot->amount ), corpse );
         break;
      case LOOT_SET:
         if( ( pObjIndex = get_obj_index( loot->vnum ) ) == NULL )
         {
            bug( "%s: bad loot vnum %d", __FUNCTION__, loot->vnum );
            return;
         }
         for( count = 0; count < loot->amount; count++ )
            obj_to_obj( create_object( pObjIndex, pObjIndex->level ), corpse );
         break;
      case LOOT_RANDOM:
         if( ( obj = random_loot( loot ) ) == NULL )
         {
            bug( "%s: bad loot data", __FUNCTION__ );
            return;
         }
         for( count = 0; count < loot->amount; count++ )
            obj_to_obj( random_loot( loot ), corpse );
         break;
   }
   return;
}

OBJ_DATA *random_loot( LOOT_DATA *loot )
{
   OBJ_INDEX_DATA *pObjIndex;
   OBJ_DATA *obj;
   EXT_BV quality = roll_quality( );

   if( ( pObjIndex = get_obj_index( loot->vnum ) ) == NULL )
   {
      bug( "%s: bad loot vnum %d", __FUNCTION__, loot->vnum );
      return NULL;
   }
   obj = create_object( pObjIndex, pObjIndex->level );
   xCLEAR_BITS( obj->quality );
   xSET_BITS( obj->quality, quality );

   if( xIS_SET( obj->quality, QUALITY_COMMON ) )
      obj->max_pool = number_range( 0, 2 );
   else if( xIS_SET( obj->quality, QUALITY_UNCOMMON ) )
   {
      obj->max_pool = number_range( 1, 4 );
      if( obj->item_type == ITEM_WEAPON )
      {
         obj->value[1] = (int)( obj->value[1] * 1.1 );
         obj->value[2] = (int)( obj->value[2] * 1.1 );
      }
      else if( obj->item_type == ITEM_ARMOR )
      {
         obj->value[0] = (int)( obj->value[0] * 1.1 );
         obj->value[2] = (int)( obj->value[2] * 1.1 );
      }
      obj->cost = (int)( obj->cost * 1.25 );
   }
   else if( xIS_SET( obj->quality, QUALITY_RARE ) )
   {
      obj->max_pool = number_range( 2, 6 );
      if( obj->item_type == ITEM_WEAPON )
      {
         obj->value[1] = (int)( obj->value[1] * 1.25 );
         obj->value[2] = (int)( obj->value[2] * 1.25 );
      }
      else if( obj->item_type == ITEM_ARMOR )
      {
         obj->value[0] = (int)( obj->value[0] * 1.25 );
         obj->value[2] = (int)( obj->value[2] * 1.25 );
      }
      obj->cost = (int)( obj->cost * 1.60 );

   }
   if( obj->max_pool > 0 )
      load_pools( obj );

   return obj;
}

EXT_BV roll_quality( void )
{
   int number = number_range( 1, 150);

   if( number < RARE_CHANCE )
      return meb( QUALITY_RARE );
   if( number < UNCOMMON_CHANCE )
      return meb( QUALITY_UNCOMMON );
   if( number < COMMON_CHANCE )
      return meb( QUALITY_COMMON );
   return meb( 0 );
}

void load_pools( OBJ_DATA *obj )
{
   POOL_DATA *pool;
   int total_pool = get_total_pools( );
   int chances = POOL_CHANCE_PER_SLOT * obj->max_pool;
   int x, success;

   for( x = 0, success = 0; x < chances; x++ )
   {
      pool = get_pool_from_count( number_range( 1, total_pool ) );
      if( obj->level >= pool->minlevel && obj->level <= pool->maxlevel && rule_check( obj, pool ) )
      {
         LINK( create_affect_from_pool( pool ), obj->first_affect, obj->last_affect, next, prev );
         if( ++success == obj->max_pool )
            return;
      }
   }
}

bool rule_check( OBJ_DATA *obj, POOL_DATA *pool )
{
   int x;

   for( x = 0; x < MAX_ITEM_WEAR; x++ )
      if( IS_SET( obj->wear_flags, 1 << x ) && pool->rules[x] == 1 )
         return TRUE;
   return FALSE;
}

AFFECT_DATA *create_affect_from_pool( POOL_DATA *pool )
{
   AFFECT_DATA *af;

   CREATE( af, AFFECT_DATA, 1 );
   af->from = NULL;
   af->from_pool = pool;
   af->affect_type = AFFECT_BUFF;
   af->type = -1;
   af->duration = 0;
   af->location = pool->location;
   af->modifier = number_range( pool->minstat, pool->maxstat );
   xCLEAR_BITS( af->bitvector );
   af->factor_id = -1;
   af->apply_type = -1;
   return af;
}

