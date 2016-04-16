//****************************************************************************
//  Copyright (c) 2008-2012  Daniel D Miller
//  common_win.cpp - common functions for Windows and other programs.
//  These functions will comprise all the functions which require linking
//  comctl32, gdi32, and other non-standard gcc/g++ libraries.
//  
//  This module, which has been entirely compiled from public-domain sources, 
//  is itself declared in the public domain.
//  
//  Collected and Organized by:  Dan Miller
//****************************************************************************

#include <windows.h>
#ifdef UNICODE
#include <tchar.h>
#endif
#include <time.h>
#include <stdio.h>   //  vsprintf
#include <math.h>    //  fabs()
#include <limits.h>
#ifdef _lint
#include <stdlib.h>
#endif
#include <shlobj.h>  //  ShellGetPath()

#include "common.h"
#include "commonw.h"

//lint -e1065 Symbol declared as "C" conflicts with itself

//***********************************************************************
//  from: http://www.codeguru.com/forum/showthread.php?t=128562
//  Vinoj Kumar posted this function on March 30th, 2001, 10:59 AM
//  
//  In the previous post, the poster asked if he had to "clean up" 
//  the LPITEMIDLIST variable. 
//  I did not know the answer.  Is that what you are doing with :
//  HGLOBAL hgMem;
//  hgMem = GlobalHandle(lpil);
//  GlobalFree(hgMem);
// 
//  Answer:  yes, it is
//  
//***********************************************************************
//  requires -lshell32
//***********************************************************************

//***********************************************************************
//lint -esym(714, ShellGetPath)
//lint -esym(759, ShellGetPath)
//lint -esym(765, ShellGetPath)
BOOL ShellGetPath(HANDLE hDlg, char lpszPath[])
{
   BOOL bRet;
   char szPath[MAX_PATH];
   LPITEMIDLIST lpil;
   HGLOBAL hgMem;
   BROWSEINFOA bi;

   bi.hwndOwner = (HWND) hDlg;
   bi.pidlRoot = NULL;
   bi.pszDisplayName = szPath;
   bi.lpszTitle = "Select Folder";
   // bi.ulFlags = BIF_RETURNONLYFSDIRS;
   //  The purpose of including BIF_DONTGOBELOWDOMAIN was to prevent access
   //  to network (i.e., URL) paths, which I thought would break fopen/fgets.
   //  However, those functions work *fine* with URL paths, so I didn't need to worry!!
   // bi.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_NEWDIALOGSTYLE ;
   bi.ulFlags = BIF_NEWDIALOGSTYLE ;
   bi.lpfn = NULL;
   bi.lParam = 0L;
   bi.iImage = 0;
   lpil = SHBrowseForFolderA (&bi);
   if (lpil == NULL)
      return FALSE;
   bRet = SHGetPathFromIDListA (lpil, lpszPath);
   hgMem = GlobalHandle (lpil);
   GlobalFree (hgMem);
   return bRet;
}
//****************************************************************************
//lint -esym(714, copy_buffer_to_clipboard)
//lint -esym(759, copy_buffer_to_clipboard)
//lint -esym(765, copy_buffer_to_clipboard)
void copy_buffer_to_clipboard(char *cbbfr, unsigned blen)
{
   // test to see if we can open the clipboard first before
   // wasting any cycles with the memory allocation
   if (OpenClipboard(NULL)) {
      // Empty the Clipboard. This also has the effect
      // of allowing Windows to free the memory associated
      // with any data that is in the Clipboard
      EmptyClipboard ();

      // Ok. We have the Clipboard locked and it's empty. 
      // Now let's allocate the global memory for our data.

      // Here I'm simply using the GlobalAlloc function to 
      // allocate a block of data equal to the text in the
      // "to clipboard" edit control plus one character for the
      // terminating null character required when sending
      // ANSI text to the Clipboard.
      HGLOBAL hClipboardData = GlobalAlloc (GMEM_DDESHARE, blen + 1);

      // Calling GlobalLock returns to me a pointer to the data
      // associated with the handle returned from GlobalAlloc
      char *pchData = (char *) GlobalLock (hClipboardData);

      // At this point, all I need to do is use the standard 
      // C/C++ strcpy function to copy the data from the local 
      // variable to the global memory.
      strcpy (pchData, cbbfr);

      // Once done, I unlock the memory - remember you 
      // don't call GlobalFree because Windows will free the 
      // memory automatically when EmptyClipboard is next called. 
      GlobalUnlock (hClipboardData);

      // Now, set the Clipboard data by specifying that ANSI text 
      // is being used and passing the handle to the global memory.
      SetClipboardData (CF_TEXT, hClipboardData);

      // Finally, when finished I simply close the Clipboard
      // which has the effect of unlocking it so that other
      // applications can examine or modify its contents.
      CloseClipboard ();
   }
}  //lint !e429

//****************************************************************************
//  find screen coords of upper-left corner of CommPort dialog.
//  From this, hopefully, I can accurately find ctrl_top value.
//****************************************************************************
//lint -esym(714, get_bottom_line)
//lint -esym(759, get_bottom_line)
//lint -esym(765, get_bottom_line)
uint get_bottom_line(HWND hwnd, uint ctrl_id)
{
   POINT pt = { 0, 0 } ;
   ClientToScreen(hwnd, &pt) ;
   // putf("upper-left corner is at %ld,%ld", pt.x, pt.y) ;

   RECT myRect ;
   GetWindowRect(GetDlgItem(hwnd, ctrl_id), &myRect) ;
   // putf("bottom of DeviceDetection box: %u", (uint) myRect.bottom) ;
   return (uint) ((uint) myRect.bottom - (uint) pt.y) ;
}

//****************************************************************************
//lint -esym(714, WriteFileF)
//lint -esym(759, WriteFileF)
//lint -esym(765, WriteFileF)
int WriteFileF(HANDLE hdl, const char *fmt, ...)
{
   DWORD bytesWritten ;
   char consoleBuffer[260];
   va_list al; //lint !e522

   va_start(al, fmt);   //lint !e1055 !e530
   vsprintf(consoleBuffer, fmt, al);   //lint !e64

   WriteFile(hdl, consoleBuffer, strlen(consoleBuffer), &bytesWritten, NULL) ;

   va_end(al);
   return 1;
}

//******************************************************************
//lint -esym(714, Line)
//lint -esym(759, Line)
//lint -esym(765, Line)
void Line(HDC hdc, uint x1, uint yy1, uint x2, uint y2)
{
   MoveToEx(hdc, x1, yy1, NULL) ;
   LineTo(hdc, x2, y2) ;
}

//*****************************************************************************
//  this function reads the edit-control contents into msgstr.
//  However, the returned pointer skips any leading spaces or tabs
//  in the data.  This corrects for my habit of putting leadings spaces
//  in front of edit-control data to improve readability.
//*****************************************************************************
//lint -esym(714, read_edit_control)
//lint -esym(759, read_edit_control)
//lint -esym(765, read_edit_control)
char *read_edit_control(HWND hwnd, char *msgstr)
{
   uint tempEditLength = GetWindowTextLengthA (hwnd);
   GetWindowTextA (hwnd, msgstr, tempEditLength + 1);
   msgstr[tempEditLength] = 0;
   return strip_leading_spaces(msgstr) ;
}

//*****************************************************************************
// static TCHAR szPalFilter[] = 
//    TEXT ("Text Files (*.TXT)\0*.txt\0")  
//    TEXT ("All Files (*.*)\0*.*\0\0") ;
static char const szPalFilter[] = 
   "Text Files (*.TXT)\0*.txt\0"  \
   "All Files (*.*)\0*.*\0\0" ;

//******************************************************************
//lint -esym(714, select_text_file)
//lint -esym(759, select_text_file)
//lint -esym(765, select_text_file)
bool select_text_file(HWND hDlgWnd, char *command_filename)
{
   // syslog("A handles=%d\n", get_handle_count());
   OPENFILENAMEA ofn;       // common dialog box structure
   char szFile[MAX_PATH];       // buffer for file name
   char oldFile[MAX_PATH];       // buffer for file name
   char dirFile[MAX_PATH];       // buffer for file name

   // Initialize OPENFILENAME
   ZeroMemory(&ofn, sizeof(ofn));
   ofn.lStructSize = sizeof(ofn);
   ofn.hwndOwner = hDlgWnd;
   ofn.lpstrFile = szFile;
   //
   // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
   // use the contents of szFile to initialize itself.
   //
   ofn.lpstrFile[0] = '\0';
   strcpy(dirFile, command_filename) ;
   char *strptr = strrchr(dirFile, '\\') ;
   if (strptr != 0) {
      strptr++ ;  //  leave the backslash in place
      *strptr = 0 ;  //  strip off filename
      // OutputDebugStringA(dirFile) ;
   }
   ofn.lpstrInitialDir = dirFile ;
   ofn.nMaxFile = sizeof(szFile);
   ofn.lpstrFilter = szPalFilter ;
   ofn.nFilterIndex = 1;
   ofn.lpstrTitle = "select desired file" ;
   ofn.lpstrFileTitle = NULL ;
//    ofn.lpstrDefExt = TEXT ("txt") ;
   ofn.lpstrDefExt = "txt" ;
   // ofn.nMaxFileTitle = 0;
   // ofn.lpstrInitialDir = NULL;
   ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

   // Display the Open dialog box.
   // syslog("B handles=%d\n", get_handle_count());
   if (GetOpenFileNameA(&ofn)) {
   // syslog("C handles=%d\n", get_handle_count());
      strncpy(oldFile, command_filename, sizeof(oldFile)) ;
      strncpy(command_filename, ofn.lpstrFile, MAX_PATH) ;

      SetFocus(hDlgWnd) ;
      return true;
   }
   // syslog("D handles=%d\n", get_handle_count());
   return false;
}

//******************************************************************
//  Note that 'ext' should be *only* the extension, 
//  no dot, wildcard, or other text
//******************************************************************
// static char szGenFilter[] = 
//    "Text Files (*.TXT)\0*.txt\0"  
//    "All Files (*.*)\0*.*\0\0" ;

//lint -esym(714, select_file)
//lint -esym(759, select_file)
//lint -esym(765, select_file)
bool select_file(HWND hDlgWnd, char *command_filename, char *ext)
{
   char szGenFilter[80] ;
   char *gfptr = szGenFilter ;
   uint slen = 0 ;
   slen += (uint) sprintf(gfptr+slen, "%s Files (*.%s)", ext, ext) ;
   slen++ ; //  leave current NULL-term in place
   slen += (uint) sprintf(gfptr+slen, "*.%s", ext) ;
   slen++ ; //  leave current NULL-term in place
   slen += (uint) sprintf(gfptr+slen, "All Files (*.*)") ;
   slen++ ; //  leave current NULL-term in place
   slen += (uint) sprintf(gfptr+slen, "*.*") ;
   slen++ ; //  leave current NULL-term in place
   *(gfptr+slen) = 0 ;  //  add a terminating NULL-term

   // syslog("A handles=%d\n", get_handle_count());
   OPENFILENAMEA ofn;       // common dialog box structure
   char szFile[PATH_MAX];       // buffer for file name
   char oldFile[PATH_MAX];       // buffer for file name
   char dirFile[PATH_MAX];       // buffer for file name

   // Initialize OPENFILENAME
   ZeroMemory(&ofn, sizeof(ofn));
   ofn.lStructSize = sizeof(ofn);
   ofn.hwndOwner = hDlgWnd;
   ofn.lpstrFile = szFile;
   //
   // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
   // use the contents of szFile to initialize itself.
   //
   ofn.lpstrFile[0] = '\0';
   strcpy(dirFile, command_filename) ;
   char *strptr = strrchr(dirFile, '\\') ;
   if (strptr != 0) {
      strptr++ ;  //  leave the backslash in place
      *strptr = 0 ;  //  strip off filename
      // OutputDebugStringA(dirFile) ;
   }
   ofn.lpstrInitialDir = dirFile ;
   ofn.nMaxFile = sizeof(szFile);
   ofn.lpstrFilter = szGenFilter ;
   ofn.nFilterIndex = 1;
   ofn.lpstrTitle = "select desired file" ;
   ofn.lpstrFileTitle = NULL ;
//    ofn.lpstrDefExt = TEXT ("txt") ;
   ofn.lpstrDefExt = ext ;
   // ofn.nMaxFileTitle = 0;
   // ofn.lpstrInitialDir = NULL;
   ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

   // Display the Open dialog box.
   // syslog("B handles=%d\n", get_handle_count());
   if (GetOpenFileNameA(&ofn)) {
   // syslog("C handles=%d\n", get_handle_count());
      strncpy(oldFile, command_filename, sizeof(oldFile)) ;
      strncpy(command_filename, ofn.lpstrFile, PATH_MAX) ;

      SetFocus(hDlgWnd) ;
      return true;
   }
   // syslog("D handles=%d\n", get_handle_count());
   return false;
}

//*************************************************************************
//lint -esym(714, build_font)
//lint -esym(759, build_font)
//lint -esym(765, build_font)
HFONT build_font(TCHAR *fname, unsigned fheight, unsigned flags)
{
   int fbold      = (flags & EZ_ATTR_BOLD     ) ? TRUE : FALSE ;
   int fitalic    = (flags & EZ_ATTR_ITALIC   ) ? TRUE : FALSE ;
   int funderline = (flags & EZ_ATTR_UNDERLINE) ? TRUE : FALSE ;
   int fstrikeout = (flags & EZ_ATTR_STRIKEOUT) ? TRUE : FALSE ;
   HFONT hfont = CreateFont(
         //  46, 28, 215, 0,  //  height, width, escapement, orientation
         fheight, 0, 0, 0,
         (fbold == FALSE) ? FW_NORMAL : FW_BOLD,
         fitalic,
         funderline,
         fstrikeout,
         DEFAULT_CHARSET, 0, 0, 0, DEFAULT_PITCH, //  other stuff
         fname);
   return hfont;
}

//*************************************************************************
//lint -esym(714, EzCreateFont)
//lint -esym(759, EzCreateFont)
//lint -esym(765, EzCreateFont)
HFONT EzCreateFont(HDC hdc, char * szFaceName, int iDeciPtHeight,
       int iDeciPtWidth, unsigned iAttributes, int textangle, BOOL fLogRes)
{
   FLOAT      cxDpi, cyDpi ;
   HFONT      hFont ;
   LOGFONTA    lf ;
   POINT      pt ;
   TEXTMETRIC txtm ;

   SaveDC (hdc) ;

   SetGraphicsMode (hdc, GM_ADVANCED) ;
   ModifyWorldTransform (hdc, NULL, MWT_IDENTITY) ;
   SetViewportOrgEx (hdc, 0, 0, NULL) ;
   SetWindowOrgEx   (hdc, 0, 0, NULL) ;

   if (fLogRes) {
      cxDpi = (FLOAT) GetDeviceCaps (hdc, LOGPIXELSX) ;
      cyDpi = (FLOAT) GetDeviceCaps (hdc, LOGPIXELSY) ;
   }
   else {
      cxDpi = (FLOAT) (25.4 * GetDeviceCaps (hdc, HORZRES) /
                              GetDeviceCaps (hdc, HORZSIZE)) ;

      cyDpi = (FLOAT) (25.4 * GetDeviceCaps (hdc, VERTRES) /
                              GetDeviceCaps (hdc, VERTSIZE)) ;
   }

   pt.x = (long) (iDeciPtWidth  * cxDpi / 72) ;
   pt.y = (long) (iDeciPtHeight * cyDpi / 72) ;

   DPtoLP (hdc, &pt, 1) ;

   lf.lfHeight         = - (int) (fabs ((double) pt.y) / 10.0 + 0.5) ;
   lf.lfWidth          = 0 ;
   lf.lfEscapement     = textangle ;
   lf.lfOrientation    = textangle ;
   lf.lfWeight         = iAttributes & EZ_ATTR_BOLD      ? 700 : 0 ;
   lf.lfItalic         = iAttributes & EZ_ATTR_ITALIC    ?   1 : 0 ;
   lf.lfUnderline      = iAttributes & EZ_ATTR_UNDERLINE ?   1 : 0 ;
   lf.lfStrikeOut      = iAttributes & EZ_ATTR_STRIKEOUT ?   1 : 0 ;
   lf.lfCharSet        = 0 ;
   lf.lfOutPrecision   = 0 ;
   lf.lfClipPrecision  = 0 ;
   lf.lfQuality        = 0 ;
   lf.lfPitchAndFamily = 0 ;
   strcpy(lf.lfFaceName, szFaceName) ;
   hFont = CreateFontIndirectA(&lf) ;

   if (iDeciPtWidth != 0) {
      hFont = (HFONT) SelectObject (hdc, hFont) ;

      GetTextMetrics (hdc, &txtm) ;

      DeleteObject (SelectObject (hdc, hFont)) ;

      lf.lfWidth = (int) (txtm.tmAveCharWidth *
                          fabs ((double) pt.x) / fabs ((double) pt.y) + 0.5) ;

      hFont = CreateFontIndirectA(&lf) ;
   }
   RestoreDC (hdc, -1) ;
   return hFont ;
}

#ifdef UNICODE
//****************************************************************************
//lint -esym(714, str_unicode_to_ascii)
//lint -esym(759, str_unicode_to_ascii)
//lint -esym(765, str_unicode_to_ascii)
int str_unicode_to_ascii(WCHAR *UnicodeStr, char *AsciiStr, uint AsciiLen)
{
   return WideCharToMultiByte(CP_ACP, 0, UnicodeStr, -1, AsciiStr, AsciiLen, NULL, NULL);
}

//****************************************************************************
//lint -esym(714, str_ascii_to_unicode)
//lint -esym(759, str_ascii_to_unicode)
//lint -esym(765, str_ascii_to_unicode)
int str_ascii_to_unicode(char *AsciiStr, WCHAR *UnicodeStr, uint AsciiLen)
{
   return MultiByteToWideChar(CP_ACP, 0, AsciiStr, -1, UnicodeStr, AsciiLen+1);
}

//****************************************************************************
//lint -esym(714, ascii2unicode)
//lint -esym(759, ascii2unicode)
//lint -esym(765, ascii2unicode)
WCHAR *ascii2unicode(char *AsciiStr, uint AsciiLen)
{
   static WCHAR UnicodeStr[MAX_UNICODE_LEN+1] ;
   if (AsciiLen > MAX_UNICODE_LEN)
       AsciiLen = MAX_UNICODE_LEN ;
   MultiByteToWideChar(CP_ACP, 0, AsciiStr, -1, UnicodeStr, AsciiLen+1);
   return UnicodeStr;
}

//****************************************************************************
//lint -esym(714, ascii2unicode)
//lint -esym(759, ascii2unicode)
//lint -esym(765, ascii2unicode)
WCHAR *ascii2unicode(char *AsciiStr)
{
   uint AsciiLen = strlen(AsciiStr) ;
   static WCHAR UnicodeStr[MAX_UNICODE_LEN+1] ;
   if (AsciiLen > MAX_UNICODE_LEN)
       AsciiLen = MAX_UNICODE_LEN ;
   MultiByteToWideChar(CP_ACP, 0, AsciiStr, -1, UnicodeStr, AsciiLen+1);
   return UnicodeStr;
}

//****************************************************************************
//lint -esym(714, unicode2ascii)
//lint -esym(759, unicode2ascii)
//lint -esym(765, unicode2ascii)
char *unicode2ascii(WCHAR *UnicodeStr)
{
   static char AsciiStr[MAX_UNICODE_LEN+1] ;
   WideCharToMultiByte(CP_ACP, 0, UnicodeStr, -1, AsciiStr, MAX_UNICODE_LEN, NULL, NULL);
   return AsciiStr ;
}
#endif


