#include <windows.h>

#include "resource.h"
#include "common.h"
#include "wizard.h"

static HWND hwndCCdlgStatic1 = 0 ;
static HWND hwndCCdlgStatic2 = 0 ;
static HWND hwndCCdlgStatic3 = 0 ;
static HWND hwndCCdlgStatic4 = 0 ;
static HWND hwndCCdlgStatic5 = 0 ;

static int xpoints = 8 ;

static unsigned costs[4] = { 0, 10, 20, 30 } ;

//****************************************************************************
static void update_statics(void)
{
   char stext[20] ;
   wsprintf(stext, " %u", player.gold) ;
   SetWindowText(hwndCCdlgStatic1, stext) ;
   wsprintf(stext, " %u", xpoints) ;
   SetWindowText(hwndCCdlgStatic2, stext) ;
   wsprintf(stext, " %u", player.str) ;
   SetWindowText(hwndCCdlgStatic3, stext) ;
   wsprintf(stext, " %u", player.dex) ;
   SetWindowText(hwndCCdlgStatic4, stext) ;
   wsprintf(stext, " %u", player.iq) ;
   SetWindowText(hwndCCdlgStatic5, stext) ;
}

//****************************************************************************
static BOOL CALLBACK CreateDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
   // static HWND hCtrlBlock ;
   static int temp, iTemp, iRace, iArmour, iWeapon ;
   char stext[30] ;
   
   switch (message) {
   case WM_INITDIALOG:
      //  initialize player values
      player.gold = 60 ;
      player.race = 0 ;
      player.armour = 0 ;
      player.armour_points = 0 ;
      player.weapon = 0 ;
      player.str = 8 ;
      player.dex = 8 ;
      player.iq  = 8 ;
      player.has_lamp = false ;
      
      iRace   = IDC_HUMAN ;
      iArmour = IDC_ANONE ;
      iWeapon = IDC_WNONE ;
      
      CheckRadioButton (hDlg, IDC_HUMAN, IDC_ELF, iRace) ;
      CheckRadioButton (hDlg, IDC_ANONE, IDC_APLATE, iArmour) ;
      CheckRadioButton (hDlg, IDC_WNONE, IDC_WSWORD, iWeapon) ;
      
      hwndCCdlgStatic1 = GetDlgItem(hDlg, IDC_CCDATA1) ;
      hwndCCdlgStatic2 = GetDlgItem(hDlg, IDC_CCDATA2) ;
      hwndCCdlgStatic3 = GetDlgItem(hDlg, IDC_CCDATA3) ;
      hwndCCdlgStatic4 = GetDlgItem(hDlg, IDC_CCDATA4) ;
      hwndCCdlgStatic5 = GetDlgItem(hDlg, IDC_CCDATA5) ;

      update_statics() ;
      
      SetFocus (GetDlgItem (hDlg, iRace)) ;
      return FALSE ;
        
   case WM_COMMAND:
      switch (LOWORD (wParam)) {
      case IDOK:
         // player.hit_points = player.str ;
         adjust_hit_points() ;
         // player.gold += 5000 ;   //@@@  debug
         EndDialog (hDlg, TRUE) ;
         return TRUE ;
           
      case IDC_HUMAN:
      case IDC_DWARF:
      case IDC_HOBBIT:
      case IDC_ELF:
         iRace = LOWORD (wParam) ;
         CheckRadioButton (hDlg, IDC_HUMAN, IDC_ELF, iRace) ;
         // CheckRadioButton (hDlg, IDC_BLACK, IDC_WHITE, LOWORD (wParam)) ;
         // PaintTheBlock (hCtrlBlock, iColor, iFigure) ;
         player.race = iRace - IDC_HUMAN ;
         switch (player.race) {
         case 0:  //  human
            player.str = 8 ;
            player.dex = 8 ;
            player.iq  = 8 ;
            xpoints = 8 ;
            update_statics() ;
            break;

         case 1:  //  dwarf
            player.str = 10 ;
            player.dex = 8 ;
            player.iq  = 6 ;
            xpoints = 8 ;
            update_statics() ;
            break;

         case 2:  //  hobbit
            player.str = 4 ;
            player.dex = 8 ;
            player.iq  = 12 ;
            xpoints = 8 ;
            update_statics() ;
            break;

         case 3:  //  elf
            player.str = 6 ;
            player.dex = 8 ;
            player.iq  = 10 ;
            xpoints = 8 ;
            update_statics() ;
            break;

         }  //lint !e744
         return TRUE ;
           
      case IDC_ANONE :
      case IDC_ALEATH:
      case IDC_ACHAIN:
      case IDC_APLATE:
         iTemp = LOWORD (wParam) ;
         temp = iTemp - IDC_ANONE ;
         player.gold += costs[player.armour] ;
         if (player.gold < costs[temp]) {
            player.gold -= costs[player.armour] ;
            break;
         } 

         player.gold -= costs[temp] ;
         iArmour = iTemp ;
         CheckRadioButton (hDlg, IDC_ANONE, IDC_APLATE, iArmour) ;
         player.armour = temp ;
         player.armour_points = 7 * temp ;
         update_statics() ;
         return TRUE ;
           
      case IDC_WNONE  :
      case IDC_WDAGGER:
      case IDC_WMACE  :
      case IDC_WSWORD :
         iTemp = LOWORD (wParam) ;
         temp = iTemp - IDC_WNONE ;
         player.gold += costs[player.weapon] ;
         if (player.gold < costs[temp]) {
            player.gold -= costs[player.weapon] ;
            break;
         } 

         player.gold -= costs[temp] ;
         iWeapon = iTemp ;
         CheckRadioButton (hDlg, IDC_WNONE, IDC_WSWORD, iWeapon) ;
         player.weapon = temp ;
         update_statics() ;
         return TRUE ;
           
      case IDC_LAMP:
         if (player.has_lamp) {
            player.gold += 20 ;
            wsprintf(stext, " %u", player.gold) ;
            SetWindowText(hwndCCdlgStatic1, stext) ;
            player.has_lamp = false ;
            SendMessage((HWND) lParam, BM_SETCHECK, 0, 0) ;
         } else if (player.gold >= 20) {
            player.gold -= 20 ;
            wsprintf(stext, " %u", player.gold) ;
            SetWindowText(hwndCCdlgStatic1, stext) ;
            player.has_lamp = true ;
            SendMessage((HWND) lParam, BM_SETCHECK, 1, 0) ;
         }
         return TRUE ;

      case IDC_STRUP:
         if (xpoints > 0  &&  player.str < 18) {
            player.str++ ;
            xpoints-- ;
            update_statics() ;
         }
         return TRUE ;

      case IDC_STRDN:
         if (player.str > 0) {
            player.str-- ;
            xpoints++ ;
            update_statics() ;
         }
         return TRUE ;

      case IDC_DEXUP:
         if (xpoints > 0  &&  player.dex < 18) {
            player.dex++ ;
            xpoints-- ;
            update_statics() ;
         }
         return TRUE ;

      case IDC_DEXDN:
         if (player.dex > 0) {
            player.dex-- ;
            xpoints++ ;
            update_statics() ;
         }
         return TRUE ;

      case IDC_INTUP:
         if (xpoints > 0  &&  player.iq < 18) {
            player.iq++ ;
            xpoints-- ;
            update_statics() ;
         }
         return TRUE ;

      case IDC_INTDN:
         if (player.iq > 0) {
            player.iq-- ;
            xpoints++ ;
            update_statics() ;
         }
         return TRUE ;

      }  //lint !e744
      break ;

   //********************************************************************
   //  application shutdown handlers
   //********************************************************************
   case WM_CLOSE:
      DestroyWindow(hDlg);
      return TRUE ;

   case WM_DESTROY:
      // if (main_timer_id != 0) {
      //    KillTimer(hwnd, main_timer_id) ;
      //    main_timer_id = 0 ;
      // }
      // release_led_images() ;
      set_default_keymap();
      draw_beginning_screen() ;
      // PostQuitMessage(0);
      return TRUE ;

   default:
      break;
   }  //lint !e744
   return FALSE ;
}

//****************************************************************************
void draw_init_screen(HWND hwnd)
{
   // if (DialogBox (g_hinst, TEXT ("CreateBox"), hwnd, CreateDlgProc))
   int result = DialogBox (g_hinst, MAKEINTRESOURCE(IDD_CREATEBOX), NULL, CreateDlgProc) ;
   if (result < 0) {
      syslog("Init: DialogBox: %s\n", get_system_message()) ;
   } 
}

