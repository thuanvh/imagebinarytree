#ifndef _CONVERT_IMAGE_H_
#define _CONVERT_IMAGE_H_

#include "../lib/LibTIM/Common/Image.h"
#include <cmath>
#include <limits>
#include <float.h>
using namespace LibTIM;
/// convert rgb to gray
//template<class T>
//void cvtRGB2Gray(Image<U8C3> const & rgb, Image<T>& gray);
/// convert rgb to lab
void cvtRGB2Lab(Image<U8C3> const & rgb, Image<U8C3>& lab);
/// convert gray to rgb
void cvtGray2RGB(Image<U8> const & gray, Image<U8C3>& rgb);
/// merge 2 image rgb by coefficient
void mergeRGB(Image<U8C3> const & rgb1, Image<U8C3> const & rgb2, float coef1, float coef2, Image<U8C3>& rgb3);
/// divide 1 image rgb to 3 image of each channel

//void divide3Chains(Image<U8C3> const & image, Image<T>* imageChains);
/**
 * divide an image by 3 channels
 */
template<class T>
void divide3Chains(Image<U8C3> const & image, Image<T>* imageChains) {
  int sizex = image.getSizeX();
  int sizey = image.getSizeY();

  for (int i = 0; i < sizex; i++) {
    for (int j = 0; j < sizey; j++) {
      U8C3 a = image(i, j);
      for (int k = 0; k < 3; k++) {
        ((imageChains[k]))(i, j) = (T) a[k];
      }
    }
  }
}
template<class T>
void merge3Chains(Image<U8C3> & image, Image<T>* imageChains) {
  int sizex = image.getSizeX();
  int sizey = image.getSizeY();
  //image.fill(0);
  for (int i = 0; i < sizex; i++) {
    for (int j = 0; j < sizey; j++) {
      //U8C3 a = image(i, j);
      for (int k = 0; k < 3; k++) {
        //((imageChains[k]))(i, j) = (T) a[k];
        image(i, j)[k] = (U8)(imageChains[k])(i, j);
      }
    }
  }
}

template<class T>
void scaleImage(Image<T> & imageIn, Image<T> & imageOut, double scale, double translate) {
  int sizex = imageIn.getSizeX();
  int sizey = imageIn.getSizeY();
  for (int i = 0; i < sizex; i++) {
    for (int j = 0; j < sizey; j++) {
      imageOut(i, j) = (T)(imageIn(i, j) * scale + translate);
    }
  }
}

template<class T>
void scaleImageRange(Image<T> & imageIn, Image<T> & imageOut, double maxValue, double minValue) {
  int sizex = imageIn.getSizeX();
  int sizey = imageIn.getSizeY();
  float currentMaxValue = FLT_MIN;
  float currentMinValue = FLT_MAX;
  for (int i = 0; i < sizex; i++) {
    for (int j = 0; j < sizey; j++) {
      currentMaxValue = currentMaxValue > imageIn(i, j) ? currentMaxValue : imageIn(i, j);
      currentMinValue = currentMinValue < imageIn(i, j) ? currentMinValue : imageIn(i, j);
    }
  }
  double scale = (maxValue - minValue) / (currentMaxValue - currentMinValue);
  double translate = -(currentMinValue * scale - minValue);
  for (int i = 0; i < sizex; i++) {
    for (int j = 0; j < sizey; j++) {
      imageOut(i, j) = (T)(imageIn(i, j) * scale + translate);
    }
  }
}
/**
 * convert rgb to gray
 */
template<class T>
void cvtRGB2Gray(const LibTIM::Image< U8C3 >& rgb, Image<T >& gray) {
  int sizex = rgb.getSizeX();
  int sizey = rgb.getSizeY();

  for (int i = 0; i < sizex; i++) {
    for (int j = 0; j < sizey; j++) {
      U8C3 rgbvar = rgb(i, j);
      gray(i, j) = (T)((rgbvar[0] + rgbvar[1] + rgbvar[2]) / (T)3);
      //std::cout<<(int)gray(i,j)<<" ";
    }
  }
}
#endif
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;  replace-tabs on;
