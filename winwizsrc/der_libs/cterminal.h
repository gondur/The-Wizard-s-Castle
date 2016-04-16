//****************************************************************************
//  Copyright (c) 2008-2013  Anacom, Inc
//  AnaComm.exe - a utility for communicating with ODUs and other devices.
//  cterminal.cpp - Terminal class, derived from VListView class
//
//  This module, which has been entirely compiled from public-domain sources, 
//  is itself declared in the public domain.
//
//  Written by:  Dan Miller
//****************************************************************************
//  08/09/12 - I though of modifying CTerminal to inherit from CStrList
//  as well as CVListView, since CTerminal is essentially duplicating the
//  work done by CStrList, but adding color management.  
//  
//  In retrospect, however, this ended up being a disproportionate amount
//  of work, for very little gain.  Keeping track of record positions 
//  between the two classes was a very challenging job.
//  
//  Instead, I may make color management a configuration option for CTerminal,
//  this might result in CStrList becoming redundant.
//****************************************************************************

#include "vlistview.h"

// lint -esym(1719, CVListView, CTerminal)
// lint -esym(1720, CVListView, CTerminal)
// lint -esym(1722, CVListView, CTerminal)
// lint -esym(1704, CVListView::CVListView, CTerminal::CTerminal)

//  if a function needs a longer line than this, they should
//  define their own termout() derivative function(s)
#define  MAX_TERM_CHARS    1024

enum file_type_e {
   FTYP_TEXT=0,
   FTYP_HTML
} ;

//*****************************************************************
//lint -esym(756, attrib_table_t)
typedef struct attrib_table_s {
   COLORREF fgnd ;
   COLORREF bgnd ;
} attrib_table_t ;

//********************************************************************
//  list struct for terminal data list (virtual listview)
//********************************************************************
typedef struct term_lview_item_s {
   struct term_lview_item_s *next ;
   char *msg ;
   uint idx ;
   bool marked ;
   COLORREF fgnd ;
   COLORREF bgnd ;
} term_lview_item_t, *term_lview_item_p ;

// cterminal.h  76  Info 1712: default constructor not defined for class 'CTerminal'
//lint -esym(1712, CTerminal)

// cterminal.h  12  Info 1790: Base class 'CVListView' has no non-destructor virtual functions
//lint -esym(1790, CVListView)

// cterminal.h  43  Warning 1509: base class destructor for class 'CVListView' is not virtual
//lint -esym(1509, CVListView)

//********************************************************************
class CTerminal : public CVListView {
private:   
   //  list pointers for Virtual ListView data list
   term_lview_item_p tlv_top ;
   term_lview_item_p tlv_tail ;

   COLORREF term_fgnd ;
   COLORREF term_bgnd ;

   //  terminal window fields
   unsigned curr_row ;
   unsigned rows ;
   unsigned cols ;

   CTerminal operator=(const CTerminal src) ;
   CTerminal(const CTerminal&);

   //  private functions
   void set_terminal_dimens(void);
   term_lview_item_p get_lview_element(char *lpBuf, COLORREF fgnd, COLORREF bgnd);

public:   
   CTerminal(HWND hwndParent, uint ControlID, HINSTANCE local_g_hinst, 
      uint x0, uint y0, uint dx, uint dy, uint style_flags) ;
   ~CTerminal() ;

   void resize_terminal_window(uint x0, uint y0, uint dx, uint dy);
   void resize_terminal_rows(uint new_rows);
   void resize_terminal_width(uint new_dx);
   void resize_terminal_pixels(uint new_dx, uint new_dy);
   void get_terminal_dimens(uint *array_of_four) const;
   void scroll_terminal(bool scroll_down);
   void clear_message_area(void);
   int  copy_selected_rows(void);
   uint get_term_rows(void) const
      { return rows ; }
   uint get_term_columns(void) const
      { return cols ; }
   uint get_term_char_dy(void) const
      { return cyChar ; }
   uint get_element_count(void) const 
      { return curr_row; } ;
   uint rows2pixels(uint new_rows) const
      { return (new_rows * (cyChar + 1)) ; } ;
   void mark_element(uint idx);
   void clear_marked_elements(void);
   void delete_list(void);
   term_lview_item_p find_element(uint idx);
   int  save_terminal_contents(char *outfile, file_type_e file_type);
   void set_terminal_font(char * szFaceName, int iDeciPtHeight, unsigned iAttributes);
   void set_term_attr(COLORREF fgnd, COLORREF bgnd);
   void set_term_attr_default(void);
   void get_term_attr(COLORREF *prev_fgnd, COLORREF *prev_bgnd) ;
   void reverse_list_entries(void);
   void copy_elements_to_clipboard(void);
   void copy_list_to_clipboard(void);
   LRESULT TerminalCustomDraw (LPARAM lParam) ;
   void get_terminal_entry(LPARAM lParam);
   char *get_last_term_entry(void);
   void put(char *lpBuf);
   void put(char *lpBuf, COLORREF fgnd, COLORREF bgnd);
   void append(char *lpBuf);
   void replace(char *lpBuf);
   //**************************************************************************
   //  we want to split each line into < this_port->cols lengths (or less).
   //  Later note: This function didn't work all that consistently, 
   //  and it isn't that critical in listview dialogs, so I've removed it.
   //**************************************************************************
   int  termout(const char *fmt, ...);
   int  termadd(const char *fmt, ...);
   int  termupdate(const char *fmt, ...);

   WNDPROC terminal_lview_subclass(LONG TermSubclassProc);
} ;

