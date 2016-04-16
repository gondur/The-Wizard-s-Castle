//*****************************************************************************
//  Copyright (c) 2012-2013  Daniel D Miller
//  Converting original statbar.cpp utility to a status-bar class
//
//  This module, which has been entirely compiled from public-domain sources, 
//  is itself declared in the public domain.
//*****************************************************************************
/*-------------------------------------------
   STATBAR.C -- Status bar helper functions.
                (c) Paul Yao, 1996
  -------------------------------------------*/
#include <windows.h>
#include <commctrl.h>
#ifndef _lint
#include <tchar.h>
#endif

#include "common.h"
#include "commonw.h"
#include "statbar.h"

//lint -esym(755, Status_GetRect, Status_GetText, Status_GetTextLength)
//lint -esym(755, Status_SetMinHeight, Status_SetParts)
//lint -esym(755, IDM_STAT_IGNORESIZE)

//lint -esym(1714, CStatusBar::IsStatusBarVisible, CStatusBar::RebuildStatusBar)
//lint -esym(1714, CStatusBar::StatusBarMessage, CStatusBar::SetParts)

// variable not directly freed or zeroed by destructor
//lint -esym(1740, CStatusBar::hwndStatusBar, CStatusBar::hwndParent)
//lint -esym(1740, CStatusBar::hdcSelf, CStatusBar::hfont)

// statbar.cpp  82  Info 1762: Member function 'CStatusBar::height(void)' could be made const
//lint -esym(1762, CStatusBar::height)

static DWORD dwStatusBarStyles = 
   WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | CCS_BOTTOM 
   // | SBARS_SIZEGRIP
   ;

static BOOL MoveStatusBar(HWND hwndStatusBar, unsigned x0, unsigned y0, unsigned dx, unsigned dy);
static BOOL StatusBar_SetText(HWND hwndStatusBar, unsigned iPart, unsigned uType, TCHAR *szText);

//*****************************************************************************
CStatusBar::CStatusBar(HWND parent_window) :
   hwndParent(parent_window),
   hwndStatusBar(NULL),
   bar_height(0),
   hdcSelf(NULL),
   hfont(NULL)
   // ,
   // cxChar(0),
   // cyChar(0)
{
   hwndStatusBar = CreateStatusWindowA((LONG) dwStatusBarStyles, "Ready", parent_window, 2);
}

//*****************************************************************************
CStatusBar::~CStatusBar()
{
   DestroyWindow(hwndStatusBar);
   hwndStatusBar = NULL ;
}

//*****************************************************************************
unsigned CStatusBar::MoveToBottom(unsigned cxClient, unsigned cyClient)
{
   RECT statusRect ;
   GetWindowRect (hwndStatusBar, &statusRect);

   unsigned cyStatus = (unsigned) (statusRect.bottom - statusRect.top) ;
   // syslog("cyStatus=%u\n", cyStatus) ; //  cyStatus=22
   unsigned stTop = cyClient - cyStatus ;
   // syslog("cxClient=%u, cyClient=%u, stTop=%u, cyStatus=%u\n", 
   //    cxClient, cyClient, stTop, cyStatus) ;
   MoveStatusBar(hwndStatusBar, 0, stTop, cxClient, cyStatus);
   bar_height = cyStatus ;
   
   return cyStatus ;
}

//*****************************************************************************
void CStatusBar::show_message(TCHAR *msg)
{
   StatusBar_SetText(hwndStatusBar, 0, 0, msg) ;
}

//*****************************************************************************
void CStatusBar::show_message(unsigned idx, TCHAR *msg)
{
   StatusBar_SetText(hwndStatusBar, 0, idx, msg) ;
}

//*****************************************************************************
//  unfortunately, this sets the entire background color,
//  *not* just one desired segment.
//*****************************************************************************
void CStatusBar::set_bgnd_color(COLORREF bgnd)
{
   // StatusBar_SetBgndColor(hwndStatusBar, bgnd) ;
   SendMessage(hwndStatusBar, SB_SETBKCOLOR, 0, bgnd) ;
}

//*****************************************************************************
// "Courier New", 100, 0, EZ_ATTR_BOLD, 0, true) ;
void CStatusBar::set_statusbar_font(char * szFaceName, int iDeciPtHeight, unsigned iAttributes)
{
   // TEXTMETRICA  txtm ;
   hdcSelf = GetDC(hwndStatusBar) ;
   hfont = EzCreateFont(hdcSelf, szFaceName, iDeciPtHeight, 0, iAttributes, 0, true) ;
   SelectObject(hdcSelf, hfont) ;
   // SetTextAlign(hdcSelf, TA_TOP | TA_LEFT) ; //  ??  probably not for listview
   // GetTextMetricsA(hdcSelf, &txtm) ;
   // cxChar =  txtm.tmAveCharWidth ;
   // cyChar =  txtm.tmHeight + txtm.tmExternalLeading ;
   SendMessage (hwndStatusBar, WM_SETFONT, (WPARAM) hfont, TRUE) ;
}

//-------------------------------------------------------------------
static void FlipStyleFlag (LPDWORD dwStyle, DWORD flag)
{
	if (*dwStyle & flag) {		  // Flag on -- turn off
		*dwStyle &= (~flag);
	}
	else {							  // Flag off -- turn on
		*dwStyle |= flag;
	}
}

//-------------------------------------------------------------------
bool CStatusBar::IsStatusBarVisible(void)
{
   if (hwndStatusBar == 0)
      return false ;
   return IsWindowVisible (hwndStatusBar) ;  //lint !e1786
}

//-------------------------------------------------------------------
bool CStatusBar::SetParts(int nParts, int *sbparts)
{
   return SendMessageA(hwndStatusBar, SB_SETPARTS, 
      (WPARAM) nParts, (LPARAM) (LPINT) sbparts) ;  //lint !e1786
}

//-------------------------------------------------------------------
bool CStatusBar::RebuildStatusBar (WORD wFlag)
{
   // HWND hwndSB;
	RECT r;

	switch (wFlag) {
   case IDM_STAT_SIZEGRIP:
      FlipStyleFlag (&dwStatusBarStyles, SBARS_SIZEGRIP);
      break;

   case IDM_STAT_TOP:
      dwStatusBarStyles &= 0xFFFFFFFC;
      dwStatusBarStyles |= CCS_TOP;
      break;

   case IDM_STAT_BOTTOM:
      dwStatusBarStyles &= 0xFFFFFFFC;
      dwStatusBarStyles |= CCS_BOTTOM;
      break;

   case IDM_STAT_NOMOVEY:
      dwStatusBarStyles &= 0xFFFFFFFC;
      dwStatusBarStyles |= CCS_NOMOVEY;
      break;

   case IDM_STAT_NOPARENTALIGN:
      FlipStyleFlag (&dwStatusBarStyles, CCS_NOPARENTALIGN);
      break;

   case IDM_STAT_NORESIZE:
      FlipStyleFlag (&dwStatusBarStyles, CCS_NORESIZE);
      break;
   }  //lint !e744

   // InitStatusBar (hwndParent);
   hwndStatusBar = CreateStatusWindowA((LONG) dwStatusBarStyles, "ReReady", hwndParent, 2);

	// Post parent a WM_SIZE message to resize children
	GetClientRect (hwndParent, &r);
	PostMessageA (hwndParent, WM_SIZE, 0, MAKELPARAM (r.right, r.bottom));

   return true ;
}

//-------------------------------------------------------------------
void CStatusBar::StatusBarMessage(WORD wMsg)
{
	switch (wMsg) {
   case IDM_ST_GETBORDERS:
      {
      char ach[180];
      int aiBorders[3];

      Status_GetBorders (hwndStatusBar, (LPARAM) &aiBorders[0]);   //lint !e665 !e522
      wsprintfA (ach, "Horiz Width = %d\n"
         "Vert Width = %d\n"
         "Separator Width = %d",
         aiBorders[0], aiBorders[1], aiBorders[2]);
      OutputDebugStringA(ach) ;
      break;
      }

   case IDM_ST_GETPARTS:
      {
      char ach[80];
      int nParts = Status_GetParts (hwndStatusBar, 0, 0);   //lint !e665 !e522
      wsprintfA (ach, "Part Count = %d", nParts);
      OutputDebugStringA(ach) ;
      break;
      }

   case IDM_ST_SETTEXT:
      Status_SetText(hwndStatusBar, 0, 0, "SB_SETTEXT Message Sent");   //lint !e1773 !e522
      break;

   case IDM_ST_SIMPLE:
      {
      static BOOL bSimple = TRUE;
      Status_Simple (hwndStatusBar, bSimple);   //lint !e665 !e522
      bSimple = (!bSimple);
      break;
      }
   }  //lint !e744 
}

//-------------------------------------------------------------------
static BOOL MoveStatusBar(HWND hwndStatusBar, unsigned x0, unsigned y0, unsigned dx, unsigned dy)
{
   return MoveWindow (hwndStatusBar, x0, y0, dx, dy, TRUE); //lint !e713
}

//-------------------------------------------------------------------
static BOOL StatusBar_SetText(HWND hwndStatusBar, unsigned iPart, unsigned uType, TCHAR *szText)
{
   if (hwndStatusBar == 0)
      return false ;
   // return SendMessageA(hwndStatusBar, SB_SETTEXT, (WPARAM) (iPart | uType), (LPARAM) (LPSTR) szText);
   return SendMessage(hwndStatusBar, SB_SETTEXT, (WPARAM) (iPart | uType), (LPARAM) szText);
}

