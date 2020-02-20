/**
 * This file is the convert from the method TextonBoost C#
 */
#ifndef CONVOLUTION_H
#define CONVOLUTION_H

#include <lib/LibTIM/Common/Image.h>
#include <cmath>

using namespace std;
using namespace LibTIM;

template<class T>
class SeparableKernel2D;
template<class T>
class LoGKernel2D;
enum ExtendMode { Black, Extend };

template<class T>
class Kernel1D {


public:
  int centre, size;
  T* data;
  Kernel1D() {}
  ~Kernel1D(){
    delete []this->data;
  }
  Kernel1D(int size, int centre) {
    this->size = size;
    this->centre = centre;
    this->data = new T[size];
  }
  /**
   * @brief Save the kernel to a stream
   * @param os Output stream
   * @param k kernel
   */
  static void save(ostream & os, Kernel1D<T>& k) {
    os << k.size << " " << k.centre << " ";
    for (int i = 0; i < k.size; i++) {
      os << k.data[i] << " ";
    }
    os << endl;
  }
  /**
   * @brief Load a kernel from a given stream
   * @param is A given input stream
   */
  static Kernel1D<T>* load(istream& is) {
    int size, centre;
    is >> size;
    is >> centre;
    //cout<<size<< " "<<centre;
    //getchar();
    Kernel1D<T>* a = new Kernel1D<T>(size, centre);
    for (int i = 0; i < size; i++) {
      is >> a->data[i];
    }
    //a->printData();
    //getchar();
    
    return a;
  }

  void printData() {
    for (int i = 0; i < size; i++) {
      cout << this->data[i] << " ";
    }
    cout << endl;
  }
  /**
   * @brief Convolution with the channel Y
   */
  Image<T> ConvolveY(Image<T>& imgIn, ExtendMode extend) {
    Image<T> imgOut(imgIn.getSize());
    ConvolveY(imgOut, imgIn, extend);
    return imgOut;
  }
  /**
   * @brief Convolution with the channel X
   */
  Image<T> ConvolveX(Image<T>& imgIn, ExtendMode extend) {
    Image<T> imgOut(imgIn.getSize());
    ConvolveX(imgOut, imgIn, extend);
    return imgOut;
  }

  /**
   * @brief Create the Gaussian kernel
   */
  static Kernel1D<T>* CreateGaussian(double sigma) {
    int halfSize = (int) ceil(3.0 * sigma);
    Kernel1D<T>* kernel = new Kernel1D<T>(2*halfSize + 1, halfSize);

    double s2 = sigma * sigma;
    double f = 1.0 / sqrt(2.0 * M_PI * s2);
    double w2 = 1.0 / (2.0 * s2);

    for (int i = 0; i < kernel->size; i++) {
      int p = i - kernel->centre;
      kernel->data[i] = (T)(f * exp(-(p * p) * w2));
    }
    cout << "gaussian" << endl;
    kernel->printData();
    return kernel;

  }
  /**
   * @brief Create the DoG kernel
   */
  static Kernel1D<T>* CreateGaussianDerivative(double sigma) {

    int halfSize = (int) ceil(3.0 * sigma);
    Kernel1D<T>* kernel = new Kernel1D<T>(2*halfSize + 1, halfSize);

    double s2 = sigma * sigma;
    double f = 1.0 / sqrt(2.0 * M_PI * s2);
    double w = 1.0 / s2;
    double w2 = 1.0 / (2.0 * s2);

    for (int i = 0; i < kernel->size; i++) {
      int p = i - kernel->centre;
      kernel->data[i] = (T)(-p * w * f * exp(-(p * p) * w2));
    }
    cout << "gaussianDerivate" << endl;
    kernel->printData();
    return kernel;

  }


  /**
   * @brief Create the LoG kernel
   */
  static Kernel1D<T>* CreateGaussian2ndDerivative(double sigma) {

    int halfSize = (int) ceil(3.0 * sigma);
    Kernel1D<T>* kernel = new Kernel1D<T>(2*halfSize + 1, halfSize);

    double s2 = sigma * sigma;
    double f = 1.0 / sqrt(2.0 * M_PI * s2);
    double w = 1.0 / s2;
    double w2 = 1.0 / (2.0 * s2);

    for (int i = 0; i < kernel->size; i++) {
      int p = i - kernel->centre;
      kernel->data[i] = (T)((p * p * w * w - w) * f * exp(-(p * p) * w2));
    }
    cout << "gaussian2ndDerivative" << endl;
    kernel->printData();
    return kernel;

  }

  enum ConvolveDirection { X, Y };
  /**
   * @brief Convolution Y
   */
  void ConvolveY(Image<T>& imgOut, Image<T>& imgIn, ExtendMode extend) {
    Convolve(Y, imgOut, imgIn, extend);
  }
  /**
   * @brief Convolution X
   */
  void ConvolveX(Image<T>& imgOut, Image<T>& imgIn, ExtendMode extend) {
    Convolve(X, imgOut, imgIn, extend);
  }
  /**
   * @brief Convolve x or y
   */
  void Convolve(ConvolveDirection direction, Image<T>& imgOut, Image<T>& imgIn, ExtendMode extend) {
    int sizeX = imgIn.getSizeX();
    int sizeY = imgIn.getSizeY();
    int step = 0;
    switch (direction) {
      case X:
        step = 1;
        for (int x = 0; x < sizeX; x++) {
          for (int y = 0; y < sizeY; y++) {
            float val = 0;
            int startIndex = (x > centre ? 0 : centre - x);
            int endIndex = (x + centre >= sizeX ? size - (x + centre - sizeX) - 1 : size);
            int startx = (x > centre ? x - centre : 0);
            int pos = y * sizeX + startx;
            T* startData = imgIn.getData() + pos;
            for (int i = startIndex; i < endIndex; i++) {
              //cout<<data[i]<<"*"<<(*startData)<<"+";
              val += data[i] * (*startData);
              startData += step;
            }
            //cout<<"="<<val<<endl;
            imgOut(x, y) = val;
          }
          //cout<<endl;
        }
        break;
      case Y:
        step = sizeX;
        for (int x = 0; x < sizeX; x++) {
          for (int y = 0; y < sizeY; y++) {
            float val = 0;
            int startIndex = (y > centre ? 0 : centre - y);
            int endIndex = (y + centre >= sizeY ? size - (y + centre - sizeY) - 1 : size);
            int starty = (y > centre ? y - centre : 0);
            int pos = starty * sizeX + x;
            T* startData = imgIn.getData() + pos;
            for (int i = startIndex; i < endIndex; i++) {
              //cout<<data[i]<<"*"<<(*startData)<<"+";
              val += data[i] * (*startData);
              startData += step;
            }
            //cout<<"="<<val<<endl;
            imgOut(x, y) = val;
          }
          //cout<<endl;
        }
        break;
      default:
        break;
    }
  }
};
/**
 * @brief Class Kernel2D with 2 kernel 1D
 */
template <class T>
class Kernel2D {
public:
  Image<T>* Convolve(Image<T>& imgIn, ExtendMode extend) {
    Image<T>* imgOut = new Image<T>(imgIn.Size, imgIn.Bands);
    Convolve(imgOut, imgIn, extend);
    return imgOut;
  }

  virtual void Convolve(Image<T> & imgOut, Image<T> & imgIn, ExtendMode extend) = 0;

  virtual void save(ostream& os) = 0;
  virtual void load(istream& is) = 0;

  /**
   * @brief Create Gaussian Kernel
   */
  static Kernel2D<T>* CreateGaussian(double sigma) {
    SeparableKernel2D<T>* kernel = new SeparableKernel2D<T>();

    kernel->kernelX = Kernel1D<T>::CreateGaussian(sigma);
    kernel->kernelY = kernel->kernelX;

    return (Kernel2D<T>*)kernel;
  }
  /**
   * @brief Create DoG X
   */
  static Kernel2D<T>* CreateGaussianDerivativeX(double sigma) {
    return CreateGaussianDerivativeX(sigma, sigma);
  }
  /**
   * @brief Create DoG X
   */
  static Kernel2D<T>* CreateGaussianDerivativeX(double sigmaX, double sigmaY) {
    SeparableKernel2D<T>* kernel = new SeparableKernel2D<T>();

    kernel->kernelX = Kernel1D<T>::CreateGaussianDerivative(sigmaX);
    kernel->kernelY = Kernel1D<T>::CreateGaussian(sigmaY);

    return (Kernel2D<T>*)kernel;
  }
  /**
   * @brief Create DoG Y
   */
  static Kernel2D<T>* CreateGaussianDerivativeY(double sigma) {
    return CreateGaussianDerivativeY(sigma, sigma);
  }
  /**
   * @brief Create DoG Y
   */
  static Kernel2D<T>* CreateGaussianDerivativeY(double sigmaX, double sigmaY) {
    SeparableKernel2D<T>* kernel = (SeparableKernel2D<T>*) Kernel2D<T>::CreateGaussianDerivativeX(sigmaY, sigmaX);

    // Swap x and y
    Kernel1D<T>* temp = kernel->kernelX;
    kernel->kernelX = kernel->kernelY;
    kernel->kernelY = temp;

    return (Kernel2D<T>*)kernel;
  }
  /**
   * @brief Create LoG
   */
  static Kernel2D<T>* CreateLaplacian(double sigma) {
    LoGKernel2D<T>* kernel = new LoGKernel2D<T>();

    kernel->kernel2ndDerivative = Kernel1D<T>::CreateGaussian2ndDerivative(sigma);
    kernel->kernelGaussian = Kernel1D<T>::CreateGaussian(sigma);

    return (Kernel2D<T>*)kernel;
  }



};

template<class T>
class SeparableKernel2D : Kernel2D<T> {
public:
  Kernel1D<T>* kernelY, * kernelX;
  
  void Convolve(Image<T>& imgOut, Image<T>& imgIn, ExtendMode extend) {

    Image<T> imgTemp(imgIn.getSize());

    kernelY->ConvolveY(imgTemp, imgIn, extend);
    kernelX->ConvolveX(imgOut, imgTemp, extend);
  }
  /**
   * @brief Save kernel to output stream
   * @param os Output stream
   */
  void save(ostream& os) {
    Kernel1D<T>::save(os, *this->kernelX);
    Kernel1D<T>::save(os, *this->kernelY);
  }
  /**
   * @brief Load kernel from input stream
   * @param is Input stream
   */
  void load(istream& is) {
    kernelX = Kernel1D<T>::load(is);
    kernelY = Kernel1D<T>::load(is);
  }
  SeparableKernel2D(){
    kernelX=kernelY=NULL;
  }
  ~SeparableKernel2D(){
    if(kernelX!=NULL)
      delete kernelX;
    if(kernelY!=NULL)
      delete kernelY;
  }
};

template<class T>
class NonSeparableKernel2D : Kernel2D<T> {
public:
  /**
   * @brief Convolve image
   */
  void Convolve(Image<T>& imgOut, Image<T>& imgIn, ExtendMode extend) {

    throw "The method or operation is not implemented.";
  }
  void save(ostream& os) {

    throw "The method or operation is not implemented.";
  }
  void load(istream& is) {

    throw "The method or operation is not implemented.";
  }
};

template<class T>
class LoGKernel2D : Kernel2D<T> {
public:
  Kernel1D<T> *kernel2ndDerivative, *kernelGaussian;
  LoGKernel2D(){
    kernel2ndDerivative=kernelGaussian=NULL;
  }
  ~LoGKernel2D(){
    if(kernel2ndDerivative!=NULL)
      delete kernel2ndDerivative;
    if(kernelGaussian!=NULL)
      delete kernelGaussian;
  }
  /**
   * @brief Convolve image
   */
  void Convolve(Image<T>& imgOut, Image<T>& imgIn, ExtendMode extend) {

    Image<T> imgTemp(imgIn.getSize());
    Image<T> imgTemp2(imgIn.getSize());

    // Convolve in one direction
    kernel2ndDerivative->ConvolveY(imgTemp, imgIn, extend);
    kernelGaussian->ConvolveX(imgOut, imgTemp, extend);

    // Convolve in other direction
    kernel2ndDerivative->ConvolveX(imgTemp, imgIn, extend);
    kernelGaussian->ConvolveY(imgTemp2, imgTemp, extend);

    // Sum results
    //Image<T>.BinaryOperation("V1 = (T1) (V1+V2);", "", "", imgOut, imgTemp2, null);
    imgOut += imgTemp2;
  }
  /**
   * @brief Save kernel to output stream
   * @param os Output stream
   */
  void save(ostream& os) {
    Kernel1D<T>::save(os, *this->kernel2ndDerivative);
    Kernel1D<T>::save(os, *this->kernelGaussian);
  }
  /**
   * @brief Load kernel from input stream
   * @param is Input stream
   */
  void load(istream& is) {
    kernel2ndDerivative = Kernel1D<T>::load(is);
    kernelGaussian = Kernel1D<T>::load(is);
  }
};



#endif
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;
