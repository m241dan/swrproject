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
*  The MUDprograms are heavily based on the original MOBprogram code that  *
*  was written by N'Atas-ha.						   *
****************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include "mud.h"

ch_ret simple_damage( CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt );
CHAR_DATA *get_char_room_mp( CHAR_DATA * ch, const char *argument );
void transfer_char( CHAR_DATA *ch, CHAR_DATA *victim, ROOM_INDEX_DATA *location );

const char *mprog_type_to_name( int type )
{
   switch ( type )
   {
      case IN_FILE_PROG:
         return "in_file_prog";
      case ACT_PROG:
         return "act_prog";
      case SPEECH_PROG:
         return "speech_prog";
      case RAND_PROG:
         return "rand_prog";
      case FIGHT_PROG:
         return "fight_prog";
      case HITPRCNT_PROG:
         return "hitprcnt_prog";
      case DEATH_PROG:
         return "death_prog";
      case ENTRY_PROG:
         return "entry_prog";
      case GREET_PROG:
         return "greet_prog";
      case ALL_GREET_PROG:
         return "all_greet_prog";
      case GIVE_PROG:
         return "give_prog";
      case BRIBE_PROG:
         return "bribe_prog";
      case HOUR_PROG:
         return "hour_prog";
      case TIME_PROG:
         return "time_prog";
      case WEAR_PROG:
         return "wear_prog";
      case REMOVE_PROG:
         return "remove_prog";
      case SAC_PROG:
         return "sac_prog";
      case LOOK_PROG:
         return "look_prog";
      case EXA_PROG:
         return "exa_prog";
      case ZAP_PROG:
         return "zap_prog";
      case GET_PROG:
         return "get_prog";
      case DROP_PROG:
         return "drop_prog";
      case REPAIR_PROG:
         return "repair_prog";
      case DAMAGE_PROG:
         return "damage_prog";
      case PULL_PROG:
         return "pull_prog";
      case PUSH_PROG:
         return "push_prog";
      case SCRIPT_PROG:
         return "script_prog";
      case SLEEP_PROG:
         return "sleep_prog";
      case REST_PROG:
         return "rest_prog";
      case LEAVE_PROG:
         return "leave_prog";
      case USE_PROG:
         return "use_prog";
      case QUEST_PROG:
         return "quest_prog";
      case BUY_PROG:
         return "buy_prog";
      default:
         return "ERROR_PROG";
   }
}


/* A trivial rehack of do_mstat.  This doesnt show all the data, but just
 * enough to identify the mob and give its basic condition.  It does however,
 * show the MUDprograms which are set.
 */
void do_mpstat( CHAR_DATA * ch, const char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   MPROG_DATA *mprg;
   CHAR_DATA *victim;

   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      send_to_char( "MProg stat whom?\r\n", ch );
      return;
   }

   if( ( victim = get_char_world( ch, arg ) ) == NULL )
   {
      send_to_char( "They aren't here.\r\n", ch );
      return;
   }

   if( !IS_NPC( victim ) )
   {
      send_to_char( "Only Mobiles can have MobPrograms!\r\n", ch );
      return;
   }

   if( xIS_EMPTY( victim->pIndexData->progtypes ) )
   {
      send_to_char( "That Mobile has no Programs set.\r\n", ch );
      return;
   }

   ch_printf( ch, "Name: %s.  Vnum: %d.\r\n", victim->name, victim->pIndexData->vnum );

   ch_printf( ch, "Short description: %s.\r\nLong  description: %s",
              victim->short_descr, victim->long_descr[0] != '\0' ? victim->long_descr : "(none).\r\n" );

   ch_printf( ch, "Hp: %d/%d.  Mana: %d/%d.  Move: %d/%d. \r\n",
              victim->hit, victim->max_hit, victim->mana, victim->max_mana, victim->move, victim->max_move );

   ch_printf( ch,
              "Lv: %d.  Align: %d.  Evasion: %d.  Credits: %d.\r\n",
              victim->top_level, victim->alignment, GET_EVASION( victim ), victim->gold );

   for( mprg = victim->pIndexData->mudprogs; mprg; mprg = mprg->next )
      ch_printf( ch, "%s>%s %s\r\n%s\r\n", ( mprg->fileprog ? "(FILEPROG) " : "" ),
         mprog_type_to_name( mprg->type ), mprg->arglist, mprg->comlist );
   return;
}

/* Opstat - Scryn 8/12*/
void do_opstat( CHAR_DATA * ch, const char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   MPROG_DATA *mprg;
   OBJ_DATA *obj;

   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      send_to_char( "OProg stat what?\r\n", ch );
      return;
   }

   if( ( obj = get_obj_world( ch, arg ) ) == NULL )
   {
      send_to_char( "You cannot find that.\r\n", ch );
      return;
   }

   if( xIS_EMPTY( obj->pIndexData->progtypes ) )
   {
      send_to_char( "That object has no programs set.\r\n", ch );
      return;
   }

   ch_printf( ch, "Name: %s.  Vnum: %d.\r\n", obj->name, obj->pIndexData->vnum );

   ch_printf( ch, "Short description: %s.\r\n", obj->short_descr );

   for( mprg = obj->pIndexData->mudprogs; mprg; mprg = mprg->next )
      ch_printf( ch, ">%s %s\r\n%s\r\n", mprog_type_to_name( mprg->type ), mprg->arglist, mprg->comlist );

   return;

}

/* Rpstat - Scryn 8/12 */
void do_rpstat( CHAR_DATA * ch, const char *argument )
{
   MPROG_DATA *mprg;

   if( xIS_EMPTY( ch->in_room->progtypes ) )
   {
      send_to_char( "This room has no programs set.\r\n", ch );
      return;
   }

   ch_printf( ch, "Name: %s.  Vnum: %d.\r\n", ch->in_room->name, ch->in_room->vnum );

   for( mprg = ch->in_room->mudprogs; mprg; mprg = mprg->next )
      ch_printf( ch, ">%s %s\r\n%s\r\n", mprog_type_to_name( mprg->type ), mprg->arglist, mprg->comlist );
   return;
}

/* Prints the argument to all the rooms around the mobile */
void do_mpasound( CHAR_DATA * ch, const char *argument )
{
   ROOM_INDEX_DATA *was_in_room;
   EXIT_DATA *pexit;
   int actflags;

   if( !ch )
   {
      bug( "Nonexistent ch in do_mpasound!", 0 );
      return;
   }

   if( IS_AFFECTED( ch, AFF_CHARM ) )
      return;

   if( !IS_NPC( ch ) )
   {
      send_to_char( "Huh?\r\n", ch );
      return;
   }

   if( argument[0] == '\0' )
   {
      progbug( "Mpasound - No argument", ch );
      return;
   }

   actflags = ch->act;
   REMOVE_BIT( ch->act, ACT_SECRETIVE );
   was_in_room = ch->in_room;
   for( pexit = was_in_room->first_exit; pexit; pexit = pexit->next )
   {
      if( pexit->to_room && pexit->to_room != was_in_room )
      {
         ch->in_room = pexit->to_room;
         MOBtrigger = FALSE;
         act( AT_SAY, argument, ch, NULL, NULL, TO_ROOM );
      }
   }
   ch->act = actflags;
   ch->in_room = was_in_room;
   return;
}

/* lets the mobile kill any player or mobile without murder*/

void do_mpkill( CHAR_DATA * ch, const char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;

   if( !ch )
   {
      bug( "Nonexistent ch in do_mpkill!", 0 );
      return;
   }

   if( IS_AFFECTED( ch, AFF_CHARM ) )
      return;

   if( !IS_NPC( ch ) )
   {
      send_to_char( "Huh?\r\n", ch );
      return;
   }

   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      progbug( "MpKill - no argument", ch );
      return;
   }

   if( ( victim = get_char_room_mp( ch, arg ) ) == NULL )
   {
      progbug( "MpKill - Victim not in room", ch );
      return;
   }

   if( victim == ch )
   {
      progbug( "MpKill - Bad victim to attack", ch );
      return;
   }

   if( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == victim )
   {
      progbug( "MpKill - Charmed mob attacking master", ch );
      return;
   }

   if( ch->position == POS_FIGHTING )
   {
      progbug( "MpKill - Already fighting", ch );
      return;
   }

   multi_hit( ch, victim, TYPE_UNDEFINED );
   return;
}


/* lets the mobile destroy an object in its inventory
   it can also destroy a worn object and it can destroy
   items using all.xxxxx or just plain all of them */

void do_mpjunk( CHAR_DATA * ch, const char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   OBJ_DATA *obj_next;

   if( IS_AFFECTED( ch, AFF_CHARM ) )
      return;

   if( !IS_NPC( ch ) )
   {
      send_to_char( "Huh?\r\n", ch );
      return;
   }

   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      progbug( "Mpjunk - No argument", ch );
      return;
   }

   if( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
   {
      if( ( obj = get_obj_wear( ch, arg ) ) != NULL )
      {
         unequip_char( ch, obj );
         extract_obj( obj );
         return;
      }
      if( ( obj = get_obj_carry( ch, arg ) ) == NULL )
         return;
      extract_obj( obj );
   }
   else
      for( obj = ch->first_carrying; obj; obj = obj_next )
      {
         obj_next = obj->next_content;
         if( arg[3] == '\0' || is_name( &arg[4], obj->name ) )
         {
            if( obj->wear_loc != WEAR_NONE )
               unequip_char( ch, obj );
            extract_obj( obj );
         }
      }

   return;

}

/*
 * This function examines a text string to see if the first "word" is a
 * color indicator (e.g. _red, _whi_, _blu).  -  Gorog
 */
int get_color( const char *argument )  /* get color code from command string */
{
   char color[MAX_INPUT_LENGTH];
   const char *cptr;
   static const char *color_list = "_bla_red_dgr_bro_dbl_pur_cya_cha_dch_ora_gre_yel_blu_pin_lbl_whi";
   static const char *blink_list = "*bla*red*dgr*bro*dbl*pur*cya*cha*dch*ora*gre*yel*blu*pin*lbl*whi";

   one_argument( argument, color );
   if( color[0] != '_' && color[0] != '*' )
      return 0;
   if( ( cptr = strstr( color_list, color ) ) )
      return ( cptr - color_list ) / 4;
   if( ( cptr = strstr( blink_list, color ) ) )
      return ( cptr - blink_list ) / 4 + AT_BLINK;
   return 0;
}


/* prints the message to everyone in the room other than the mob and victim */

void do_mpechoaround( CHAR_DATA * ch, const char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   int actflags;
   short color;

   if( IS_AFFECTED( ch, AFF_CHARM ) )
      return;

   if( !IS_NPC( ch ) )
   {
      send_to_char( "Huh?\r\n", ch );
      return;
   }

   argument = one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      progbug( "Mpechoaround - No argument", ch );
      return;
   }

   if( !( victim = get_char_room_mp( ch, arg ) ) )
   {
      progbug( "Mpechoaround - victim does not exist", ch );
      return;
   }

   actflags = ch->act;
   REMOVE_BIT( ch->act, ACT_SECRETIVE );
   
   /*
    * DONT_UPPER prevents argument[0] from being captilized. --Shaddai
    */
   DONT_UPPER = TRUE;

   if( ( color = get_color( argument ) ) )
   {
      argument = one_argument( argument, arg );
      act( color, argument, ch, NULL, victim, TO_NOTVICT );
   }
   else
      act( AT_ACTION, argument, ch, NULL, victim, TO_NOTVICT );
   
   DONT_UPPER = FALSE;

   ch->act = actflags;
}


/* prints message only to victim */

void do_mpechoat( CHAR_DATA * ch, const char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   int actflags;
   short color;

   if( IS_AFFECTED( ch, AFF_CHARM ) )
      return;

   if( !IS_NPC( ch ) )
   {
      send_to_char( "Huh?\r\n", ch );
      return;
   }

   argument = one_argument( argument, arg );

   if( arg[0] == '\0' || argument[0] == '\0' )
   {
      progbug( "Mpechoat - No argument", ch );
      return;
   }

   if( !( victim = get_char_room_mp( ch, arg ) ) )
   {
      progbug( "Mpechoat - victim does not exist", ch );
      return;
   }

   actflags = ch->act;
   REMOVE_BIT( ch->act, ACT_SECRETIVE );
   
   /*
    * DONT_UPPER prevents argument[0] from being captilized. --Shaddai
    */
   DONT_UPPER = TRUE;

   if( ( color = get_color( argument ) ) )
   {
      argument = one_argument( argument, arg );
      act( color, argument, ch, NULL, victim, TO_VICT );
   }
   else
      act( AT_ACTION, argument, ch, NULL, victim, TO_VICT );

   DONT_UPPER = FALSE;

   ch->act = actflags;
}


/* prints message to room at large. */

void do_mpecho( CHAR_DATA * ch, const char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   short color;
   int actflags;

   if( IS_AFFECTED( ch, AFF_CHARM ) )
      return;

   if( !IS_NPC( ch ) )
   {
      send_to_char( "Huh?\r\n", ch );
      return;
   }

   if( argument[0] == '\0' )
   {
      progbug( "Mpecho - called w/o argument", ch );
      return;
   }

   actflags = ch->act;
   REMOVE_BIT( ch->act, ACT_SECRETIVE );
   
   /*
    * DONT_UPPER prevents argument[0] from being captilized. --Shaddai
    */
   DONT_UPPER = TRUE;

   if( ( color = get_color( argument ) ) )
   {
      argument = one_argument( argument, arg1 );
      act( color, argument, ch, NULL, NULL, TO_ROOM );
   }
   else
      act( AT_ACTION, argument, ch, NULL, NULL, TO_ROOM );

   DONT_UPPER = FALSE;

   ch->act = actflags;
}


/* lets the mobile load an item or mobile.  All items
are loaded into inventory.  you can specify a level with
the load object portion as well. */

void do_mpmload( CHAR_DATA * ch, const char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   MOB_INDEX_DATA *pMobIndex;
   CHAR_DATA *victim;

   if( IS_AFFECTED( ch, AFF_CHARM ) )
      return;

   if( !IS_NPC( ch ) )
   {
      send_to_char( "Huh?\r\n", ch );
      return;
   }

   one_argument( argument, arg );

   if( arg[0] == '\0' || !is_number( arg ) )
   {
      progbug( "Mpmload - Bad vnum as arg", ch );
      return;
   }

   if( ( pMobIndex = get_mob_index( atoi( arg ) ) ) == NULL )
   {
      progbug( "Mpmload - Bad mob vnum", ch );
      return;
   }

   victim = create_mobile( pMobIndex );
   char_to_room( victim, ch->in_room );
   return;
}

void do_mpoload( CHAR_DATA * ch, const char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   OBJ_INDEX_DATA *pObjIndex;
   OBJ_DATA *obj;
   int level;
   int timer = 0;

   if( IS_AFFECTED( ch, AFF_CHARM ) )
      return;

   if( !IS_NPC( ch ) )
   {
      send_to_char( "Huh?\r\n", ch );
      return;
   }

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if( arg1[0] == '\0' || !is_number( arg1 ) )
   {
      progbug( "Mpoload - Bad syntax", ch );
      return;
   }

   if( arg2[0] == '\0' )
      level = get_trust( ch );
   else
   {
      /*
       * New feature from Alander.
       */
      if( !is_number( arg2 ) )
      {
         progbug( "Mpoload - Bad level syntax", ch );
         return;
      }
      level = atoi( arg2 );
      if( level < 0 || level > get_trust( ch ) )
      {
         progbug( "Mpoload - Bad level", ch );
         return;
      }

      /*
       * New feature from Thoric.
       */
      timer = atoi( argument );
      if( timer < 0 )
      {
         progbug( "Mpoload - Bad timer", ch );
         return;
      }
   }

   if( ( pObjIndex = get_obj_index( atoi( arg1 ) ) ) == NULL )
   {
      progbug( "Mpoload - Bad vnum arg", ch );
      return;
   }

   obj = create_object( pObjIndex, level );
   obj->timer = timer;
   if( CAN_WEAR( obj, ITEM_TAKE ) )
      obj_to_char( obj, ch );
   else
      obj_to_room( obj, ch->in_room );

   return;
}

/* lets the mobile purge all objects and other npcs in the room,
   or purge a specified object or mob in the room.  It can purge
   itself, but this had best be the last command in the MUDprogram
   otherwise ugly stuff will happen */

void do_mppurge( CHAR_DATA * ch, const char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   OBJ_DATA *obj;

   if( IS_AFFECTED( ch, AFF_CHARM ) )
      return;

   if( !IS_NPC( ch ) )
   {
      send_to_char( "Huh?\r\n", ch );
      return;
   }

   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      /*
       * 'purge' 
       */
      CHAR_DATA *vnext;

      for( victim = ch->in_room->first_person; victim; victim = vnext )
      {
         vnext = victim->next_in_room;
         if( IS_NPC( victim ) && victim != ch )
            extract_char( victim, TRUE );
      }
      while( ch->in_room->first_content )
         extract_obj( ch->in_room->first_content );

      return;
   }

   if( !( victim = get_char_room( ch, arg ) ) )
   {
      if ( (obj = get_obj_here( ch, arg )) != NULL ) 
      {
	    separate_obj( obj );
	    extract_obj( obj );
      } 
      else
	    progbug( "Mppurge - Bad argument", ch );
      return;
   }

   if( !IS_NPC( victim ) )
   {
      progbug( "Mppurge - Trying to purge a PC", ch );
      return;
   }

   if( victim == ch )
   {
      progbug( "Mppurge - Trying to purge oneself", ch );
      return;
   }

   if( IS_NPC( victim ) && victim->pIndexData->vnum == MOB_VNUM_SUPERMOB )
   {
      progbug( "Mppurge: trying to purge supermob", ch );
      return;
   }

   extract_char( victim, TRUE );
   return;
}

/* Allow mobiles to go wizinvis with programs -- SB */

void do_mpinvis( CHAR_DATA * ch, const char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   short level;

   if( !IS_NPC( ch ) )
   {
      send_to_char( "Huh?\r\n", ch );
      return;
   }

   argument = one_argument( argument, arg );
   if( arg[0] != '\0' )
   {
      if( !is_number( arg ) )
      {
         progbug( "Mpinvis - Non numeric argument ", ch );
         return;
      }
      level = atoi( arg );
      if( level < 2 || level > 51 )
      {
         progbug( "MPinvis - Invalid level ", ch );
         return;
      }

      ch->mobinvis = level;
      ch_printf( ch, "Mobinvis level set to %d.\r\n", level );
      return;
   }

   if( ch->mobinvis < 2 )
      ch->mobinvis = ch->top_level;

   if( IS_SET( ch->act, ACT_MOBINVIS ) )
   {
      REMOVE_BIT( ch->act, ACT_MOBINVIS );
      act( AT_IMMORT, "$n slowly fades into existence.", ch, NULL, NULL, TO_ROOM );
      send_to_char( "You slowly fade back into existence.\r\n", ch );
   }
   else
   {
      SET_BIT( ch->act, ACT_MOBINVIS );
      act( AT_IMMORT, "$n slowly fades into thin air.", ch, NULL, NULL, TO_ROOM );
      send_to_char( "You slowly vanish into thin air.\r\n", ch );
   }
   return;
}

/* lets the mobile goto any location it wishes that is not private */

void do_mpgoto( CHAR_DATA * ch, const char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   ROOM_INDEX_DATA *location;

   if( IS_AFFECTED( ch, AFF_CHARM ) )
      return;

   if( !IS_NPC( ch ) )
   {
      send_to_char( "Huh?\r\n", ch );
      return;
   }

   one_argument( argument, arg );
   if( arg[0] == '\0' )
   {
      progbug( "Mpgoto - No argument", ch );
      return;
   }

   if( ( location = find_location( ch, arg ) ) == NULL )
   {
      progbug( "Mpgoto - No such location", ch );
      return;
   }

   if( ch->fighting )
      stop_fighting( ch, TRUE );

   char_from_room( ch );
   char_to_room( ch, location );

   return;
}

/* lets the mobile do a command at another location. Very useful */

void do_mpat( CHAR_DATA * ch, const char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   ROOM_INDEX_DATA *location;
   ROOM_INDEX_DATA *original;
   CHAR_DATA *wch;

   if( IS_AFFECTED( ch, AFF_CHARM ) )
      return;

   if( !IS_NPC( ch ) )
   {
      send_to_char( "Huh?\r\n", ch );
      return;
   }

   argument = one_argument( argument, arg );

   if( arg[0] == '\0' || argument[0] == '\0' )
   {
      progbug( "Mpat - Bad argument", ch );
      return;
   }

   if( ( location = find_location( ch, arg ) ) == NULL )
   {
      progbug( "Mpat - No such location", ch );
      return;
   }

   original = ch->in_room;
   char_from_room( ch );
   char_to_room( ch, location );
   interpret( ch, argument );

   /*
    * See if 'ch' still exists before continuing!
    * Handles 'at XXXX quit' case.
    */
   for( wch = first_char; wch; wch = wch->next )
      if( wch == ch )
      {
         char_from_room( ch );
         char_to_room( ch, original );
         break;
      }

   return;
}

/* allow a mobile to advance a player's level... very dangerous */
void do_mpadvance( CHAR_DATA * ch, const char *argument )
{
   return;
}

/* lets the mobile transfer people.  the all argument transfers
   everyone in the current room to the specified location 
   the area argument transfers everyone in the current area to the
   specified location */
void do_mptransfer( CHAR_DATA * ch, const char *argument )
{
   char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
   ROOM_INDEX_DATA *location;
   CHAR_DATA *victim, *nextinroom;
   DESCRIPTOR_DATA *d;

   if( !IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) )
   {
      send_to_char( "Huh?\r\n", ch );
      return;
   }

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if( arg1[0] == '\0' )
   {
      progbug( "Mptransfer - Bad syntax", ch );
      return;
   }

   if( arg2[0] != '\0' )
   {
      if( !( location = find_location( ch, arg2 ) ) )
      {
         progbug( "Mptransfer - No such location", ch );
         return;
      }
   }
   else
      location = ch->in_room;

   /*
    * Put in the variable nextinroom to make this work right. -Narn 
    */
   if( !str_cmp( arg1, "all" ) )
   {
      for( victim = ch->in_room->first_person; victim; victim = nextinroom )
      {
         nextinroom = victim->next_in_room;

         if( ch == victim )
            continue;

         transfer_char( ch, victim, location );
      }
      return;
   }

   /*
    * This will only transfer PC's in the area not Mobs --Shaddai 
    */
   if( !str_cmp( arg1, "area" ) )
   {
      for( d = first_descriptor; d; d = d->next )
      {
         if( !d->character || ( d->connected != CON_PLAYING &&  d->connected != CON_EDITING )
             || ch->in_room->area != d->character->in_room->area )
            continue;

         if( ch == d->character )
            continue;

         transfer_char( ch, d->character, location );
      }
      return;
   }

   if( !( victim = get_char_world( ch, arg1 ) ) )
   {
      progbug( "Mptransfer - No such person", ch );
      return;
   }
   transfer_char( ch, victim, location );
   return;
}

/* lets the mobile force someone to do something.  must be mortal level
   and the all argument only affects those in the room with the mobile */

void do_mpforce( CHAR_DATA * ch, const char *argument )
{
   char arg[MAX_INPUT_LENGTH];

   if( IS_AFFECTED( ch, AFF_CHARM ) )
      return;

   if( !IS_NPC( ch ) || ch->desc )
   {
      send_to_char( "Huh?\r\n", ch );
      return;
   }

   argument = one_argument( argument, arg );

   if( arg[0] == '\0' || argument[0] == '\0' )
   {
      progbug( "Mpforce - Bad syntax", ch );
      return;
   }

   if( !str_cmp( arg, "all" ) )
   {
      CHAR_DATA *vch;

      for( vch = ch->in_room->first_person; vch; vch = vch->next_in_room )
         if( get_trust( vch ) < get_trust( ch ) && can_see( ch, vch ) )
            interpret( vch, argument );
   }
   else
   {
      CHAR_DATA *victim;

      if( ( victim = get_char_room_mp( ch, arg ) ) == NULL )
      {
         progbug( "Mpforce - No such victim", ch );
         return;
      }

      if( victim == ch )
      {
         progbug( "Mpforce - Forcing oneself", ch );
         return;
      }

      if( !IS_NPC( victim ) && ( !victim->desc ) && IS_IMMORTAL( victim ) )
      {
         progbug( "Mpforce - Attempting to force link dead immortal", ch );
         return;
      }


      interpret( victim, argument );
   }

   return;
}



/*
 *  Haus' toys follow:
 */

/*
 * syntax:  mppractice victim spell_name max%
 *
 */
void do_mp_practice( CHAR_DATA * ch, const char *argument )
{
}

/*
 * syntax: mpslay (character)
 */
void do_mp_slay( CHAR_DATA * ch, const char *argument )
{
   return;
}

/*
 * syntax: mpdamage (character) (#hps)
 */
void do_mp_damage( CHAR_DATA * ch, const char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   int dam;

   if( IS_AFFECTED( ch, AFF_CHARM ) )
      return;

   if( !IS_NPC( ch ) || ( ch->desc && get_trust( ch ) < LEVEL_IMMORTAL ) )
   {
      send_to_char( "Huh?\r\n", ch );
      return;
   }
   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if( arg1[0] == '\0' )
   {
      send_to_char( "mpdamage whom?\r\n", ch );
      progbug( "Mpdamage: invalid argument1", ch );
      return;
   }

   if( arg2[0] == '\0' )
   {
      send_to_char( "mpdamage inflict how many hps?\r\n", ch );
      progbug( "Mpdamage: invalid argument2", ch );
      return;
   }

   if( ( victim = get_char_room_mp( ch, arg1 ) ) == NULL )
   {
      send_to_char( "Victim must be in room.\r\n", ch );
      progbug( "Mpdamage: victim not in room", ch );
      return;
   }

   if( victim == ch )
   {
      send_to_char( "You can't mpdamage yourself.\r\n", ch );
      progbug( "Mpdamage: trying to damage self", ch );
      return;
   }

   dam = atoi( arg2 );

   if( ( dam < 0 ) || ( dam > 32000 ) )
   {
      send_to_char( "Mpdamage how much?\r\n", ch );
      progbug( "Mpdamage: invalid (nonexistent?) argument", ch );
      return;
   }

   /*
    * this is kinda begging for trouble        
    */
   /*
    * Note from Thoric to whoever put this in...
    * Wouldn't it be better to call damage(ch, ch, dam, dt)?
    * I hate redundant code
    */
   if( simple_damage( ch, victim, dam, TYPE_UNDEFINED ) == rVICT_DIED )
   {
      stop_fighting( ch, FALSE );
      stop_hating( ch );
      stop_fearing( ch );
      stop_hunting( ch );
   }

   return;
}


/*
 * syntax: mprestore (character) (#hps)                Gorog
 */
void do_mp_restore( CHAR_DATA * ch, const char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   int hp;

   if( IS_AFFECTED( ch, AFF_CHARM ) )
      return;

   if( !IS_NPC( ch ) || ( ch->desc && get_trust( ch ) < LEVEL_IMMORTAL ) )
   {
      send_to_char( "Huh?\r\n", ch );
      return;
   }
   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );

   if( arg1[0] == '\0' )
   {
      send_to_char( "mprestore whom?\r\n", ch );
      progbug( "Mprestore: invalid argument1", ch );
      return;
   }

   if( arg2[0] == '\0' )
   {
      send_to_char( "mprestore how many hps?\r\n", ch );
      progbug( "Mprestore: invalid argument2", ch );
      return;
   }

   if( ( victim = get_char_room_mp( ch, arg1 ) ) == NULL )
   {
      send_to_char( "Victim must be in room.\r\n", ch );
      progbug( "Mprestore: victim not in room", ch );
      return;
   }

   hp = atoi( arg2 );

   if( ( hp < 0 ) || ( hp > 32000 ) )
   {
      send_to_char( "Mprestore how much?\r\n", ch );
      progbug( "Mprestore: invalid (nonexistent?) argument", ch );
      return;
   }
   hp += victim->hit;
   victim->hit = ( hp > 32000 || hp < 0 || hp > victim->max_hit ) ? victim->max_hit : hp;
}

void do_mpgain( CHAR_DATA * ch, const char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char arg3[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   long gexp;
   int ability;

   if( IS_AFFECTED( ch, AFF_CHARM ) )
      return;

   if( !IS_NPC( ch ) || ( ch->desc && get_trust( ch ) < LEVEL_IMMORTAL ) )
   {
      send_to_char( "Huh?\r\n", ch );
      return;
   }
   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   argument = one_argument( argument, arg3 );

   if( arg1[0] == '\0' )
   {
      send_to_char( "mpgain whom?\r\n", ch );
      progbug( "Mpgain: invalid argument1", ch );
      return;
   }

   if( arg2[0] == '\0' )
   {
      send_to_char( "mpgain in what ability?\r\n", ch );
      progbug( "Mpgain: invalid argument2", ch );
      return;
   }

   if( arg3[0] == '\0' )
   {
      send_to_char( "mpgain how much exp?\r\n", ch );
      progbug( "Mpgain: invalid argument3", ch );
      return;
   }

   if( ( victim = get_char_room_mp( ch, arg1 ) ) == NULL )
   {
      send_to_char( "Victim must be in room.\r\n", ch );
      progbug( "Mpgain: victim not in room", ch );
      return;
   }

   ability = atoi( arg2 );
   gexp = atol( arg3 );

   if( ability < 0 || ability >= MAX_ABILITY )
   {
      send_to_char( "Mpgain which ability?\r\n", ch );
      progbug( "Mpgain: ability out of range", ch );
      return;
   }

   if( ( gexp < 1 ) )
   {
      send_to_char( "Mpgain how much?\r\n", ch );
      progbug( "Mpgain: experience out of range", ch );
      return;
   }

   gexp = URANGE( 1, gexp, ( exp_level( victim->skill_level[ability] + 1 ) - exp_level( victim->skill_level[ability] ) ) );

   ch_printf( victim, "You gain %ld %s experience.\r\n", gexp, ability_name[ability] );
   gain_exp( victim, gexp, ability );
   return;
}

/*
 * Syntax mp_open_passage x y z
 *
 * opens a 1-way passage from room x to room y in direction z
 *
 *  won't mess with existing exits
 */
void do_mp_open_passage( CHAR_DATA * ch, const char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char arg3[MAX_INPUT_LENGTH];
   ROOM_INDEX_DATA *targetRoom, *fromRoom;
   int targetRoomVnum, fromRoomVnum, exit_num;
   EXIT_DATA *pexit;

   if( IS_AFFECTED( ch, AFF_CHARM ) )
      return;

   if( !IS_NPC( ch ) || ( ch->desc && get_trust( ch ) < LEVEL_IMMORTAL ) )
   {
      send_to_char( "Huh?\r\n", ch );
      return;
   }

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   argument = one_argument( argument, arg3 );

   if( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
   {
      progbug( "MpOpenPassage - Bad syntax", ch );
      return;
   }

   if( !is_number( arg1 ) )
   {
      progbug( "MpOpenPassage - Bad syntax", ch );
      return;
   }

   fromRoomVnum = atoi( arg1 );
   if( ( fromRoom = get_room_index( fromRoomVnum ) ) == NULL )
   {
      progbug( "MpOpenPassage - Bad syntax", ch );
      return;
   }

   if( !is_number( arg2 ) )
   {
      progbug( "MpOpenPassage - Bad syntax", ch );
      return;
   }

   targetRoomVnum = atoi( arg2 );
   if( ( targetRoom = get_room_index( targetRoomVnum ) ) == NULL )
   {
      progbug( "MpOpenPassage - Bad syntax", ch );
      return;
   }

   if( !is_number( arg3 ) )
   {
      progbug( "MpOpenPassage - Bad syntax", ch );
      return;
   }

   exit_num = atoi( arg3 );
   if( ( exit_num < 0 ) || ( exit_num > MAX_DIR ) )
   {
      progbug( "MpOpenPassage - Bad syntax", ch );
      return;
   }

   if( ( pexit = get_exit( fromRoom, exit_num ) ) != NULL )
   {
      if( !IS_SET( pexit->exit_info, EX_PASSAGE ) )
         return;
      progbug( "MpOpenPassage - Exit exists", ch );
      return;
   }

   pexit = make_exit( fromRoom, targetRoom, exit_num );
   pexit->keyword = STRALLOC( "" );
   pexit->description = STRALLOC( "" );
   pexit->key = -1;
   pexit->exit_info = EX_PASSAGE;

   /*
    * act( AT_PLAIN, "A passage opens!", ch, NULL, NULL, TO_CHAR ); 
    */
   /*
    * act( AT_PLAIN, "A passage opens!", ch, NULL, NULL, TO_ROOM ); 
    */

   return;
}

/*
 * Syntax mp_close_passage x y 
 *
 * closes a passage in room x leading in direction y
 *
 * the exit must have EX_PASSAGE set
 */
void do_mp_close_passage( CHAR_DATA * ch, const char *argument )
{
   char arg1[MAX_INPUT_LENGTH];
   char arg2[MAX_INPUT_LENGTH];
   char arg3[MAX_INPUT_LENGTH];
   ROOM_INDEX_DATA *fromRoom;
   int fromRoomVnum, exit_num;
   EXIT_DATA *pexit;

   if( IS_AFFECTED( ch, AFF_CHARM ) )
      return;

   if( !IS_NPC( ch ) || ( ch->desc && get_trust( ch ) < LEVEL_IMMORTAL ) )
   {
      send_to_char( "Huh?\r\n", ch );
      return;
   }

   argument = one_argument( argument, arg1 );
   argument = one_argument( argument, arg2 );
   argument = one_argument( argument, arg3 );

   if( arg1[0] == '\0' || arg2[0] == '\0' || arg2[0] == '\0' )
   {
      progbug( "MpClosePassage - Bad syntax", ch );
      return;
   }

   if( !is_number( arg1 ) )
   {
      progbug( "MpClosePassage - Bad syntax", ch );
      return;
   }

   fromRoomVnum = atoi( arg1 );
   if( ( fromRoom = get_room_index( fromRoomVnum ) ) == NULL )
   {
      progbug( "MpClosePassage - Bad syntax", ch );
      return;
   }

   if( !is_number( arg2 ) )
   {
      progbug( "MpClosePassage - Bad syntax", ch );
      return;
   }

   exit_num = atoi( arg2 );
   if( ( exit_num < 0 ) || ( exit_num > MAX_DIR ) )
   {
      progbug( "MpClosePassage - Bad syntax", ch );
      return;
   }

   if( ( pexit = get_exit( fromRoom, exit_num ) ) == NULL )
   {
      return;  /* already closed, ignore...  so rand_progs */
      /*
       * can close without spam 
       */
   }

   if( !IS_SET( pexit->exit_info, EX_PASSAGE ) )
   {
      progbug( "MpClosePassage - Exit not a passage", ch );
      return;
   }

   extract_exit( fromRoom, pexit );

   /*
    * act( AT_PLAIN, "A passage closes!", ch, NULL, NULL, TO_CHAR ); 
    */
   /*
    * act( AT_PLAIN, "A passage closes!", ch, NULL, NULL, TO_ROOM ); 
    */

   return;
}



/*
 * Does nothing.  Used for scripts.
 */
void do_mpnothing( CHAR_DATA * ch, const char *argument )
{
   if( IS_AFFECTED( ch, AFF_CHARM ) )
      return;

   if( !IS_NPC( ch ) || ( ch->desc && get_trust( ch ) < LEVEL_IMMORTAL ) )
   {
      send_to_char( "Huh?\r\n", ch );
      return;
   }
   return;
}


/*
 *   Sends a message to sleeping character.  Should be fun
 *    with room sleep_progs
 *
 */
void do_mpdream( CHAR_DATA * ch, const char *argument )
{
   char arg1[MAX_STRING_LENGTH];
   CHAR_DATA *vict;

   if( IS_AFFECTED( ch, AFF_CHARM ) )
      return;

   if( !IS_NPC( ch ) || ( ch->desc && get_trust( ch ) < LEVEL_IMMORTAL ) )
   {
      send_to_char( "Huh?\r\n", ch );
      return;
   }

   argument = one_argument( argument, arg1 );

   if( ( vict = get_char_world( ch, arg1 ) ) == NULL )
   {
      progbug( "Mpdream: No such character", ch );
      return;
   }

   if( vict->position <= POS_SLEEPING )
   {
      send_to_char( argument, vict );
      send_to_char( "\r\n", vict );
   }
   return;
}

void do_mpapply( CHAR_DATA * ch, const char *argument )
{
   CHAR_DATA *victim;

   if( !IS_NPC( ch ) )
   {
      send_to_char( "Huh?\r\n", ch );
      return;
   }

   if( argument[0] == '\0' )
   {
      progbug( "Mpapply - bad syntax", ch );
      return;
   }

   if( ( victim = get_char_room_mp( ch, argument ) ) == NULL )
   {
      progbug( "Mpapply - no such player in room.", ch );
      return;
   }

   if( !victim->desc )
   {
      send_to_char( "Not on linkdeads.\r\n", ch );
      return;
   }

   if( !NOT_AUTHED( victim ) )
      return;

   if( victim->pcdata->auth_state >= 1 )
      return;

   sprintf( log_buf, "%s@%s new %s applying for authorization...",
            victim->name, victim->desc->host, race_table[victim->race].race_name );
   log_string( log_buf );
   to_channel( log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL );
   victim->pcdata->auth_state = 1;
   return;
}

void do_mpauth( CHAR_DATA *ch, const char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;

   argument = one_argument( argument, arg );

   if( !IS_NPC( ch ) )
      return;

   if( ( victim = get_char_room_mp( ch, arg ) ) == NULL )
   {
      progbug( "mpauth: trying to auth a person that's not here.", ch );
      return;
   }

   if( IS_NPC( victim ) )
      return;

   if( !NOT_AUTHED( victim ) )
      return;

   if( !IS_SET( victim->pcdata->flags, PCFLAG_UNAUTHED ) )
      return;

   victim->pcdata->auth_state = 3;
   REMOVE_BIT( victim->pcdata->flags, PCFLAG_UNAUTHED );
   if( victim->pcdata->authed_by )
      STRFREE( victim->pcdata->authed_by );
   victim->pcdata->authed_by = STRALLOC( "I agree room" );
   return;
}


void do_mpapplyb( CHAR_DATA * ch, const char *argument )
{
   CHAR_DATA *victim;

   if( !IS_NPC( ch ) )
   {
      send_to_char( "Huh?\r\n", ch );
      return;
   }

   if( argument[0] == '\0' )
   {
      progbug( "Mpapplyb - bad syntax", ch );
      return;
   }

   if( ( victim = get_char_room_mp( ch, argument ) ) == NULL )
   {
      progbug( "Mpapplyb - no such player in room.", ch );
      return;
   }

   if( !victim->desc )
   {
      send_to_char( "Not on linkdeads.\r\n", ch );
      return;
   }

   if( !NOT_AUTHED( victim ) )
      return;

   if( get_timer( victim, TIMER_APPLIED ) >= 1 )
      return;

   switch ( victim->pcdata->auth_state )
   {
      case 0:
      case 1:
      default:
         send_to_char( "You attempt to regain the gods' attention.\r\n", victim );
         sprintf( log_buf, "%s@%s new %s applying for authorization...",
                  victim->name, victim->desc->host, race_table[victim->race].race_name );
         log_string( log_buf );
         to_channel( log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL );
         add_timer( victim, TIMER_APPLIED, 10, NULL, 0 );
         victim->pcdata->auth_state = 1;
         break;

      case 2:
         send_to_char
            ( "Your name has been deemed unsuitable by the gods.  Please choose a more apropriate name with the 'name' command.\r\n",
              victim );
         add_timer( victim, TIMER_APPLIED, 10, NULL, 0 );
         break;

      case 3:
         send_to_char( "The gods permit you to enter the Star Wars Reality.\r\n", victim );
         REMOVE_BIT( victim->pcdata->flags, PCFLAG_UNAUTHED );
         if( victim->fighting )
            stop_fighting( victim, TRUE );
         char_from_room( victim );
         char_to_room( victim, get_room_index( ROOM_VNUM_SCHOOL ) );
         act( AT_WHITE, "$n enters this world from within a column of blinding light!", victim, NULL, NULL, TO_ROOM );
         do_look( victim, "auto" );
         break;
   }

   return;
}

/*
 * Deposit some gold into the current area's economy		-Thoric
 */
void do_mp_deposit( CHAR_DATA * ch, const char *argument )
{
   char arg[MAX_STRING_LENGTH];
   int gold;

   if( !IS_NPC( ch ) )
   {
      send_to_char( "Huh?\r\n", ch );
      return;
   }

   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      progbug( "Mpdeposit - bad syntax", ch );
      return;
   }
   gold = atoi( arg );
   if( gold <= ch->gold && ch->in_room )
   {
      ch->gold -= gold;
      boost_economy( ch->in_room->area, gold );
   }
}


/*
 * Withdraw some gold from the current area's economy		-Thoric
 */
void do_mp_withdraw( CHAR_DATA * ch, const char *argument )
{
   char arg[MAX_STRING_LENGTH];
   int gold;

   if( !IS_NPC( ch ) )
   {
      send_to_char( "Huh?\r\n", ch );
      return;
   }

   one_argument( argument, arg );

   if( arg[0] == '\0' )
   {
      progbug( "Mpwithdraw - bad syntax", ch );
      return;
   }
   gold = atoi( arg );
   if( ch->gold < 1000000000 && gold < 1000000000 && ch->in_room && economy_has( ch->in_room->area, gold ) )
   {
      ch->gold += gold;
      lower_economy( ch->in_room->area, gold );
   }
}

void do_mp_advancequest( CHAR_DATA * ch, const char *argument )
{
   CHAR_DATA *victim;
   QUEST_DATA *quest;
   PLAYER_QUEST *pquest;
   char arg[MAX_STRING_LENGTH];

   if( !IS_NPC( ch ) )
   {
      send_to_char( "Huh?\r\n", ch );
      return;
   }

   argument = one_argument( argument, arg );

   if( arg[0] == '\0' || argument[0] == '\0' )
   {
      progbug( "mpadvancequest: bad syntax", ch );
      return;
   }

   if( ( victim = get_char_room_mp( ch, arg ) ) == NULL )
      return;

   if( IS_NPC( victim ) )
      return;

   if( ( quest = get_quest_from_id( atoi( argument ) ) ) == NULL && ( quest = get_quest_from_name( argument ) ) == NULL )
   {
      progbug( "mpadvancequest: bad quest id/name given.", ch );
      return;
   }

   if( ( pquest = get_player_quest( victim, quest ) ) == NULL )
   {
      progbug( "mpadvancequest: mpadvancequest on a quest player doesn't have.", ch );
      return;
   }

   if( pquest->stage < 1 )
   {
      progbug( "mpadvancequest: mpadvancequest trying to advance a quest a player doesn't have started.", ch );
      return;
   }

   pquest->stage++;
   save_char_obj( victim );
   saving_char = NULL;
   return;
}

void do_mp_completequest( CHAR_DATA *ch, const char *argument )
{
   CHAR_DATA *victim;
   QUEST_DATA *quest;
   PLAYER_QUEST *pquest;
   char arg[MAX_STRING_LENGTH];

   if( !IS_NPC( ch ) )
   {
      send_to_char( "Huh?\r\n", ch );
      return;
   }

   argument = one_argument( argument, arg );

   if( arg[0] == '\0' || argument[0] == '\0' )
   {
      progbug( "mpcompletequest: bad syntax", ch );
      return;
   }

   if( ( victim = get_char_room_mp( ch, arg ) ) == NULL )
      return;

   if( IS_NPC( victim ) )
      return;

   if( ( quest = get_quest_from_id( atoi( argument ) ) ) == NULL && ( quest = get_quest_from_name( argument ) ) == NULL )
   {
      progbug( "mpcompletequest: bad quest id/name given.", ch );
      return;
   }

   if( ( pquest = get_player_quest( victim, quest ) ) == NULL )
   {
      progbug( "mpcompletequest: mpcompletequest on a quest player doesn't have.", ch );
      return;
   }

   if( pquest->stage < 1 )
   {
      progbug( "mpcompletequest: mpcompletequest trying to advance a quest a player doesn't have started.", ch );
      return;
   }

   switch( quest->type )
   {
      case QUEST_TYPE_ONETIME:
         pquest->stage = QUEST_COMPLETE;
         break;
      case QUEST_TYPE_REPEATABLE:
         pquest->stage = QUEST_COMPLETE_REPEATABLE;
         break;
   }
   pquest->times_completed++;
   ch_printf( victim, "You completed %s.\r\n", pquest->quest->name );

   save_char_obj( victim );
   saving_char = NULL;
   return;
}

void do_mp_progress( CHAR_DATA *ch, const char *argument )
{
   CHAR_DATA *victim;
   QUEST_DATA *quest;
   PLAYER_QUEST *pquest;
   char arg[MAX_STRING_LENGTH];
   char arg2[MAX_STRING_LENGTH];
   char arg3[MAX_STRING_LENGTH];

   if( !IS_NPC( ch ) )
   {
      send_to_char( "Huh?\r\n", ch );
      return;
   }

   argument = one_argument( argument, arg );
   argument = one_argument( argument, arg2 );
   argument = one_argument( argument, arg3 );

   if( arg[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
   {
      progbug( "mpprogress: bad syntax", ch );
      return;
   }

   if( ( victim = get_char_room_mp( ch, arg ) ) == NULL )
      return;

   if( IS_NPC( victim ) )
      return;

   if( ( quest = get_quest_from_id( atoi( arg2 ) ) ) == NULL && ( quest = get_quest_from_name( arg2 ) ) == NULL )
   {
      progbug( "mpprogress: bad quest id/name given.", ch );
      return;
   }

   if( ( pquest = get_player_quest( victim, quest ) ) == NULL )
   {
      progbug( "mpprogress: mpprogress on a quest player doesn't have.", ch );
      return;
   }

   if( pquest->stage < 1 )
   {
      progbug( "mpprogress: mpprogress trying to advance a quest a player doesn't have started.", ch );
      return;
   }

   if( !str_cmp( arg3, "clear" ) && pquest->progress[0] != '\0' )
   {
      STRFREE( pquest->progress );
      pquest->progress = STRALLOC( "" );
   }
   else if( !str_cmp( arg3, "update" ) )
   {
      char buf[MAX_STRING_LENGTH];
      sprintf( buf, "%s%s\n", pquest->progress, argument );
      if( pquest->progress[0] != '\0' )
         STRFREE( pquest->progress );
      pquest->progress = STRALLOC( buf );
      ch_printf( victim, "&z[&rQuest Update&z] &W%s&w\r\n", pquest->progress );
   }

   save_char_obj( victim );
   saving_char = NULL;
   return;
}


void do_mppkset( CHAR_DATA * ch, const char *argument )
{
   send_to_char( "mppkset has been zapped into the realm of useless old code.\r\n", ch );
   return;

}



/*
 * Inflict damage from a mudprogram
 *
 *  note: should be careful about using victim afterwards
 */
ch_ret simple_damage( CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt )
{
   short dameq;
   bool npcvict;
   OBJ_DATA *damobj;
   ch_ret retcode;


   retcode = rNONE;

   if( !ch )
   {
      bug( "Damage: null ch!", 0 );
      return rERROR;
   }
   if( !victim )
   {
      progbug( "Damage: null victim!", ch );
      return rVICT_DIED;
   }

   if( victim->position == POS_DEAD )
   {
      return rVICT_DIED;
   }

   npcvict = IS_NPC( victim );

   if( dam )
   {
      if( IS_FIRE( dt ) )
         dam = ris_damage( victim, dam, RIS_FIRE );
      else if( IS_COLD( dt ) )
         dam = ris_damage( victim, dam, RIS_COLD );
      else if( IS_ACID( dt ) )
         dam = ris_damage( victim, dam, RIS_ACID );
      else if( IS_ELECTRICITY( dt ) )
         dam = ris_damage( victim, dam, RIS_ELECTRICITY );
      else if( IS_ENERGY( dt ) )
         dam = ris_damage( victim, dam, RIS_ENERGY );
      else if( dt == gsn_poison )
         dam = ris_damage( victim, dam, RIS_POISON );
      else if( dt == ( TYPE_HIT + 7 ) || dt == ( TYPE_HIT + 8 ) )
         dam = ris_damage( victim, dam, RIS_BLUNT );
      else if( dt == ( TYPE_HIT + 2 ) || dt == ( TYPE_HIT + 11 ) )
         dam = ris_damage( victim, dam, RIS_PIERCE );
      else if( dt == ( TYPE_HIT + 1 ) || dt == ( TYPE_HIT + 3 ) )
         dam = ris_damage( victim, dam, RIS_SLASH );
      if( dam < 0 )
         dam = 0;
   }

   if( victim != ch )
   {
      /*
       * Damage modifiers.
       */
      if( IS_AFFECTED( victim, AFF_SANCTUARY ) )
         dam /= 2;

      if( IS_AFFECTED( victim, AFF_PROTECT ) && IS_EVIL( ch ) )
         dam -= ( int )( dam / 4 );

      if( dam < 0 )
         dam = 0;

      /*
       * dam_message( ch, victim, dam, dt ); 
       */
   }

   /*
    * Check for EQ damage.... ;)
    */

   if( dam > 10 )
   {
      /*
       * get a random body eq part 
       */
      dameq = number_range( WEAR_LIGHT, WEAR_EYES );
      damobj = get_eq_char( victim, dameq );
      if( damobj )
      {
         if( dam > get_obj_resistance( damobj ) )
         {
            set_cur_obj( damobj );
            damage_obj( damobj );
         }
      }
   }

   /*
    * Hurt the victim.
    * Inform the victim of his new state.
    */
   victim->hit -= dam;
   if( !IS_NPC( victim ) && get_trust( victim ) >= LEVEL_IMMORTAL && victim->hit < 1 )
      victim->hit = 1;

   if( !npcvict && get_trust( victim ) >= LEVEL_IMMORTAL && get_trust( ch ) >= LEVEL_IMMORTAL && victim->hit < 1 )
      victim->hit = 1;
   update_pos( victim );

   switch ( victim->position )
   {
      case POS_MORTAL:
         act( AT_DYING, "$n is mortally wounded, and will die soon, if not aided.", victim, NULL, NULL, TO_ROOM );
         act( AT_DANGER, "You are mortally wounded, and will die soon, if not aided.", victim, NULL, NULL, TO_CHAR );
         break;

      case POS_INCAP:
         act( AT_DYING, "$n is incapacitated and will slowly die, if not aided.", victim, NULL, NULL, TO_ROOM );
         act( AT_DANGER, "You are incapacitated and will slowly die, if not aided.", victim, NULL, NULL, TO_CHAR );
         break;

      case POS_STUNNED:
         if( !IS_AFFECTED( victim, AFF_PARALYSIS ) )
         {
            act( AT_ACTION, "$n is stunned, but will probably recover.", victim, NULL, NULL, TO_ROOM );
            act( AT_HURT, "You are stunned, but will probably recover.", victim, NULL, NULL, TO_CHAR );
         }
         break;

      case POS_DEAD:
         act( AT_DEAD, "$n is DEAD!!", victim, 0, 0, TO_ROOM );
         act( AT_DEAD, "You have been KILLED!!\r\n", victim, 0, 0, TO_CHAR );
         break;

      default:
         if( dam > victim->max_hit / 4 )
            act( AT_HURT, "That really did HURT!", victim, 0, 0, TO_CHAR );
         if( victim->hit < victim->max_hit / 4 )
            act( AT_DANGER, "You wish that your wounds would stop BLEEDING so much!", victim, 0, 0, TO_CHAR );
         break;
   }

   /*
    * Payoff for killing things.
    */
   if( victim->position == POS_DEAD )
   {
      if( !npcvict )
      {
         sprintf( log_buf, "%s killed by %s at %d",
                  victim->name, ( IS_NPC( ch ) ? ch->short_descr : ch->name ), victim->in_room->vnum );
         log_string( log_buf );
         to_channel( log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL );


      }
      set_cur_char( victim );
      raw_kill( ch, victim );
      victim = NULL;

      return rVICT_DIED;
   }

   if( victim == ch )
      return rNONE;

   /*
    * Take care of link dead people.
    */
   if( !npcvict && !victim->desc )
   {
      if( number_range( 0, victim->wait ) == 0 )
      {
         do_recall( victim, "" );
         return rNONE;
      }
   }

   /*
    * Wimp out?
    */
   if( npcvict && dam > 0 )
   {
      if( ( IS_SET( victim->act, ACT_WIMPY ) && number_bits( 1 ) == 0
            && victim->hit < victim->max_hit / 2 )
          || ( IS_AFFECTED( victim, AFF_CHARM ) && victim->master && victim->master->in_room != victim->in_room ) )
      {
         start_fearing( victim, ch );
         stop_hunting( victim );
         do_flee( victim, "" );
      }
   }

   if( !npcvict && victim->hit > 0 && victim->hit <= victim->wimpy && victim->wait == 0 )
      do_flee( victim, "" );
   else if( !npcvict && IS_SET( victim->act, PLR_FLEE ) )
      do_flee( victim, "" );

   tail_chain(  );
   return rNONE;
}

CHAR_DATA *get_char_room_mp( CHAR_DATA * ch, const char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *rch;
   int number, count, vnum;

   number = number_argument( argument, arg );
   if( !str_cmp( arg, "self" ) )
      return ch;

   if( get_trust( ch ) >= LEVEL_SAVIOR && is_number( arg ) )
      vnum = atoi( arg );
   else
      vnum = -1;

   count = 0;

   for( rch = ch->in_room->first_person; rch; rch = rch->next_in_room )
      if( ( nifty_is_name( arg, rch->name ) || ( IS_NPC( rch ) && vnum == rch->pIndexData->vnum ) ) )
      {
         if( number == 0 && !IS_NPC( rch ) )
            return rch;
         else if( ++count == number )
            return rch;
      }

   if( vnum != -1 )
      return NULL;
   count = 0;
   for( rch = ch->in_room->first_person; rch; rch = rch->next_in_room )
   {
      if( !nifty_is_name_prefix( arg, rch->name ) )
         continue;
      if( number == 0 && !IS_NPC( rch ) )
         return rch;
      else if( ++count == number )
         return rch;
   }

   return NULL;
}

void do_mphunt( CHAR_DATA *ch, const char *argument )
{
   CHAR_DATA *victim;
   int max_dist;
   short ret;

   if( !IS_NPC( ch ) )
      return;

   if( ( victim = most_threat( ch ) ) == NULL )
      return;

   if( !ch->hunting )
      start_hunting( ch, victim );

   if( ch->hunting->who != victim )
      start_hunting( ch, victim );

   if( ch->fom != FOM_HUNTING )
      change_mind( ch, FOM_HUNTING );

   if( victim->in_room == ch->in_room )
   {
      stop_hunting( ch );
      set_fighting( ch, victim );
      add_queue( ch, COMBAT_ROUND );
      change_mind( ch, FOM_FIGHTING );
      return;
   }

   max_dist = ( ch->in_room->area->hi_r_vnum - ch->in_room->area->low_r_vnum ) * 2;

   ret = find_first_step( ch->in_room, victim->in_room, max_dist );
   if( ret == BFS_NO_PATH )
   {
      THREAT_DATA *threat;
      if( ( threat = has_threat( victim, ch ) ) != NULL )
         free_threat( threat );
      if( ( victim = most_threat( ch ) ) != NULL )
      {
         do_mphunt( ch, "" );
         return;
      }
   }
   if( ret < 0 )
   {
      stop_hunting( ch );
      change_mind( ch, FOM_IDLE );
   }
   else
   {
      move_char( ch, get_exit( ch->in_room, ret ), FALSE );
      if( char_died( ch ) )
         return;
      if( !ch->hunting )
      {
         if( !ch->in_room )
         {
            char buf[MAX_STRING_LENGTH];
            sprintf( buf, "Hunt_victim: no ch->in_room!  Mob #%d, name: %s.  Placing mob in limbo.",
                     ch->pIndexData->vnum, ch->name );
            bug( buf, 0 );
            char_to_room( ch, get_room_index( ROOM_VNUM_LIMBO ) );
         }
         return;
      }
   }
   if( ch->in_room == victim->in_room )
   {
      stop_hunting( ch );
      set_fighting( ch, victim );
      add_queue( ch, COMBAT_ROUND );
      change_mind( ch, FOM_FIGHTING );
   }
   return;
}

void do_mpwander( CHAR_DATA *ch, const char *argument )
{
   EXIT_DATA *exit;
   int vdir;

   if( !IS_NPC( ch ) )
      return;

   if( !ch->in_room )
   {
      progbug( "mpwander: not in a room???", ch );
      return;
   }

   vdir = number_range( 1, get_num_exits( ch->in_room ) );

   if( ( exit = get_exit_num( ch->in_room, vdir ) ) == NULL )
      return;

   move_char( ch, exit, FALSE );
   return;
}

void do_mpreward( CHAR_DATA *ch, const char *argument )
{
   CHAR_DATA *victim;
   char arg[MAX_STRING_LENGTH];
   int value;

   if( !IS_NPC( ch ) )
      return;

   argument = one_argument( argument, arg );

   if( ( victim = get_char_room_mp( ch, arg ) ) == NULL )
   {
      progbug( "mpreward:  NULL victim", ch );
      return;
   }

   argument = one_argument( argument, arg );

   if( !str_cmp( arg, "credits" ) || !str_cmp( arg, "gold" ) )
   {
      if( !is_number( argument ) )
      {
         progbug( "mpreward: no number entered for credits", ch );
         return;
      }
      if( ( value = atoi( argument ) ) < 1 )
      {
         progbug( "mpreward: number for credits entered less than 1", ch );
         return;
      }
      adjust_stat( victim, STAT_GOLD, value );
      ch_printf( victim, "You have been reawrded with %d credits.\r\n", value );
      save_char_obj( victim );
      return;
   }

   if( !str_cmp( arg, "exp" ) || !str_cmp( arg, "experience" ) )
   {
      int Class;
      argument = one_argument( argument, arg );
      if( !is_number( arg ) )
      {
         progbug( "mpreward: exp not proper class number put in.", ch );
         return;
      }
      if( ( Class = atoi( arg ) ) < 0 || Class >= MAX_ABILITY )
      {
         progbug( "mpreward: exp not proper class selection.", ch );
         return;
      }
      if( !is_number( argument ) )
      {
         progbug( "mpreward: no number put in for amount of exp to reward", ch );
         return;
      }
      if( ( value = atoi( argument ) ) < 1 )
      {
         progbug( "mpreward: exp number must be greater than 0", ch );
         return;
      }
      gain_exp( victim, value, Class );
      ch_printf( victim, "You have been rewarded with %d %s experience points.\r\n", value, ability_name[Class] );
      save_char_obj( victim );
      return;
   }
}

void do_mpblank( CHAR_DATA *ch, const char *argument )
{
   CHAR_DATA *victim;

   if( !IS_NPC( ch ) )
      return;

   if( ( victim = get_char_room_mp( ch, argument ) ) == NULL )
   {
      progbug( "mpblank: bad victim", ch );
      return;
   }

   send_to_char( "\r\n", victim );
   return;
}

void do_mpabort( CHAR_DATA *ch, const char *argument )
{
   CHAR_DATA *victim;

   if( !IS_NPC( ch ) )
      return;

   if( ( victim = get_char_room_mp( ch, argument ) ) == NULL )
   {
      progbug( "mpabort: bad victim", ch );
      return;
   }

   if( victim && victim->desc )
   {
      victim->desc->inbuf[0] = '\0';
      ch_printf( victim, "%s stops you in your tracks.\r\n", ch->name );
   }
   return;
}
