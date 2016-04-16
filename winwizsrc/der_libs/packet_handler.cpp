//****************************************************************************
//  Copyright (c) 2011-2013  Daniel D Miller
//  packet_handler.cpp - encapsulate packet-handler functionality
//
//  This module, which has been entirely compiled from public-domain sources, 
//  is itself declared in the public domain.
//  
//  Written by:   Daniel D. Miller
//****************************************************************************

#include <windows.h>
// #include <tchar.h>
#include <stdio.h>
#include <time.h> //  time(), used to get unique event names 

#include "common.h"
#include "packet_handler.h"

// Info 1714: Member function 'CPacketList::set_reporting_function(...) not referenced
//lint -esym(1714, CPacketList::set_reporting_function)
//lint -esym(1714, CPacketList::set_eth_rx_queueing, CPacketList::get_eth_rx_queueing)

bool CPacketList::first_construct = false ;

//********************************************************************
void CPacketList::report_func(const char *fmt, ...)
{
   char consoleBuffer[MAX_REPORT_CHARS + 1];
   va_list al; //lint !e522

   va_start(al, fmt);   //lint !e1055 !e530
   vsprintf(consoleBuffer, fmt, al);   //lint !e64

   if (reporting_func == NULL) {
      OutputDebugStringA(consoleBuffer) ;
   } else {
      (*reporting_func)(consoleBuffer, rpt_private_data) ;
   }

   va_end(al);
   // return 1;
}

//****************************************************************************
void CPacketList::set_reporting_function(void (*rpt_func)(char *msg, LPVOID pdata), LPVOID private_data)
{
   reporting_func = rpt_func ;
   rpt_private_data = private_data ;
}

//********************************************************************
//  This mutex is released by:
//    ReleaseMutex(this_port->hEthRxBfrAccessMutex) ;
//********************************************************************
int CPacketList::claim_list_mutex(void)
{
   //  let's time out after a few seconds and give up,
   //  just in case something is wrong...
   //  Later: we've found that when polling is running at a high rate
   //  (as Ron likes to run it), and a user tries to send a command
   //  via the command field, the user command may time out waiting for
   //  polling to release the mutex.  So I'm stretching out this timeout
   //  to see if I can make sure commands successfully get through
   //  even when polling is active.
   // uint wait_seconds = 3 ;
   uint wait_seconds = mutex_timeout_secs ;
   bool done = false ;
   while (!done) {
      wait_seconds-- ;
      DWORD mresult = WaitForSingleObject(hListAccessMutex, 1000) ;
      switch (mresult) {
      case WAIT_TIMEOUT:
         if (wait_seconds == 0) {
            report_func("CPacketList: cannot obtain mutex\n") ;
            return -(int)ERROR_BUSY;   //  used *only* for Mutex-acquisition failure!!
         }
         wait_seconds-- ;
         break;

      case WAIT_FAILED:
         mresult = GetLastError() ;
         report_func("CPacketList: WaitForSingleObject: %s\n", get_system_message(mresult)) ;
         return -(int)mresult;

      case WAIT_ABANDONED:
         report_func("CPacketList: WaitForSingleObject: operation abandoned\n") ;
         return -ERROR_INVALID_FUNCTION;

      default: //  success: mutex was acquired
         done = true ;
         break;
      }  //  end switch
   }
   return 0;
}

//********************************************************************
void CPacketList::release_list_mutex(void)
{
   ReleaseMutex(hListAccessMutex) ;
}

//********************************************************************
CPacketList::CPacketList(uint mutex_timeout, uint flags) :
      rx_pkt_list_top(0),
      rx_pkt_list_tail(0),
      eth_rx_queueing_enabled(false),
      hListAccessMutex(0),
      mutex_timeout_secs(mutex_timeout),
      // first_construct(false),
      rx_event_active(false),
      hdlRxEvent(0),
      reporting_func(NULL),
      rpt_private_data(NULL)
{
   // ZeroMemory((char *) this, sizeof(CPacketList)) ;
   mutex_timeout_secs = mutex_timeout ;

   // wsprintfA(bfr, "EVENT%u", (uint) time(NULL)) ;
   if (!first_construct) {
      first_construct = true ;
      srand2(time(NULL)) ;
      // syslog("CPacketList: init rand2\n") ;
   }

   // rpt_private_data = NULL ;
   // reporting_func = NULL ;
   //  make this default to TRUE
   eth_rx_queueing_enabled = (flags & EPL_QUEUEING_DISABLED) ? false : true ;
   if (flags & EPL_USE_RX_EVENT) {
      rx_event_active = true ;
      char bfr[40] ;
      // wsprintfA(bfr, "EVENT%u", (uint) time(NULL)) ;
      wsprintfA(bfr, "EVENT%u", (uint) rand2()) ;
      // syslog("CPacketList starting [%X] [%s]\n", flags, bfr) ;
      hdlRxEvent = CreateEventA(NULL, false, false, bfr) ;
      if (hdlRxEvent == NULL)
         syslog("CreateEvent: %s\n", get_system_message()) ;
      // else
      //    syslog("hdlRxEvent handle created\n") ;
      
   } 
   hListAccessMutex = CreateMutex(NULL, false, NULL) ;
   if (hListAccessMutex == NULL) {
      report_func("CreateMutex [CPacketList]: %s\n", get_system_message()) ;
   } else {
      //  note: this is not actually an error!
      if (GetLastError() == ERROR_ALREADY_EXISTS) {
         report_func("CreateMutex [CPacketList]:  mutex already exists!!\n") ;
      }
   }
}

//********************************************************************
CPacketList::~CPacketList()
{
   //  delete any pending packets
   packet_list_p rptr = rx_pkt_list_top ;
   rx_pkt_list_top  = NULL ;
   rx_pkt_list_tail = NULL ;
   while (rptr != NULL) {
      packet_list_p rkill = rptr ;
      rptr = rptr->next ;
      delete rkill ;
   }
   //  if RxEvent function is active, close the handle
   // Info 1740: pointer member 'CPacketList::hdlRxEvent' not directly freed or zeroed by destructor
   //    huh... so what's this following code ??
   //lint -esym(1740, CPacketList::hdlRxEvent)
   if (rx_event_active) {
      rx_event_active = false ;
      CloseHandle(hdlRxEvent) ;
      hdlRxEvent = 0 ;
   }

   //  release the mutex
   CloseHandle(hListAccessMutex) ;
   hListAccessMutex = 0 ;
   rpt_private_data = NULL ;
}

//****************************************************************************
void CPacketList::reset_event(void)
{
   if (rx_event_active) {
      ResetEvent(hdlRxEvent) ;
      // syslog("reset_event done on handle %u\n", (uint) hdlRxEvent) ;
   } else {
      syslog("reset_event: rx_event_active is not active\n") ;
   }
}

//****************************************************************************
int CPacketList::get_packet_data(u8 *rbfr)
{
   packet_list_p ptemp = get_packet();
   if (ptemp == NULL) {
      return -ERROR_NO_MORE_FILES ;  //  no packet available
   }
   // syslog("get: %u bytes\n", ptemp->bfr_len) ;
   memcpy(rbfr, ptemp->bfr, ptemp->bfr_len) ;
   int rx_bytes = ptemp->bfr_len ;
   delete ptemp ;
   return rx_bytes;
}

//***************************************************************************
//  this will be needed for normal Ethernet Rx waiting
//***************************************************************************
int CPacketList::wait_for_packet(u8 *rbfr, uint tmo_secs)
{
   int result = 0 ;
   // DWORD dresult ;
   if (!rx_event_active) {
      return -(int) ERROR_INVALID_HANDLE;
   }

   // report_func("wait for eth RX packet [%u]\n", ) ;
   // DWORD rc = WaitForSingleObject(this_port->hdlRxEvent, tmo_secs * 1000) ;
   int done = 0 ;
   while (!done) {
      // if (user_abort_signalled(this_port))
      //    return -(int) ERROR_CANCELLED ;

      // if (dbg_flags & DBG_RX_DEBUG)
      //    syslog("Wait Rx Event\n") ;
      // DWORD rc = WaitForSingleObject(hdlRxEvent, 1000) ;
      DWORD rc = WaitForSingleObject(hdlRxEvent, tmo_secs * 1000) ;
      // if (dbg_flags & DBG_RX_DEBUG)
      //    syslog("Got Rx Event, rc=%u\n", (unsigned) rc) ;
      switch (rc) {
      case WAIT_OBJECT_0:
         //*****************************************************************************
         //  this function has an interesting quirk...
         //  Even though we've received an RxEvent, indicating that the event 
         //  had been signalled, and therefore there should be data on the queue,
         //  get_packet_data() sometimes returns -ERROR_NOT_SAME_DEVICE,
         //  indicating there is NO data on the queue.
         //  Further, it appears to happen after some timeout period...
         //  How could this happen?
         //*****************************************************************************
         // syslog("read comm resp: data ready, hdl=%u\n", (uint) hdlRxEvent) ;
         result = get_packet_data(rbfr) ;  //  pop list into rx_bfr
         // if (result < 0) {
         //    DWORD dresult = GetLastError() ;
         //    syslog("w4so: GLE=%u, R=%d, tsec=%u\n", (uint) dresult, result, tmo_secs) ;
         // }
         done = 1 ;
         break;

      case WAIT_TIMEOUT:
         //  if packet timed out or errored out,
         //  take a pass at processing the partial packet,
         //  just in case we got everything except ETX
            result = -ERROR_SEM_TIMEOUT;
            done = 1 ;
            break;
//          if (tmo_secs == 0) {
//             result = -ERROR_SEM_TIMEOUT;
//             done = 1 ;
//             break;
//          }
//          //  if timeout seconds have *not* counted down to 0, keep waiting
//          if (--tmo_secs == 0) {
//             result = -ERROR_SEM_TIMEOUT;
//             done = 1 ;
//             break;
//          }
//          break;

      // case WAIT_ABANDONED: //  ummm... I don't know...
      default:
         if (rc == WAIT_ABANDONED)
            syslog("read comm resp: what does WAIT_ABANDONED mean??\n") ;
         else
            syslog("read comm resp: WaitForSingleObject [%u]: %d, [%u] %s\n", 
               (uint) hdlRxEvent, rc, (uint) GetLastError(), get_system_message()) ;
         result = -ERROR_INVALID_DATA;
         done = 1 ;
         break;
      }
   }  //  while !done

   return result;
}

//****************************************************************************
void CPacketList::put_packet(u8 *rx_bfr, uint bfr_len, u32 sender_ip)
{
   packet_list_p ptemp = new packet_list_t ;
   ZeroMemory(ptemp, sizeof(packet_list_t)) ;
   memcpy(ptemp->bfr, rx_bfr, bfr_len) ;
   ptemp->src_ip_addr = sender_ip ;
   ptemp->bfr_len = bfr_len ;

   // syslog("push: %u bytes\n", bfr_len) ;
   // hex_dump(rx_bfr, 20) ;

   //  add new packet to list
   int result = claim_list_mutex() ;
   if (result != 0) {
      report_func("PUSH: cannot obtain EthRx mutex: %s\n", show_error(result)) ;
      delete ptemp ;
      return ;
   }
      
   if (rx_pkt_list_top == NULL) {
      rx_pkt_list_top = ptemp ;
   }
   else {
      if (eth_rx_queueing_enabled) {
         rx_pkt_list_tail->next = ptemp ;
      } else {
         packet_list_p pkill = rx_pkt_list_top ;
         rx_pkt_list_top = ptemp ;
         delete pkill ;
      }
   }
   rx_pkt_list_tail = ptemp ;
   
   // syslog("EthRx put: event=%s, bfr\n", result) ;
   if (rx_event_active) {
      // syslog("signalling RX event [%d bytes]\n", bfr_len) ;
      SetEvent(hdlRxEvent) ;
   }
//    SetEvent(this_port->hdlRxEvent) ;   //  who handles the event??
   release_list_mutex();
}

//****************************************************************************
packet_list_p CPacketList::get_packet(void)
{
   int result = claim_list_mutex() ;
   if (result != 0) {
      // if (dbg_flags & DBG_ETHERNET)
         report_func("POP: cannot obtain EthRx mutex: %s\n", show_error(result)) ;
      return NULL;
   }
   packet_list_p ptemp = rx_pkt_list_top ;
   if (ptemp != NULL) {
      rx_pkt_list_top = rx_pkt_list_top->next ;
   }
   release_list_mutex();
   // if (ptemp == NULL) {
   //    report_func("get: did you think some data was available?\n") ;
   // }
   return ptemp;
}

//****************************************************************************
void CPacketList::flush_eth_buffers(void)
{
   uint ucount = 0 ;
   for (packet_list_p ptemp = rx_pkt_list_top;
        ptemp != 0; 
        ptemp = ptemp->next) {
      ucount++ ;
   }

   if (ucount != 0) {
      // if (dbg_flags & DBG_ETHERNET)
      //    infoout(this_port, "flushing RX buffers") ;

      int result = claim_list_mutex() ;
      if (result != 0) {
         report_func("FLUSH: cannot obtain EthRx mutex: %s\n", show_error(result)) ;
         return ;
      }
      packet_list_p ptemp = rx_pkt_list_top ;
      rx_pkt_list_top = NULL ;
      rx_pkt_list_tail = NULL ;
      release_list_mutex();

      while (ptemp != NULL) {
         packet_list_p pkill = ptemp ;
         ptemp = ptemp->next ;
         delete pkill ;
      }
   } 
}

//****************************************************************************
void CPacketList::set_eth_rx_queueing(bool enabled)
{
   eth_rx_queueing_enabled = enabled ;
}

//****************************************************************************
bool CPacketList::get_eth_rx_queueing(void) const
{
   return eth_rx_queueing_enabled ;
}

