//********************************************************************
//  Copyright (c) 2014  Daniel D Miller
//  This class encapsulates the interface to the LodePNG library.
//********************************************************************

#include <vector>

//lint -esym(1714, LodePng::render_bitmap, LodePng::horiz_tiles, LodePng::vert_tiles)
//lint -esym(1714, LodePng::LodePng)
//lint -esym(1704, LodePng::LodePng)

class LodePng {
private:   
   std::vector<unsigned char> bmp ;
   std::vector<unsigned char> img ;
   char *img_name ;
   HBITMAP hBitmap ;
   uint sprite_dx ;
   uint sprite_dy ;
   uint tiles_x ;
   uint tiles_y ;

   //  private functions
   void encodeBMP(std::vector<unsigned char>& bmp, const unsigned char* image, int w, int h);
   HBITMAP ConvertDibToHBitmap(void* bmpData);
   HBITMAP load_png_to_bmp(void);

   //  disable the assignment operator and copy constructor
   LodePng &operator=(const LodePng &src) ;
   LodePng(const LodePng&);
   //  disable the default constructor
   LodePng() ;
   
public:   
   // LodePng();
   LodePng(char *new_img_name) ;
   LodePng(char *new_img_name, uint dx, uint dy) ;
   ~LodePng();
   void render_bitmap(HDC hdc, unsigned x, unsigned y);
   // void render_bitmap(HDC hdc, uint xdest, uint ydest, uint xsrc, uint ysrc);
   void render_bitmap(HDC hdc, uint xdest, uint ydest, uint sprite_col, uint sprite_row);
   void render_bitmap(HDC hdc, uint xdest, uint ydest, uint tile_index);
   uint horiz_tiles(void) const
      { return tiles_x ; } ;
   uint vert_tiles(void) const
      { return tiles_y ; } ;
} ;

