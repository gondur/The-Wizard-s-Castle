//*****************************************************************************
//  Copyright (c) 2012  Daniel D Miller
//  Thread class for encapsulating Windows thread handling
//
//  This module, which has been entirely compiled from public-domain sources, 
//  is itself declared in the public domain.
//*****************************************************************************

// wthread.h  21  Info 1712: default constructor not defined for class 'CThread'
//lint -esym(1712, CThread)

//lint -esym(1714, CThread::get_thread_ID, CThread::is_thread_running)

class CThread {
private:   
   HANDLE hdlThread ;
   DWORD ThreadID ;
   VOID *private_data ;
   bool thread_running ;
   void (*close_func)(LPVOID iValue) ;

   CThread operator=(const CThread src) ;
   CThread(const CThread&);
   
public:   
   CThread(LPTHREAD_START_ROUTINE thread_func, void *private_data, void (*app_close_func)(LPVOID iValue));
   ~CThread();
   DWORD get_thread_ID(void) const
      { return ThreadID ; } ;
   bool is_thread_running(void) const
      { return thread_running ; } ;
} ;

