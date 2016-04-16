#include <windows.h>

#include "resource.h"
#include "common.h"
#include "wizard.h"

static unsigned item_prices[4] = { 0, 1250, 1500, 2000 } ;

// static char dbg[128] ;

//****************************************************************************
static void vendor_show_value(HWND hDlg, int vid, unsigned value)
{
   char stext[20] ;
   HWND hwndTemp = GetDlgItem(hDlg, vid) ;
   wsprintf(stext, " %u ", value) ;
   SetWindowText(hwndTemp, stext) ;
}

//****************************************************************************
static void update_equip_avail(HWND hDlg)
{
   unsigned j, idx, tval ;

   //  update armour/weapon entries based on available cash
   for (j=IDC_VANONE, tval=IDC_VWNONE, idx=0; idx <= 3; j++, tval++, idx++) {
      unsigned apoints = idx * 7 ;
      HWND hwndArmour = GetDlgItem(hDlg, j) ;
      HWND hwndWeapon = GetDlgItem(hDlg, tval) ;

      //  debug
      if (item_prices[idx] <= (unsigned) player.gold) {
         if (idx == 0  ||  apoints > player.armour_points) 
            EnableWindow(hwndArmour, TRUE) ;
         else 
            EnableWindow(hwndArmour, FALSE) ;

         if (idx == 0  ||  idx > (unsigned) player.weapon) 
            EnableWindow(hwndWeapon, TRUE) ;
         else
            EnableWindow(hwndWeapon, FALSE) ;
      } else {
         EnableWindow(hwndArmour, FALSE) ;
         EnableWindow(hwndWeapon, FALSE) ;
      }
      // syslog("idx%u: apoints=%u\n", idx, apoints) ;
   }
}

//****************************************************************************
static BOOL CALLBACK VendorDlgProc (
   HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
   HWND hwndChk ;
   // HWND hwndArmour, hwndWeapon ;
   int iTemp ;
   static int str_changed ;
   static int gem_stati[8] ;
   static int weap_stati[4] ;
   static int armor_stati[4] ;
   static unsigned treasure_prices[8] ;
   static unsigned stat_prices[3] ; //  how many attr/1000GP
   static unsigned stat_value[3] ;  //  temp stat value
   static unsigned stat_edits[3] ;  //  how many times has attr been selected
   unsigned j, idx, tval ;
   static int iArmour, iWeapon ;

   switch (message) {
   case WM_INITDIALOG:
      ZeroMemory((char *) &gem_stati[0],   sizeof(gem_stati)) ;
      ZeroMemory((char *) &weap_stati[0],  sizeof(weap_stati)) ;
      ZeroMemory((char *) &armor_stati[0], sizeof(armor_stati)) ;
      ZeroMemory((char *) &stat_edits[0],  sizeof(stat_edits)) ;
      str_changed = 0 ;

      //  show current available gold
      vendor_show_value(hDlg, IDC_VGOLD, player.gold) ;

      //  set up gem-sales display
      for (j=IDC_GEM1, tval=IDC_TGEM1, idx=0; j<=IDC_GEM8; j++, tval++, idx++) {
         hwndChk = GetDlgItem(hDlg, j) ;
         if (player.treasures[idx] == 0) {
            EnableWindow(hwndChk, FALSE) ;
            treasure_prices[idx] = 0 ;
         } else {
            EnableWindow(hwndChk, TRUE) ;
            treasure_prices[idx] = random(1500 * (idx+1)) ;
         }
         vendor_show_value(hDlg, tval, treasure_prices[idx]) ;
      }
      //  debug
      // wsprintf(dbg, "va=%u, hgt=%u, clhgt=%u, go_top=%u\n",
      //    VARMOR_TOP, CR_HEIGHT, CL_HEIGHT, GO_TOP) ;
      // OutputDebugString(dbg) ;

      //  mark initial armour/weapon selections
      iArmour = IDC_VANONE ;
      CheckRadioButton (hDlg, IDC_VANONE, IDC_VAPLATE, iArmour) ;
      iWeapon = IDC_VWNONE ;
      CheckRadioButton (hDlg, IDC_VWNONE, IDC_VWSWORD, iWeapon) ;

      // syslog("player gold=%u, armour=%u/%u, weapon=%u\n",
      //    player.gold, player.armour, 
      //    player.armour_points,
      //    player.weapon) ;

      update_equip_avail(hDlg) ;

      stat_prices[0] = 1+random(6) ;
      stat_value[0] = player.str ;
      vendor_show_value(hDlg, IDC_VSTRCOST, stat_prices[0]) ;
      vendor_show_value(hDlg, IDC_VSTRDATA, stat_value[0]) ;

      stat_prices[1] = 1+random(6) ;
      stat_value[1] = player.dex ;
      vendor_show_value(hDlg, IDC_VDEXCOST, stat_prices[1]) ;
      vendor_show_value(hDlg, IDC_VDEXDATA, stat_value[1]) ;

      stat_prices[2] = 1+random(6) ;
      stat_value[2] = player.iq ;
      vendor_show_value(hDlg, IDC_VINTCOST, stat_prices[2]) ;
      vendor_show_value(hDlg, IDC_VINTDATA, stat_value[2]) ;

      //  move focus to exit button
      iTemp = IDVOK ;
      SetFocus (GetDlgItem (hDlg, iTemp)) ;
      break;
        
   case WM_COMMAND:
      switch (LOWORD (wParam)) {
      case IDVOK:
         if (iWeapon != IDC_VWNONE  &&  player.gold >= item_prices[iWeapon - IDC_VWNONE]) {
            player.weapon = iWeapon - IDC_VWNONE ;
            player.gold -= item_prices[player.weapon] ;
         }
         if (iArmour != IDC_VANONE  &&  player.gold >= item_prices[iArmour - IDC_VANONE]) {
            player.armour = iArmour - IDC_VANONE ;
            player.armour_points = 7 * player.armour ;
            player.gold -= item_prices[player.armour] ;
         }

         if (stat_edits[0] > 0) 
            player.str = min(stat_value[0], 18) ;
         if (str_changed)
            adjust_hit_points() ;
         if (stat_edits[1] > 0) 
            player.dex = min(stat_value[1], 18) ;
         if (stat_edits[2] > 0) 
            player.iq  = min(stat_value[2], 18) ;
         // iCurrentColor  = iRace ;
         // iCurrentFigure = iFigure ;
         EndDialog (hDlg, TRUE) ;
         return TRUE ;
           
      case IDC_SELLGEMS:
         for (idx=0; idx<8; idx++) {
            if (gem_stati[idx] != 0) {
               j = idx + IDC_GEM1 ;
               hwndChk = GetDlgItem(hDlg, j) ;
               SendMessage(hwndChk, BM_SETCHECK, 0, 0) ;
               player.gold += treasure_prices[idx] ;
               player.treasures[idx] = 0 ;
               player.treasure_count-- ;
            }
         }
         SendMessage(hDlg, WM_INITDIALOG, 0, 0) ;
         return TRUE ;
           
      case IDC_GEM1:
      case IDC_GEM2:
      case IDC_GEM3:
      case IDC_GEM4:
      case IDC_GEM5:
      case IDC_GEM6:
      case IDC_GEM7:
      case IDC_GEM8:
         j = LOWORD (wParam) ;
         idx = j - IDC_GEM1 ;
         if (gem_stati[idx] == 0) {
            SendMessage((HWND) lParam, BM_SETCHECK, 1, 0) ;
            gem_stati[idx] = 1 ;
         } else {
            SendMessage((HWND) lParam, BM_SETCHECK, 0, 0) ;
            gem_stati[idx] = 0 ;
         }
         return TRUE ;

      case IDC_VANONE:
      case IDC_VALEATH:
      case IDC_VACHAIN:
      case IDC_VAPLATE:
         iArmour = LOWORD (wParam) ;
         CheckRadioButton (hDlg, IDC_VANONE, IDC_VAPLATE, iArmour) ;
         return TRUE ;

      case IDC_VWNONE:
      case IDC_VWDAGGER:
      case IDC_VWMACE:
      case IDC_VWSWORD:
         iWeapon = LOWORD (wParam) ;
         CheckRadioButton (hDlg, IDC_VWNONE, IDC_VWSWORD, iWeapon) ;
         return TRUE ;

      case IDC_VSTRMORE:
         str_changed++ ;
         if (stat_value[0] < 18  &&  player.gold >= 1000) {
            stat_edits[0]++ ;
            stat_value[0] += stat_prices[0] ;
            //  don't wrap stat here, do it at checkout time.
            //  This way undo is easier.
            // if (stat_value[0] > 18)
            //     stat_value[0] = 18 ;
            player.gold -= 1000 ;
            vendor_show_value(hDlg, IDC_VSTRDATA, min(stat_value[0], 18)) ;
            vendor_show_value(hDlg, IDC_VGOLD, player.gold) ;
            update_equip_avail(hDlg) ;
         }
         return TRUE ;
         
      case IDC_VSTRLESS:
         if (str_changed > 0)
             str_changed-- ;
         if (stat_edits[0] > 0) {
            stat_edits[0]-- ;
            stat_value[0] -= stat_prices[0] ;   //  this doesn't undo wrap correctly
            player.gold += 1000 ;
            vendor_show_value(hDlg, IDC_VSTRDATA, min(stat_value[0], 18)) ;
            vendor_show_value(hDlg, IDC_VGOLD, player.gold) ;
            update_equip_avail(hDlg) ;
         }
         return TRUE ;
         
      case IDC_VDEXMORE:
         if (stat_value[1] < 18  &&  player.gold >= 1000) {
            stat_edits[1]++ ;
            stat_value[1] += stat_prices[1] ;
            //  don't wrap stat here, do it at checkout time.
            //  This way undo is easier.
            // if (stat_value[0] > 18)
            //     stat_value[0] = 18 ;
            player.gold -= 1000 ;
            vendor_show_value(hDlg, IDC_VDEXDATA, min(stat_value[1], 18)) ;
            vendor_show_value(hDlg, IDC_VGOLD, player.gold) ;
            update_equip_avail(hDlg) ;
         }
         return TRUE ;

      case IDC_VDEXLESS:
         if (stat_edits[1] > 0) {
            stat_edits[1]-- ;
            stat_value[1] -= stat_prices[1] ;   //  this doesn't undo wrap correctly
            player.gold += 1000 ;
            vendor_show_value(hDlg, IDC_VDEXDATA, min(stat_value[1], 18)) ;
            vendor_show_value(hDlg, IDC_VGOLD, player.gold) ;
            update_equip_avail(hDlg) ;
         }
         return TRUE ;
         
      case IDC_VINTMORE:
         if (stat_value[2] < 18  &&  player.gold >= 1000) {
            stat_edits[2]++ ;
            stat_value[2] += stat_prices[2] ;
            //  don't wrap stat here, do it at checkout time.
            //  This way undo is easier.
            // if (stat_value[0] > 18)
            //     stat_value[0] = 18 ;
            player.gold -= 1000 ;
            vendor_show_value(hDlg, IDC_VINTDATA, min(stat_value[2], 18)) ;
            vendor_show_value(hDlg, IDC_VGOLD, player.gold) ;
            update_equip_avail(hDlg) ;
         }
         return TRUE ;
         
      case IDC_VINTLESS:
         if (stat_edits[2] > 0) {
            stat_edits[2]-- ;
            stat_value[2] -= stat_prices[2] ;   //  this doesn't undo wrap correctly
            player.gold += 1000 ;
            vendor_show_value(hDlg, IDC_VINTDATA, min(stat_value[2], 18)) ;
            vendor_show_value(hDlg, IDC_VGOLD, player.gold) ;
            update_equip_avail(hDlg) ;
         }
         return TRUE ;
      }  //lint !e744  switch on target checkbox
      break;

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
      update_status();
      // PostQuitMessage(0);
      return TRUE ;

   default:
      break;
   }  //lint !e744
   return FALSE ;
}

//****************************************************************************
int trade_with_vendor(HWND hwnd)
{
   int contents = get_room_contents(player.x, player.y, player.level) ;
   if (contents != VENDOR) {
      put_message("Hmmm... I don't see any shops...") ;
      return 0;
   }

   int result = DialogBox (g_hinst, MAKEINTRESOURCE(IDD_VENDORBOX), hwnd, VendorDlgProc);
   if (result < 0) {
      syslog("Vendor: DialogBox: %s\n", get_system_message()) ;
   } 

   return 0;
}

