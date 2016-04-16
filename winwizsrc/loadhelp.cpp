//************************************************************************
//  Copyright (c) 1986-2012  Daniel D Miller
//  winwiz.exe - Wizard's Castle
//  loadhelp.cpp - Load Windows help file
//                         
//  01/06/11 13:22 - separated from main keyboard handler
//************************************************************************
#include <windows.h>
#ifdef _lint
#include <stdlib.h>  //  getenv
#endif
#include <htmlhelp.h>
#include <limits.h>

#include "common.h"
#include "wizard.h"  //  put_message()

//*****************************************************************
static char chmname[PATH_MAX] ;

static int find_chm_location(void)
{
   static char cmd_line_copy[260] ;
   static char curpath[PATH_MAX] ;
   char refstr[4] = { ' ', '\n', '\r', 0 } ;
   char *strptr ;
   // struct stat st ;

   //  see if we can find and show parent filename
   // if (GetModuleFileName(g_hInst, filename, sizeof(filename)) == 0) {
   if (GetModuleFileName(NULL, chmname, sizeof(chmname)) != 0) {
      strptr = strrchr(chmname, '.') ;
      //  if no extention on existing path, just concat the name
      if (strptr == 0) {
         strcat(chmname, ".chm") ;
         // chmmode = 1 ;  //  DEBUG 
      } 
      //  if extension *is* present, overwrite with .chm
      else {
         strcpy(strptr, ".chm") ;
         // chmmode = 2 ;  //  DEBUG 
      }
   } 
   //  if GetModuleFileName fails, then we'll use the old method
   else {
      //  find our chm file.
      //  In some cases (Win98SE), GetCommandLine() returns its
      //  result enclosed in double-quotes!!  We need to deal with that.
      strncpy(cmd_line_copy, GetCommandLine(), sizeof(cmd_line_copy)) ;
      // fprintf(fd, "cmdline:[%s]\n", cmd_line_copy) ;
      if (cmd_line_copy[0] == '"') {
         strcpy(cmd_line_copy, &cmd_line_copy[1]) ;
         // fprintf(fd, "cmdline1:[%s]\n", cmd_line_copy) ;
         strptr = cmd_line_copy ;
         while (1) {
            if (*strptr == '"') {
               strcpy(strptr, strptr+1) ;
               break;
            }
            if (*strptr == 0)
               break;
            strptr++ ;
         }
         // fprintf(fd, "cmdline2:[%s]\n", cmd_line_copy) ;
      }

      //  the results of 
      strptr = strpbrk(cmd_line_copy, refstr) ;
      if (strptr != 0) {
         *strptr = 0 ;
      }
      //  interesting lessons from WinNT 4.0:
      //  If the OS is WinNT 4.0, and;
      //  If the executable file is located in the current directory,
      //  THEN:
      //    argv[0] does NOT contain the fully-qualified
      //    path of the EXE, it *only* contains the EXE name.
      //    In all other situations, argv[0] is fully qualified!!
      //  
      //  P.S.  While we're here, derive default chm filename also
      strptr = strrchr(cmd_line_copy, '\\') ;
      //  no path present
      if (strptr == 0) {
         //*******************************************************************
         //  The original version of this program passed NULL to 
         //  SearchPath(), and failed because the actual search path
         //  was not what was expected:
         //*******************************************************************
         //  Note from MSDN:
         //  
         //  Pointer to a null-terminated string that specifies the path to be 
         //  searched for the file.  If this parameter is NULL, the function 
         //  searches for a matching file in the following directories in the 
         //  following sequence: 
         //  
         //  - The directory from which the application loaded. 
         //    (DDM: I wish I could find this directly!!)
         //  - The current directory. 
         //  - Windows 95: The Windows system directory. 
         //    Use the GetSystemDirectory function to get the path of this dir.
         //  - Windows NT: The 32-bit Windows system directory. 
         //    Use the GetSystemDirectory function to get the path of this 
         //    directory. The name of this directory is SYSTEM32. 
         // 
         //  - Windows NT: The 16-bit Windows system directory. There is no 
         //    Win32 function that obtains the path of this directory, but 
         //    it is searched. The name of this directory is SYSTEM. 
         //  - The Windows directory. Use theGetWindowsDirectory function to get 
         //    the path of this directory. 
         //  - The directories that are listed in the PATH environment variable. 
         //*******************************************************************
         //  Later note:
         //  Note that using getenv("PATH") directly does *not* 
         //  search the current directory!!  
         //  Thus, I'll manually prepend it here...
         //*******************************************************************
         wsprintf(curpath, ".;%s", getenv("PATH")) ;
         SearchPath(curpath, cmd_line_copy, ".exe", sizeof(cmd_line_copy), chmname, NULL) ;
      }
      else {
         //  pick up chm filename
         strcpy(chmname, cmd_line_copy) ;
      }
      strptr = strrchr(chmname, '\\') ;
      if (strptr != 0) {
         char *p ;

         strptr++ ;  //  skip past the backslash
         p = strchr(strptr, '.') ;
         //  if no extention on existing path, just concat the name
         if (p == 0) {
            strcat(chmname, ".chm") ;
            // chmmode = 1 ;  //  DEBUG 
         } 
         //  if extension *is* present, overwrite with .chm
         else {
            strcpy(p, ".chm") ;
            // chmmode = 2 ;  //  DEBUG 
         }
      } else {
         //  this really shouldn't happen
         strcat(chmname, ".chm") ;
         // chmmode = 3 ;  //  DEBUG 
      }
   }

   //  lastly, see if file already exists
   // int result = stat(chmname, &st) ;
   // chm_exists = (result == 0) ? 1 : 0 ;
   return 0;
}

//*************************************************************
void view_help_screen(HWND hwnd)
{
   char msgstr[81] ;
   find_chm_location() ;
   
   wsprintf(msgstr, "help=[%s]", chmname) ;
   put_message(msgstr) ;
   //  MinGw gives a couple of indecipherable warnings about this:
   // Warning: .drectve `-defaultlib:uuid.lib ' unrecognized
   // Warning: .drectve `-defaultlib:uuid.lib ' unrecognized   
   //  But ignoring them doesn't seem to hurt anything...
   HtmlHelp(hwnd, chmname, HH_DISPLAY_TOPIC, 0L);
   return ;
}

