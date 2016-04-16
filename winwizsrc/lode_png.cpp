//********************************************************************
//  Copyright (c) 2014  Daniel D Miller
//  This class encapsulates the interface to the LodePNG library.
//********************************************************************

#include <windows.h>
// #include <vector>
#include <string>

#include "common.h"
#include "lode_png.h"
#include "lodepng.h"

//lint -esym(1762, LodePng::encodeBMP, LodePng::ConvertDibToHBitmap)

//*************************************************************************************
//  this function is from example_png2bmp.cpp in the LodePNG example files.
//  
//Input image must be RGB buffer (3 bytes per pixel), but you can easily make it
//support RGBA input and output by changing the inputChannels and/or outputChannels
//in the function to 4.
//*************************************************************************************
void LodePng::encodeBMP(std::vector<unsigned char>& bmp, const unsigned char* image, int w, int h) 
{  //lint !e578
  //3 bytes per pixel used for both input and output.
  int inputChannels = 3;
  int outputChannels = 3;
  
  //bytes 0-13
  bmp.push_back('B'); bmp.push_back('M'); //0: bfType
  bmp.push_back(0); bmp.push_back(0); bmp.push_back(0); bmp.push_back(0); //2: bfSize; size not yet known for now, filled in later.
  bmp.push_back(0); bmp.push_back(0); //6: bfReserved1
  bmp.push_back(0); bmp.push_back(0); //8: bfReserved2
  bmp.push_back(54 % 256); bmp.push_back(54 / 256); bmp.push_back(0); bmp.push_back(0); //lint !e778   10: bfOffBits (54 header bytes)

  //bytes 14-53
  bmp.push_back(40); bmp.push_back(0); bmp.push_back(0); bmp.push_back(0);  //14: biSize
  bmp.push_back(w % 256); bmp.push_back(w / 256); bmp.push_back(0); bmp.push_back(0); //lint !e732  18: biWidth
  bmp.push_back(h % 256); bmp.push_back(h / 256); bmp.push_back(0); bmp.push_back(0); //lint !e732  22: biHeight
  bmp.push_back(1); bmp.push_back(0); //26: biPlanes
  bmp.push_back(outputChannels * 8); bmp.push_back(0); //28: biBitCount
  bmp.push_back(0); bmp.push_back(0); bmp.push_back(0); bmp.push_back(0);  //30: biCompression
  bmp.push_back(0); bmp.push_back(0); bmp.push_back(0); bmp.push_back(0);  //34: biSizeImage
  bmp.push_back(0); bmp.push_back(0); bmp.push_back(0); bmp.push_back(0);  //38: biXPelsPerMeter
  bmp.push_back(0); bmp.push_back(0); bmp.push_back(0); bmp.push_back(0);  //42: biYPelsPerMeter
  bmp.push_back(0); bmp.push_back(0); bmp.push_back(0); bmp.push_back(0);  //46: biClrUsed
  bmp.push_back(0); bmp.push_back(0); bmp.push_back(0); bmp.push_back(0);  //50: biClrImportant
  
  /*
  Convert the input RGBRGBRGB pixel buffer to the BMP pixel buffer format. There are 3 differences with the input buffer:
  -BMP stores the rows inversed, from bottom to top
  -BMP stores the color channels in BGR instead of RGB order
  -BMP requires each row to have a multiple of 4 bytes, so sometimes padding bytes are added between rows
  */

  int imagerowbytes = outputChannels * w;
  imagerowbytes = imagerowbytes % 4 == 0 ? imagerowbytes : imagerowbytes + (4 - imagerowbytes % 4); //must be multiple of 4
  
  for(int y = h - 1; y >= 0; y--) //the rows are stored inversed in bmp
  {
    int c = 0;
    for(int x = 0; x < imagerowbytes; x++)
    {
      if(x < w * outputChannels)
      {
        int inc = c;
        //Convert RGB(A) into BGR(A)
        if(c == 0) inc = 2;
        else if(c == 2) inc = 0;
// During Specific Walk:
//   File winwiz.cpp line 682: encodeBMP(?, [1], ?, ?) #1
// lodepng_mods.cpp  65  Warning 662: Possible creation of out-of-bounds pointer (2 beyond end of data) by operator '[' [Reference: file winwiz.cpp: line 682; file
//     lodepng_mods.cpp: lines 21, 22, 54, 57, 59, 63, 65]
// 
// During Specific Walk:
//   File winwiz.cpp line 682: encodeBMP(?, [1], ?, ?) #1
// lodepng_mods.cpp  65  Warning 661: Possible access of out-of-bounds pointer (2 beyond end of data) by operator '[' [Reference: file winwiz.cpp: line 682; file
//     lodepng_mods.cpp: lines 21, 22, 54, 57, 59, 63, 65]

        bmp.push_back(image[inputChannels * (w * y + x / outputChannels) + inc]);   //lint !e661 !e662
      }
      else bmp.push_back(0);
      c++;
      if(c >= outputChannels) c = 0;
    }
  }

  // Fill in the size
  ul2uc_t uconv ;
  uconv.ul = bmp.size() ;
  bmp[2] = uconv.uc[0] ; //  bmp.size() % 256;
  bmp[3] = uconv.uc[1] ; // (bmp.size() / 256) % 256;
  bmp[4] = uconv.uc[2] ; // (bmp.size() / 65536) % 256;
  bmp[5] = uconv.uc[3] ; //  bmp.size() / 16777216;
}

//***********************************************************************************
// http://stackoverflow.com/questions/15127010/
//          get-a-hbitmap-from-an-24bit-bmp-file-loaded-into-memory
// 
// To create a HBITMAP from file data that has already been loaded into memory, 
// you can do something similar to the following:
//***********************************************************************************
HBITMAP LodePng::ConvertDibToHBitmap(void* bmpData)
{
    HBITMAP hBitmap = NULL;   //lint !e578  Declaration of symbol 'hBitmap' hides symbol 'LodePng::hBitmap' (line 16, file lode_png.h, module winwiz.cpp)
    BOOL success = FALSE;

    //  NOTE: Assumes the BITMAPFILEHEADER is present 
    //        (not normally the case for an in-memory DIB)
    LPBITMAPFILEHEADER bfh = (LPBITMAPFILEHEADER) bmpData;
    LPBITMAPINFOHEADER bih = (LPBITMAPINFOHEADER) (bfh + 1);  //lint !e740  Unusual pointer cast (incompatible indirect types)
    void *pixels = (char*) (bih + 1); // NOTE: Assumes no color table (i.e., bpp >= 24)

    HDC hdc = GetDC(NULL);
    if (hdc != NULL) {
        hBitmap = CreateCompatibleBitmap(hdc, bih->biWidth, bih->biHeight);
        if (hBitmap != NULL) {
            HDC hdcMem = CreateCompatibleDC(hdc);
            if (hdcMem != NULL) {
                HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
                if (StretchDIBits(hdcMem, 0, 0, bih->biWidth, bih->biHeight,
                        0, 0, bih->biWidth, bih->biHeight, pixels,
                        (LPBITMAPINFO) bih, DIB_RGB_COLORS, SRCCOPY) > 0) //lint !e740
                    success = TRUE;

                SelectObject(hdcMem, hOldBitmap);
                DeleteDC(hdcMem);
            }
        }

        ReleaseDC(NULL, hdc);
    }

    if (!success && hBitmap != NULL) {
        DeleteObject(hBitmap);
        hBitmap = NULL;
    }

    return hBitmap;
}

//*************************************************************************************
HBITMAP LodePng::load_png_to_bmp(void)
{
   // static std::vector<unsigned char> image; //the raw pixels
   unsigned width, height, error;
   error = lodepng::decode(img, width, height, img_name, LCT_RGB, 8);
   if (error) {
      // std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
      // syslog("decodeWithState: [%u] %s\n", error, lodepng_error_text(error)) ;
      syslog("decodeWithState: %u\n", error) ;
      return 0;
   }

   // During Specific Walk:
   //   File winwiz.cpp line 678: encodeBMP(?, [1], ?, ?) #1
   // lodepng_mods.cpp  65  Warning 661: Possible access of out-of-bounds pointer 
   //    (2 beyond end of data) by operator '[' [Reference: file winwiz.cpp: line 678; file
   //    lodepng_mods.cpp: lines 21, 22, 54, 57, 59, 63, 65]
   encodeBMP(bmp, &img[0], width, height); //lint !e661 !e713

   if (sprite_dx == 0) {
      sprite_dx = width ;
      tiles_x = 1 ;
      // syslog("set image width to %u\n", width) ;
   } else {
      tiles_x = width  / sprite_dx ;
   }
   if (sprite_dy == 0) {
      sprite_dy = height ;
      tiles_y = 1 ;
      // syslog("set image height to %u\n", height) ;
   } else {
      tiles_y = height / sprite_dy ;
   }

   HBITMAP hdlBitmap = ConvertDibToHBitmap(&bmp[0]);
   if (hdlBitmap == NULL) {
      // wsprintf(tempstr, "LoadImage: %s", get_system_message()) ;
      syslog("ConvertDibToHBitmap: %s\n", get_system_message()) ;
   }
   return hdlBitmap ;
}

//***********************************************************************
//  this form of the renderer is for drawing single images
//***********************************************************************
void LodePng::render_bitmap(HDC hdc, unsigned xdest, unsigned ydest)
{
   HDC hdcMem = CreateCompatibleDC (hdc);
   SelectObject (hdcMem, (HGDIOBJ) hBitmap);

   if (!BitBlt (hdc, xdest, ydest, sprite_dx, sprite_dy, hdcMem, 0, 0, SRCCOPY)) {  //lint !e713
      syslog("BitBlt: %s", get_system_message()) ;
   }

   DeleteDC (hdcMem);
}

//********************************************************************
//  
//********************************************************************
void LodePng::render_bitmap(HDC hdc, uint xdest, uint ydest, uint sprite_col, uint sprite_row)
{
   HDC hdcMem = CreateCompatibleDC (hdc);
   SelectObject (hdcMem, (HGDIOBJ) hBitmap);
   
   uint xsrc = sprite_col * sprite_dx  ;
   uint ysrc = sprite_row * sprite_dy ;
   if (!BitBlt (hdc, xdest, ydest, sprite_dx, sprite_dy, hdcMem, xsrc, ysrc, SRCCOPY)) {  //lint !e713
      syslog("BitBlt: %s", get_system_message()) ;
   }
   DeleteDC (hdcMem);
}

//********************************************************************
void LodePng::render_bitmap(HDC hdc, uint xdest, uint ydest, uint tile_index)
{
   HDC hdcMem = CreateCompatibleDC (hdc);
   SelectObject (hdcMem, (HGDIOBJ) hBitmap);
   uint x_idx = tile_index % tiles_x ;
   uint y_idx = tile_index / tiles_x ;
   uint xsrc = x_idx * sprite_dx ;
   uint ysrc = y_idx * sprite_dy ;
   
   if (!BitBlt (hdc, xdest, ydest, sprite_dx, sprite_dy, hdcMem, xsrc, ysrc, SRCCOPY)) {  //lint !e713
      syslog("BitBlt: %s", get_system_message()) ;
   }
   
   DeleteDC (hdcMem);
}

//********************************************************************
LodePng::LodePng(char *new_img_name) :
   bmp(NULL),
   img(NULL),
   img_name(NULL),
   hBitmap(NULL),
   sprite_dx(0),
   sprite_dy(0),
   tiles_x(1),
   tiles_y(1)
{
   img_name = new char[strlen(new_img_name)+1] ;
   strcpy(img_name, new_img_name) ;
   hBitmap = load_png_to_bmp() ; //  this will init bmp, img
}

//********************************************************************
LodePng::LodePng(char *new_img_name, uint dx, uint dy) :
   bmp(NULL),
   img(NULL),
   img_name(NULL),
   hBitmap(NULL),
   sprite_dx(dx),
   sprite_dy(dy),
   tiles_x(0),
   tiles_y(0)
{
   img_name = new char[strlen(new_img_name)+1] ;
   strcpy(img_name, new_img_name) ;
   hBitmap = load_png_to_bmp() ; //  this will init bmp, img
}

//********************************************************************
LodePng::~LodePng()
{
   if (img_name != NULL)
      delete [] img_name ;
   hBitmap = NULL ;
}

