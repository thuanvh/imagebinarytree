#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

#include "bptsegment.h"
/**
 * @brief segment the image into the flat regions
 */
void flatRegionSegmentation(Image<U8C3> const & src, Image<TLabel>& imageSegment, int& regionNumber) {
  int delta = 0;
  Image<U8C3> srcblur = src;
  //cvtColor(src,srcblur,CV_RGB2HSV);
  //GaussianBlur(srcblur,srcblur,Size(5,5),0.5,2);
  //imshow("blur",srcblur);
  regionNumber = 0;
  int x[_8_NEIGHBORS_] = _8_NEIGHBORS_DX_;
  int y[_8_NEIGHBORS_] = _8_NEIGHBORS_DY_;
  int ysize = src.getSizeY();
  int xsize = src.getSizeX();
  imageSegment.setSize(src.getSize());
  imageSegment.fill(0);

  for (int k = 0; k < 1; k++) {
    for (int j = 0; j < ysize; j++) {
      for (int i = 0; i < xsize; i++) {
        bool found = false;
        int min = MAXINT;
        int newx = i;
        int newy = j;

        for (int v = 0; v < _NEIGHBORS_; v++) {
          newx = x[v] + i;
          newy = y[v] + j;

          if (newx >= 0 && newx<xsize && newy >= 0 && newy < ysize) {
            /*if (src.channels()==1) {
            *                           int val=srcblur.at<uchar>(i,j);
            *                           /// gray image
            *                           int newval=srcblur.at<uchar>(newx,newy);
            *                           if (val==newval) {
            *                               //if (abs(val-newval)<delta) {
            *                               if (imageSegment.at<int>(newx,newy)!=0) {
            *                                   if (min>imageSegment.at<int>(newx,newy)) {
            *                                       min=imageSegment.at<int>(newx,newy);
            }
            found=true;
            //break;
            }
            }
            } else {*/
            U8C3 val = srcblur(i, j);
            /// color image
            U8C3 newval = srcblur(newx, newy);

            if (val[0] == newval[0] && val[1] == newval[1] && val[2] == newval[2]) {
              //if (abs(val[0]-newval[0])+abs(val[1]-newval[1])+abs(val[2]-newval[2])<5) {
              int iseg = imageSegment(newx, newy);

              if (iseg != 0) {
                if (min > iseg) {
                  min = iseg;
                }

                found = true;

                //break;
              }
            }

            //}
          }
        }

        if (!found) {
          regionNumber++;
          imageSegment(i, j) = regionNumber;
        } else {
          //imageSegment.at<int>(i,j)=imageSegment.at<int>(newx,newy);
          imageSegment(i, j) = min;
        }
      }
    }

    for (int i = xsize - 1; i >= 0; i--) {
      for (int j = ysize - 1; j >= 0; j--) {
        bool found = false;
        int min = MAXINT;
        int newx = i;
        int newy = j;

        for (int v = 0; v < _NEIGHBORS_; v++) {
          newx = x[v] + i;
          newy = y[v] + j;

          if (newx >= 0 && newx<xsize && newy >= 0 && newy < ysize) {
            /*if (src.channels()==1) {
            *                           int val=srcblur(i,j);
            *                           /// gray image
            *                           int newval=srcblur.at<uchar>(newx,newy);
            *                           if (val==newval) {
            *                               if (imageSegment.at<int>(newx,newy)!=0) {
            *                                   if (min>imageSegment.at<int>(newx,newy)) {
            *                                       min=imageSegment.at<int>(newx,newy);
            }
            found=true;
            //break;
            }
            }
            } else {*/
            U8C3 val = srcblur(i, j);
            /// color image
            U8C3 newval = srcblur(newx, newy);

            if (val[0] == newval[0] && val[1] == newval[1] && val[2] == newval[2]) {
              int iseg = imageSegment(newx, newy);

              if (iseg != 0) {
                if (min > iseg) {
                  min = iseg;
                }

                found = true;

                //break;
              }
            }

            //}
          }
        }

        if (!found) {
          //regionNumber++;
          //imageSegment(i,j)=regionNumber;
        } else {
          //imageSegment.at<int>(i,j)=imageSegment.at<int>(newx,newy);
          imageSegment(i, j) = min;
        }
      }
    }
  }
}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
