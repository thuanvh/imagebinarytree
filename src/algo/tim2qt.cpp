#include "tim2qt.h"
/**
 * @brief Change image to qt image
 */
QImage& getQImageFromMat(Image<U8C3> const & mat) {
  int rows = mat.getSizeY();
  int cols = mat.getSizeX();
  uchar imageData[4*rows*cols];// = new unsigned char;
  int pixels = rows * cols;

  for (int i = 0; i < pixels; i++) {
    imageData[i*4+3] = 0xFF;
  }


  Image <U8C3>::const_iterator end = mat.end();

  Image <U8C3>::const_iterator src = mat.begin();
  //Image<U8C3>::const_iterator end = mat.end();
  //    uchar* srcEnd = src + (3*pixels);
  uchar* dest = imageData;

  for (;src != end;src++) {
    U8C3 a = *src;
    dest[0] = a[2];//bleu
    dest[1] = a[1];//green
    dest[2] = a[0];//red
    dest += 4;
  }

  QImage *tImg = new QImage(imageData, cols, rows, QImage::Format_RGB32);

  return *tImg;
}
/**
 * @brief Change image to qt image
 */
QImage& getQImageFromMat(Image<U8> const& mat) {
  int rows = mat.getSizeY();
  int cols = mat.getSizeX();
  uchar imageData[4*rows*cols];// = new unsigned char;
  int pixels = rows * cols;

  for (int i = 0; i < pixels; i++) {
    imageData[i*4+3] = 0xFF;
  }

  Image <U8>::const_iterator end = mat.end();

  Image <U8>::const_iterator src = mat.begin();
  //Image<U8C3>::const_iterator end = mat.end();
  //    uchar* srcEnd = src + (3*pixels);
  uchar* dest = imageData;

  for (;src != end;src++) {
    U8 a = *src;
    dest[0] = a;
    dest[1] = a;
    dest[2] = a;
    dest += 4;
  }

  QImage *tImg = new QImage(imageData, cols, rows, QImage::Format_RGB32);

  return *tImg;
}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
