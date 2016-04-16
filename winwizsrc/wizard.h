//****************************************************
//  wizard.h - Declarations for Wizard's Castle
//  
//  Written by:   Daniel D. Miller
//****************************************************


//  screen-positioning constants
#define  HORZ_DIVIDER   360
#define  VERT_DIVIDER   360

//************************************************************
#define  OFF   0
#define  ON    1

//****************************************************************************
//  debug: message-reporting data 
//****************************************************************************

#define  DBG_VERBOSE       0x01
#define  DBG_WINMSGS       0x02
#define  DBG_RX_DEBUG      0x04
#define  DBG_CTASK_TRACE   0x08
#define  DBG_SMTP_RECV     0x10
#define  DBG_POLLING       0x20
#define  DBG_ETHERNET      0x40

extern uint dbg_flags ;

extern HWND hwndMain ;

//  user-defined Windows messages
static const UINT WM_ARE_YOU_ME = (WM_USER + 106) ;

//***********************************************************************
//  sprite handler constants
//***********************************************************************
#define  IMAGE_WIDTH       359
#define  IMAGE_HEIGHT      362

#define  SPRITE_WIDTH      32
#define  SPRITE_HEIGHT     32

enum {
TILE_COMBAT=0,
TILE_VICTORY,
TILE_DEATH
} ;

//***********************************************************************
extern HINSTANCE g_hinst ; //  top-level hinstance
//************************************************************
//  player attributes
//************************************************************
typedef struct player_info_s {
   unsigned castle_nbr ; //  index into castle-name table
   unsigned x, y, level ;
   unsigned str, dex, iq ;
   unsigned hit_points ;
   unsigned race ;
   // unsigned sex ; //  not really used anywhere, even in original
   unsigned armour, armour_points, weapon ;
   unsigned flares ;
   unsigned gold ;

   int   turns ;
   int   treasure_count ;
   unsigned treasures[8] ;  //  bitmapped field

   bool  has_orb_of_Zot ;
   bool  has_runestaff ;
   bool  has_lamp ;

   //  curses and afflictions
   unsigned curse_flags ;  //  bitmapped field
   bool  is_blind ;
   int   blind_count ;
   bool  has_book ;
   int   book_count ;

} player_info_t ;

extern player_info_t  player ;

//  treasure flags constants
#define  TR_RUBY_RED       0
#define  TR_NORN_STONE     1
#define  TR_PALE_PEARL     2
#define  TR_OPAL_EYE       3
#define  TR_GREEN_GEM      4
#define  TR_BLUE_FLAME     5
#define  TR_PALANTIR       6
#define  TR_SILMARIL       7

//  curse flags
#define  CR_LETHARGY       1
#define  CR_LEECH          2
#define  CR_FORGET         4

//  curse indices
#define  CURSE_OF_LETHARGY    0
#define  CURSE_OF_LEECH       1
#define  CURSE_OF_FORGET      2

//****************************************************
typedef struct castle_room_s {
   unsigned x, y, level ;  //  only used for special rooms (curse/orb/runestaff)
   unsigned is_known ;
   unsigned contents ;  //  index into object_info[] 
} castle_room_t ;

// int rooms[8][8][8] ;
#define  DIMEN_COUNT    8
extern castle_room_t castle[DIMEN_COUNT][DIMEN_COUNT][DIMEN_COUNT] ; //  Castle definitions
extern castle_room_t runestaff_room, orb_room ;
extern castle_room_t curse_rooms[3] ;

//  constants for referencing room contents.  This must match
//  display_objects_t display_objects[] 
#define  UNSEEN_ROOM        0
#define  EMPTY_ROOM         1
#define  CASTLE_ENTRANCE    2
#define  STAIRS_UP          3
#define  STAIRS_DOWN        4

#define  OBJECT_BASE        5
#define  POOL               5
#define  CHEST              6
#define  GOLD_PIECES        7
#define  FLARES             8
#define  WARP               9
#define  SINKHOLE          10
#define  CRYSTAL_ORB       11
#define  BOOK              12
#define  OBJECT_END        12

#define  MONSTER_BASE      13
#define  KOBOLD            13
#define  ORC               14
#define  KRESH             15
#define  GOBLIN            16
#define  OGRE              17
#define  TROLL             18
#define  MINOTAUR          19
#define  UR_VILE           20
#define  GARGOYLE          21
#define  CHIMERA           22
#define  BALROG            23
#define  DRAGON            24
#define  MONSTER_END       24

#define  VENDOR            25

#define  TREASURE_BASE     26
#define  RUBY_RED          26
#define  NORN_STONE        27
#define  PALE_PEARL        28
#define  OPAL_EYE          29
#define  GREEN_GEM         30
#define  BLUE_FLAME        31
#define  PALANTIR          32
#define  SILMARIL          33
#define  TREASURE_END      33

#define  RUNESTAFF         34
#define  ORB_OF_ZOT        35

#define  PLAYER            36

#define  LAST_OBJECT       37

typedef struct object_data_s {
   // unsigned obj_idx ;
   unsigned sprite_row ;
   unsigned sprite_col ;
   char*    desc ;
} object_data_t ;
extern object_data_t object_data[LAST_OBJECT] ;

//**************************************************************
//  misc string arrays
//**************************************************************
extern char *names[10] ;
extern char *race_str[4] ;
extern char *weapon_str[5] ;
extern char *armour_str[4] ;
extern char *meal[8] ;
extern char *curse_str[3] ;

extern char tempstr[128] ;

//**************************************************************
//  misc variables
//**************************************************************
// extern int vendor_repeat_visit_unwelcome ;

//**************************************************************
// #ifdef USE_BMP
// extern HBITMAP hSpriteBitmap ;
// extern HBITMAP hStreamBitmap ;
// extern HBITMAP hDeathBitmap  ;
// extern HBITMAP hCombatBitmap ;
// #endif

//**************************************************************
//  function prototypes
//**************************************************************

//  winwiz.cpp
extern uint cxClient, cyClient ;
void status_message(char *msgstr);
void status_message(uint idx, char *msgstr);
int  termout(const char *fmt, ...);
int  infoout(const char *fmt, ...);
int  queryout(const char *fmt, ...);
int  term_append(const char *fmt, ...);
int  term_replace(const char *fmt, ...);
void put_message(char *msgstr);
int  put_message(COLORREF attr, const char *fmt, ...);
int  put_message(COLORREF fgnd, COLORREF bgnd, const char *fmt, ...);
unsigned random(unsigned Q);

//****************************************************************************
//  indices for put_color_msg()
enum {
TERM_NORMAL = 0,
TERM_INFO,
TERM_QUERY,
TERM_PLAYER_HIT,
TERM_MONSTER_HIT,
TERM_RUNESTAFF,
TERM_DEATH,
TERM_ATMOSPHERE
} ;
int put_color_msg(uint idx, const char *fmt, ...);
void set_term_attr_default(void);

//  CastleInit.cpp
void init_player(void);
void init_castle_contents(void);

//  keyboard.cpp
typedef enum keymap_states_e {
KEYMAP_DEFAULT = 0,
KEYMAP_LOOKDIR,
KEYMAP_POOLDIR,
KEYMAP_MONSTER,
KEYMAP_SPELL,
KEYMAP_ZOT_SCR,
KEYMAP_WAIT_END,
KEYMAP_TELEPORT,
KEYMAP_HELP,
KEYMAP_INTRO,
KEYMAP_INITCHAR,
KEYMAP_SPELL_SELECTION,
MAX_KEYMAP_STATES 
} keymap_states_t ;

void set_default_keymap(void);
bool is_default_keymap(void) ;
bool is_intro_screen_active(void);
int push_keymap(keymap_states_t new_keymap);
int pop_keymap(void);
int process_keystroke(HWND hwnd, unsigned inchr);

//  from wfuncs.cpp
void set_up_working_spaces(HWND hwnd);
void draw_intro_screen(HWND hwnd);
int  move_south(HWND hwnd);
int  move_north(HWND hwnd);
int  move_east(HWND hwnd);
int  move_west(HWND hwnd);
int  move_down(HWND hwnd);
int  move_up(HWND hwnd);
void move_one_square(HWND hwnd);
int  look_in_direction(HWND hwnd, unsigned chr);
unsigned get_room_contents(void);
// unsigned get_room_contents(int x, int y, int level);
unsigned get_room_contents(unsigned x, unsigned y, unsigned level);
char *get_room_contents_str(void);
char *get_object_name(int index);
void show_player(void);
void player_dies(HWND hwnd);
void win_game(HWND hwnd);
void draw_main_screen(HDC hdcUnused);
void clear_room(HDC hdcUnused);
void redraw_map(void);
void adjust_hit_points(void);
void show_dex(void);
void show_int(void);
void show_hit_points(void);
void show_weapon(void);
void show_armour(void);
void show_gold(void);
void draw_beginning_screen(void);
void update_status(void);
void show_treasures(void);
bool starts_with_vowel(char *monster);
void update_cursor(void);
void render_combat_bitmap(void);
void draw_current_screen(void);

//  iface_lib.cpp
// int  textheight(HDC hdc, unsigned selected);
// void set_text_attributes(unsigned new_attribs);
// int  set_text_font(char *fontname, int psize);
// void dprints_centered_x(HDC hdc, int y, unsigned attr, char *str);
// void dprints_centered_x(HWND hwnd, int y, unsigned fg, unsigned bg, char *str);

//  from combat.cpp
void react_to_monsters(HDC hdcUnused);

//  tooltips.cpp
HWND create_tooltips(HWND hwnd, uint max_width, uint popup_msec, uint stayup_msec);
void add_main_tooltips(HWND hwnd, HWND hwndToolTip);

