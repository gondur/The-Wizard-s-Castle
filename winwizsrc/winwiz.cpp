//****************************************************************************
//  Copyright (c) 1985-2014  Daniel D Miller
//  winwiz.exe - Win32 version of Wizard's Castle
//
//  Written by:  Dan Miller
//****************************************************************************
//  WIZARD'S CASTLE GAME FROM JULY/AUGUST 1980                       
//  ISSUE OF RECREATIONAL COMPUTING MAGAZINE.                        
//  WRITTEN FOR EXIDY SORCERER BY JOSEPH R. POWER                    
//                                                                   
//  FALL QUARTER, 1985:                                              
//  REWRITTEN FOR IBM PC USING GWBASIC                               
//  BY DANIEL D. (derelict) MILLER                                   
//  CALIFORNIA POLYTECHNIC STATE UNIVERSITY                          
//  SAN LUIS OBISPO, CA.                                             
//                                                                   
//  Summer, 1986:                                                    
//  Converted to Turbo Pascal by the same derelict                   
//  Last Pascal revision: October 7, 1986                            
//                                                                   
//  Fall 2004                                                        
//  Converted to Win32, compiled with either MinGw or VC++6.         
//  Also, moved revision history from here to history.winwiz.txt     
//                                                                   
//  January 2011                                                     
//  Converted to dialog-box application, using my terminal object    
//  based on the Virtual ListView control                            
//****************************************************************************

static const char *Version = "Wizard's Castle, Version 1.38" ;

//lint -esym(767, _WIN32_WINNT)
#define  _WIN32_WINNT   0x0501
#include <windows.h>
#include <stdio.h>   //  vsprintf, sprintf, which supports %f
#include <time.h>
#ifdef _lint
#include <stdlib.h>  //  RAND_MAX
#endif

#include "resource.h"
#include "common.h"
#include "commonw.h"
#include "statbar.h"
#include "cterminal.h" 
#include "winmsgs.h"
#include "wizard.h"
#include "keywin32.h"
#include "lode_png.h"

static char szAppName[] = "winwiz";

//lint -esym(714, dbg_flags)
//lint -esym(759, dbg_flags)
//lint -esym(765, dbg_flags)

//***********************************************************************
HINSTANCE g_hinst = 0;

HWND hwndMain ;

uint dbg_flags = 0
   // | DBG_WINMSGS
   ;

static uint main_timer_id = 0 ;

uint cxClient = 0 ;
uint cyClient = 0 ;

static CStatusBar *MainStatusBar = NULL;
CTerminal *myTerminal = NULL;
static HWND hToolTip ;  /* Tooltip handle */

static bool redraw_in_progress = false ;
bool prog_init_done = false ;

//***********************************************************************
LodePng pngSprites("tiles32.png", SPRITE_HEIGHT, SPRITE_WIDTH) ;
LodePng pngTiles  ("images.png",  IMAGE_WIDTH,   IMAGE_HEIGHT) ;

//*******************************************************************
void status_message(char *msgstr)
{
   MainStatusBar->show_message(msgstr);
}

//*******************************************************************
void status_message(uint idx, char *msgstr)
{
   MainStatusBar->show_message(idx, msgstr);
}

//****************************************************************************
#define  NUM_TERM_ATTR_ENTRIES   8
static attrib_table_t term_atable[NUM_TERM_ATTR_ENTRIES] = {
   { WIN_CYAN,   WIN_BLACK },    // TERM_NORMAL 
   { WIN_BCYAN,  WIN_GREY },     // TERM_INFO
   { WIN_YELLOW, WIN_BLUE },     // TERM_QUERY
   { WIN_RED,    WIN_BLACK },    // TERM_PLAYER_HIT
   { WIN_BLUE,    WIN_BLACK },   // TERM_MONSTER_HIT
   { WIN_GREY,    WIN_BLACK },   // TERM_RUNESTAFF
   { WIN_BBLUE,   WIN_BLACK },   // TERM_DEATH
   { WIN_GREEN,   WIN_BLACK }    // TERM_ATMOSPHERE
} ;

//****************************************************************************
static void set_local_terminal_colors(void)
{
   COLORREF std_bgnd = GetSysColor(COLOR_WINDOW) ;
   term_atable[TERM_NORMAL].fgnd = GetSysColor(COLOR_WINDOWTEXT) ;
   term_atable[TERM_NORMAL].bgnd = std_bgnd ;

   //  set standard background for other color sets which use it
   term_atable[TERM_PLAYER_HIT].bgnd = std_bgnd ;
   term_atable[TERM_MONSTER_HIT].bgnd = std_bgnd ;
   term_atable[TERM_RUNESTAFF].bgnd = std_bgnd ;
   term_atable[TERM_DEATH].bgnd = std_bgnd ;
   term_atable[TERM_ATMOSPHERE].bgnd = std_bgnd ;
}

//********************************************************************
static void set_term_attr(uint atidx)
{
   if (atidx >= NUM_TERM_ATTR_ENTRIES) {
      syslog("set_term_attr: invalid index %u\n", atidx) ;
      return ;
   }
      
   myTerminal->set_term_attr(term_atable[atidx].fgnd, term_atable[atidx].bgnd) ;
}

//********************************************************************
void set_term_attr_default(void)
{
   myTerminal->set_term_attr(term_atable[TERM_NORMAL].fgnd, term_atable[TERM_NORMAL].bgnd) ;
}

//********************************************************************
int termout(const char *fmt, ...)
{
   char consoleBuffer[MAX_TERM_CHARS + 1];
   va_list al; //lint !e522

   va_start(al, fmt);   //lint !e1055 !e530
   vsprintf(consoleBuffer, fmt, al);   //lint !e64
   // syslog("ecterm attribs: fgnd=%06X, bgnd=%06X\n", 
   //    term->term_fgnd, term->term_bgnd) ;
   // myTerminal->set_term_attr_default() ;
   // myTerminal->put(consoleBuffer);
   set_term_attr(TERM_NORMAL) ;
   myTerminal->put(consoleBuffer);
   va_end(al);
   return 1;
}

//********************************************************************
int term_append(const char *fmt, ...)
{
   char consoleBuffer[MAX_TERM_CHARS + 1];
   va_list al; //lint !e522

   va_start(al, fmt);   //lint !e1055 !e530
   vsprintf(consoleBuffer, fmt, al);   //lint !e64
   // syslog("ecterm attribs: fgnd=%06X, bgnd=%06X\n", 
   //    term->term_fgnd, term->term_bgnd) ;
   // myTerminal->set_term_attr_default() ;
   // myTerminal->put(consoleBuffer);
   set_term_attr(TERM_NORMAL) ;
   myTerminal->append(consoleBuffer);
   va_end(al);
   return 1;
}

//********************************************************************
int term_replace(const char *fmt, ...)
{
   char consoleBuffer[MAX_TERM_CHARS + 1];
   va_list al; //lint !e522

   va_start(al, fmt);   //lint !e1055 !e530
   vsprintf(consoleBuffer, fmt, al);   //lint !e64
   // syslog("ecterm attribs: fgnd=%06X, bgnd=%06X\n", 
   //    term->term_fgnd, term->term_bgnd) ;
   // myTerminal->set_term_attr_default() ;
   // myTerminal->put(consoleBuffer);
   set_term_attr(TERM_NORMAL) ;
   myTerminal->replace(consoleBuffer);
   va_end(al);
   return 1;
}

//********************************************************************
int queryout(const char *fmt, ...)
{
   char consoleBuffer[MAX_TERM_CHARS + 1];
   va_list al; //lint !e522

   va_start(al, fmt);   //lint !e1055 !e530
   vsprintf(consoleBuffer, fmt, al);   //lint !e64
   // syslog("ecterm attribs: fgnd=%06X, bgnd=%06X\n", 
   //    term->term_fgnd, term->term_bgnd) ;
   // myTerminal->set_term_attr(WIN_YELLOW, WIN_BLUE) ;
   set_term_attr(TERM_QUERY) ;
   myTerminal->put(consoleBuffer);
   va_end(al);
   return 1;
}

//********************************************************************
int infoout(const char *fmt, ...)
{
   char consoleBuffer[MAX_TERM_CHARS + 1];
   va_list al; //lint !e522

   va_start(al, fmt);   //lint !e1055 !e530
   vsprintf(consoleBuffer, fmt, al);   //lint !e64
   // syslog("ecterm attribs: fgnd=%06X, bgnd=%06X\n", 
   //    term->term_fgnd, term->term_bgnd) ;
   set_term_attr(TERM_INFO) ;
   myTerminal->put(consoleBuffer);
   va_end(al);
   return 1;
}

//*******************************************************************
//  this uses default font and color
//*******************************************************************
void put_message(char *msgstr)
{
   termout(msgstr) ;
}

//********************************************************************
//  this function is only used for displaying RuneStaff and/or
//  Orb Of Zot, in "treasures" listing
//********************************************************************
int put_message(COLORREF attr, const char *fmt, ...)
{
   char consoleBuffer[MAX_TERM_CHARS + 1];
   va_list al; //lint !e522

   va_start(al, fmt);   //lint !e1055 !e530
   vsprintf(consoleBuffer, fmt, al);   //lint !e64
   COLORREF prev_fgnd, prev_bgnd ;
   myTerminal->get_term_attr(&prev_fgnd, &prev_bgnd) ;
   myTerminal->set_term_attr(attr, term_atable[TERM_NORMAL].bgnd) ;
   myTerminal->put(consoleBuffer);
   myTerminal->set_term_attr(prev_fgnd, prev_bgnd) ;
   va_end(al);
   return 1;
}

//********************************************************************
//  this function is only used for displaying RuneStaff and/or
//  Orb Of Zot, in "treasures" listing
//********************************************************************
int put_message(COLORREF fgnd, COLORREF bgnd, const char *fmt, ...)
{
   char consoleBuffer[MAX_TERM_CHARS + 1];
   va_list al; //lint !e522

   va_start(al, fmt);   //lint !e1055 !e530
   vsprintf(consoleBuffer, fmt, al);   //lint !e64
   COLORREF prev_fgnd, prev_bgnd ;
   myTerminal->get_term_attr(&prev_fgnd, &prev_bgnd) ;
   myTerminal->set_term_attr(fgnd, bgnd) ;
   myTerminal->put(consoleBuffer);
   myTerminal->set_term_attr(prev_fgnd, prev_bgnd) ;
   va_end(al);
   return 1;
}

//********************************************************************
//  this *cannot* be called with a color attribute;
//  it must be called with an index into term_atable[] !!
//********************************************************************
int put_color_msg(uint idx, const char *fmt, ...)
{
   char consoleBuffer[MAX_TERM_CHARS + 1];
   va_list al; //lint !e522

   va_start(al, fmt);   //lint !e1055 !e530
   vsprintf(consoleBuffer, fmt, al);   //lint !e64
   set_term_attr(idx) ;
   myTerminal->put(consoleBuffer);
   va_end(al);
   return 1;
}

//*************************************************************
//  Notes stolen from the Linux man page for rand():
//  
// In Numerical Recipes in C: The Art of Scientific Computing 
//  (William H. Press, Brian P. Flannery, Saul A. Teukolsky, 
//   William T. Vetterling; New York: Cambridge University Press,
//   1990 (1st ed, p. 207)), the following comments are made: 
// 
// "If you want to generate a random integer between 1 and 10, 
//  you should always do it by 
// 
//    j=1+(int) (10.0*rand()/(RAND_MAX+1.0)); 
// 
// and never by anything resembling 
// 
//    j=1+((int) (1000000.0*rand()) % 10); 
// 
// (which uses lower-order bits)." 
//*************************************************************
//  return a random number between 0-(Q-1)
//*************************************************************
unsigned random(unsigned Q)
{
   unsigned j = (unsigned) ((Q * rand2()) / (RAND_MAX+1)); 
   return j;
}         

//***********************************************************************
static BOOL WeAreAlone(LPSTR szName)
{
   HANDLE hMutex = CreateMutexA(NULL, true, szName);
   if (GetLastError() == ERROR_ALREADY_EXISTS) {
      CloseHandle(hMutex);
      return false;
   }
   return true;
}

//***********************************************************************
//  http://www.flounder.com/nomultiples.htm
//  
//  This technique sends a user-defined message to each enumerated
//  top-level window on the current desktop.
//  It expects the sought window to respond with the same
//  user-defined message.
//***********************************************************************
static BOOL CALLBACK searcher(HWND hWnd, LPARAM lParam)
{
   DWORD result;
   LRESULT ok = SendMessageTimeoutA(hWnd,
                                   WM_ARE_YOU_ME,
                                   0, 0, 
                                   SMTO_BLOCK | SMTO_ABORTIFHUNG,
                                   200, &result);
   if (ok == 0)
      return true; // ignore this and continue
   if (result == WM_ARE_YOU_ME) { /* found it */
      HWND *target = (HWND *) lParam;
      *target = hWnd;
      return false; // stop search
   } /* found it */
   return true; // continue search
} // CMyApp::searcher

//******************************************************************
static bool term_notify(HWND hwnd, LPARAM lParam)
{
   int msg_code = (int) ((NMHDR FAR *) lParam)->code ;
   if (dbg_flags & DBG_WINMSGS) {
      switch (msg_code) {
      //  list messages to be ignored
      case LVN_GETDISPINFO:   //lint !e650  Constant '4294967146' out of range for operator 'case'
      case NM_CUSTOMDRAW:
      case NM_KILLFOCUS:
      case TTN_SHOW:          //lint !e650  Constant '4294967146' out of range for operator 'case'
      case TTN_POP:           //lint !e650  Constant '4294967146' out of range for operator 'case'
      case LVN_ODCACHEHINT:   //lint !e650  Constant '4294967146' out of range for operator 'case'
      case LVN_KEYDOWN:       //lint !e650  Constant '4294967146' out of range for operator 'case'
         break;
      default:
         syslog("TNOT [%s]\n", lookup_winmsg_name(msg_code)) ;
         break;
      }
   }

   switch (msg_code) {

   //**********************************************************
   //  terminal listview notifications
   //**********************************************************
   case LVN_GETDISPINFO:  //lint !e650
      // get_terminal_entry(term, lParam) ;
      myTerminal->get_terminal_entry(lParam) ;
      return true;

   case NM_CUSTOMDRAW:
      SetWindowLongA (hwnd, DWL_MSGRESULT, (LONG) myTerminal->TerminalCustomDraw(lParam));
      return true;

   default:
      // if (dbg_flags & DBG_WINMSGS)
      //    syslog("WM_NOTIFY: [%d] %s\n", msg_code, lookup_winmsg_name(msg_code)) ;
      return false;
   }
}  //lint !e715

//******************************************************************
// Subclass procedure for the Terminal Virtual ListView control
// http://msdn.microsoft.com/en-us/library/ms633570%28VS.85%29.aspx#subclassing_window
//******************************************************************
//  mon.support.cpp
static WNDPROC wpOrigTermProc; 
uint key_mask = 0 ;

static LRESULT APIENTRY TermSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
   char inchr ;

   if (uMsg == WM_GETDLGCODE) 
      return DLGC_WANTALLKEYS; 
 
   switch (uMsg) {
   case WM_KEYUP:
   case WM_SYSKEYUP:
      // ShowKey (hwnd, 0, "WM_KEYUP", wParam, lParam) ;
      switch (wParam) {
      case VK_SHIFT:   
         key_mask &= ~kShift;
         return 0;
      case VK_CONTROL: 
         key_mask &= ~kCtrl;
         return 0;
      case VK_MENU:    
         key_mask &= ~kAlt;
         return 0;
      default:
         break;
      }
      return 0;

   case WM_KEYDOWN:
   case WM_SYSKEYDOWN:
      // syslog("WM_KEYDOWN: WPARAM=0x%04X, LPARAM=%u\n", wParam, lParam) ;
      // inchr = (char) wParam ;
      // if (inchr == CtrlC) {
      //    // term_info_p tiSelf = find_term_from_hwnd(hwnd) ;
      //    copy_selected_rows(term) ;
      //    return 0;
      // }
      switch (wParam) {
      // ShowKey (hwnd, 0, "WM_KEYDOWN", wParam, lParam) ;
      case VK_SHIFT:   key_mask |= kShift;  return 0 ;
      case VK_CONTROL: key_mask |= kCtrl;  return 0 ;
      case VK_MENU:    key_mask |= kAlt;  return 0 ;
      // else {
      //    wParam |= key_mask;
      //    process_keystroke (hwnd, wParam);
      // }
      case VK_LEFT:
      case VK_UP:
      case VK_RIGHT:
      case VK_DOWN:
         // syslog("WM_KEYDOWN: process_keystroke\n") ;
         process_keystroke (hwnd, wParam) ;
         return 0;
      default:
         break;
      }
      break;

   // case WM_KEYUP:
   //    // syslog("WM_UP: [0x%X]\n", wParam) ;
   //    break;

   case WM_CHAR:
      // syslog("WM_CHAR: WPARAM=0x%04X, LPARAM=%u\n", wParam, lParam) ;
      inchr = (char) wParam ;
      if (inchr == CtrlC) {
         // term_info_p tiSelf = find_term_from_hwnd(hwnd) ;
         myTerminal->copy_selected_rows() ;
         return 0;
      }
      // syslog("WM_CHAR: process_keystroke [0x%X]\n", wParam) ;
      process_keystroke (hwnd, wParam);
      return 0;
   }  //lint !e744

   return CallWindowProc(wpOrigTermProc, hwnd, uMsg, wParam, lParam); 
}

//***********************************************************************
static uint screen_width  = 0 ;
static uint screen_height = 0 ;

static void get_monitor_dimens(HWND hwnd)
{
   HMONITOR currentMonitor;      // Handle to monitor where fullscreen should go
   MONITORINFO mi;               // Info of that monitor
   currentMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
   mi.cbSize = sizeof(MONITORINFO);
   if (GetMonitorInfo(currentMonitor, &mi) != FALSE) {
      screen_width  = mi.rcMonitor.right  - mi.rcMonitor.left ;
      screen_height = mi.rcMonitor.bottom - mi.rcMonitor.top ;
   }
   // curr_dpi = GetScreenDPI() ;
}

//***********************************************************************
static void center_window(void)
{
   get_monitor_dimens(hwndMain);
   
   RECT myRect ;
   GetWindowRect(hwndMain, &myRect) ;
   // GetClientRect(hwnd, &myRect) ;
   uint dialog_width = (myRect.right - myRect.left) ;
   uint dialog_height = (myRect.bottom - myRect.top) ;

   uint x0 = (screen_width  - dialog_width ) / 2 ;
   uint y0 = (screen_height - dialog_height) / 2 ;

   SetWindowPos(hwndMain, HWND_TOP, x0, y0, 0, 0, SWP_NOSIZE) ;
}

//***********************************************************************
static void do_init_dialog(HWND hwnd)
{
   char msgstr[81] ;
   // hwndTopLevel = hwnd ;   //  do I need this?
   wsprintfA(msgstr, "%s", Version) ;
   SetWindowTextA(hwnd, msgstr) ;

   SetClassLongA(hwnd, GCL_HICON,   (LONG) LoadIcon(g_hinst, (LPCTSTR)WINWIZICO));
   SetClassLongA(hwnd, GCL_HICONSM, (LONG) LoadIcon(g_hinst, (LPCTSTR)WINWIZICO));

   hwndMain = hwnd ;

   set_up_working_spaces(hwnd) ; //  do this *before* tooltips !!
   //***************************************************************************
   //  add tooltips and bitmaps
   //***************************************************************************
   hToolTip = create_tooltips(hwnd, 150, 100, 10000) ;
   add_main_tooltips(hwnd, hToolTip) ;

   // RECT rWindow;
   // unsigned stTop ;
   RECT myRect ;
   // GetWindowRect(hwnd, &myRect) ;
   GetClientRect(hwnd, &myRect) ;
   cxClient = (myRect.right - myRect.left) ;
   cyClient = (myRect.bottom - myRect.top) ;

   center_window() ;
   //****************************************************************
   //  create/configure status bar
   //****************************************************************
   MainStatusBar = new CStatusBar(hwnd) ;
   MainStatusBar->MoveToBottom(cxClient, cyClient) ;
   //  re-position status-bar parts
   {
   int sbparts[3];
   sbparts[0] = (int) (6 * cxClient / 10) ;
   sbparts[1] = (int) (8 * cxClient / 10) ;
   sbparts[2] = -1;
   MainStatusBar->SetParts(3, &sbparts[0]);
   }
   
   //****************************************************************
   //  create/configure terminal
   //****************************************************************
   uint ctrl_bottom = get_bottom_line(hwnd, IDC_MAP_AREA) ;
   uint lvdy = cyClient - ctrl_bottom - MainStatusBar->height() ;

   myTerminal = new CTerminal(hwnd, IDC_TERMINAL, g_hinst, 
      0, ctrl_bottom, cxClient-1, lvdy,
      LVL_STY_VIRTUAL | LVL_STY_NO_HEADER | LVL_STY_PAGE_TO_END ) ;
   myTerminal->set_terminal_font("Courier New", 100, EZ_ATTR_BOLD) ;
   myTerminal->lview_assign_column_headers() ;

   set_local_terminal_colors() ; //  should this be wrapped in the terminal module?

   // Subclass the terminal ListView
   // wpOrigTermProc = (WNDPROC) SetWindowLongPtr(term->hwndSelf, GWL_WNDPROC, (LONG) TermSubclassProc); 
   wpOrigTermProc = (WNDPROC) myTerminal->terminal_lview_subclass((LONG) TermSubclassProc); 
      
   // SetClassLong(this_port->cpterm->hwndSelf, GCL_HCURSOR,(long) 0);  //  disable class cursor
   // termout("terminal size: columns=%u, screen rows=%u", term->cols, term->rows) ;
   termout("terminal size: columns=%u, screen rows=%u",
      myTerminal->get_term_columns(), myTerminal->get_term_rows()) ;

   //****************************************************************
   main_timer_id = SetTimer(hwnd, IDT_TIMER_MAIN, 100, (TIMERPROC) NULL) ;
}

//***********************************************************************
static LRESULT CALLBACK TermProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
   static bool main_timer_ran_once = false ;
   static uint quarter_seconds = 0 ;
   // static uint curr_redraw_counts = 0 ;
   // static uint ref_redraw_counts = 0 ;

   //***************************************************
   //  debug: log all windows messages
   //***************************************************
   if (dbg_flags & DBG_WINMSGS) {
      switch (iMsg) {
      //  list messages to be ignored
      case WM_CTLCOLORBTN:
      case WM_CTLCOLORSTATIC:
      case WM_CTLCOLOREDIT:
      case WM_CTLCOLORDLG:
      case WM_MOUSEMOVE:
      case 295:  //  WM_CHANGEUISTATE
      case WM_NCMOUSEMOVE:
      case WM_NCMOUSELEAVE:
      case WM_NCHITTEST:
      case WM_SETCURSOR:
      case WM_ERASEBKGND:
      case WM_TIMER:
      case WM_NOTIFY:
      case WM_COMMAND:  //  prints its own msgs below
         break;
      default:
         syslog("TOP [%s]\n", lookup_winmsg_name(iMsg)) ;
         break;
      }
   }

   switch(iMsg) {
   case WM_INITDIALOG:
      do_init_dialog(hwnd) ;
      // wpOrigMainProc = (WNDPROC) SetWindowLongPtr(hwnd, GWL_WNDPROC, (LONG) MainSubclassProc); 
      return TRUE;

   //********************************************************************
   case WM_TIMER:
      switch (wParam) {
      case IDT_TIMER_MAIN:
         if (!main_timer_ran_once) {
            main_timer_ran_once = true ;
            KillTimer(hwnd, main_timer_id) ;
   
            draw_intro_screen(hwnd) ;
            prog_init_done = true ;
            // main_timer_id = SetTimer(hwnd, IDT_TIMER_MAIN, 1000, (TIMERPROC) NULL) ;
            main_timer_id = SetTimer(hwnd, IDT_TIMER_MAIN, 244, (TIMERPROC) NULL) ;
         } else {
            if (++quarter_seconds >= 4) {
               quarter_seconds = 0 ;
               update_cursor() ;
            }
            // syslog("next_timer...redraw_in_progress=%s\n", (redraw_in_progress) ? "true" : "false") ;
            if (redraw_in_progress) {
               // syslog("redraw counts: %u vs %u\n", curr_redraw_counts, ref_redraw_counts) ;
               // if (ref_redraw_counts != 0  &&  ref_redraw_counts == curr_redraw_counts) {
                  // syslog("redraw main screen\n") ;
                  redraw_in_progress = false ;
                  if (!is_intro_screen_active()) {
                     draw_current_screen() ;
                     show_treasures() ;
                  }
               // }
               // ref_redraw_counts = curr_redraw_counts ;
            }
         }
         return TRUE;

      default:
         break;
         // return DefWindowProcA(hwnd, iMsg, wParam, lParam);
      }
      break;

   case WM_NOTIFY:
      return term_notify(hwnd, lParam) ;

   //***********************************************************************************************
   //  04/16/14 - unfortunately, I cannot use WM_SIZE, nor any other message, to draw my graphics,
   //  because some other message occurs later and over-writes my work...
   //***********************************************************************************************
   case WM_SIZE:
      if (wParam == SIZE_RESTORED) {
         // syslog("WM_SIZE\n") ;
         redraw_in_progress = true ;
      } 
      //********************************************************************************************
      //  The last operations in the dialog redraw, are subclassed WM_CTLCOLORSTATIC messages.
      //  So, to determine when it is all done, I need to somehow recognize when these are done,
      //  and then update our graphics objects.
      //********************************************************************************************
      return TRUE;

   //  this occurs during program startup
   case WM_ERASEBKGND:
      // syslog("WM_ERASEBKGND\n") ;
      redraw_in_progress = true ;
      break;

   case WM_COMMAND:
      {  //  create local context
      DWORD cmd = HIWORD (wParam) ;
      DWORD target = LOWORD(wParam) ;

      switch (cmd) {
      case FVIRTKEY:  //  keyboard accelerators: WARNING: same code as CBN_SELCHANGE !!
         //  fall through to BM_CLICKED, which uses same targets
      case BN_CLICKED:
         switch(target) {
            
         case IDB_HELP:
            queryout("Terminal keyboard shortcuts") ;
            infoout("Alt-s = send command (i.e., print command in terminal)") ;
            infoout("Alt-h = show this help screen") ;
            infoout("Alt-c = Close this program") ;
            break;
            
         case IDB_CLOSE:
            PostMessageA(hwnd, WM_CLOSE, 0, 0);
            break;
         } //lint !e744  switch target
         return true;
      } //lint !e744  switch cmd
      break;
      }  //lint !e438 !e10  end local context

   //********************************************************************
   //  application shutdown handlers
   //********************************************************************
   case WM_CLOSE:
      DestroyWindow(hwnd);
      break;

   case WM_DESTROY:
      PostQuitMessage(0);
      break;

   // default:
   //    return false;
   }  //lint !e744  switch(iMsg) 

   return false;
}

//***********************************************************************
//lint -esym(1784, WinMain)
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
   {
   //***************************************************************
   //  note that szAppName is not a very unique name; if either:
   //  1. someone else created a mutex with this name, or
   //  2. Elsewhere in this, or some other program, we created
   //     another mutex using szAppName,
   //  then this would not have the results intended!!
   //***************************************************************
   if (!WeAreAlone (szAppName)) {
      //  The old technique:
      //  We are already running, display message and quit
      
      //  The new technique:
      //  We are already running, switch to first instance
      HWND hOther = NULL;
      EnumWindows(searcher, (LPARAM) &hOther);

      if ( hOther != NULL ) { /* pop up */
         SetForegroundWindow( hOther );

         if ( IsIconic( hOther ) )  { /* restore */
            ShowWindow( hOther, SW_RESTORE );
         } /* restore */
      } /* pop up */
      return 0;
   }

   g_hinst = hInstance;

   //Plant seed for random number generator with system time
   time_t ti ;
   time(&ti) ;
   srand2((unsigned) ti);
   // sprintf(tempstr, "ti=%u, rand=%u", ti, rand()) ;
   // OutputDebugString(tempstr) ;

   //  set up initial data structs
   // read_config_data() ;
   init_castle_contents() ;
   init_player() ;

   // hdlTopLevel = OpenProcess(PROCESS_ALL_ACCESS, false, _getpid()) ;
   HWND hwnd = CreateDialog(g_hinst, MAKEINTRESOURCE(IDD_MAIN_DIALOG), NULL, (DLGPROC) TermProc) ;
   if (hwnd == NULL) {
      syslog("CreateDialog: %s\n", get_system_message()) ;
      return 0;
   }
   HACCEL hAccel = LoadAccelerators(g_hinst, MAKEINTRESOURCE(IDR_ACCELERATOR1));  
   // [2920] hInstance=4194304, 4194304, 4194304
   // syslog("hInstance=%u, %u, %u\n", 
   //    hInstance, 
   //    GetWindowLong(hwnd, GWL_HINSTANCE),
   //    GetModuleHandle(NULL)
   //    );

   MSG Msg;
   while(GetMessage(&Msg, NULL,0,0)) {
      if(!TranslateAccelerator(hwnd, hAccel, &Msg)  &&  !IsDialogMessage(hwnd, &Msg)) {
      // if(!IsDialogMessage(hwnd, &Msg)) {
          TranslateMessage(&Msg);
          DispatchMessage(&Msg);
      }
   }

   return (int) Msg.wParam ;
}  //lint !e715

