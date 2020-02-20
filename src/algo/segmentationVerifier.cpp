#include "segmentationVerifier.h"
#include "bptsegment.h"
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <algorithm>
#include <math.h>
using namespace std;
/**
 * @brief get value of FMeasure
 */
double segmentationVerifier::getFMeasure() {
  double alpha = 0.5;
  double p = getPrecision();
  double r = getRecall();
  double F = 1 / (alpha / p + (1 - alpha) / r);
  return F;
}
/**
 * @brief get value of recall
 */
double segmentationVerifier::getRecall() {
  return truePositive / (truePositive + falseNegative);
}
/**
 * @brief get value of accuracy
 */
double segmentationVerifier::getAccuracy() {
  return (trueNegative + truePositive) / (trueNegative + truePositive + falseNegative + falsePositive);
}
/**
 * @brief get value of precision
 */
double segmentationVerifier::getPrecision() {
  return truePositive / (truePositive + falsePositive);
}
/**
 * @brief get value of true negative rate
 */
double segmentationVerifier::getTrueNegativeRate() {
  return trueNegative / (trueNegative + falsePositive);
}

/**
 * @brief Segmentation verifier
 */
segmentationVerifier::segmentationVerifier() {
  trueNegative = truePositive = falseNegative = falsePositive = 0;
  segmentImage = NULL;
  sampleSegmentFile = NULL;
  segmentImage = NULL;
  contourSampleImage = NULL;
  contourSegmentImage = NULL;
}


/**
 * @brief verify the value of 2 image
 */
void segmentationVerifier::verify(Image< U8 > segmentedImage, Image< U8 > sampleImage) {
  int ysize = segmentedImage.getSizeY();
  int xsize = segmentedImage.getSizeX();

  for (int i = 0; i < xsize; i++) {
    for (int j = 0; j < ysize; j++) {
      int valseg = segmentedImage(i, j);
      int valsam = sampleImage(i,j);
      if (valseg != 0) {
        if (valsam != 0) {
          truePositive++;
        }else{
          falsePositive++;
        }
      } else {
        if (valsam != 0) {
          falseNegative++;
        }else{
          trueNegative++;
        }
      }
    }
  }
}
/**
 * @brief trim a string
 */
string trim(string str)
{
  string::size_type pos = str.find_last_not_of(' ');
  if(pos != string::npos) {
    str.erase(pos + 1);
    pos = str.find_first_not_of(' ');
    if(pos != string::npos) str.erase(0, pos);
  }
  else str.erase(str.begin(), str.end());
  return str;
}
/*string trim(string str) {
  while (str[0] == ' ') {
    str.erase(str.begin());
  }
  while (str[str.size() - 1] == ' ') {
    str.erase(str.end());
  }
  return str;
}*/
/**
 * @brief Read seg file
 * A seg file sample:
 * 
 * format ascii cr
 * date Fri Mar 9 16:43:30 2001
 * image 33039
 * user 1102
 * width 321
 * height 481
 * segments 12
 * gray 0
 * invert 0
 * flipflop 0
 * data
 */
void segmentationVerifier::readSegFile(const char* fileSeg) {

  char str[2000];
  fstream file_op(fileSeg);

  //SegImage* segimage = new SegImage();
  int width;
  int height;
  int segments;
  bool gray;
  bool invert;
  bool flipflop;
  // read header
  while (!file_op.eof()) {
    file_op.getline(str, 2000);
    string a(str);
    if (a[0] == '#')
      continue;

    int pos = a.find(' ');
    string cmd = a.substr(0, pos);
    string strValue = a.substr(pos + 1);
    strValue = trim(strValue);

    if (cmd.compare("format") == 0) {
      pos = strValue.find(' ');
      string ascii = strValue.substr(0, pos);
      if (ascii.compare("ascii") == 0) {
        strValue = strValue.substr(pos + 1);
        strValue = trim(strValue);
        if (strValue[0] == 'c' && strValue[1] == 'r') {
          continue;
        } else {
          throw "Error: Not seg cr file.";
        }

      } else {
        throw "Error: Not seg ascii file.";
      }
    } else
      if (cmd.compare("date") == 0) {
        continue;
      } else
        if (cmd.compare("image") == 0) {
          continue;
        } else
          if (cmd.compare("user") == 0) {
            continue;
          } else
            if (cmd.compare("width") == 0) {
              pos = strValue.find(' ');
              string strwidth = strValue.substr(0, pos);
              width = atoi(strwidth.c_str());
            } else
              if (cmd.compare("height") == 0) {
                pos = strValue.find(' ');
                string strheight = strValue.substr(0, pos);
                height = atoi(strheight.c_str());
              } else
                if (cmd.compare("segments") == 0) {
                  pos = strValue.find(' ');
                  string strsegments = strValue.substr(0, pos);
                  segments = atoi(strsegments.c_str());
                } else
                  if (cmd.compare("gray") == 0) {
                    pos = strValue.find(' ');
                    string strgray = strValue.substr(0, pos);
                    gray = (strgray[0] == '0') ? false : true;
                  } else
                    if (cmd.compare("invert") == 0) {
                      pos = strValue.find(' ');
                      string strinvert = strValue.substr(0, pos);
                      invert = (strinvert[0] == '0') ? false : true;
                    } else
                      if (cmd.compare("flipflop") == 0) {
                        pos = strValue.find(' ');
                        string strflipflop = strValue.substr(0, pos);
                        flipflop = (strflipflop[0] == '0') ? false : true;
                      } else
                        if (cmd.compare("data") == 0) {
                          break;
                        }

  }
  //
  //segimage->image = cvCreateImage(cvSize(segimage->width, segimage->height), IPL_DEPTH_8U, 1);
  sampleSegmentFile = new Image<TLabel>(width, height);

  // read data
  while (!file_op.eof()) {
    file_op.getline(str, 2000);
    string a(str);
    int pos = a.find(' ');
    string strseg = trim(a.substr(0, pos));
    int seg = atoi(strseg.c_str());

    a = trim(a.substr(pos + 1));
    pos = a.find(' ');
    string strrow = trim(a.substr(0, pos));
    int row = atoi(strrow.c_str());

    a = trim(a.substr(pos + 1));
    pos = a.find(' ');
    string strcol1 = trim(a.substr(0, pos));
    int col1 = atoi(strcol1.c_str());

    a = trim(a.substr(pos + 1));
    pos = a.find(' ');
    string strcol2 = trim(a.substr(0, pos));
    int col2 = atoi(strcol2.c_str());

    int min = col1 < col2 ? col1 : col2;
    int max = col1 > col2 ? col1 : col2;
    for (int i = min; i <= max; i++) {
      //cvSetAt(segimage->image, cvScalar(seg), row, i);
      (*sampleSegmentFile)(i, row) = seg;
    }
  }
  file_op.close();
  //return segimage;
}
/**
 * @brief segmentation verifier
 */
void segmentationVerifier::compaire(Image< TLabel >& image) {
  segmentImage = &image;
  if (contourSegmentImage != NULL)
    delete contourSegmentImage;
  contourSegmentImage = new Image<U8>(segmentImage->getSize());
  contourSegmentImage->fill(0);
  regionSeparation(*segmentImage, *contourSegmentImage);
  verify(*contourSegmentImage, *contourSampleImage);
}
/**
 * @brief Convert a segment image to contour image
 * @param imageSegment A segment image
 * @param imageOut A output contour image
 */
void segmentationVerifier::regionSeparation(Image< TLabel > const & imageSegment, Image<U8>& imageOut) {
  int x[_4_NEIGHBORS_] = _4_NEIGHBORS_DX_;
  int y[_4_NEIGHBORS_] = _4_NEIGHBORS_DY_;
  int ysize = imageSegment.getSizeY();
  int xsize = imageSegment.getSizeX();

  for (int i = 0; i < xsize; i++) {
    for (int j = 0; j < ysize; j++) {
      int idx = imageSegment(i, j);
      for (int v = 0; v < _4_NEIGHBORS_; v++) {
        int newx = i + x[v];
        int newy = j + y[v];
        if (imageSegment.isPosValid(newx,newy)) {
          int iseg = imageSegment(newx, newy);
          if (idx != iseg) {
            int imd = imageOut(newx, newy);
            if (imd == 0) {              
              imageOut(i, j) = 255;
              break;
            }
          }
        }
      }
    }
  }
}
/**
 * @brief Set file path of Seg file
 * @param filepath Seg file path
 */
void segmentationVerifier::setSegFilePath(const char* filepath) {
  fileSegPath = filepath;
  if (sampleSegmentFile != NULL)
    delete sampleSegmentFile;
  readSegFile(filepath);
  if (contourSampleImage != NULL)
    delete contourSampleImage;
  contourSampleImage = new Image<U8>(sampleSegmentFile->getSize());
  contourSampleImage->fill(0);
  regionSeparation(*sampleSegmentFile, *contourSampleImage);
}
/**
 * @brief Destructor of class
 */
segmentationVerifier::~segmentationVerifier() {
  if (sampleSegmentFile != NULL)
    delete sampleSegmentFile;
  if (contourSampleImage != NULL)
    delete contourSampleImage;
}
/**
 * @brief calculate the distance Hausdorff
 */
double segmentationVerifier::distanceHausdorff(const Image<U8> img, const Image<U8> img2){
  int sizex=img.getSizeX();
  int sizey=img.getSizeY();
  int size=sizex<sizey?sizey:sizex;// maxsize
  int dx[_4_NEIGHBORS_]=_4_NEIGHBORS_DX_;
  int dy[_4_NEIGHBORS_]=_4_NEIGHBORS_DY_;
  double distance=0;
  for(int x=0; x<sizex; x++){
    for(int y=0; y<sizey; y++){      
      int val=img(x,y);
      if(val==0) continue;
      //cout<<x<<","<<y<<" : ";
      int s=0;
      bool found=false;
      int px=x,py=y;
      while(s<size){
        int s2=s/2;
        for(int i=0; i<_4_NEIGHBORS_; i++){
          int newx=x + dx[i]*s;
          int newy=y + dy[i]*s;
          for(int d=0; d<=s2; d++){
            for(int k=0; k<2; k++){
              px=newx+d*(k%2?1:-1);
              py=newy+d*(k%2?1:-1);
              
              if(img2.isPosValid(px,py)){
                if(img2(px,py)==val){
                  found=true;
                  //cout<<px<<","<<py<<" ; ";
                  break;
                }
              }              
              if(found){
                break;
              }
            }
            if(found)
              break;
          }
          if(found)
            break;
        }
        if(found)
          break;
        s++;
      }
      distance+=sqrt(pow(px-x,2)+pow(py-y,2));
      //cout<<endl;
    }
  }
  return distance;
}
/**
 * @brief get the distance hausdorff
 */
double segmentationVerifier::getDistanceHausdorff(double& h, double& hinvert)
{
  h=distanceHausdorff(*contourSampleImage,*contourSegmentImage);
  hinvert=distanceHausdorff(*contourSegmentImage,*contourSampleImage);
  return h+hinvert;
}

// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
