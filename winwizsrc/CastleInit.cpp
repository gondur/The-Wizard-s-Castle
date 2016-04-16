#include <windows.h>
#include <stdlib.h>  //  srand() 

#include "common.h"
#include "wizard.h"

//*************************************************************
//  Select random locations for room contents, 
//  and avoid duplicate room assignments. 
//*************************************************************
//  This could be done more randomly by having a list of
//  unallocated rooms, and deleting them from list as
//  they were used, but that's alot of work.
//*************************************************************
static void LOCATE(unsigned Z, unsigned Q)
{                                         
   unsigned X = (unsigned) random(DIMEN_COUNT);
   unsigned Y = (unsigned) random(DIMEN_COUNT);

   while (1) {
      if (castle[X][Y][Z].contents == EMPTY_ROOM) {
          castle[X][Y][Z].contents = Q;
          break;
      }
      else {
         if (++X > 7) {
            X = 0 ;
            if (++Y > 7) 
               Y = 0 ;
         }
      }
   }
}

//*************************************************************
static void place_stairs(unsigned level)
{
   unsigned X = random(DIMEN_COUNT);
   unsigned Y = random(DIMEN_COUNT);

   while (1) {
      if (castle[X][Y][level].contents == EMPTY_ROOM) {
          castle[X][Y][level].contents = STAIRS_DOWN ;
          castle[X][Y][level+1].contents = STAIRS_UP;
         break;
      }
      if (++X > 7) {
         X = 0 ;
         if (++Y > 7) 
            Y = 0 ;
      }
   }
}

//*************************************************************
static void place_treasure(unsigned Q)
{
   unsigned X = random(DIMEN_COUNT);
   unsigned Y = random(DIMEN_COUNT);
   unsigned Z = random(DIMEN_COUNT);

   while (1) {
      if (castle[X][Y][Z].contents == EMPTY_ROOM) {
         castle[X][Y][Z].contents = Q;
         break;
         }
      else {
         if (++X > 7) {
            X = 0 ;
            if (++Y > 7) 
               Y = 0 ;
         }
      }
   }
}

//*************************************************************
static void place_curse(unsigned k)
{
   unsigned X = random(DIMEN_COUNT);
   unsigned Y = random(DIMEN_COUNT);
   unsigned Z = random(DIMEN_COUNT);

   while (1) {
      if (castle[X][Y][Z].contents == EMPTY_ROOM) {
         curse_rooms[k].x = X ;
         curse_rooms[k].y = Y ;
         curse_rooms[k].level = Z ;
         break;
      } else {
         if (++X > 7) {
            X = 0 ;
            if (++Y > 7) 
               Y = 0 ;
         }
      }
   }
}

//*************************************************************
//  runestaff must be placed on a monster,
//  which must be killed to obtain it.
//*************************************************************
static void place_runestaff(void)
{
   unsigned X = random(DIMEN_COUNT);
   unsigned Y = random(DIMEN_COUNT);
   unsigned Z = random(DIMEN_COUNT);

   while (1) {
      if (castle[X][Y][Z].contents >= MONSTER_BASE  &&
          castle[X][Y][Z].contents <= MONSTER_END) {
          runestaff_room.x = X ;
          runestaff_room.y = Y ;
          runestaff_room.level = Z ;
         break;
      } else {
         if (++X > 7) {
            X = 0 ;
            if (++Y > 7) 
               Y = 0 ;
         }
      }
   }
}         

//*************************************************************
static void place_orb(void)
{
   unsigned X = random(DIMEN_COUNT);
   unsigned Y = random(DIMEN_COUNT);
   unsigned Z = random(DIMEN_COUNT);

   while (1) {
      if (castle[X][Y][Z].contents == EMPTY_ROOM) {
         orb_room.x = X ;
         orb_room.y = Y ;
         orb_room.level = Z ;
         break;
      } else {
         if (++X > 7) {
            X = 0 ;
            if (++Y > 7) 
               Y = 0 ;
         }
      }
   }
}         

//***************************************************
void init_castle_contents(void)
   {
   unsigned j, k, x, y, level ;

   /* Init. all rooms to 101 (hidden, empty rooms) */
   for (level=0; level<8; level++) {
   for (y=0; y<8; y++) {
   for (x=0; x<8; x++) {
      castle[x][y][level].contents = EMPTY_ROOM ;
      castle[x][y][level].is_known = 0 ;
   } } } 

   /* Place entrance */
   castle[0][3][0].contents = CASTLE_ENTRANCE ;         
   castle[0][3][0].is_known = 1 ;

   //  place two stairs per level
   for (level=0; level<7; level++) {
      place_stairs(level) ;
      place_stairs(level) ;
   }

   //  place objects on each floor
   for (level=0; level<8; level++) {
      //  place monsters
      for (j=MONSTER_BASE; j<=MONSTER_END; j++)  
         LOCATE(level, j) ;

      for (k=0; k<3; k++) {
         //  place room contents
         for (j=OBJECT_BASE; j<=OBJECT_END; j++)  
            LOCATE(level, j) ;

         LOCATE(level, VENDOR) ;  //  place vendor
      }
   }

   //  place treasures
   for (j=TREASURE_BASE; j<=TREASURE_END; j++) {
      place_treasure(j) ;
   }

   for (j=0; j<3; j++) {
      place_curse(j) ;
   }

   //  place the runestaff
   place_runestaff() ;
   place_orb() ;  //  place orb last, so it ends up in empty room
}

//*************************************************************
void init_player(void)
{
   // unsigned itemp = 0 ; //  later, derive from options

   ZeroMemory((char *) &player, sizeof(player_info_t)) ;
   player.castle_nbr = random(DIMEN_COUNT) ;
   player.str = 8 ;
   player.iq = 8 ;
   player.dex = 16 ;
   adjust_hit_points() ;
   race_str[3] = "DERELICT"; 
   player.armour = 1 ; 
   player.armour_points = 7 ;
   player.weapon = 3 ;
   player.has_lamp = true ;
   player.x = 0 ;
   player.y = 3 ;
   player.level = 0 ;
}         

