//************************************************************
//  globals.cpp
//  global variables for Wizard's Castle program
//  
//  Written by:   Daniel D. Miller
//  
//  Last Update:  03/01/01 18:52
//  
//************************************************************

#include <windows.h>
#include "common.h"
#include "wizard.h"

player_info_t player ;    //  player attributes

castle_room_t castle[DIMEN_COUNT][DIMEN_COUNT][DIMEN_COUNT] ; //  Castle definitions

char tempstr[128] ;

//  locations of special objects which share room with other objects
castle_room_t runestaff_room, orb_room ;
castle_room_t curse_rooms[3] ;

//**************************************************************
// typedef struct object_data_s {
//    unsigned sprite_row ;
//    unsigned sprite_col ;
//    char*    desc ;
// } object_data_t ;
object_data_t object_data[LAST_OBJECT] = {
{ 25,  9, "unassigned room" }, // UNSEEN_ROOM    ,  
{ 21,  1, "normal room"     }, // EMPTY_ROOM     ,  
{ 21,  4, "entrance"        }, // CASTLE_ENTRANCE,  
{ 21, 11, "stairs up"       }, // STAIRS_UP      ,  
{ 21, 12, "stairs down"     }, // STAIRS_DOWN    ,  

// { OBJECT_BASE    ,   0,  6 },
{ 22, 17, "a pool"        }, // POOL           ,  
{ 14, 26, "a chest"       }, // CHEST          ,  
{ 14, 28, "gold pieces"   }, // GOLD_PIECES    ,  
{ 16,  3, "flares"        }, // FLARES         ,  
{ 22, 11, "a warp"        }, // WARP           ,  
{ 22,  2, "a sinkhole"    }, // SINKHOLE       ,  
{  0, 32, "a crystal orb" }, // CRYSTAL_ORB    ,  
{ 18,  0, "a book"        }, // BOOK           ,  
// { OBJECT_END     ,  26,  9 },

// { MONSTER_BASE   ,  26,  9 },
{  1, 21, "kobold"   }, // KOBOLD         ,    
{  1, 35, "orc"      }, // ORC            ,   
{  0, 22, "kresh"    }, // KRESH          ,   
{  1,  5, "goblin"   }, // GOBLIN         ,   
{  5,  5, "ogre"     }, // OGRE           ,   
{  4, 12, "troll"    }, // TROLL          ,   
{  8, 16, "minotaur" }, // MINOTAUR       ,   
{  9, 12, "ur-vile"  }, // UR_VILE        ,   
{  1, 14, "gargoyle" }, // GARGOYLE       ,   
{  7, 33, "chimera"  }, // CHIMERA        ,   
{  7, 17, "balrog"   }, // BALROG         ,   
{  3, 27, "dragon"   }, // DRAGON         ,   
// { MONSTER_END    ,  26,  9 },

{ 14, 34, "vendor" }, // VENDOR         ,  

// { TREASURE_BASE  ,  26,  9 },
{ 19, 29, "the Ruby Red"   }, // RUBY_RED       ,  
{ 19, 30, "the Norn Stone" }, // NORN_STONE     ,  
{ 20,  1, "the Pale Pearl" }, // PALE_PEARL     ,  
{ 16, 13, "the Opal Eye"   }, // OPAL_EYE       ,  
{ 19, 33, "the Green Gem"  }, // GREEN_GEM      ,  
{ 19, 31, "the Blue Flame" }, // BLUE_FLAME     ,  
{ 20,  3, "the Palantir"   }, // PALANTIR       ,  
{ 20,  0, "the Silmaril"   }, // SILMARIL       ,  

{ 19, 25, "the RuneStaff"  }, // RUNESTAFF       ,  
{  0, 33, "the Orb of Zot" }, // ORB_OF_ZOT       ,  
{  8, 39, "player" }  // PLAYER         ,  
} ;

//**************************************************************
//  misc variables
//**************************************************************
// int vendor_repeat_visit_unwelcome = 0 ;

//**************************************************************
//  misc string arrays
//**************************************************************
char *names[10] = {
   "THE REALM OF BYDL", "GRIMMERDHORE",  "DRAGON'S EGG",    "RYJECK THOME",
   "SABTO'S DEMISE",    "LYDEN VELDT",   "DERELICT'S DOOM", 
   "MAXWELL'S FIELD",   "SEER'S SOJOURN", "TREACHER'S GORGE"
};

char *race_str[4]   = { "Human ","Dwarf ","Hobbit","Elf   " } ;
char *weapon_str[5] = { "Hands ", "Dagger", "Mace  ", "Sword ", "A Book" } ;
char *armour_str[4] = { "Prayers","Leather","Chainmail","Plate" } ;
char *meal[8] = { "sandwich", "stew", "soup","burger","roast", "filet","taco","pie" } ;
char *curse_str[3]  = { "CURSE OF LETHARGY","CURSE OF THE LEECH","CURSE OF AMNESIA" } ;

