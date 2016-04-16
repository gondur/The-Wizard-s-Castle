//*****************************************************************************
//  Copyright (c) 2013  Daniel D Miller
//  This class is released into the public domain.
//  class for handling string list
//*****************************************************************************

#include <windows.h>
#include <stdio.h>

//lint -sem(CStrList::delete_list,cleanup)

//lint -esym(1714, CStrList::add, CStrList::remove, CStrList::str_exists)
//lint -esym(1714, CStrList::get_next, CStrList::write_to_file, CStrList::get_element_count)

#include "common.h"
#include "str_list.h"

//*****************************************************************************
void CStrList::delete_list(void)
{
   cstr_list_p cptr = top;
   top = NULL ;
   tail = NULL ;
   while (cptr != NULL) {
      cstr_list_p skill = cptr ;
      cptr = cptr->next ;
      delete[] skill->sptr ;
      delete skill ;
   }
}

//*****************************************************************************
CStrList::CStrList() :
   top(NULL),
   tail(NULL),
   element_count(0)
{
   
}

//*****************************************************************************
CStrList::~CStrList()
{
   // Warning 1551: Function may throw exception '...' in destructor 'CStrList::~CStrList(void)'
   delete_list(); //lint !e1551
}

//*****************************************************************************
void CStrList::add(char *str)
{
   cstr_list_p cptr = new cstr_list_t ;
   ZeroMemory((char *) cptr, sizeof(cstr_list_t)) ;
   cptr->sptr = new char[strlen(str)+1] ;
   strcpy(cptr->sptr, str) ;
   element_count++ ;

   //  add new list entry to list
   if (top == NULL)
      top = cptr ;
   else
      tail->next = cptr ;  //lint !e613
   tail = cptr ;  //lint !e613
}

//*****************************************************************************
cstr_list_p CStrList::get_next(cstr_list_p curr_element)
{
   if (curr_element == NULL) 
      return top;
   return curr_element->next ;
}

//*****************************************************************************
bool CStrList::remove(char *del_str)
{
   if (top == NULL)
      return false;
   bool elemented_deleted = false ;
   cstr_list_p cptr  ;
   //  see if target item is at start of list
   if (strcasecmp(del_str, top->sptr) == 0) {
      cptr = top ;
      top = top->next ;
      delete cptr->sptr ;
      delete cptr ;
      elemented_deleted = true ;
   } else {
      cptr = top ;
      cstr_list_p tptr = cptr->next ;
      while (LOOP_FOREVER) {
         if (tptr == NULL)
            break;
         if (strcasecmp(del_str, tptr->sptr) == 0) {
            cptr->next = tptr->next ;  //  point past the tptr element
            delete tptr->sptr ;
            delete tptr ;
            elemented_deleted = true ;
            break;
         }
         cptr = tptr ;
         tptr = tptr->next ;
      }
   }
   if (elemented_deleted) {
      element_count-- ;
   }
   return elemented_deleted ;
}

//*****************************************************************************
uint CStrList::write_to_file(FILE *fd)
{
   uint ecount = 0 ;
   cstr_list_p cptr = top;
   while (cptr != NULL) {
      fprintf(fd, "%s\n", cptr->sptr) ;
      ecount++ ;
      cptr = cptr->next ;
   }
   return ecount ;
}

//****************************************************************************
bool CStrList::str_exists(char *cbentry)
{
   cstr_list_p cptr  ;
   for (cptr=top; cptr != 0; cptr = cptr->next) {
      if (strcmp(cbentry, cptr->sptr) == 0)
         return true;
   }
   return false;
}

