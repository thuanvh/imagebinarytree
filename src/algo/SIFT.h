#ifndef SIFT_H
#define SIFT_H

#include <iostream>
#include <lib/LibTIM/Common/Image.h>
#include <lib/vl/sift.h>
using namespace std;
using namespace LibTIM;

/* From the standard C libaray: */
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

class SIFTPoint{
public:
  static int SIFTSize;

  int x;
  int y;
  int scale;
  int angle;
  double* descriptors;//128 values
  void write(ostream& os,bool includedHeading);
  void load(istream& os,bool includedHeading);
  SIFTPoint();
  ~SIFTPoint();
};

class SIFTAdapter{
public:
  void getSiftPoint(Image<U8> const& img, vector<SIFTPoint*>& siftarray);
  void getSiftDictionary(vector<SIFTPoint*>);
  void loadListSIFTPoint(istream& is, vector<SIFTPoint*> & siftarray,bool includedHeading);
  void getIndexVectorFromDictionary(vector<SIFTPoint*>& dictionary,vector<SIFTPoint*>& siftarray,double* indexvector);
  void matching(std::vector< SIFTPoint* >& dictionary, std::vector< SIFTPoint* >& siftarrayB, double*  indexvector, double threshold);
};

#endif