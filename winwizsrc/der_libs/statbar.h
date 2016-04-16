//*****************************************************************************
//  Copyright (c) 2012-2013  Daniel D Miller
//  Converting original statbar.cpp utility to a status-bar class
//
//  This module, which has been entirely compiled from public-domain sources, 
//  is itself declared in the public domain.
//*****************************************************************************
//  Class usage:
//  
// static CStatusBar *MainStatusBar = NULL;
//  
//    //  in WM_INITDIALOG
//    MainStatusBar = new CStatusBar(hwnd) ;
//    MainStatusBar->MoveToBottom(cxClient, cyClient) ;
// 
//    //  re-position status-bar parts
//    int sbparts[3];
//    sbparts[0] = (int) (6 * cxClient / 10) ;
//    sbparts[1] = (int) (8 * cxClient / 10) ;
//    sbparts[2] = -1;
//    MainStatusBar->SetParts(3, &sbparts[0]);
// 
//    //  writing to status bar:
// //*******************************************************************
// static void status_message(char *msgstr)
// {
//    MainStatusBar->show_message(msgstr);
// }
// //*******************************************************************
// static void status_message(uint idx, char *msgstr)
// {
//    MainStatusBar->show_message(idx, msgstr);
// }
//*****************************************************************************
/*-------------------------------------------
 * original code:
   STATBAR.C -- Status bar helper functions.
                (c) Paul Yao, 1996
  -------------------------------------------*/

//lint -esym(1714, CStatusBar::show_message, CStatusBar::set_bgnd_color)
//lint -esym(1714, CStatusBar::set_statusbar_font, CStatusBar::height)

//lint -esym(1719, CStatusBar)
//lint -esym(1720, CStatusBar)
//lint -esym(1722, CStatusBar)
//lint -esym(1704, CStatusBar::CStatusBar)

// Info 1712: default constructor not defined for class 'CStatusBar'
//lint -esym(1712, CStatusBar)
class CStatusBar {
private:
   HWND hwndParent ;   
   HWND hwndStatusBar ;
   unsigned bar_height ;
   HDC   hdcSelf ;   //  used in conjunction with hfont
   HFONT hfont ;  
   CStatusBar operator=(const CStatusBar src) ;
   CStatusBar(const CStatusBar&);

// protected:
//    unsigned cxChar ; //  width of char
//    unsigned cyChar ; //  height of char

public:
   CStatusBar(HWND hwndParent) ;
   ~CStatusBar();
   bool IsStatusBarVisible(void);
   unsigned MoveToBottom(unsigned cxClient, unsigned cyClient);
   bool SetParts(int nParts, int *sbparts);
   void show_message(TCHAR *msg);
   void show_message(unsigned idx, TCHAR *msg);
   void set_statusbar_font(char * szFaceName, int iDeciPtHeight, unsigned iAttributes);
   void set_bgnd_color(COLORREF bgnd);
   bool RebuildStatusBar (WORD wFlag);
   void StatusBarMessage(WORD wMsg);
   unsigned height(void) const 
      { return bar_height ; } ;
} ;

//-------------------------------------------------------------------

//  flags for RebuildStatusBar()
#define IDM_STAT_IGNORESIZE             600
#define IDM_STAT_SIZEGRIP               700
#define IDM_STAT_TOP                    701
#define IDM_STAT_BOTTOM                 702
#define IDM_STAT_NOMOVEY                703
#define IDM_STAT_NOPARENTALIGN          704
#define IDM_STAT_NORESIZE               705

//  flags for StatusBarMessage()
#define IDM_ST_GETBORDERS               800
#define IDM_ST_GETPARTS                 801
#define IDM_ST_SETTEXT                  802
#define IDM_ST_SIMPLE                   803

//-------------------------------------------------------------------------------
//  Status Bar Helper Macros
//  Note that none of these macros are usable within the context of this class,
//  because they require access to the HWND, which is private data.
//-------------------------------------------------------------------------------
#define Status_GetBorders(hwnd, aBorders) \
    (BOOL) SendMessageA((hwnd), SB_GETBORDERS, 0, (LPARAM) (LPINT) aBorders)

#define Status_GetParts(hwnd, nParts, aRightCoord) \
    (int)  SendMessageA((hwnd), SB_GETPARTS, (WPARAM) nParts, (LPARAM) (LPINT) aRightCoord)

#define Status_GetRect(hwnd, iPart, lprc) \
    (BOOL) SendMessageA((hwnd), SB_GETRECT, (WPARAM) iPart, (LPARAM) (LPRECT) lprc)

#define Status_GetText(hwnd, iPart, szText) \
    (DWORD)SendMessageA((hwnd), SB_GETTEXT, (WPARAM) iPart, (LPARAM) (LPSTR) szText)

#define Status_GetTextLength(hwnd, iPart) \
    (DWORD)SendMessageA((hwnd), SB_GETTEXTLENGTH, (WPARAM) iPart, 0L)

#define Status_SetMinHeight(hwnd, minHeight) \
    (void) SendMessageA((hwnd), SB_SETMINHEIGHT, (WPARAM) minHeight, 0L)

#define Status_SetParts(hwnd, nParts, aWidths) \
    (BOOL) SendMessageA((hwnd), SB_SETPARTS, (WPARAM) nParts, (LPARAM) (LPINT) aWidths)

#define Status_SetText(hwnd, iPart, uType, szText) \
    (BOOL) SendMessageA((hwnd), SB_SETTEXT, (WPARAM) (iPart | uType), (LPARAM) (LPSTR) szText)

#define Status_Simple(hwnd, fSimple) \
    (BOOL) SendMessageA((hwnd), SB_SIMPLE, (WPARAM) (BOOL) fSimple, 0L)

