#include "GFT.h"

#include <cmath>



/**
 * @brief calculate the generic fourier transformation
 * @param image Image processing
 * @param r Value max of range
 * @param angle Value max of angle
 * @param FD Array of output feature descriptors
 */
void GFT::getGFT(const Image< U8 > & image, int r, int angle, double* FD) {
  // get the centroid of image
  int xc, yc;
  int sizex = image.getSizeX();
  int sizey = image.getSizeY();
  xc = image.getSizeX() / 2;
  yc = image.getSizeY() / 2;

  double maxRadius = sqrt(xc * xc + yc * yc);
  double FR[r][angle];
  double FI[r][angle];
  int rad = 0;
  int ang = 0;

  for (rad = 0; rad < r;rad ++) {
    for (ang = 0; ang < angle; ang++) {
      FR[rad][ang] = FI[rad][ang] = 0;
      //cout<<FR[rad][ang]<<":"<<FI[rad][ang]<<" ";
    }
  }

  // calculate the Polar Fourier Transformation

  for (rad = 0; rad < r;rad ++) {
    for (ang = 0; ang < angle; ang++) {
      //double fr=0;
      //double fi=0;
      for (int x = 0; x < sizex; x++) {
        if (x == xc)
          continue;
        for (int y = 0; y < sizey; y++) {
          double radius = sqrt(pow(x - maxRadius, 2) + pow(y - maxRadius, 2));

          double theta = atan((y - yc) / (double)(x - xc));
          if (theta < 0)
            theta += 2 * M_PI;
          double temp = 2 * M_PI * rad * (radius / maxRadius) + ang * theta;
          //cout<<temp<<":"<<(int)image(x,y)<<" ";
          //cout<<rad<<":"<<ang<<":"<<theta<<":"<<radius<<":"<<maxRadius<<":"<< temp<<":"<<cosf(temp)<<":"<<sinf(temp)<<" ";
          FR[rad][ang] += (int)image(x, y) * cos(temp);
          FI[rad][ang] += (int)image(x, y) * sin(temp);
          //FR[rad][ang] += cosf(temp);
          //FI[rad][ang] += sinf(temp);
          //fr+=cosf(temp);
          //fi+=cosf(temp);
          //if(isnan(fr) || isnan(fi)){
          //  cout<<"ahaaaaaaaaa"<<endl;
//            pause();
          //        }
        }
      }
      //cout<<FR[rad][ang]<<":"<<FI[rad][ang]<<" ";
      //cout<<(float)fr<<":"<<(int)fi<<" ";
    }
  }
  //for (rad = 0; rad < r;rad ++) {
  //   for (ang = 0; ang < angle; ang++) {
  //     cout<<FR[rad][ang]<<":"<<FI[rad][ang]<<" ";
//        }
  //  }

  // calculate the FD
  for (rad = 0; rad < r;rad ++) {
    for (ang = 0; ang < angle; ang++) {
      //rotate and scale normalisation
      if (rad == 0 && ang == 0) {
        FD[0] = sqrt(pow(FR[0][0], 2) + pow(FI[0][0], 2)) / (M_PI * pow(maxRadius, 2));// F00/Area
      } else {
        FD[rad*angle+ang] = sqrt(pow(FR[rad][ang], 2) + pow(FI[rad][ang], 2)) / FD[0];// Fij/F00
      }
    }
  }
  //for (int i=0; i<r*angle; i++)
  //cout<<FD[5]<<" ";

  // Normalize par ecartype
  double sum = 0;
  for (rad = 0; rad < r;rad ++) {
    for (ang = 0; ang < angle; ang++) {
      //rotate and scale normalisation
      sum += FD[rad*angle+ang];
    }
  }
  double avg = sum / (r * angle);
  double ecarttype = 0;
  for (rad = 0; rad < r;rad ++) {
    for (ang = 0; ang < angle; ang++) {
      ecarttype += pow(FD[rad*angle+ang] - avg, 2);
    }
  }
  ecarttype = sqrt(ecarttype / r * angle);
  // normalise
  for (rad = 0; rad < r;rad ++) {
    for (ang = 0; ang < angle; ang++) {
      FD[rad*angle+ang] = (FD[rad*angle+ang] - avg) / ecarttype;
    }
  }

}
/**
 * @brief Calculate the generic fourier transformation from the list of point
 * @param image Image processed
 * @param pointList List of points of a region
 * @param minx The bouding
 * @param miny The bouding
 * @param maxx The bouding
 * @param maxy The bouding
 * @param r Max value of radius
 * @param angle Max value of angle
 * @param FD List of output value
 */
void GFT::getGFT(const Image<U8>& image, std::vector< int* > pointList, int minx, int miny, int maxx, int maxy, int r, int angle, double* FD) {
  // get the centroid of image
  int xc = (maxx - minx) / 2, yc = (maxy - miny) / 2;
  int maxRadius = sqrt(pow(maxy - miny, 2) + pow(maxx - minx, 2));
  double FR[r][angle];
  double FI[r][angle];
  int rad = 0;
  int ang = 0;

  for (rad = 0; rad < r;rad ++) {
    for (ang = 0; ang < angle; ang++) {
      FR[rad][ang] = FI[rad][ang] = 0;
    }
  }

  // calculate the Polar Fourier Transformation
  int pointsize = pointList.size();
  for (rad = 0; rad < r;rad ++) {
    for (ang = 0; ang < angle; ang++) {
      for (int i = 0; i < pointsize; i++) {
        int x = pointList[i][0];
        int y = pointList[i][1];
        if (x == xc)
          continue;
        double radius = sqrt(pow(x - maxRadius, 2) + pow(y - maxRadius, 2));
        double theta = atan((y - yc) / (double)(x - xc));
        if (theta < 0)
          theta += 2 * M_PI;
        double temp = 2 * M_PI * rad * (radius / maxRadius) + ang * theta;
        FR[rad][ang] += image(x, y) * cos(temp);
        FI[rad][ang] += image(x, y) * sin(temp);

      }
    }
  }


  // calculate the FD
  for (rad = 0; rad < r;rad ++) {
    for (ang = 0; ang < angle; ang++) {
      //rotate and scale normalisation
      if (rad == 0 && ang == 0) {
        FD[0] = sqrt(pow(FR[0][0], 2) + pow(FI[0][0], 2)) / (M_PI * pow(maxRadius, 2));// F00/Area
      } else {
        FD[rad*angle+ang] = sqrt(pow(FR[rad][ang], 2) + pow(FI[rad][ang], 2)) / FD[0];// Fij/F00
      }
    }
  }
}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;   replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;
