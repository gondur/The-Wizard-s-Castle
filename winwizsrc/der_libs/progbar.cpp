//*****************************************************************************
//  Copyright (c) 2013  Daniel D Miller
//  class to handle WinAPI progress bar control
//
//  This module, which has been entirely compiled from public-domain sources, 
//  is itself declared in the public domain.
//*****************************************************************************

#include <windows.h>
#include <tchar.h>
#include <commctrl.h>

#include "common.h"
#include "progbar.h"

//*****************************************************************************
CProgBar::CProgBar(uint x0, uint y0, uint dx, uint dy, HWND hwndParent, HINSTANCE g_hinst) :
   hwndProgBar(NULL)
{
   // syslog("creating progress bar: X%u Y%u dx%u dy%u\n", rectWnd.left, rectWnd.top+20, dx, dy) ;
   hwndProgBar = CreateWindowEx(WS_EX_CLIENTEDGE,PROGRESS_CLASS,_T(""),WS_CHILD|WS_VISIBLE|PBS_SMOOTH,
      // 105, 222, dx, dy, hwnd,NULL,g_hinst,NULL);
      x0, y0, dx, dy, hwndParent, NULL, g_hinst, NULL);
}

//*****************************************************************************
CProgBar::~CProgBar()
{
   hwndProgBar = NULL ;
}

//*****************************************************************************
void CProgBar::set_dimens(uint range, uint step, COLORREF bar_attr)
{
   SendMessage(hwndProgBar, PBM_SETRANGE, 0, MAKELPARAM(0,range));  //Set range from 0-100
   SendMessage(hwndProgBar, PBM_SETSTEP, (WPARAM) step, 0);  //set step to 1 each time
   SendMessage(hwndProgBar, PBM_SETBARCOLOR,0,(LPARAM) bar_attr);  //set progress bar color, wparam is 0, lparam hold color value
}

//*****************************************************************************
void CProgBar::show_bar(bool shown)
{
   if (shown) {
      ShowWindow(hwndProgBar, SW_SHOW) ;
   } else {
      ShowWindow(hwndProgBar, SW_HIDE) ;
   }
}

//*****************************************************************************
void CProgBar::set_bar(uint bar_value)
{
   SendMessage(hwndProgBar, PBM_SETPOS,(WPARAM)bar_value, 0);
}

//*****************************************************************************
void CProgBar::reset_bar(void)
{
   SendMessage(hwndProgBar, PBM_SETPOS,(WPARAM) 0, 0);
}

