#include "../lib/LibTIM/Common/Image.h"
#include "../algo/convertImage.h"
#include <math.h>

double getRawMoment(Image<U8> const & img, int i, int j) {
  int sizex = img.getSizeX();
  int sizey = img.getSizeY();
  double m = 0;

  for (int x = 0; x < sizex; x++) {
    for (int y = 0; y < sizey; y++) {
      m += pow(x, i) * pow(y, j) * img(x, y);
    }
  }

  return m;
}

double getNij(double uij, double u00, int i, int j) {
  return uij / pow(u00, 1 + (i + j) / 2);
}

void getHuMoment(Image<U8> const & img, double * HuMoment) {

  double xavg, yavg;
  double M00 = getRawMoment(img, 0, 0);
  double M01 = getRawMoment(img, 0, 1);
  double M10 = getRawMoment(img, 1, 0);
  double M11 = getRawMoment(img, 1, 1);
  double M20 = getRawMoment(img, 2, 0);
  double M02 = getRawMoment(img, 0, 2);
  double M21 = getRawMoment(img, 2, 1);
  double M12 = getRawMoment(img, 1, 2);
  double M30 = getRawMoment(img, 3, 0);
  double M03 = getRawMoment(img, 0, 3);

  xavg = M10 / M00;
  yavg = M01 / M00;

  double u00 = M00;
  double u01 = 0;
  double u10 = 0;
  double u11 = M11 - xavg * M01;
  double u20 = M20 - xavg * M10;
  double u02 = M02 - yavg * M01;
  double u21 = M21 - 2 * xavg * M11 - yavg * M20 + 2 * pow(xavg, 2) * M01;
  double u12 = M12 - 2 * yavg * M11 - xavg * M02 + 2 * pow(yavg, 2) * M10;
  double u30 = M30 - 3 * xavg * M20 + 2 * pow(xavg, 2) * M10;
  double u03 = M03 - 3 * yavg * M02 + 2 * pow(yavg, 2) * M01;

  double n20 = getNij(u20, u00, 2, 0);
  double n02 = getNij(u02, u00, 0, 2);
  double n11 = getNij(u11, u00, 1, 1);
  double n30 = getNij(u30, u00, 3, 0);
  double n12 = getNij(u12, u00, 1, 2);
  double n21 = getNij(u21, u00, 2, 1);
  double n03 = getNij(u03, u00, 0, 3);


  double I1 = n20 + n02;
  double I2 = pow(n20 - n02, 2) + pow(2 * n11, 2);
  double I3 = pow(n30 - 3 * n12, 2) + pow(3 * n21 - n03, 2);
  double I4 = pow(n30 + n12, 2) + pow(n21 + n03, 2);
  double I5 = (n30 - 3 * n12) * (n30 + n12) * (pow(n30 + n12, 2) - 3 * pow(n21 + n03, 2)) + (3 * n21 - n03) * (n21 + n03) * (3 * pow(n30 + n12, 2) - pow(n21 + n03, 2));
  double I6 = (n20 - n20) * (pow(n30 + n12, 2) - pow(n21 + n03, 2)) + 4 * n11 * (n30 + n12) * (n21 + n03);
  double I7 = (3 * n21 - n03) * (n30 + n12) * (pow(n30 + n12, 2) - 3 * pow(n21 + n03, 2)) - (n30 - 3 * n12) * (n21 + n03) * (3 * pow(n30 + n12, 2) - pow(n21 + n03, 2));
  double I8 = n11 * (pow(n30 + n12, 2) - pow(n03 + n21, 2)) - (n20 - n02) * (n30 + n12) * (n03 + n21);

  HuMoment[0] = I1;
  HuMoment[1] = I2;
  HuMoment[2] = I3;
  HuMoment[3] = I4;
  HuMoment[4] = I5;
  HuMoment[5] = I6;
  HuMoment[6] = I7;
  HuMoment[7] = I8;
}

/*
 * Extraire des valeurs de HuMoment pour chaque image
 */
/*void extractHuMomentImage(const Mat& img, ostream& output) {
  //  cout << "convert gray" << endl;
  Mat srcgray;
  cvtColor(img, srcgray, CV_RGB2GRAY);

  //  Find contour
  Mat dst;
  //  Canny(srcgray,dst,150, 180);
  Laplacian(srcgray, dst, 5);
  Mat imgerode;
  uchar a[9] = {0, 1, 0,
  1, 1, 1,
  0, 1, 0};
  Mat mat4c(3, 3, CV_8UC1, a);
  erode(dst, imgerode, mat4c, Point(-1, -1), 1);
  //  imshow("gray", srcgray);
  //  imshow("contour", dst);
  //  waitKey();

  // Calculer des moment
  //  Moments moment = moments(dst);
  Moments moment = moments(imgerode);
  // Calculer Hu
  double huValue[7];
  HuMoments(moment, huValue);
  for (int i = 0; i < 7; i++) {
    output << huValue[i] << " ";
  }
}
*/
/*
 * executer la recherche par Moment Hu
 */
/*
void dosearchHuMoment(const char* filesearch, const char* fileLearn, int numberNeighbor, const double* huMomentArray,
                      int huMomentArraySize, vector<double*>& distanceVector,
                      vector<string*>& fileResultVector) {
  double maxKDistance = 1e6;

  // fichier de caracteristique
  ifstream ifFileLearn;
  ifFileLearn.open(fileLearn);
  // fichier d'apprentissage
  string firstline;
  getline(ifFileLearn, firstline);

  while (ifFileLearn.good()) {
    // obtenir chaque ligne
    string learnline;
    getline(ifFileLearn, learnline);

    stringstream sslearn(learnline);
    string learnfilename;
    sslearn >> learnfilename;

    if (learnfilename.compare("") == 0)
      break;
    if (learnfilename.compare(filesearch) == 0)
      continue;

    // obtenir la valeur de Hu Moment
      double* huMomentEle = new double[huMomentArraySize];
      for (int i = 0; i < huMomentArraySize; i++) {
        sslearn >> huMomentEle[i];
      }

      // calculate distance between 2 vector
      double dist = getHuMomentVectorDistance(huMomentEle, huMomentArray);
      // Merge sort
      if (maxKDistance > dist || distanceVector.size() < numberNeighbor) {
        if (distanceVector.size() == 0 || maxKDistance < dist) {
          distanceVector.push_back(new double(dist));
          fileResultVector.push_back(new string(learnfilename));
          maxKDistance = dist;
          continue;
        }
        vector<double*>::iterator itdistance = distanceVector.begin();
        vector<string*>::iterator itclass = fileResultVector.begin();

        for (; itdistance <= distanceVector.end(); ++itdistance, ++itclass) {
          if ((**itdistance) > dist) {
            double* a = new double;
            *a = dist;
            distanceVector.insert(itdistance, a);
            fileResultVector.insert(itclass, new string(learnfilename));
            break;
          }
        }
        if (distanceVector.size() >= numberNeighbor) {
          distanceVector.pop_back();
          fileResultVector.pop_back();
          maxKDistance = **(distanceVector.end());
        }
      }
      delete [] huMomentEle;
  }

  ifFileLearn.close();
}
*/
/**
 * @brief entree pour la fonction de recher par Moment Hu
 */
/*
void searchHuMoment(const char* fileLearn, const char* fileTest, int k, const char* fileRef, const char* refFileOutDir) {
  ifstream ifFileLearn;
  ifFileLearn.open(fileLearn);

  string firstline;
  getline(ifFileLearn, firstline);
  // Lire l'entete de fichier
  stringstream ssfirst(stringstream::in | stringstream::out);
  ssfirst << firstline;
  string dirPath;
  string name;
  ssfirst >> dirPath;
  ssfirst >> name;
  cout << fileLearn << "-" << dirPath << "-" << name << endl;
  cout << "File test:" << fileTest << endl;

  double* huMomentArray = new double[7];

  while (ifFileLearn.good()) {
    // load testing vector
    string testline;
    getline(ifFileLearn, testline);

    stringstream sstest(stringstream::in | stringstream::out);
    sstest << testline;
    string testfilename;
    sstest >> testfilename;
    if (testfilename.compare("") == 0) {
      break;
    }
    if (testfilename.compare(fileTest) != 0) {
      continue;
    }

    cout << "hu momment array test";
    for (int i = 0; i < 7; i++) {
      sstest >> huMomentArray[i];
    }

    break;
  }
  ifFileLearn.close();

  cout << "end get hu moment" << endl;
  // Search
  vector<double*> distanceVector;
  vector<string*> fileResultVector;
  dosearchHuMoment(fileTest, fileLearn, k, huMomentArray,7, distanceVector, fileResultVector);

  cout << "end search" << endl;
  // Print out the result
  for (int i = 0; i < k || i < distanceVector.size(); i++) {
    cout << *fileResultVector[i] << " " << *distanceVector[i] << endl;
  }

  // Evaluer des resultat de recherche
  int trueTotal = 0;
  evalueSearchCoil_100(fileTest, fileResultVector, trueTotal);
  cout << "Performance: " << trueTotal << "/" << fileResultVector.size() << "=" << ((double) trueTotal) / fileResultVector.size() << endl;

  // Sortie le fichier de html
  ofstream htmlout;
  char filehtml[255];
  sprintf(filehtml, "%s/%s.%d.hu.html", refFileOutDir, fileTest, k);
  htmlout.open(filehtml);
  //  sortir le contenu
  htmlout << "<image src=\"" << dirPath << "/" << fileTest << "\" width=100 height=100 />" << fileTest << endl;
  htmlout << "<br/>Result Performance: " << trueTotal << "/" << fileResultVector.size() << "=" << ((double) trueTotal) / fileResultVector.size() << endl;
  htmlout << "<br/>" << endl;
  for (int i = 0; i < k || i < distanceVector.size(); i++) {
    htmlout << "<div style=\"float:left;margin:2px;\"><image src=\"" << dirPath << "/" << *fileResultVector[i] << "\" width=100 height=100 /><br/>";
    htmlout << *fileResultVector[i] << "<br/>" << *distanceVector[i] << "</div>" << endl;
  }
  htmlout.close();
  // free memory
  freeVector(distanceVector, distanceVector.size());
  freeVector(fileResultVector, fileResultVector.size());
  delete []huMomentArray;

}*/
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;    replace-tabs on;
