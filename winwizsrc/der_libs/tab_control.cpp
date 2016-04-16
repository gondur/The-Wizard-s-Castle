//****************************************************************************
//  Copyright (c) 2011-2013  Daniel D Miller
//  tab_control.cpp - a Tab Control class, to simplify future implementations
//  Written by:  Dan Miller
//****************************************************************************
#include <windows.h>
#define  _WIN32_IE   0x0501
#include <commctrl.h>
#ifdef UNICODE            
#include <tchar.h>
#endif

#include "common.h"
#ifdef UNICODE            
#include "commonw.h"
#endif
#include "tab_control.h"

//****************************************************************************
CTabControl::CTabControl(HWND hwndParent, uint ControlID, uint max_array_elements) :
   // hwndSelf(0),
   hwndSelf(GetDlgItem (hwndParent, ControlID)),
   tab_list(NULL),
   max_elements(max_array_elements),
   curr_elements(0)
{
   // hwndSelf = GetDlgItem (hwndParent, ControlID);
   tab_list = (tab_data_p *) new tab_data_p[max_elements] ;
   ZeroMemory((char *) tab_list, max_elements * sizeof(tab_data_p)) ;
   // ZeroMemory((char *) &tab_list[0], sizeof(tab_list)) ;
   //  so I cannot use sizeof(tab_list)
   // tab_list size: 4 / 128
   // syslog("tab_list size: %u / %u\n", sizeof(tab_list), max_elements * sizeof(tab_data_p)) ;
   // syslog("tab_list size: %u\n", sizeof(tab_list)) ;
}

//****************************************************************************
CTabControl::~CTabControl()
{
   //  if this *should* be called, it should be called here...
   // MonTabControl->close_all_tabs() ;
   delete [] tab_list ;
   tab_list = NULL ;
   hwndSelf = NULL ;
   max_elements = 0 ;
}

//*******************************************************************************************
tab_data_p CTabControl::create_tab(uint idx, uint ControlID, char *label, 
   BOOL CALLBACK (*DialogFunc)(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam),
   LPVOID iValue)
{
   HWND hwndParent = hwndSelf ;
   tab_data_p tabp = new tab_data_t ;
   ZeroMemory(tabp, sizeof(tab_data_t)) ;
#ifdef UNICODE            
   _tcsncpy(tabp->label, ascii2unicode(label), MAX_TAB_LABEL_LEN) ;
#else
   strncpy(tabp->label, label, MAX_TAB_LABEL_LEN) ;
#endif
   // wsprintf(tabp->label, _T("%s"), label) ;
   tabp->idx = idx ;
   tabp->hwndParent = hwndParent ;
   tabp->ControlID  = ControlID ;
   tabp->DialogProc = DialogFunc ;
   
   TC_ITEM tcItem ;
   ZeroMemory(&tcItem, sizeof(TC_ITEM)) ;
   tcItem.mask = TCIF_STATE | TCIF_TEXT;
   // max_tab_pages = idx ;
   // syslog("idx %u: tab [%s] is resource %u\n", idx, tab_labels[idx], tab_pages[idx]) ;
   // tcItem.pszText = TEXT (label);
#ifdef UNICODE            
   tcItem.pszText = ascii2unicode(label) ;
#else
   tcItem.pszText = label ;
#endif
   LRESULT lResult = SendMessage (tabp->hwndParent, (UINT) TCM_INSERTITEM, idx, (LPARAM) & tcItem);
   if (-1 == lResult) {
      delete tabp ;
      syslog("idx %u: TCM_INSERTITEM: %s\n", idx, get_system_message()) ;
      return NULL ;
   }

   // [3692] idx 0: CreateDialogParam: Invalid window handle.
   tabp->hwndSelf = CreateDialogParam(GetModuleHandle(0), MAKEINTRESOURCE(ControlID), 
         hwndParent, (DLGPROC) DialogFunc, (LPARAM) iValue);
   if (tabp->hwndSelf == NULL) {
      delete tabp ;
      syslog("idx %u: CreateDialogParam: %s\n", idx, get_system_message()) ;
      return NULL ;
   }
   // SetWindowLong(tabp->hwndSelf, GWL_STYLE, 0); //remove all window styles
   // ShowWindow (tabp->hwndSelf, SW_SHOW);
   ShowWindow (tabp->hwndSelf, SW_HIDE);
   UpdateWindow (tabp->hwndSelf);
   store_tab_element(idx, tabp);
   // syslog("create: tabp=%08X, CID=%u, idx=%u\n", (uint) tabp, ControlID, (uint) idx) ;
   return tabp ;
}

//****************************************************************************
tab_data_p CTabControl::find_tab_data(uint idx) const
{
   if (idx >= curr_elements)
      return NULL;
   return tab_list[idx] ;
}

//******************************************************************
bool CTabControl::is_tab_hwnd(HWND hwndTarget) const 
{
   if (hwndSelf == NULL)
      return false;
   return (hwndTarget == hwndSelf) ? true : false ;
}

//********************************************************************************************
void CTabControl::store_tab_element(uint idx, tab_data_p tabp)
{
   tab_list[idx] = tabp ;
   curr_elements++ ;
}

//********************************************************************************************
void CTabControl::tab_swap(uint old_tab, uint new_tab)
{
   if (curr_elements <= 1)
      return ;

   // syslog("swap tab %u to left\n", curr_idx) ;
   //  swap the two tab structs
   tab_data_p tabp ;
   tabp = tab_list[old_tab] ;
   tab_list[old_tab] = tab_list[new_tab] ;
   tab_list[new_tab] = tabp ;

   tab_data_p old_tabp = tab_list[old_tab] ;
   tab_data_p new_tabp = tab_list[new_tab] ;

   uint idx = old_tabp->idx ;
   old_tabp->idx = new_tabp->idx ;
   new_tabp->idx = idx ;
}

//********************************************************************************************
void CTabControl::tab_move(uint old_tab, uint new_tab)
{
   if (curr_elements <= 1  ||  old_tab == new_tab)
      return ;

   // syslog("swap tab %u to left\n", curr_idx) ;
   //  move old_tab to new_tab position
   uint idx ;
   tab_data_p tabp ;

   tabp = tab_list[old_tab] ;
   //  move old_tab down, shift everything else up
   if (old_tab > new_tab) {
      for (idx=old_tab; idx>=(new_tab+1); idx--) {
         tab_list[idx] = tab_list[idx-1] ;
         tab_list[idx]->idx = idx ;
      }
   } 
   //  move old_tab up, shift everything else down
   else {
      for (idx=old_tab; idx<=(new_tab-1); idx++) {
         tab_list[idx] = tab_list[idx+1] ;
         tab_list[idx]->idx = idx ;
      }
   }
   tab_list[new_tab] = tabp ;
   tab_list[idx]->idx = new_tab ;
}

//****************************************************************************
void CTabControl::renumber_tabs(uint target_idx)
{
   if (curr_elements == 0)
      return ;
   uint idx ;
   // syslog("renumbering from %u of %u\n", target_idx, curr_elements) ;
   for (idx=target_idx; idx<curr_elements; idx++) {
      tab_list[idx] = tab_list[idx+1] ;
      // tab_data_p tabp = tab_list[idx] ;
      if (tab_list[idx] != NULL) {
         tab_list[idx]->idx = idx ;
      }
   }
   // syslog("renumbering done, idx=%u\n", idx) ;
   tab_list[idx] = NULL ;
}

//****************************************************************************
void CTabControl::close_tab(uint target_idx)
{
   tab_data_p tabp = find_tab_data(target_idx) ;
   if (tabp == NULL) {
      syslog("target %u, tabp not found\n", target_idx) ;
      return ;
   }
   SendMessage (tabp->hwndParent, (UINT) TCM_DELETEITEM, tabp->idx, 0);
   delete tabp ;
   curr_elements-- ;
   // syslog("close_tab: target=%u, curr_elements=%u\n", target_idx, curr_elements) ;
   renumber_tabs(target_idx);
}

//****************************************************************************
//  I don't know if this is actually needed or not...
//****************************************************************************
void CTabControl::close_all_tabs(void)
{
   uint idx ;
   for (idx=0; idx<curr_elements; idx++) {
      DestroyWindow(tab_list[idx]->hwndSelf);
   }
   curr_elements = 0 ;
}

//********************************************************************************************
int CTabControl::cursor_in_tab(void) const
{
   HWND hwndTabCtrl = hwndSelf ;
   POINT point ;
   GetCursorPos(&point) ;   
   ScreenToClient(hwndTabCtrl, &point);

   int selected_tab = -1 ;
   RECT rect ;
   uint idx ;
   for (idx=0; idx<curr_elements; idx++) {
      //  TCM_GETITEMRECT provides the rectangle for the TAB,
      //  not for the dialog attached to the tab!
      SendMessage(hwndTabCtrl, TCM_GETITEMRECT, idx, (LPARAM) &rect);
      // syslog("%u: L%u R%u T%u B%u\n", idx, rect.left, rect.right, rect.top, rect.bottom) ;
      BOOL found_in = PtInRect(&rect, point) ;
      if (found_in) {
         selected_tab = (int) idx ;
         break;
      }
      // syslog("tab %u is %s\n", idx, (found_in) ? "selected" : "NOT selected") ;
   }
   return selected_tab ;
}

//****************************************************************************
//  select_tab() vs select_active_tab()
//  select_tab() will render the dialog contents for the page.
//     However, it does *not* actually activate the tab.
//  select_active_tab() will activate the tab *and* render the dialog
//****************************************************************************
void CTabControl::select_tab(uint curr_idx) const
{
   uint idx ;
   for (idx=0; idx<curr_elements; idx++) {
      // syslog("tab %u hwnd=%u\n", idx, (uint) tab_list[idx]->hwndSelf) ;
      ShowWindow (tab_list[idx]->hwndSelf, SW_HIDE);
   }
   ShowWindow (tab_list[curr_idx]->hwndSelf, SW_SHOW);
   // syslog("SW: tab %u gle=%u\n", curr_idx, GetLastError()) ;
   UpdateWindow (tab_list[curr_idx]->hwndSelf);
   // syslog("UW: tab %u gle=%u\n", curr_idx, GetLastError()) ;
}

//****************************************************************************
//  select_tab() vs select_active_tab()
//  select_tab() will render the dialog contents for the page.
//     However, it does *not* actually activate the tab.
//  select_active_tab() will activate the tab *and* render the dialog
//****************************************************************************
void CTabControl::set_active_tab(tab_data_p tabp) const
{
   ShowWindow (tabp->hwndSelf, SW_HIDE);
   //  Note: 
   //  TCM_SETCURFOCUS generates TCN_SELCHANGE
   //  TCM_SETCURSEL does not...
   LRESULT lResult = SendMessage (tabp->hwndParent, (UINT) TCM_SETCURFOCUS, tabp->idx, 0);
   // LRESULT lResult = SendMessage (tabp->hwndParent, (UINT) TCM_SETCURSEL, tabp->idx, 0);
   if (-1 == lResult) {
      syslog("TCM_SETCURFOCUS[%u]: %s\n", tabp->idx, get_system_message()) ;
      return;
   }
   ShowWindow (tabp->hwndSelf, SW_SHOW);
   UpdateWindow (tabp->hwndSelf);
}

//********************************************************************************************
void CTabControl::set_active_tab(uint idx) const
{
   tab_data_p tabp = find_tab_data(idx) ;
   if (tabp == NULL) {
      syslog("find_tab_data: index %u not found\n", idx) ;
      return ;
   }
   // syslog("idx %u=%u, id=%u\n", idx, (uint) tabp, tabp->idx) ;
   ShowWindow (tabp->hwndSelf, SW_HIDE);
   //  Note: 
   //  TCM_SETCURFOCUS generates TCN_SELCHANGE
   //  TCM_SETCURSEL does not...
   LRESULT lResult = SendMessage (tabp->hwndParent, (UINT) TCM_SETCURFOCUS, tabp->idx, 0);
   // LRESULT lResult = SendMessage (tabp->hwndParent, (UINT) TCM_SETCURSEL, tabp->idx, 0);
   if (-1 == lResult) {
      syslog("TCM_SETCURFOCUS[%u]: %s\n", tabp->idx, get_system_message()) ;
      return;
   }
   ShowWindow (tabp->hwndSelf, SW_SHOW);
   UpdateWindow (tabp->hwndSelf);
}

//********************************************************************************************
//  This function is superceded by a more generic function in monitors.cpp
//********************************************************************************************
void CTabControl::set_tab_label(tab_data_p tabp) const
{
   TC_ITEM tcItem ;
   ZeroMemory(&tcItem, sizeof(TC_ITEM)) ;
   tcItem.mask = TCIF_TEXT;
   tcItem.pszText = tabp->label ;
   SendMessage (tabp->hwndParent, (UINT) TCM_SETITEM, tabp->idx, (LPARAM) & tcItem);
}

//********************************************************************************************
int CTabControl::GetRowCount(void)
{
   return TabCtrl_GetRowCount(hwndSelf) ;
}

//********************************************************************************************
uint CTabControl::GetCurFocus(void)
{
   return SendMessage (hwndSelf, (UINT) TCM_GETCURFOCUS, 0, 0) ;
}

//****************************************************************************
int CTabControl::get_curr_tab(void)
{
   return SendMessageA(hwndSelf, TCM_GETCURSEL, 0, 0);
}

//******************************************************************
void CTabControl::resize_window(uint dx, uint dy)
{
   ShowWindow(hwndSelf, SW_HIDE) ;
   SetWindowPos(hwndSelf, NULL, 0, 0, dx, dy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
   ShowWindow(hwndSelf, SW_SHOW) ;
}

//******************************************************************
void CTabControl::GetItemRect(RECT *rectTabCtrl)
{
   TabCtrl_GetItemRect(hwndSelf, 0, rectTabCtrl) ;   //lint !e522
}

