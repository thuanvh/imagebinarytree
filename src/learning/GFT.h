#ifndef _GFT_H_
#define _GFT_H_
#include "../lib/LibTIM/Common/Image.h" 
using namespace LibTIM;
using namespace std;
class GFT{  
public:
  static void getGFT(const Image< U8 >& image, int r, int angle, double* FD);
  static void getGFT(const Image<U8> & image, std::vector< int* > pointList, int minx, int miny, int maxx, int maxy, int r, int angle, double* FD);
//  static int RValueDefault;
//  static int AngleDefault;
};
#define GFT_RValueDefault 5;
#define GFT_AngleDefault 5;

#endif