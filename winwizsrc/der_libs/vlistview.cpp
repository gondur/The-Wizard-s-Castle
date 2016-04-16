//*********************************************************************************
//  Copyright (c) 2011-2013  Daniel D Miller
//  vlistview.cpp - functions for virtual listview control Class
//
//  This module, which has been entirely compiled from public-domain sources, 
//  is itself declared in the public domain.
//  
//  Written by:   Daniel D. Miller
//*********************************************************************************
//  This is based heavily on Microsoft's VListVw example:
// ===========================================
// Common Controls - Virtual ListView Control 
// ===========================================
// © 1999-2000 Microsoft Corporation 
// Last Updated: Sept.19,1999
// 
// SUMMARY
// ========
// VListVw is a simple example of implementing a 
// virtual ListView common control in an application. 
//*********************************************************************************
//  Note: All listviews have the ClassName WC_LISTVIEW
//  The difference between conventional listview and virtual listview is
//  that the virtual listview includes LVS_OWNERDATA in the window style,
//  causing WM_NOTIFY messages to be sent to the control in order to manage it.
//*********************************************************************************
#include <windows.h>
#include <tchar.h>
// #define  _WIN32_IE   0x0501
// #include <commctrl.h>

#include "common.h"
#include "commonw.h"
#include "vlistview.h"

// Info 1714: Member function not referenced
//lint -esym(1714, CVListView::listview_resize, CVListView::listview_assign_column_headers)
//lint -esym(1714, CVListView::goto_element, CVListView::set_selected_row)
//lint -esym(1714, CVListView::header_subclass, CVListView::create_tooltips, CVListView::check_header_hover)
//lint -esym(1714, CVListView::reset_header_hover)

//*****************************************************************************
CVListView::CVListView(HWND hwndParent, uint ControlID, HINSTANCE g_hinst, 
   uint x0, uint y0, uint dx, uint dy, uint local_style_flags) :
   hwndVListView(NULL),
   hwndLVHeader(NULL),
   hwndLViewToolTips(NULL),
   hwndLVHdrToolTips(NULL),
   force_end_of_page(true),
   hdcSelf(NULL),
   hfont(NULL),
   cxClient(dx),
   cyClient(dy),
   style_flags(local_style_flags),
   columns_created(false),
   old_cursor_x(-1),
   old_cursor_y(-1),
   old_hover_column(-1),
   cxChar(8),
   cyChar(16)

{
   // hwndVListView = CreateListView(hwnd, ControlID, g_hinst, lvx0, lvy0, lvdx, lvdy, local_style_flags) ;
   // DWORD dwStyle = WS_TABSTOP | WS_CHILD | WS_BORDER | WS_VISIBLE | LVS_REPORT 
   //               // | LVS_AUTOARRANGE 
   //               | LVS_NOCOLUMNHEADER 
   //               | LVS_OWNERDATA    //  make a virtual listview
   //               ;
   DWORD dwStyle = WS_TABSTOP | WS_CHILD | WS_BORDER | WS_VISIBLE | LVS_REPORT ;
   // syslog("CVListView constructor enter\n") ;

   if (local_style_flags & LVL_STY_NO_HEADER) {
      dwStyle |= LVS_NOCOLUMNHEADER ;
   }
   if (local_style_flags & LVL_STY_EDIT_LABELS) {
      dwStyle |= LVS_EDITLABELS ;
   }
   if (local_style_flags & LVL_STY_VIRTUAL) {
      dwStyle |= LVS_OWNERDATA ;
   }
   //  nope, this didn't work...
   // if (local_style_flags & LVL_STY_EX_FULL_ROW) {
   //    dwStyle |= LVS_EX_FULLROWSELECT ;
   // }

   if (local_style_flags & LVL_STY_PAGE_TO_END) {
      force_end_of_page = true ;
   } else {
      force_end_of_page = false ;
   }
   // syslog("VLV Create: dx=%u, dy=%u\n", dx, dy) ;

   hwndVListView = CreateWindowEx(WS_EX_CLIENTEDGE,   // ex style
      WC_LISTVIEW,              // class name - defined in commctrl.h
      _T(""),                   // dummy text
      dwStyle,                  // style
      x0,                        // x position
      y0,                        // y position
      dx,                        // width
      dy,                        // height
      hwndParent,               // parent                  GLOBAL !!
      (HMENU) ControlID,        // ID                      GLOBAL !!
      g_hinst,                  // instance                GLOBAL !!
      NULL);                    // no extra data

   //  this doesn't work either...
   // if (local_style_flags & LVL_STY_EX_FULL_ROW) {
   //    ListView_SetExtendedListViewStyleEx(hwndVListView, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
   // }
   // listview header: 3543374 / 3543374
   // listview header: 5181976 / 5181976
   // HWND hwndLVHeaderTemp = GetWindow(hwndVListView, GW_CHILD) ;
   hwndLVHeader = ListView_GetHeader(hwndVListView) ;  //  same as preceding
}

//*****************************************************************************
CVListView::~CVListView()
{
   // syslog("CVListView destructor enter\n") ;
   DeleteObject(hfont) ;
   ReleaseDC(hwndVListView, hdcSelf) ;
   hfont = 0 ;
   hdcSelf = 0 ;
   DestroyWindow (hwndVListView);
   hwndVListView = 0 ;
   hwndLVHeader = 0 ;
   hwndLViewToolTips = 0 ;
   hwndLVHdrToolTips = 0;
}

//*******************************************************************************
void CVListView::insert_column_header(lv_cols_p lvptr, uint idx)
{
   LVCOLUMN LvCol;                 // Make Column struct for ListView
   memset (&LvCol, 0, sizeof (LvCol)); // Reset Column
   LvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;   // Type of mask
   LvCol.fmt = LVCFMT_CENTER ;   //  only centers text
   LvCol.pszText = lvptr->txt ;
   LvCol.cx = lvptr->cx ;
   SendMessage(hwndVListView, LVM_INSERTCOLUMN, idx, (LPARAM) &LvCol); // Insert/Show the column
}

//*******************************************************************************
//  if VListView class is used, this function is incorporated within the class
//*******************************************************************************
void CVListView::lview_assign_column_headers(void)
{
   DWORD styles = 0 ;
   if (style_flags & LVL_STY_EX_GRIDLINES) {
      styles |= LVS_EX_GRIDLINES ;
   }
   if (style_flags & LVL_STY_EX_FULL_ROW) {
      /// styles |= (LVS_EX_FULLROWSELECT | LVS_SHOWSELALWAYS) ;
      styles |= LVS_EX_FULLROWSELECT ;
   }
   
   // LVS_EX_INFOTIP enables tooltips
   // LVS_EX_LABELTIP ensures the full tooltip is shown and is not partially hidden
   if (style_flags & LVL_STY_EX_TOOLTIPS) {
      styles |= (LVS_EX_INFOTIP | LVS_EX_LABELTIP) ;
   }
   if (styles != 0)
      ListView_SetExtendedListViewStyle(hwndVListView, styles) ;   //lint !e522

   //  set extended styles
   // ListView_SetExtendedListViewStyle(tiSelf->hwndRxData, LVS_EX_ONECLICKACTIVATE) ;

   // initialize the column
   //  Even though we've disabled column headers with LVS_NOCOLUMNHEADER,
   //  we *still* need to insert the column.
   //  Mind you, this should *only* be done ONCE...
   LVCOLUMN LvColumn;                 // Make Column struct for ListView
   memset (&LvColumn, 0, sizeof (LvColumn)); // Reset Column
   
   LvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
   LvColumn.fmt = LVCFMT_LEFT;
   //  shorten column by width of the vertical scroll bar,
   //  so we don't get a horizontal scroll bar once the 
   //  vertical scroll bar gets drawn.
   // syslog("vertical scroll-bar width=%u pixels\n", iOffset) ;
   uint iOffset = GetSystemMetrics (SM_CXVSCROLL);
   iOffset += 6 ;
   LvColumn.cx = cxClient - iOffset ;
   // LvColumn.pszText = "comm-port terminal" ;
#ifdef UNICODE            
   LvColumn.pszText = ascii2unicode(".") ;
#else
   LvColumn.pszText = "." ;   //lint !e1778  asmt is not const safe
#endif
   SendMessage(hwndVListView, LVM_INSERTCOLUMN, 0, (LPARAM)(const LV_COLUMN*)(&LvColumn)) ;
}

//*******************************************************************************
//  this form of the function takes a pointer to an array of structs
//*******************************************************************************
void CVListView::lview_assign_column_headers(lv_cols_p lv_cols, LPARAM image_list)
{
   // HWND hList = hwndLVtop ;
   if (columns_created)
      return ;

   DWORD styles = 0
      // You can obtain the state of the check box for a given item with ListView_GetCheckState. 
      // To set the check state, use ListView_SetCheckState.  If this style is set, the list-view 
      // control automatically toggles the check state when the user clicks the check box or 
      // presses the space bar.
      // To detect clicks on the checkbox:
      // You can subclass the parent window(usually the form), 
      // and watch for WM_NOTIFY, then look for LVN_ITEMCHANGED in NMHDR.code. 
      // If you search the newsgroups for "vb LVN_ITEMCHANGED", you will probably find samples.
      // | LVS_EX_CHECKBOXES  //  so far, only in column 0
      // | LVS_EX_FULLROWSELECT
      ;

   if (style_flags & LVL_STY_EX_GRIDLINES) {
      styles |= LVS_EX_GRIDLINES ;
   }
   if (style_flags & LVL_STY_EX_FULL_ROW) {
      styles |= LVS_EX_FULLROWSELECT ;
   }

   if (image_list != 0) {
      styles |= LVS_EX_SUBITEMIMAGES ;  //  allow images on SubItems
   }

   // LVS_EX_INFOTIP enables tooltips
   // LVS_EX_LABELTIP ensures the full tooltip is shown and is not partially hidden
   if (style_flags & LVL_STY_EX_TOOLTIPS) {
      styles |= (LVS_EX_INFOTIP | LVS_EX_LABELTIP) ;
   }
   if (styles != 0)
      ListView_SetExtendedListViewStyle(hwndVListView, styles) ;   //lint !e522

   // SendMessage(hwndLVtop, LVM_SETEXTENDEDLISTVIEWSTYLE, styles, styles);  
   if (styles != 0)
      ListView_SetExtendedListViewStyle(hwndVListView, styles) ;   //lint !e522
   // (void) ListView_SetImageList(hwndLVtop, get_image_list(), LVSIL_SMALL);
   if (image_list != 0) {
      SendMessage(hwndVListView, LVM_SETIMAGELIST, (WPARAM) LVSIL_SMALL, (LPARAM) image_list);
   }

   columns_created = true ;
   LVCOLUMN LvCol;                 // Make Column struct for ListView
   memset (&LvCol, 0, sizeof (LvCol)); // Reset Column
   // LvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;   // Type of mask
   //  I will try to center elements within the columns, hoping that my
   //  LEDs will be centered.  Of course, it didn't work that way...
   //  Text is centered, images are not...
   LvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;   // Type of mask
   LvCol.fmt = LVCFMT_CENTER ;   //  only centers text
   unsigned dx = 0 ;

   // Inserting columns as much as we want
   unsigned idx ;
   // for (idx=0; lv_cols[idx].txt != 0; idx++) {
   lv_cols_p lvptr = lv_cols ;
   for (idx=0; lvptr->txt != 0; idx++, lvptr++) {
      LvCol.pszText = lvptr->txt ;
      if (lvptr->cx == 0) {
         LvCol.cx = cxClient - dx - 24;
         // syslog("idx=%u, end of line, cxClient=%u\n", idx, cxClient) ;
      }
      else {
         LvCol.cx = lvptr->cx ;
         // syslog("idx=%u, cx=%u\n", idx, LvCol.cx) ;
      }
      SendMessage(hwndVListView, LVM_INSERTCOLUMN, idx, (LPARAM) &LvCol); // Insert/Show the column
      dx += (uint) LvCol.cx ;
   }
}

//***********************************************************************************
//  this form of the function takes a pointer to an array of POINTERS TO structs.
//  The reason for having this form, is because we want to store the struct array
//  as an pointer array, so we can easily re-arrange the list without needing
//  to copy the actual structures.  The earliest use of this form, was for
//  allowing re-arranging of columns in the listview.
//***********************************************************************************
void CVListView::lview_assign_column_headers(lv_cols_p lv_cols[], LPARAM image_list)
{
   // HWND hList = hwndLVtop ;
   if (columns_created)
      return ;

   DWORD styles = 0
      // You can obtain the state of the check box for a given item with ListView_GetCheckState. 
      // To set the check state, use ListView_SetCheckState.  If this style is set, the list-view 
      // control automatically toggles the check state when the user clicks the check box or 
      // presses the space bar.
      // To detect clicks on the checkbox:
      // You can subclass the parent window(usually the form), 
      // and watch for WM_NOTIFY, then look for LVN_ITEMCHANGED in NMHDR.code. 
      // If you search the newsgroups for "vb LVN_ITEMCHANGED", you will probably find samples.
      // | LVS_EX_CHECKBOXES  //  so far, only in column 0
      // | LVS_EX_FULLROWSELECT
      ;

   if (style_flags & LVL_STY_EX_GRIDLINES) {
      styles |= LVS_EX_GRIDLINES ;
   }
   if (style_flags & LVL_STY_EX_FULL_ROW) {
      styles |= LVS_EX_FULLROWSELECT ;
   }

   if (image_list != 0) {
      styles |= LVS_EX_SUBITEMIMAGES ;  //  allow images on SubItems
   }

   // LVS_EX_INFOTIP enables tooltips
   // LVS_EX_LABELTIP ensures the full tooltip is shown and is not partially hidden
   if (style_flags & LVL_STY_EX_TOOLTIPS) {
      styles |= (LVS_EX_INFOTIP | LVS_EX_LABELTIP) ;
   }
   if (styles != 0)
      ListView_SetExtendedListViewStyle(hwndVListView, styles) ;   //lint !e522

   // SendMessage(hwndLVtop, LVM_SETEXTENDEDLISTVIEWSTYLE, styles, styles);  
   if (styles != 0)
      ListView_SetExtendedListViewStyle(hwndVListView, styles) ;   //lint !e522
   // (void) ListView_SetImageList(hwndLVtop, get_image_list(), LVSIL_SMALL);
   if (image_list != 0) {
      SendMessage(hwndVListView, LVM_SETIMAGELIST, (WPARAM) LVSIL_SMALL, (LPARAM) image_list);
   }

   columns_created = true ;
   LVCOLUMN LvCol;                 // Make Column struct for ListView
   memset (&LvCol, 0, sizeof (LvCol)); // Reset Column
   // LvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;   // Type of mask
   //  I will try to center elements within the columns, hoping that my
   //  LEDs will be centered.  Of course, it didn't work that way...
   //  Text is centered, images are not...
   LvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;   // Type of mask
   LvCol.fmt = LVCFMT_CENTER ;   //  only centers text
   unsigned dx = 0 ;

   // Inserting columns as much as we want
   unsigned idx ;
   // for (idx=0; lv_cols[idx]->txt != 0; idx++) {
   lv_cols_p lvptr = lv_cols[0] ;
   // Warning 441: for clause irregularity: loop variable 'idx' not found in 2nd for expression
   for (idx=0; lvptr->txt != 0; idx++) {  //lint !e441
      lvptr = lv_cols[idx] ;
      if (!lvptr->active)
         continue;
      // syslog("VL: %02u: %u: %s\n", idx, lvptr->menu_id, unicode2ascii(lvptr->txt)) ;
      LvCol.pszText = lvptr->txt ;
      LvCol.cx = lvptr->cx ;
      SendMessage(hwndVListView, LVM_INSERTCOLUMN, idx, (LPARAM) &LvCol); // Insert/Show the column
      dx += (uint) lvptr->cx ;
   }
   if (dx > 0) {
      resize(0, 0, dx+24, cyClient) ;
   }
}

//***********************************************************************
//  apparently this only works for ComCtl version 6.0.0.0
//  I would have to use a Manifest to specify that.
//  Linking a manifest requires adding a line to resource file:
//  CREATEPROCESS_MANIFEST_RESOURCE_ID RT_MANIFEST "Svr2009.manifest"
//***********************************************************************
// http://www.gamedev.net/topic/569020-set-list-view-header-sort-arrow-c-win32/
BOOL CVListView::SetHeaderSortImage(int  columnIndex, SHOW_ARROW showArrow) 
{  
   HDITEM  hdrItem  = { 0 };

   HWND hHeader = ListView_GetHeader(hwndVListView);
   if (hHeader) {
      hdrItem.mask = HDI_FORMAT;

      if ( Header_GetItem(hHeader, columnIndex, &hdrItem) ) {
         if (showArrow == SHOW_UP_ARROW) {
            hdrItem.fmt = (hdrItem.fmt & ~HDF_SORTDOWN) | HDF_SORTUP;
         } else 
         if (showArrow == SHOW_DOWN_ARROW) {
            hdrItem.fmt = (hdrItem.fmt & ~HDF_SORTUP) | HDF_SORTDOWN;
         } else 
         {
            hdrItem.fmt = hdrItem.fmt & ~(HDF_SORTDOWN | HDF_SORTUP);
         }

         return Header_SetItem(hHeader, columnIndex, &hdrItem);
      }
   }

   return FALSE;
};

//****************************************************************************
//  These listview-header tooltips do not depend on the HWND for each
//  header column, which is fortunate since I don't know any of those handles!
//  Instead, according to the one example that I've found so far:
//  http://bb4w.wikispaces.com/List+View+with+header+tooltips
//  They then used a polling mechanism to check for cursor being within
//  one of the relevant headers.  This seems like a pretty clutzy method
//  for solving this problem; I hope there's a better way somewhere...
//****************************************************************************
bool CVListView::create_tooltips(uint max_width, uint popup_msec, uint stayup_msec)
{
   if (hwndLViewToolTips != NULL)
      return true ;
   hwndLViewToolTips = ListView_GetToolTips(hwndVListView) ;
   // hwndLViewToolTips = CreateWindowEx(0, TOOLTIPS_CLASS, NULL, TTS_ALWAYSTIP,
   //       CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, //lint !e569
   //       hwndVListView, NULL, GetModuleHandle(NULL), NULL);
   if (hwndLViewToolTips == NULL) {
      syslog("ToolTip CreateWindowEx: %s\n", get_system_message()) ;
      return false;
   } 
   SendMessage(hwndLViewToolTips, TTM_SETMAXTIPWIDTH, 0, max_width) ;
   SendMessage(hwndLViewToolTips, TTM_SETDELAYTIME, TTDT_INITIAL, popup_msec) ;
   SendMessage(hwndLViewToolTips, TTM_SETDELAYTIME, TTDT_AUTOPOP, stayup_msec) ;

   if (hwndLVHdrToolTips != NULL)
      return true ;
   hwndLVHdrToolTips = ListView_GetToolTips(hwndVListView) ;
   // hwndLVHdrToolTips = CreateWindowEx(0, TOOLTIPS_CLASS, NULL, TTS_ALWAYSTIP,
   //       CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, //lint !e569
   //       hwndVListView, NULL, GetModuleHandle(NULL), NULL);
   if (hwndLVHdrToolTips == NULL) {
      syslog("ToolTip CreateWindowEx: %s\n", get_system_message()) ;
      return false;
   } 
   SendMessage(hwndLVHdrToolTips, TTM_SETMAXTIPWIDTH, 0, max_width) ;
   SendMessage(hwndLVHdrToolTips, TTM_SETDELAYTIME, TTDT_INITIAL, popup_msec) ;
   SendMessage(hwndLVHdrToolTips, TTM_SETDELAYTIME, TTDT_AUTOPOP, stayup_msec) ;

//    TOOLINFO ti;
//    ti.cbSize = sizeof(TOOLINFO);
//    ti.uFlags = TTF_SUBCLASS;
//    ti.hwnd = hwndLVHeader ;
//    // ti.uId = (UINT) target;
//    // ti.lpszText = msg ;
//    bool bresult = SendMessage(hwndLVHdrToolTips, TTM_ADDTOOL, 0, (LPARAM) &ti);
//    if (!bresult) {
//       syslog("TTM_ADDTOOL: %s\n", get_system_message()) ;
//    }
   return true;
}

//*******************************************************************************
//  This function is called from WM_MOUSEMOVE 
//  in the listview header subclass handler
//*******************************************************************************
void CVListView::check_header_hover(void)
{
   // int result ;
   POINT pt ;
   GetCursorPos(&pt) ;
   ScreenToClient(hwndLVHeader, &pt) ;
   if (pt.x != old_cursor_x  ||  pt.y != old_cursor_y) {
      HD_HITTESTINFO hdh ;
      old_cursor_x = pt.x ;
      old_cursor_y = pt.y ;
      hdh.pt.x = pt.x ;
      hdh.pt.y = pt.y ;
      int column = SendMessage(hwndLVHeader, HDM_HITTEST, 0, (LPARAM) &hdh) ;
      if (column >= 0  &&  column != old_hover_column) {
         syslog("hover on header column %d\n", column) ;
         old_hover_column = column ;
         //********************************************************************************
         //  DDM note: everything up to here works fine, and I detect the hover column.
         //  Unfortunately, the following code simply does not display
         //  the tooltip, contrary to the example referenced at
         //  http://bb4w.wikispaces.com/List+View+with+header+tooltips
         //  
         //  Looking further at MSDN, that reference states that
         //  both TTM_UPDATETIPTEXT and TTM_NEWTOOLRECT require hwnd and uId to be valid, 
         //  which in turn suggests that TTF_IDISHWND is required in uFlags.
         //  However, I don't normally have uId values for the listview header.
         //********************************************************************************

         RECT rc ;
         Header_GetItemRect(hwndLVHeader, column, &rc) ; //lint !e522  Highest operation, a 'cast', lacks side-effects
         TCHAR msgstr[81] ;
         // [1872] hover on header column 8
         // [1872] Tooltip for column 8, L561 R611 T0 B19
         // syslog("Tooltip for column %u, L%d R%d T%d B%d", column,
         //    rc.left, rc.right, rc.top, rc.bottom) ;
         //  Note on UNICODE syntax:
         //  Although the L"..." syntax is more convenient than _T("...") syntax,
         //  the former code will not build on non-UNICODE systems,
         //  while the latter syntax will.
         //  Thus, for library modules, the latter syntax should be used.
         // wsprintf(msgstr, L"Tooltip for column %u", column) ;
         wsprintf(msgstr, _T("Tooltip for column %u"), column) ;
         TOOLINFO ti;
         ZeroMemory(&ti, sizeof(TOOLINFO));
         ti.cbSize = sizeof(TOOLINFO);
         ti.uFlags = TTF_SUBCLASS;
         // ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
         // ti.hwnd = hwndLVHeader ;
         // ti.hwnd = hwndVListView ;
         // ti.uId = 0 ;
         ti.lpszText = msgstr ;
         ti.rect.left   = pt.x ;// rc.left ;
         ti.rect.right  = pt.x + 50 ;// rc.right ;
         ti.rect.top    = pt.y + 30 ;// rc.top +30;
         ti.rect.bottom = pt.y + 60 ;// rc.bottom +30;
         // SendMessage(hwndLVHdrToolTips, TTM_ADDTOOL, 0, (LPARAM) &ti);
         // SendMessage(hwndLVHdrToolTips, TTM_ACTIVATE, 0, (LPARAM) 0);
         SendMessage(hwndLVHdrToolTips, TTM_UPDATETIPTEXT, 0, (LPARAM) &ti);
         SendMessage(hwndLVHdrToolTips, TTM_NEWTOOLRECT, 0, (LPARAM) &ti);
      } else if (column < 0) {
         old_hover_column = -1 ;
      }
      // PROCtooltip(pt.x, pt.y)
   } else {
      old_hover_column = -1 ;
   }
}

//*******************************************************************************
void CVListView::reset_header_hover(void)
{
   old_hover_column = -1 ;
}

//*******************************************************************************
void CVListView::recalc_dx(lv_cols_p lvptr, uint min_dx)
{
   unsigned dx = 0 ;
   for (; lvptr->txt != 0; lvptr++) {
      if (lvptr->active)
         dx += (uint) lvptr->cx ;
   }
   dx += 24 ;  //  allow room for vertical scroll bar
   if (dx < min_dx) {
       dx = min_dx ;
   }
   cxClient = dx ;
}

//*****************************************************************************
WNDPROC CVListView::lview_subclass(LONG TermSubclassProc)
{
   return (WNDPROC) SetWindowLongPtr(hwndVListView, GWL_WNDPROC, (LONG) TermSubclassProc); 
}

//*****************************************************************************
WNDPROC CVListView::header_subclass(LONG TermSubclassProc)
{
   return (WNDPROC) SetWindowLongPtr(hwndLVHeader, GWL_WNDPROC, (LONG) TermSubclassProc); 
}

//*************************************************************************
void CVListView::clear_listview(void)
{
   if (hwndVListView == NULL)
      return ;
   SendMessage(hwndVListView, LVM_DELETEALLITEMS, 0, 0) ;
}

//*************************************************************************
void CVListView::goto_element(uint element_num)
{
   if (hwndVListView == NULL)
      return ;
   //  well, this highlighted the element, but didn't move the
   //  listview display to there, which is what I want !
   // ListView_SetItemState(hwndVListView, element_num, (LVIS_FOCUSED | LVIS_SELECTED), 0x000F) ;

   //  LVM_ENSUREVISIBLE makes sure the selected element is visible,
   //  but usually will leave it at bottom of screen.
   //  I will typically want it in the upper half of the screen
   //  
   //  for now, I am hard-coding a row offset here.
   //  This is based on my first example program, which had 29 displayed lines.
   //  Obviously, this should be more dynamically handled!!
   SendMessage(hwndVListView, LVM_ENSUREVISIBLE, element_num+20, MAKELPARAM((false), 0)) ;
}

//****************************************************************************
void CVListView::listview_update(uint idx)
{
   // syslog("CVListView: listview_update: %u, hwnd=%u\n", idx, hwndVListView) ;
   if (hwndVListView == NULL)
      return ;
   //  this message will generate LVN_GETDISPINFO messages, 
   //  to redraw the virtual listview.
   SendMessage(hwndVListView, LVM_SETITEMCOUNT,(WPARAM)(idx+1),(LPARAM)(LVSICF_NOSCROLL));

   //  This forces the listview to scroll to end of buffer
   if (force_end_of_page)
      // ListView_EnsureVisible(hwndVListView, idx, false) ;   //lint !e522
      SendMessage(hwndVListView, LVM_ENSUREVISIBLE, idx, MAKELPARAM((false), 0)) ;
}

//****************************************************************************
void CVListView::resize(uint x0, uint y0, uint dx, uint dy)
{
   ShowWindow(hwndVListView, SW_HIDE) ;
   MoveWindow(hwndVListView, x0, y0, dx, dy, true) ;
   ShowWindow(hwndVListView, SW_SHOW) ;
   cxClient = dx ;
   cyClient = dy ;
}

//****************************************************************************
void CVListView::resize(uint dx, uint dy)
{
   ShowWindow(hwndVListView, SW_HIDE) ;
   // SetWindowPos(hwndVListView, HWND_TOP, 0, 0, dx, dy, SWP_NOMOVE) ;
   SetWindowPos(hwndVListView, HWND_TOP, 0, 0, dx, dy, SWP_NOMOVE) ;
   ShowWindow(hwndVListView, SW_SHOW) ;
   cxClient = dx ;
   cyClient = dy ;
   // syslog("resize: new height=%u pixels", dy) ;
}

//****************************************************************************
void CVListView::resize_column(uint dx)
{
   SendMessage(hwndVListView, LVM_SETCOLUMNWIDTH, 0, dx) ;
}

//****************************************************************************
void CVListView::hide_horiz_scrollbar(void)
{
   ShowWindow(hwndVListView, SW_HIDE) ;
   ShowScrollBar(hwndVListView, (int) SB_HORZ, false);
   ShowWindow(hwndVListView, SW_SHOW) ;
}

//******************************************************************
bool CVListView::is_lview_hwnd(HWND hwndTarget) const 
{
   if (hwndVListView == NULL)
      return false;
   return (hwndTarget == hwndVListView) ? true : false ;
}

//****************************************************************************
int CVListView::get_next_listview_index(int nCurItem)
{
   if (hwndVListView == NULL)
      return -1;
   // return SendMessage(hwndVListView, LVM_GETNEXTITEM, nCurItem, MAKELPARAM((LVNI_SELECTED), 0)) ;
   return ListView_GetNextItem(hwndVListView, (WPARAM) nCurItem, LVNI_SELECTED);
}

//*****************************************************************************
int CVListView::HitTest(LVHITTESTINFO *lvhti, LPARAM lParam)
{
   lvhti->pt.x = LOWORD(lParam);
   lvhti->pt.y = HIWORD(lParam);
   // ClientToScreen(hwndTopLevel, &lvhti->pt);
   ClientToScreen(GetParent(hwndVListView), &lvhti->pt);
   ScreenToClient(hwndVListView, &lvhti->pt);
   return ListView_HitTest(hwndVListView, lvhti);
}

//*****************************************************************************
uint CVListView::GetItemState(int iItem)
{
   LVITEM lvi ;
   lvi.iItem = iItem;
   lvi.iSubItem = 0;
   lvi.mask = LVIF_STATE;
   lvi.stateMask = LVIS_SELECTED;
   ListView_GetItem(hwndVListView, &lvi); //lint !e522
   return lvi.state ;
}

//*****************************************************************************
uint CVListView::get_selected_count(void)
{
   return SendMessage(hwndVListView, LVM_GETSELECTEDCOUNT, 0, 0) ;
}

//******************************************************************
// determine which row receives the click
//******************************************************************
void CVListView::find_selected_row(NMHDR* pNMHDR, int *clicked_row, int *clicked_column) 
{
   // retrieve message info.
   LPNMITEMACTIVATE pItemAct = (LPNMITEMACTIVATE) (char *) pNMHDR;   //lint !e826

   LVHITTESTINFO  hitTest;
   ZeroMemory(&hitTest, sizeof(LVHITTESTINFO));
   hitTest.pt = pItemAct->ptAction;
   SendMessage(hwndVListView, LVM_SUBITEMHITTEST, 0, (LPARAM) &hitTest);
   // putf("ListView row %d, subrow %d clicked", hitTest.iItem, hitTest.iSubItem) ;
   // return hitTest.iItem;
   *clicked_row = hitTest.iItem ;
   *clicked_column = hitTest.iSubItem ;
}

//*****************************************************************************
//  Interestingly, replacing SendMessage() with PostMessage() here,
//  caused Summary listview to stop updating, and shortly afterwards,
//  crashed the program.
//*****************************************************************************
void CVListView::update_column(LVITEM *lviptr)
{
   SendMessage(hwndVListView, LVM_SETITEM, 0, (LPARAM) lviptr);  // Send to the Listview
   // PostMessage(hwndVListView, LVM_SETITEM, 0, (LPARAM) lviptr);  // Send to the Listview
}

//*****************************************************************************
void CVListView::insert_column(LVITEM *lviptr)
{
   SendMessage(hwndVListView, LVM_INSERTITEM, 0, (LPARAM) lviptr);  // Send to the Listview
}

//*****************************************************************************
void CVListView::delete_column(uint iCol)
{
   if (hwndVListView == NULL)
      return ;
   SendMessage(hwndVListView, LVM_DELETECOLUMN, iCol, 0) ;
}

//*****************************************************************************
void CVListView::update_lview_text(uint column, LVCOLUMN *lc)
{
   SendMessage(hwndVListView, LVM_SETCOLUMN, column, (LPARAM) lc) ;
}

//***********************************************************************************
// http://www.codeproject.com/Articles/2890/Using-ListView-control-under-Win32-API
//***********************************************************************************
void CVListView::set_selected_row(int row)
{
   ListView_SetItemState(hwndVListView, (UINT) -1, 0, LVIS_SELECTED); // deselect all items
   if (row >= 0) {
      SendMessage(hwndVListView,LVM_ENSUREVISIBLE, (WPARAM) row, FALSE); // if item is far, scroll to it
      ListView_SetItemState(hwndVListView,row, LVIS_SELECTED, LVIS_SELECTED); // select item
      ListView_SetItemState(hwndVListView,row, LVIS_FOCUSED , LVIS_FOCUSED); // optional
   }
}  

//*****************************************************************************
void CVListView::set_header_text(uint idx, char *msg)
{
   HWND hwndHeader = ListView_GetHeader(hwndVListView);
   if (hwndHeader) {
      HDITEM item;
      item.mask = HDI_TEXT ;
#ifdef UNICODE            
      item.pszText = ascii2unicode(msg) ;
      item.cchTextMax = _tcslen(item.pszText) ;
#else
      item.pszText = msg ;
      item.cchTextMax = strlen(msg) ;
#endif
      item.fmt = HDF_STRING | HDF_CENTER ;
      Header_SetItem(hwndHeader, idx, &item);  //lint !e522
   } else {
      syslog("ListView_GetHeader: %s\n", get_system_message()) ;
   }
}
//*****************************************************************************
void CVListView::scroll_listview(int scroll_count)
{
   ListView_Scroll(hwndVListView, 0, scroll_count) ;  //lint !e522
}

//*****************************************************************************
// "Courier New", 100, 0, EZ_ATTR_BOLD, 0, true) ;
void CVListView::set_listview_font(char * szFaceName, int iDeciPtHeight, unsigned iAttributes)
{
   TEXTMETRICA  txtm ;
   hdcSelf = GetDC(hwndVListView) ;
   hfont = EzCreateFont(hdcSelf, szFaceName, iDeciPtHeight, 0, iAttributes, 0, true) ;
   SelectObject(hdcSelf, hfont) ;
   // SetTextAlign(hdcSelf, TA_TOP | TA_LEFT) ; //  ??  probably not for listview
   GetTextMetricsA(hdcSelf, &txtm) ;
   cxChar =  txtm.tmAveCharWidth ;
   cyChar =  txtm.tmHeight + txtm.tmExternalLeading ;
   SendMessage (hwndVListView, WM_SETFONT, (WPARAM) hfont, TRUE) ;
}

