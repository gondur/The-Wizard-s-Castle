//*****************************************************************************
//  Copyright (c) 2013  Daniel D Miller
//  class to handle WinAPI progress bar control
//
//  This module, which has been entirely compiled from public-domain sources, 
//  is itself declared in the public domain.
//*****************************************************************************

class CProgBar {
private:   
   HWND hwndProgBar ;

   //  disable assignment and copy operators
   CProgBar operator=(const CProgBar src) ;
   CProgBar(const CProgBar&);
   
public:   
   CProgBar(uint x0, uint y0, uint dx, uint dy, HWND hwndParent, HINSTANCE g_hinst);
   ~CProgBar();
   void set_dimens(uint range, uint step, COLORREF bar_attr);
   void show_bar(bool shown);
   void set_bar(uint bar_value);
   void reset_bar(void);
} ;

