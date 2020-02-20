#include <stdlib.h>
#include "convertImage.h"
#include <cmath>

using namespace LibTIM;
/**
 * @brief convert gray to rgb
 */
void cvtGray2RGB(const LibTIM::Image< U8 >& gray, Image< U8C3 >& rgb) {
  int sizex = gray.getSizeX();
  int sizey = gray.getSizeY();

  for (int i = 0; i < sizex; i++) {
    for (int j = 0; j < sizey; j++) {
      U8C3* rgbvar = &rgb(i, j);
      (*rgbvar)[0] = (*rgbvar)[1] = (*rgbvar)[2] = gray(i, j);
    }
  }
}

/**
 * @brief convert rgb to xyz
 */
void cvtRGB2XYZ(const LibTIM::Image<U8C3>& rgb, Image<U8C3>& xyz) {
  float a[3][3] = {0.412453, 0.357580, 0.180423,
      0.212671, 0.715160, 0.072169,
      0.019334, 0.119193, 0.950227
                  };

  int sizex = rgb.getSizeX();
  int sizey = rgb.getSizeY();

  for (int i = 0; i < sizex; i++) {
    for (int j = 0; j < sizey; j++) {
      // convert to xyz
      float x, y, z;
      U8C3 rgbele = rgb(i, j);

      x = a[0][0] * rgbele[0] + a[0][1] * rgbele[1] + a[0][2] * rgbele[2];
      y = a[1][0] * rgbele[0] + a[1][1] * rgbele[1] + a[1][2] * rgbele[2];
      z = a[2][0] * rgbele[0] + a[2][1] * rgbele[1] + a[2][2] * rgbele[2];


      xyz(i, j)[0] = x;
      xyz(i, j)[1] = y;
      xyz(i, j)[2] = z;
    }
  }

}
/*float f(float x) {
  return (x) > 0.008856 ? pow(x, 1 / 3.0f) : 7.787 * (x) + 16.0f / 116.0f;
}*/
/**
 * @brief convert rgb to lab
 */
void cvtRGB2Lab(const LibTIM::Image<U8C3>& rgb, Image<U8C3>& lab) {
  int sizex = rgb.getSizeX();
  int sizey = rgb.getSizeY();

  for (int i = 0; i < sizex; i++) {
    for (int j = 0; j < sizey; j++) {
      // convert to xyz
      double X, Y, Z;
      U8C3 rgbele = rgb(i, j);

      double R = rgbele[0];
      double G = rgbele[1];
      double B = rgbele[2];

      double var_R = (R / 255.f) ;         //R from 0 to 255
      double var_G = (G / 255.f) ;         //G from 0 to 255
      double var_B = (B / 255.f) ;         //B from 0 to 255

      if (var_R > 0.04045)
        var_R = pow(((var_R + 0.055) / 1.055) , 2.4);
      else
        var_R = var_R / 12.92;
      if (var_G > 0.04045)
        var_G = pow(((var_G + 0.055) / 1.055) , 2.4);
      else
        var_G = var_G / 12.92;
      if (var_B > 0.04045)
        var_B = pow(((var_B + 0.055) / 1.055) , 2.4);
      else
        var_B = var_B / 12.92;

      var_R = var_R * 100;
      var_G = var_G * 100;
      var_B = var_B * 100;

      //Observer. = 2°, Illuminant = D65
      X = var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805;
      Y = var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722;
      Z = var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505;


      double ref_X =  95.047;
      double ref_Y = 100.000;
      double ref_Z = 108.883;
      double var_X = X / ref_X   ;       //ref_X =  95.047   Observer= 2°, Illuminant= D65
      double var_Y = Y / ref_Y  ;        //ref_Y = 100.000
      double var_Z = Z / ref_Z    ;      //ref_Z = 108.883

      if (var_X > 0.008856)
        var_X = pow(var_X , (1 / 3.f));
      else
        var_X = (7.787 * var_X) + (16 / 116.f);
      if (var_Y > 0.008856)
        var_Y = pow(var_Y , (1 / 3.f));
      else
        var_Y = (7.787 * var_Y) + (16 / 116.f);
      if (var_Z > 0.008856)
        var_Z = pow(var_Z , (1 / 3.f));
      else
        var_Z = (7.787 * var_Z) + (16 / 116.f);

      double L = (116.f * var_Y) - 16.f;
      double a = 500.f * (var_X - var_Y);
      double b = 200.f * (var_Y - var_Z);

      L *= 255.f / 100.0f;
      a += 128.f;
      b += 128.f;

      L = L < 0 ? 0 : L;
      lab(i, j)[0] = L;
      lab(i, j)[1] = a;
      lab(i, j)[2] = b;
      //std::cout << L << "," << a << "," << b << " ";
      //std::cout << std::endl;
    }
  }
}
/**
 * @brief convert rgb to lab2
 */
void cvtRGB2Lab2(const LibTIM::Image<U8C3>& rgb, Image<U8C3>& lab) {
  float a[3][3] = {0.412453, 0.357580, 0.180423,
      0.212671, 0.715160, 0.072169,
      0.019334, 0.119193, 0.950227
                  };

  int sizex = rgb.getSizeX();
  int sizey = rgb.getSizeY();

  for (int i = 0; i < sizex; i++) {
    for (int j = 0; j < sizey; j++) {
      // convert to xyz
      float x, y, z;
      U8C3 rgbele = rgb(i, j);

      x = a[0][0] * rgbele[0] + a[0][1] * rgbele[1] + a[0][2] * rgbele[2];
      y = a[1][0] * rgbele[0] + a[1][1] * rgbele[1] + a[1][2] * rgbele[2];
      z = a[2][0] * rgbele[0] + a[2][1] * rgbele[1] + a[2][2] * rgbele[2];
      std::cout << "x,y,z:" << x << "," << y << "," << z << " ";
      // convert xyz to lab
      x /= 0.950456;
      z /= 1.088754;

      std::cout << "x,y,z:" << x << "," << y << "," << z << " ";

#define f(x) ((x)>0.008856 ? pow(x,1/3.f) : 7.787 * (x) + 16.f/116.f)
      float l = (y > 0.008856) ? 116.f * pow(y, 1 / 3.0f) - 16.f : 903.3 * y;
#define delta 128
      float a = 500.f * (f(x) - f(y)) + delta;
      float b = 200.f * (f(y) - f(z)) + delta;
      std::cout << l << "," << a << "," << b << " ";
      l *= 255.f / 100.0f;
      a += 128.f;
      b += 128.f;

      lab(i, j)[0] = l;
      lab(i, j)[1] = a;
      lab(i, j)[2] = b;
      std::cout << l << "," << a << "," << b << " ";
      std::cout << std::endl;
    }
  }
}
/**
 * @brief merge 2 image rgb 
 */
void mergeRGB(const LibTIM::Image< U8C3 >& rgb1, const LibTIM::Image< U8C3 >& rgb2, float coef1, float coef2, Image< U8C3 >& rgb3) {
  float c1 = coef1 / (coef1 + coef2);
  float c2 = coef2 / (coef1 + coef2);
  int sizex = rgb1.getSizeX();
  int sizey = rgb1.getSizeY();

  for (int i = 0; i < sizex; i++) {
    for (int j = 0; j < sizey; j++) {
      U8C3* rgb = &rgb3(i, j);
      (*rgb)[0] = (U8)(c1 * rgb1(i, j)[0] + c2 * rgb2(i, j)[0]);
      (*rgb)[1] = (U8)(c1 * rgb1(i, j)[1] + c2 * rgb2(i, j)[1]);
      (*rgb)[2] = (U8)(c1 * rgb1(i, j)[2] + c2 * rgb2(i, j)[2]);
    }
  }
}

