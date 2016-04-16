#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

//*************************************************************
//  Notes stolen from the Linux man page for rand():
//  
// In Numerical Recipes in C: The Art of Scientific Computing 
//  (William H. Press, Brian P. Flannery, Saul A. Teukolsky, 
//   William T. Vetterling; New York: Cambridge University Press,
//   1990 (1st ed, p. 207)), the following comments are made: 
// 
// "If you want to generate a random integer between 1 and 10, 
//  you should always do it by 
// 
//    j=1+(int) (10.0*rand()/(RAND_MAX+1.0)); 
// 
// and never by anything resembling 
// 
//    j=1+((int) (1000000.0*rand()) % 10); 
// 
// (which uses lower-order bits)." 
//*************************************************************
//  return a random number between 0-(Q-1)
//*************************************************************
unsigned random(unsigned Q)
{
   unsigned j = (unsigned) (((long) Q * (long) rand()) / (RAND_MAX+1)); 
   return j;
}         

//*************************************************************
void scramble(unsigned str, unsigned dex, unsigned iq)
{
   unsigned j=0, k=0, l=0, attr_temp = str + dex + iq ;

   j = (1 + random(min(attr_temp, 18))) ;
   attr_temp -= j ;
   if (attr_temp == 0)  goto skipping ;
   k = (1 + random(min(attr_temp, 18))) ;
   attr_temp -= k ;
   if (attr_temp == 0)  goto skipping ;
   l = (1 + random(min(attr_temp, 18))) ;
   attr_temp -= l ;
skipping:
   // printf("init str=%u, dex=%u, int=%u, j=%u, k=%u, l=%u, temp=%u\n", 
   //    str, dex, iq, j, k, l, attr_temp) ;

   //  distribute remaining points evenly across all stats
   while (attr_temp != 0) {
      unsigned touched = 0 ;
      if (j < 18) {
         j++ ;
         touched++ ;
         if (--attr_temp == 0)
            continue;
      }
      if (k < 18) {
         k++ ;
         touched++ ;
         if (--attr_temp == 0)
            continue;
      }
      if (l < 18) {
         l++ ;
         touched++ ;
         if (--attr_temp == 0)
            continue;
      }
      if (!touched) {
         printf("no touch: str=%u, dex=%u, int=%u\n", j, k, l) ;
         break;
      }
   }
   printf("str=%2u, dex=%2u, int=%2u\n", j, k, l) ;
}

//*************************************************************
int main(void)
{
   while (1) {
      scramble(5, 5, 5) ;
      getch() ;
   }
   // return 0;
}
