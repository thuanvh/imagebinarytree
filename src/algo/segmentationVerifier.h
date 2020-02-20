#ifndef _SEGMENTATION_VERIFIER_H
#define _SEGMENTATION_VERIFIER_H

#include "../lib/LibTIM/Common/Image.h"
#include <iostream>
using namespace LibTIM;
using namespace std;

class segmentationVerifier{
private:
  string fileSegPath;
  Image<TLabel>* sampleSegmentFile;
  Image<TLabel>* segmentImage;
  Image<U8>* contourSampleImage;
  Image<U8>* contourSegmentImage;
  void verify(Image<U8> segmentedImage, Image<U8> sampleImage);
  
  void readSegFile(const char* fileSeg);
public:
  segmentationVerifier();
  ~segmentationVerifier();
  void setSegFilePath(const char* filepath);
  void compaire(Image<TLabel> &image);

  static void regionSeparation(const Image<TLabel>& imageSegment, Image<U8>& imageOut);
  Image<TLabel>* getSampleSegmentImage(){return sampleSegmentFile;}
  Image<U8>* getContourSampleImage(){return contourSampleImage;};
  Image<U8>* getContourSegmentImage(){return contourSegmentImage;};
  static double distanceHausdorff(const Image<U8> img, const Image<U8> img2);

  double getFMeasure();  
  double getPrecision();
  double getRecall();
  double getTrueNegativeRate();
  double getAccuracy();
  double getDistanceHausdorff(double& h,double & hinvert);
  
  double truePositive;
  double falsePositive;
  double trueNegative;
  double falseNegative;
};

#endif