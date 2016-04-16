//****************************************************************************
//  Copyright (c) 2011-2012  Daniel D Miller
//  packet_handler.cpp - encapsulate packet-handler functionality
//
//  This module, which has been entirely compiled from public-domain sources, 
//  is itself declared in the public domain.
//  
//  Written by:   Daniel D. Miller
//****************************************************************************
//  How to use this class
//  This class can be used in two ways, simple and event-driven.
//  
//  simple usage  ************************************************************
//  In this mode, the class is instantiated with the EPL_SIMPLE_LIST flag.
//  The mutex_timeout value is unused, because the mutex is unused.
//  
//  The packet Producer calls put_packet() to put a packet on the list,
//  then uses some external mechanism to notify the packet Consumer that
//  a packet is available on the list.
//  
//  The packet Consumer calls get_packet() to remove a packet from the list.
//  It is up to the Consumer to delete the packet once it is no longer needed.
//  
//  event-driven usage  ******************************************************
//  In this mode, the class is instantiated with the EPL_USE_RX_EVENT flag.
//  A reasonable value for mutex_timeout is 3 (3 seconds).
//  
//  The packet Consumer calls wait_for_packet() to wait for a packet to 
//  become available.  The return value from wait_for_packet() is number of
//  bytes in packet, if the value is > 0.  If return value from wait_for_packet()
//  is negative, it is a Windows error code indicating what went wrong;
//  -ERROR_SEM_TIMEOUT indicates that no packet was received within the 
//  specified timeout period.
//  
//  The packet Producer calls put_packet() to put a packet on the list,
//  but does not need to signal anything to the Consumer; in fact, 
//  it does not even need to know who the consumer is!
//  put_packet() will signal to wait_for_packet() that a packet is 
//  available for removal from the list.
//****************************************************************************
//  NOTES on eth_rx_queueing()
//  This mechanism is not normally needed in any normal application;
//  it was implemented for debugging communications issues external to
//  the class.  The EPL_QUEUEING_DISABLED should be disregarded by all users.
//****************************************************************************

// Info 1712: default constructor not defined for class 'CPacketList'
//lint -esym(1712, CPacketList)

//  config flags
#define  EPL_SIMPLE_LIST         0x00
#define  EPL_QUEUEING_DISABLED   0x01
#define  EPL_USE_RX_EVENT        0x02

#define  MAX_ETH_PACKET          1524
#define  MAX_REPORT_CHARS        260

#define  PH_NO_TIMEOUT           0

//****************************************************************************
typedef struct packet_list_s {
   struct packet_list_s *next ;
   u32 src_ip_addr ;
   uint bfr_len ;
   u8 bfr[MAX_ETH_PACKET] ;
} packet_list_t, *packet_list_p ;

//****************************************************************************

class CPacketList {
private:
   //  packet-list pointers
   packet_list_p rx_pkt_list_top ;    
   packet_list_p rx_pkt_list_tail ;   

   bool eth_rx_queueing_enabled ;
   HANDLE hListAccessMutex ;          
   uint mutex_timeout_secs ;          

   //  optional RxEvent handle
   static bool first_construct ;
   bool rx_event_active ;
   HANDLE hdlRxEvent ;                

   //  user-specified reporting function
   void (*reporting_func)(char *msg, LPVOID private_data) ;
   LPVOID rpt_private_data ;
   void report_func(const char *fmt, ...);

   CPacketList operator=(const CPacketList src) ;
   CPacketList(const CPacketList&);

   //  other local class functions
   int claim_list_mutex(void);
   void release_list_mutex(void);
   int get_packet_data(u8 *rbfr);

public:
   CPacketList(uint mutex_timeout, uint flags) ;
   ~CPacketList() ;
   void put_packet(u8 *rx_bfr, uint bfr_len, u32 sender_ip);
   packet_list_p get_packet(void);
   int  wait_for_packet(u8 *rbfr, uint tmo_secs);
   void reset_event(void);
   void flush_eth_buffers(void);
   void set_reporting_function(void (*rpt_func)(char *msg, LPVOID pdata), LPVOID private_data);
   void set_eth_rx_queueing(bool enabled);
   bool get_eth_rx_queueing(void) const ;
} ;      
   
