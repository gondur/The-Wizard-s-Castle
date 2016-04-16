//****************************************************************************
//  Copyright (c) 2008-2013  Daniel D Miller
//  common_funcs.cpp - common functions for Windows and other programs.
//  common.h - Contains Windows-specific and System-specific definitions
//  This module, which has been entirely compiled from public-domain sources, 
//  is itself declared in the public domain.
//  
//  Collected and Organized by:  Dan Miller
//****************************************************************************

typedef unsigned char         u8 ;
typedef unsigned short        u16 ;
typedef unsigned int          uint ;
typedef unsigned long         u32 ;
typedef unsigned long long    u64 ;

#define  STX      2
#define  ETX      3
#define  CtrlC    3
#define  BSP      8
#define  HTAB     9
#define  CR       13
#define  LF       10
#define  SPC      32

//  define bounds of printable ASCII characters
#define  FIRST_PCHAR  32
#define  LAST_PCHAR   126

extern const TCHAR  TCR  ;
extern const TCHAR  TLF  ;
extern const TCHAR  TTAB ;

#define  LOOP_FOREVER   1

#define  MAX_UNICODE_LEN   1024

#define  MAX_TOOLTIP_LEN   1024

#define  ONE_SECOND        (1000)
// #define  FIVE_SECONDS      (5 * ONE_SECOND)

//  used by get_dtimes_str()
#define  GET_TIME_LEN   30

//  Windows standard COLORREF values
#define  WIN_BLACK      (0x00000000U)
#define  WIN_BLUE       (0x00AA0000U)
#define  WIN_GREEN      (0x0000AA00U)
#define  WIN_CYAN       (0x00AAAA00U)
#define  WIN_RED        (0x000000AAU)
#define  WIN_MAGENTA    (0x00AA00AAU)
#define  WIN_BROWN      (0x000055AAU)
#define  WIN_WHITE      (0x00AAAAAAU)
#define  WIN_GREY       (0x00555555U)
#define  WIN_BBLUE      (0x00FF5555U)
#define  WIN_BGREEN     (0x0055FF55U)
#define  WIN_BCYAN      (0x00FFFF55U)
#define  WIN_BRED       (0x005555FFU)
#define  WIN_BMAGENTA   (0x00FF55FFU)
#define  WIN_YELLOW     (0x0055FFFFU)
#define  WIN_BWHITE     (0x00FFFFFFU)

//*****************************************************************
//  lookup-table struct, used for Windows Message handlers
//*****************************************************************
//lint -esym(768, winproc_table_s::win_code, winproc_table_s::winproc_func)
typedef struct winproc_table_s {
   uint win_code ;
   bool (*winproc_func)(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, LPVOID private_data) ;
} winproc_table_t ;

//*****************************************************************
//  struct for building list of strings
//*****************************************************************
//lint -esym(756, string_list_p, string_list_t)
//lint -esym(768, string_list_s::next, string_list_s::msg)
//lint -esym(768, string_list_s::idx, string_list_s::marked)
typedef struct string_list_s {
   struct string_list_s *next ;
   char *msg ;
   uint idx ;     //  required for listview interfaces
   bool marked ;  //  required for listview interfaces
} string_list_t, *string_list_p ;

//*****************************************************************
//  my standard construct for translating between
//  bytes, words, and dwords
//*****************************************************************
//lint -esym(754, ul2uc_u::us)
//lint -esym(756, ul2uc_t)
//lint -esym(768, ul2uc_u::ul, ul2uc_u::us, ul2uc_u::uc)
typedef union ul2uc_u {
   u32 ul ;
   u16 us[2] ;
   u8  uc[4] ;
} ul2uc_t ;

//  windows defs which are not present in MinGW headers
//  Used for PSCB_PRECREATE message
//lint -esym(751, LPDLGTEMPLATEEX)
//lint -esym(754, DLGTEMPLATEEX_s::dlgVer, DLGTEMPLATEEX_s::helpID, DLGTEMPLATEEX_s::exStyle)
//lint -esym(754, DLGTEMPLATEEX_s::cDlgItems, DLGTEMPLATEEX_s::x, DLGTEMPLATEEX_s::y)
//lint -esym(754, DLGTEMPLATEEX_s::cx, DLGTEMPLATEEX_s::cy)
//lint -esym(756, LPDLGTEMPLATEEX, DLGTEMPLATEEX)
//lint -esym(768, DLGTEMPLATEEX_s::dlgVer, DLGTEMPLATEEX_s::helpID, DLGTEMPLATEEX_s::exStyle)
//lint -esym(768, DLGTEMPLATEEX_s::cDlgItems, DLGTEMPLATEEX_s::x, DLGTEMPLATEEX_s::y)
//lint -esym(768, DLGTEMPLATEEX_s::cx, DLGTEMPLATEEX_s::cy)
//lint -esym(768, DLGTEMPLATEEX_s::signature, DLGTEMPLATEEX_s::style)
typedef struct DLGTEMPLATEEX_s {
        WORD dlgVer;
        WORD signature;
        DWORD helpID;
        DWORD exStyle;
        DWORD style;
        WORD cDlgItems;
        short x;
        short y;
        short cx;
        short cy;
        // etc..
} DLGTEMPLATEEX, *LPDLGTEMPLATEEX; 

// #define PSN_FIRST                (0U-200U)
#define  PSN_GETOBJECT               (-210)
#define  PSN_TRANSLATEACCELERATOR    (-212)
#define  PSN_QUERYINITIALFOCUS       (-213)

#define  LVN_HOTTRACK                (-121)

//*************************************************************
inline void delay_ms(uint msec)
{
   SleepEx(msec, false) ;
}

//  common_funcs.cpp
u64 proc_time(void);
u64 get_clocks_per_second(void);
char *get_system_message(void);
char *get_system_message(DWORD errcode);
int syslog(const char *fmt, ...);
int syslogW(const TCHAR *fmt, ...);
char *show_error(int error_code);
char *next_field(char *q);
void strip_newlines(char *rstr);
char *find_newlines(char *hd);
char *strip_leading_spaces(char *str);
void strip_trailing_spaces(char *rstr);
uint crc_16(uint start, u8 *code_ptr, uint code_size);
u8 get_hex8(char *ptr);
u16 get_hex16(char *ptr);
u32 get_hex32(char *ptr);
uint uabs(uint uvalue1, uint uvalue2);
double dabs(double dvalue1, double dvalue2);
int hex_dump(u8 *bfr, int bytes, unsigned addr);
int hex_dump(u8 *bfr, int bytes);
int file_copy_by_line(char *source_file, char *dest_file);
DWORD load_exec_filename(void);
LRESULT derive_file_path(char *drvbfr, char *filename);
LRESULT derive_filename_from_exec(char *drvbfr, char *new_ext);
LRESULT get_base_filename(char *drvbfr);
LRESULT get_base_path(char *drvbfr);
LRESULT get_base_path_wide(TCHAR *drvbfr);
bool IsCharNum(char inchr);
char *get_dtimes_str(char *dest);
bool control_key_pressed(void);
bool file_exists(char *fefile);
bool dir_exists(char *fefile);
bool drive_exists(char *fefile);
uint swap_rgb(uint invalue);
BOOL CenterWindow (HWND hwnd);
void resize_window(HWND hwnd, int dx, int dy);
void srand2(unsigned long seed);
u16  rand2(void);
unsigned random_int(unsigned n);

typedef enum file_time_select_e {
FILE_DATETIME_CREATE=0,
FILE_DATETIME_LASTACCESS,
FILE_DATETIME_LASTWRITE
} file_time_select_t ;
bool get_file_datetime(char *file_name, SYSTEMTIME *sdt, file_time_select_t time_select);


//  window.mgr.cpp
#ifdef USE_BUSY_CURSOR
void start_busy_cursor(HWND hwnd) ;
void stop_busy_cursor(HWND hwnd) ;
void update_busy_cursor(void);
#endif

