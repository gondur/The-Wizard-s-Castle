//****************************************************************************
//  Copyright (c) 2008-2014  Daniel D Miller
//  tooltips.cpp - contains default tooltips functions/data
//
//  Written by:  Dan Miller
//****************************************************************************
//  Usage:
//    this_port->hToolTip = create_tooltips(hwnd, 150, 100, 10000) ;
//    add_Summary_tooltips(hwnd, this_port->hToolTip) ;
//****************************************************************************

#include <windows.h>
#define  _WIN32_IE   0x0501
#include <commctrl.h>

#include "resource.h"
#include "common.h"
#include "wizard.h"

//  static tooltip-list struct
typedef struct tooltip_data_s {
   uint ControlID ;
   char *msg ;
} tooltip_data_t ;

//****************************************************************************
HWND create_tooltips(HWND hwnd, uint max_width, uint popup_msec, uint stayup_msec)
{
   HWND hToolTip = CreateWindowEx(0, TOOLTIPS_CLASS, NULL, TTS_ALWAYSTIP,
         CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, //lint !e569
         hwnd, NULL, GetModuleHandle(NULL), NULL);
   if (hToolTip == NULL) {
      syslog("ToolTip CreateWindowEx: %s\n", get_system_message()) ;
   } else {
      SendMessage(hToolTip, TTM_SETMAXTIPWIDTH, 0, max_width) ;
      SendMessage(hToolTip, TTM_SETDELAYTIME, TTDT_INITIAL, popup_msec) ;
      SendMessage(hToolTip, TTM_SETDELAYTIME, TTDT_AUTOPOP, stayup_msec) ;
   }
   return hToolTip ;
}

//****************************************************************************
static void add_tooltip_target(HWND parent, HWND target, HWND hToolTip, char *msg)
{
   // static bool hex_dump_was_run = false ;
   TOOLINFO ti;
   ti.cbSize = sizeof(TOOLINFO);
   ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
   ti.hwnd = parent;
   ti.uId = (UINT) target;
   ti.lpszText = msg ;
   bool bresult = SendMessage(hToolTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
   if (!bresult) {
      syslog("TTM_ADDTOOL: %u: %s\n", target, get_system_message()) ;
   } 
   // else {
   //    syslog("TTM_ADDTOOL: %u: success!!\n", target) ;
   // }
}

//****************************************************************************
//  main-dialog tooltips
//****************************************************************************
static tooltip_data_t main_tooltips[] = {
{ IDC_T0,    " the Ruby Red "   },  
{ IDC_T1,    " the Norn Stone " },  
{ IDC_T2,    " the Pale Pearl " },  
{ IDC_T3,    " the Opal Eye "   },  
{ IDC_RS,    " the RuneStaff "  },
{ IDC_T4,    " the Green Gem "  },  
{ IDC_T5,    " the Blue Flame " },  
{ IDC_T6,    " the Palantir "   },  
{ IDC_T7,    " the Silmaril "   },  
{ IDC_OZ,    " the Orb of Zot " },
{ IDS_HELP,  " show Help file " },
{ 0, NULL }} ;

void add_main_tooltips(HWND hwnd, HWND hwndToolTip)
{
   unsigned idx ;
   for (idx=0; main_tooltips[idx].ControlID != 0; idx++) {
      // if (idx < 10) {
      //    //  for some reason, these tooltips *never* get rendered...
      //    //  Yet the Help tooltip, which was defined identically, works fine.  
      //    //  I don't know why two identical objects behave differently...
      //    add_tooltip_target(hwnd, hwndTreasures[idx], hwndToolTip, main_tooltips[idx].msg) ;
      // } else {
         add_tooltip_target(hwnd, GetDlgItem(hwnd, main_tooltips[idx].ControlID),
            hwndToolTip, main_tooltips[idx].msg) ;
      // }
   }
}

