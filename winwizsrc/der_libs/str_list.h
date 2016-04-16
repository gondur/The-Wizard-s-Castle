//*****************************************************************************
//  Copyright (c) 2012  Daniel D Miller
//  This class is released into the public domain.
//  class for handling string list
//*****************************************************************************

//***********************************************************************
//  list-of-strings struct
//***********************************************************************
typedef struct cstr_list_s {
   struct cstr_list_s *next ;
   char *sptr ;
} cstr_list_t, *cstr_list_p ;

class CStrList {
private:   
   cstr_list_p top ;
   cstr_list_p tail ;
   uint element_count ;

   //  bypass the assignment operator and copy constructor
   CStrList operator=(const CStrList src) ;
   CStrList(const CStrList&);
   
public:   
   CStrList();
   ~CStrList();
   void add(char *str);
   bool remove(char *del_str);
   void delete_list(void);
   bool str_exists(char *cbentry);
   cstr_list_p get_next(cstr_list_p curr_element);
   uint write_to_file(FILE *fd);
   uint get_element_count(void) const
      { return element_count ; }
} ;

