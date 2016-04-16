//****************************************************************************
//  Copyright (c) 2011-2013  Daniel D Miller
//  tab_control.cpp - a Tab Control class, to simplify future implementations
//  Written by:  Dan Miller
//****************************************************************************

//lint -esym(1704, CTabControl::CTabControl)
//lint -esym(1712, CTabControl)
//lint -esym(1714, CTabControl::close_all_tabs, CTabControl::close_tab, CTabControl::set_active_tab)
//lint -esym(1714, CTabControl::tab_swap, cursor_in_tab, tab_move)

//********************************************************************************************
#define  MAX_TAB_LABEL_LEN    20

// #define  CPORT_TITLE_LEN      20
// TCHAR CPortTitle[CPORT_TITLE_LEN+1] ;

//lint -esym(768, tab_data_s::private_data)
typedef struct tab_data_s {
   uint idx ;
   uint ControlID ;
   TCHAR label[MAX_TAB_LABEL_LEN+1] ;
   BOOL CALLBACK (*DialogProc)(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) ;
   HWND hwndSelf ;
   HWND hwndParent ;
   // LPVOID private_data ;
} tab_data_t, *tab_data_p ;

//****************************************************************************
class CTabControl {
private:
   HWND hwndSelf ;

   tab_data_p *tab_list ;
   // tab_data_p tab_list[MAX_TAB_COUNT] ;
   uint max_elements ;
   uint curr_elements ;
   
   //  static functions
   void renumber_tabs(uint target_idx);
   void store_tab_element(uint idx, tab_data_p tabp);

   //  bypass copy constructor and assignment operator
   CTabControl operator=(const CTabControl src) ;
   CTabControl(const CTabControl&);

public:
   CTabControl(HWND hwndParent, uint ControlID, uint max_elements);
   ~CTabControl();
   tab_data_p create_tab(uint idx, uint ControlID, char *label, 
      BOOL CALLBACK (*DialogFunc)(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam),
      LPVOID iValue);
   bool is_tab_hwnd(HWND hwndTarget) const ;
   tab_data_p find_tab_data(uint idx) const ;
   void tab_swap(uint old_tab, uint new_tab);
   void tab_move(uint old_tab, uint new_tab);
   void select_tab(uint curr_idx) const;
   void close_tab(uint target_idx);
   void close_all_tabs(void);
   int  cursor_in_tab(void) const;
   void set_active_tab(tab_data_p tabp) const;
   void set_active_tab(uint idx) const;
   void set_tab_label(tab_data_p tabp) const;
   int  GetRowCount(void);
   uint GetCurFocus(void);
   int  get_curr_tab(void);
   void resize_window(uint dx, uint dy);
   void GetItemRect(RECT *rectTabCtrl);
} ;

