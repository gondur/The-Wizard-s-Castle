//****************************************************************************
//  Copyright (c) 2011-2013  Daniel D Miller
//  vlistview.h - declarations for virtual listview control class
//
//  This module, which has been entirely compiled from public-domain sources, 
//  is itself declared in the public domain.
//  
//  Written by:   Daniel D. Miller
//****************************************************************************
//  Sample usage:
//    CVListView SummaryLView = NULL ;
//    SummaryLView = new CVListView(hwndTopLevel, IDC_LISTVIEW1, g_hinst, x0, y0, dx, dy, 
//                                  LVL_STY_EDIT_LABELS | LVL_STY_EX_GRIDLINES);
//    SummaryLView->set_listview_font("Times New Roman", 100, 0, 0, 0, true) ;
//    SummaryLView->lview_assign_column_headers(&lv_cols[0], (LPARAM) get_image_list()) ;
//****************************************************************************

//****************************************************************************
// Warning 1512: destructor for base class 'CVListView' is not virtual
//  'virtual' on the destructor, determines what happens if a pointer of Base Class type
//  is pointing to a Derived Class object, and delete is called on the pointer.
//  
//  If the destructor is virtual, the destructor of the "most derived" class is called.
//  Otherwise, the destructor for the pointer type is called.
//****************************************************************************

#define  _WIN32_IE   0x0501
#include <commctrl.h>

//lint -esym(756, lv_cols_t)

//lint -esym(1714, CVListView::lview_subclass, CVListView::lview_assign_column_headers, CVListView::is_lview_hwnd)
//lint -esym(1714, CVListView::get_next_listview_index, CVListView::get_selected_count, CVListView::set_header_text)
//lint -esym(1714, CVListView::update_lview_text, CVListView::scroll_listview, CVListView::update_column)
//lint -esym(1714, CVListView::insert_column, CVListView::find_selected_row, CVListView::resize_listview)
//lint -esym(1714, CVListView::get_lview_dx, CVListView::get_lview_dy, CVListView::SetHeaderSortImage)

// vlistview.h  Info 1712: default constructor not defined for class 'CVListView'
//lint -esym(1712, CVListView)

//lint -esym(1719, CVListView, CTerminal)
//lint -esym(1720, CVListView, CTerminal)
//lint -esym(1722, CVListView, CTerminal)
//lint -esym(1704, CVListView::CVListView, CTerminal::CTerminal)

// Info 1711: class 'CVListView' has a virtual function but is not inherited, 
//            so none of its functions need to be virtual
//lint -esym(1711, CVListView)

#define  LVL_STY_VIRTUAL         0x01
#define  LVL_STY_NO_HEADER       0x02
#define  LVL_STY_PAGE_TO_END     0x04
#define  LVL_STY_EDIT_LABELS     0x08
#define  LVL_STY_EX_GRIDLINES    0x10
#define  LVL_STY_EX_FULL_ROW     0x20
#define  LVL_STY_EX_TOOLTIPS     0x40  //  not yet supported

//****************************************************************************
#define  HDF_SORTDOWN   0x200
#define  HDF_SORTUP     0x400

//lint -esym(769, SHOW_NO_ARROW)
typedef enum 
{
   SHOW_NO_ARROW,
   SHOW_UP_ARROW,
   SHOW_DOWN_ARROW
} SHOW_ARROW;

//****************************************************************************
//  listview column-header definition struct
//****************************************************************************
typedef struct lv_cols_s {
   TCHAR *txt ;
   uint cx ;
   bool active ;
   uint menu_id ;
   void (*renderFunc)(void *private_data, uint curr_rows, uint iSubItem) ;
} lv_cols_t, *lv_cols_p ;

//****************************************************************************
class CVListView {
private:
   HWND  hwndVListView ;
   HWND  hwndLVHeader ;
   HWND  hwndLViewToolTips ;
   HWND  hwndLVHdrToolTips ;
   bool  force_end_of_page ;
   HDC   hdcSelf ;   //  used in conjunction with hfont
   HFONT hfont ;  
   uint  cxClient ;
   uint  cyClient ;
   uint  style_flags ;
   bool  columns_created ;
   int   old_cursor_x ;
   int   old_cursor_y ;
   int   old_hover_column ;

   //  disable assignment and copy operators
   CVListView operator=(const CVListView src) ;
   CVListView(const CVListView&);

protected:
   unsigned cxChar ; //  width of char
   unsigned cyChar ; //  height of char

public:
   // CVListView() ;
   CVListView(HWND hwnd, uint ControlID, HINSTANCE g_hinst, uint lvx0, uint lvy0, uint lvdx, uint lvdy, uint style_flags);
   virtual ~CVListView();
   WNDPROC lview_subclass(LONG TermSubclassProc) ;
   WNDPROC header_subclass(LONG TermSubclassProc);
   void lview_assign_column_headers(void);
   void lview_assign_column_headers(lv_cols_p lv_cols, LPARAM image_list);
   void lview_assign_column_headers(lv_cols_p lv_cols[], LPARAM image_list);
   bool create_tooltips(uint max_width, uint popup_msec, uint stayup_msec);
   void check_header_hover(void);
   void reset_header_hover(void);
   void recalc_dx(lv_cols_p lvptr, uint min_dx);
   void listview_update(uint idx);
   void clear_listview(void);
   void goto_element(uint element_num);
   bool is_lview_hwnd(HWND hwndTarget) const ;
   bool end_of_page_active(void) const
      {  return force_end_of_page ; } ;
   void resize(uint x0, uint y0, uint dx, uint dy);
   void resize(uint dx, uint dy);
   void resize_column(uint dx);
   BOOL SetHeaderSortImage(int  columnIndex, SHOW_ARROW showArrow);
   void hide_horiz_scrollbar(void);
   int  get_next_listview_index(int nCurItem);
   int  HitTest(LVHITTESTINFO *lvhti, LPARAM lParam);
   uint GetItemState(int iItem);
   uint get_selected_count(void);
   void set_header_text(uint idx, char *msg);
   void update_lview_text(uint column, LVCOLUMN *lc);
   void scroll_listview(int scroll_count);
   void set_listview_font(char * szFaceName, int iDeciPtHeight, unsigned iAttributes);
   void update_column(LVITEM *lviptr);
   void insert_column(LVITEM *lviptr);
   void insert_column_header(lv_cols_p lvptr, uint idx);
   void delete_column(uint iCol);
   void find_selected_row(NMHDR* pNMHDR, int *clicked_row, int *clicked_column) ;
   void set_selected_row(int row);
   uint get_lview_dx(void) const 
      { return cxClient ; } ;
   uint get_lview_dy(void) const
      { return cyClient ; } ;
} ;

