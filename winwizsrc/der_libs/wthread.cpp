//*****************************************************************************
//  Copyright (c) 2012  Daniel D Miller
//  Thread class for encapsulating Windows thread handling
//
//  This module, which has been entirely compiled from public-domain sources, 
//  is itself declared in the public domain.
//*****************************************************************************

#include <windows.h>

#include "wthread.h"

//*****************************************************************************
CThread::CThread(LPTHREAD_START_ROUTINE thread_func, 
      void *my_private_data, void (*app_close_func)(LPVOID iValue)) :
   hdlThread(NULL),
   ThreadID(0),
   private_data(NULL),
   thread_running(false),
   close_func(NULL)
{
   thread_running = true ;
   close_func = app_close_func ; //  save this for thread destructor
   private_data = my_private_data ;
   hdlThread = CreateThread(NULL, 0, thread_func, (VOID *) my_private_data, 0, &ThreadID);
}

//*****************************************************************************
CThread::~CThread()
{
   thread_running = false ;
   if (close_func != NULL)
       close_func(private_data) ;
   //**********************************************************************************
   //  Well, TerminateThread() *does* terminate the thread.
   //  Note, however, that even if close_func() did not succeed,
   //  the thread still will get terminated...
   //  This *may* result in cleanup code not getting executed.
   //  This issue occurs, for example, if close_func() sent WM_CLOSE to a dialog,
   //  but that dialog was busy processing another message.
   //  
   //  MSDN notes that TerminateThread() is a dangerous function, *specifically*
   //  because it does not allow the thread to do any cleanup.
   //  It can leave the thread memory in an unstable state.
   //**********************************************************************************
   // TerminateThread(hdlThread, 0) ;
   CloseHandle(hdlThread) ;   //  this doesn't halt the thread.
   hdlThread = NULL ;
   private_data = NULL ;
}

