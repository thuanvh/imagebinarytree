/*
 * File:   texture.cpp
 * Author: thuanvh
 *
 * Created on October 17, 2010, 10:42 PM
 */
#include <vector>
#include <map>
#include "texture.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <time.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h> // for mmap
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <algorithm>
#include "assignmentoptimal.h"
#include <math.h>
#include <cmath>
#include "../lib/LibTIM/Common/Image.h"
#include "../lib/LibTIM/Common/Histogram.h"
#include "../algo/convertImage.h"

using namespace LibTIM;

using namespace std;

/**
 * @brief Reduct the color number
 */
void resizeImageColor(const Image<U8C3>& src, Image<U8C3>& dst, int num_color) {
  double scale = (double) num_color / 256;
  int sizex = src.getSizeX();
  int sizey = src.getSizeY();

  for (int i = 0; i < sizex; i++) {
    for (int j = 0; j < sizey; j++) {
      U8C3 valsrc = src(i, j);
      U8C3 valdst = dst(i, j);
      valdst[0] = valsrc[0] * scale;
      valdst[1] = valsrc[1] * scale;
      valdst[2] = valsrc[2] * scale;
    }
  }
}

/**
 * @brief extract the texture value to file
 */
/*void extractTexture(const Image<U8> & src, int graySize, double*** concurrenceArray, ostream & outfile, int & colid) {

  Image<U8> srcgray(src.getSize());
  cvtRGB2Gray(src, srcgray);
  reduireNiveauDeGris(srcgray, graySize);
  setZero(concurrenceArray, graySize);

  calculerMatrixCooccurence(srcgray, concurrenceArray, graySize);

  for (int matrixIndex = 0; matrixIndex < NUM_MATRIX; matrixIndex++) {
    vector<double> vvalue;
    extraitCaracteristicVector(concurrenceArray[matrixIndex], graySize, vvalue);
  }
}
*/

void extractTexture(const Image<U8> & src, int graySize, double*** concurrenceArray, double* textureVector,int &vectorSize) {

  Image<U8> srcgray(src.getSize());
  cvtRGB2Gray(src, srcgray);
  reduireNiveauDeGris(srcgray, graySize);
  setZero(concurrenceArray, graySize);

  calculerMatrixCooccurence(srcgray, concurrenceArray, graySize);
  double * tempVector=textureVector;
  vectorSize=0;
  for (int matrixIndex = 0; matrixIndex < NUM_MATRIX; matrixIndex++) {
    int size=0;    
    extraitCaracteristicVector(concurrenceArray[matrixIndex], graySize, tempVector,size);
    tempVector+=size;
    vectorSize+=size;
  }
}

/**
 * @brief print out the value of vector
 */
void printVector(const vector<double>& vector) {
  for (int i = 0; i < vector.size(); i++) {
    cout << vector[i] << " ";
  }

  cout << endl;
}

/**
 *  @brief print out the value of matrix
 */
void printMatrix(double** dirPath, int size) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      cout << dirPath[i][j] << " ";
    }

    cout << endl;
  }

  cout << endl;
}

/**
 * @brief executer la recherche par couleur et texture
 */
void dosearch(const char* filesearch, const char* fileLearn, int numberNeighbor, const double* textureArray,
    const double* colorHistoArray, int textureArraySize, int colorHistoSize, vector<double*>& distanceVector,
    vector<string*>& fileResultVector, double colorWeight) {

  double maxKDistance = 1e6;

  ifstream ifFileLearn;
  ifFileLearn.open(fileLearn);

  string firstline;
  getline(ifFileLearn, firstline);

  while (ifFileLearn.good()) {
    // load learning vector
    string learnline;
    getline(ifFileLearn, learnline);

    stringstream sslearn(learnline);
    string learnfilename;
    // load entete
    sslearn >> learnfilename;

    if (learnfilename.compare("") == 0)
      break;

    if (learnfilename.compare(filesearch) == 0)
      continue;

    // obtenir des valeurs de texture et couleur
    double* textureEle = new double[textureArraySize];

    double* colorHistoEle = new double[colorHistoSize];

    for (int i = 0; i < textureArraySize; i++) {
      sslearn >> textureEle[i];
    }

    for (int i = 0; i < colorHistoSize; i++) {
      sslearn >> colorHistoEle[i];
    }

    // calculate distance between 2 vector
    double texturedist = getTextureVectorDistance(textureEle, textureArray, textureArraySize);

    double colorHistoDist = getColorHistoDistance(colorHistoEle, colorHistoArray, colorHistoSize);

    double dist = (1 - colorWeight) * texturedist + colorWeight * colorHistoDist;

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

    delete [] textureEle;

    delete [] colorHistoEle;
  }

  ifFileLearn.close();
}

/**
 * @brief Get Id from filename
 */
int filename2Id(string filename) {
  string name = string(filename);
  int indexName = name.rfind(".");

  if (indexName < 0)
    return -1;

  string pref = name.substr(0, indexName);

  int id = atoi(pref.c_str());

  return id;
}

/**
 * @brief Get Id from filename for images Coil 100
 */
int filename2IdCoil_100(string filename) {
  int indexName = filename.find("_");
  string pref = filename.substr(3, indexName);
  int id = atoi(pref.c_str());
  return id;
}

/**
 * @brief Evaluer search of Coil 100
 */
void evalueSearchCoil_100(const char* fileTest, vector<string*>& resultFileVector, int& trueTotal) {
  trueTotal = 0;
  int fileIndex = filename2IdCoil_100(fileTest);

  for (int i = 0; i < resultFileVector.size(); i++) {
    int id = filename2IdCoil_100(*resultFileVector[i]);

    if (id == fileIndex) {
      trueTotal++;
    }
  }
}

/**
 * @brief Evaluer search of texture
 */
void evalueSearch(const char* fileRef, const char* fileTest, vector<string*>& resultFileVector, int& trueTotal) {
  ifstream ifFileRef;
  ifFileRef.open(fileRef);
  vector<int> classIndex;

  while (ifFileRef.good()) {
    int value;
    ifFileRef >> value;
    classIndex.push_back(value);
  }

  int fileIndex = filename2Id(fileTest);

  int min, max;

  for (int i = 0; i < classIndex.size() - 1; i++) {
    if (classIndex[i + 1] > fileIndex) {
      min = classIndex[i];
      max = classIndex[i + 1] - 1;
      break;
    }
  }

  //  cout << "min-max" << min << " " << max << endl;
  for (int i = 0; i < resultFileVector.size(); i++) {
    int id = filename2Id(*resultFileVector[i]);

    if (id >= min && id <= max) {
      trueTotal++;
    }
  }
}

/**
 * @brief l'entree pour chercher par texture et couleur
 */
void search(const char* fileLearn, const char* fileTest, int k, double colorWeight, const char* fileRef, const char* refFileOutDir) {
  ifstream ifFileLearn;
  ifFileLearn.open(fileLearn);
  // lire l'entete de fichier
  string firstline;
  getline(ifFileLearn, firstline);
  stringstream ssfirst(stringstream::in | stringstream::out);
  ssfirst << firstline;
  int texture_att_size = 0;
  int color_histo_size = 0;
  string dirPath;
  string name;
  int graySize, colorSize;
  ssfirst >> dirPath;
  ssfirst >> name;
  ssfirst >> graySize;
  ssfirst >> colorSize;
  ssfirst >> texture_att_size;
  ssfirst >> color_histo_size;

  double* textureArray = new double[texture_att_size];
  double* colorHistArray = new double[color_histo_size];

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

    // load valeur de texture
    for (int i = 0; i < texture_att_size; i++) {
      sstest >> textureArray[i];
    }

    // load valeur de couleur
    for (int i = 0; i < color_histo_size; i++) {
      sstest >> colorHistArray[i];
    }

    break;
  }

  ifFileLearn.close();

  cout << "end get texture and color att" << endl;
  // Search
  vector<double*> distanceVector;
  vector<string*> fileResultVector;
  dosearch(fileTest, fileLearn, k, textureArray,
      colorHistArray, texture_att_size, color_histo_size, distanceVector, fileResultVector, colorWeight);

  cout << "end search" << endl;
  // Print out the result

  for (int i = 0; i < k || i < distanceVector.size(); i++) {
    cout << *fileResultVector[i] << " " << *distanceVector[i] << endl;
  }

  // Evaluer le resultat
  int trueTotal = 0;

  if (fileRef != NULL) {
    evalueSearch(fileRef, fileTest, fileResultVector, trueTotal);
    cout << "Performance: " << trueTotal << "/" << fileResultVector.size() << "=" << ((double) trueTotal) / fileResultVector.size() << endl;
  }

  // print out file html result
  ofstream htmlout;

  char filehtml[255];

  sprintf(filehtml, "%s/%s.%d.%d.%d.%d.html", refFileOutDir, fileTest, k, graySize, colorSize, (int)(colorWeight * 100));

  htmlout.open(filehtml);

  //  print out the content
  htmlout << "<image src=\"" << dirPath << "/" << fileTest << "\" width=100 height=100 />" << fileTest << endl;

  htmlout << ". Param: Couleur M :" << colorSize << ", Texture Gris: " << graySize << ", Poids de Couleur:" << colorWeight << ", Nombre de resultat:" << k << endl;

  htmlout << "<br/> Result ";

  htmlout << "Performance: " << trueTotal << "/" << fileResultVector.size() << "=" << ((double) trueTotal) / fileResultVector.size() << "</br>" << endl;

  for (int i = 0; i < k || i < distanceVector.size(); i++) {
    htmlout << "<div style=\"float:left;margin:2px;\"><image src=\"" << dirPath << "/" << *fileResultVector[i] << "\" width=100 height=100 /><br/>";
    htmlout << *fileResultVector[i] << "<br/>" << *distanceVector[i] << "</div> " << endl;
  }

  htmlout.close();

  // Free memory
  freeVector(distanceVector, distanceVector.size());
  freeVector(fileResultVector, fileResultVector.size());
  delete []textureArray;
  delete []colorHistArray;
}

/**
 * @brief Calculer la distance entre 2 vecteurs
 */
double getVectorDistance(double* v1, double* v2, int size) {
  double distance = 0;
  // distance eclidienne

  for (int i = 0; i < size; i++) {
    distance += pow(v1[i] - v2[i], 2);
  }

  distance = sqrt(distance);

  // distance cosinus
  //  double v1v2 = 0;
  //  double dv1 = 0;
  //  double dv2 = 0;
  //  for (int i = 0; i < size; i++) {
  //    v1v2 += abs(v1[i] * v2[i]);
  //    dv1 += pow(v1[i], 2);
  //    dv2 += pow(v2[i], 2);
  //  }
  //  distance = 1-(v1v2 / (sqrt(dv1) * sqrt(dv2)));

  return distance;
}

/**
 * @brief Calculer la distance entre HuMoment vector
 */
double getHuMomentVectorDistance(const double* learningVector, const double* testingVector) {
  double distance = 0;

  for (int i = 0; i < 7; i++) {
    double ma = ((learningVector[i] >= 0) ? 1 : -1) * log(std::abs(learningVector[i]));
    double mb = ((testingVector[i] >= 0) ? 1 : -1) * log(std::abs(testingVector[i]));
    //cout << ma << "-" << mb << endl;
    //    cout << mb << "-" << ma << endl;
    // case 1
    //    if (ma != 0 && mb != 0)
    distance += std::abs(1 / ma - 1 / mb);
    // case 2
    //    distance += abs(ma-mb);
    // case 3
    //    distance += abs((ma-mb)/ma);
  }

  return distance;
}

/**
 * @brief Calculer la distance entre Couleur
 */
double getColorHistoDistance(const double* learningVector, const double* testingVector, int colorSize) {
  double distance;
  //  for (int i=0; i < colorSize; i++) {
  //    distance += abs(learningVector[i] - testingVector[i]);
  //  }

  for (int i = 0; i < colorSize; i++) {
    if (learningVector[i] + testingVector[i] != 0) {
      distance += pow(learningVector[i] - testingVector[i], 2) / (learningVector[i] + testingVector[i]);
    }
  }

  return distance;
}

/**
 * @brief Get texture pour le bipartie Matching
 */
double getTextureVectorDistanceBipartiMatching(const double* learningVector, const double* testingVector, int textureSize) {
  int numberPara = NUM_MATRIX_ATT;
  double distanceMatching = 0;
  double distance[NUM_MATRIX][NUM_MATRIX];

  double* v1 = new double[numberPara];
  double* v2 = new double[numberPara];

  // calculate distance between each feature vector

  for (int i = 0; i < NUM_MATRIX; i++) {
    for (int j = 0; j < NUM_MATRIX; j++) {
      for (int k = 0; k < numberPara; k++) {
        v1[k] = learningVector[i * numberPara + k];
        v2[k] = testingVector[j * numberPara + k];
      }

      distance[i][j] = getVectorDistance(v1, v2, numberPara);
    }
  }

  double assignment[NUM_MATRIX];

  assignmentoptimal(assignment, &distanceMatching, (double*) distance, NUM_MATRIX, NUM_MATRIX);

  delete [] v1;
  delete [] v2;
  return distanceMatching;
}

/**
 * @brief Get distance for texture normal vecteur
 */
double getTextureVectorDistance(const double* learningVector, const double* testingVector, int textureSize) {

  double distanceMatching = 0;

  double distancePara[NUM_MATRIX_ATT];

  for (int i = 0; i < NUM_MATRIX; i++) {
    for (int j = 0; j < NUM_MATRIX_ATT; j++) {
      int index = j * NUM_MATRIX + i;
      //      cout << i<<"*"<<j<<"="<<index << ":" << learningVector[index] << " " << testingVector[index];
      distancePara[i] += abs(((double)(learningVector[index] - testingVector[index])) / testingVector[index]);
    }
  }

  for (int i = 0; i < NUM_MATRIX_ATT; i++) {
    distanceMatching += distancePara[i];
  }

  distanceMatching /= NUM_MATRIX_ATT * NUM_MATRIX;

  //  cout << "end assignment " << distanceMatching << endl;
  return distanceMatching;
}

/**
 * @brief Reduire le niveau de gris
 */
void reduireNiveauDeGris(Image<U8>& image, int size) {
  double a = (double) size / 256;
  //  cout<<"a="<<a;
  int sizex = image.getSizeX();
  int sizey = image.getSizeY();

  for (int i = 0; i < sizex; i++) {
    for (int j = 0; j < sizey; j++) {
      image(i, j) = (unsigned char)(a * image(i, j));
    }
  }
}

/**
 * @brief Set Zero pour la matrice
 */
void setZero(double*** concurrenceArray, int graySize) {
  for (int i = 0; i < NUM_MATRIX; i++) {
    for (int j = 0; j < graySize; j++) {
      for (int k = 0; k < graySize; k++) {
        concurrenceArray[i][j][k] = 0;
      }
    }
  }
}

/**
 * @brief Free memory
 */
void freeVector(vector<int>** vector, int size) {
  for (int i = 0; i < size; i++) {
    delete vector[i];
  }

  delete[] vector;
}

/**
 * @brief Free memory
 */
void freeVector(vector<double*> vector, int size) {
  for (int i = 0; i < size; i++) {
    delete vector[i];
  }
}

/**
 * @brief Free memory
 */
void freeVector(vector<string*> vector, int size) {
  for (int i = 0; i < size; i++) {
    delete vector[i];
  }
}

/**
 * @brief Free memory
 */
void freeMatrix(double** matrix, int size) {
  for (int i = 0; i < size; i++) {
    delete[] matrix[i];
  }

  delete matrix;
}

/**
 * @brief Free memory
 */
void freeMatrix(double*** concurrenceArray, int graySize) {
  for (int i = 0; i < NUM_MATRIX; i++) {
    for (int j = 0; j < graySize; j++) {
      delete[] concurrenceArray[i][j];
    }

    delete[] concurrenceArray[i];
  }

  delete[] concurrenceArray;
}

/**
 * @brief Calculer 8 matrices de co-occurences
 */
void calculerMatrixCooccurence(Image<U8> const & image, double*** concurrenceArray, int graySize) {

  // convertir image a partir de 256 a size
  int deltaX[NUM_MATRIX] = {1, 1, 0, -1, 2, 2, 0, -2};
  int deltaY[NUM_MATRIX] = {0, 1, 1, 1, 0, 2, 2, 2};
  int numberOfPair[NUM_MATRIX] = {0};


  // calculer matrix of concurrence
  //  cout << "begin calculer matrix of concurrence" << endl;
  int sizex = image.getSizeX();
  int sizey = image.getSizeY();

  for (int i = 0; i < sizex; i++) {
    for (int j = 0; j < sizey; j++) {
      for (int k = 0; k < NUM_MATRIX; k++) {
        int ia = i + deltaY[k];
        int ja = j + deltaX[k];

        if (image.isPosValid(ia, ja)) {
          int g1 = image(i, j);
          int g2 = image(ia, ja);

          concurrenceArray[k][g1][g2]++;

          numberOfPair[k]++;
        }
      }
    }
  }

  // normalize matrix of concurrence
  for (int i = 0; i < NUM_MATRIX; i++) {
    for (int j = 0; j < graySize; j++) {
      for (int k = 0; k < graySize; k++) {
        concurrenceArray[i][j][k] /= numberOfPair[i];
      }
    }
  }
}

/**
 * @brief calculer 14 parametres and sauver le vecteur de caracteristique
 */
void extraitCaracteristicVector(double** mat, int size, double* vvalue, int& valueSize) {
  // mean
  double meani = para_meani(mat, size);
  double meanj = para_meanj(mat, size);
  double mean = para_mean(mat, size);

  // variance
  double vari = para_vari(mat, size, meani);
  double varj = para_varj(mat, size, meanj);

  double sum_avg = para_sum_average(mat, size);
  valueSize=6;
  vvalue[0]=(para_angular_second_moment(mat, size));
  //    vvalue[0]=(para_constrast(mat, size));
  vvalue[1]=(para_entropy(mat, size));
  vvalue[2]=(para_correlation(mat, size, meani, meanj, vari, varj));
  vvalue[3]=(para_dissimilarity(mat, size));
  vvalue[4]=(para_energy(mat, size));
  vvalue[5]=(para_homogenety(mat, size));

  //vvalue[6]=(sum_avg);
  //vvalue[7]=(para_sum_entropy(mat, size));
  //vvalue[8]=(para_sum_of_squares__variance(mat, size, mean));
  //vvalue[9]=(para_sum_variance(mat, size, sum_avg));
  //vvalue[10]=(para_difference_entropy(mat, size));
  //vvalue[11]=(para_difference_variance(mat, size));

  //vvalue[12]=(para_shade(mat, size, meani, meanj));
  //vvalue[13]=(para_prominence(mat, size, meani, meanj));

  // normalize value
  /*
  double vmean = 0;

  for (int i = 0; i < vvalue.size(); i++) {
    vmean += vvalue[i];
  }

  vmean /= vvalue.size();

  double sd = 0;

  for (int i = 0; i < vvalue.size(); i++) {
    sd += pow(vvalue[i] - vmean, 2);
  }

  sd = sqrt(sd / vvalue.size());

  for (int i = 0; i < vvalue.size(); i++) {
    vvalue[i] = (vvalue[i] - vmean) / sd;
  }
  */
  // output to file
  /*if (ofs != NULL) {
    for (int i = 0; i < vvalue.size(); i++) {
      if (vvalue[i] != NULL)
        (*ofs) << colid++ << ":" << vvalue[i] << " ";
      else
        colid++;
    }
  }*/
}

/**
 * @brief para texture angular_second_moment
 */
double para_angular_second_moment(double** mat, int size) {
  double value = 0;

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      value += pow(mat[i][j], 2);
    }
  }

  return value;
}

/**
 * @brief para texture constrast
 */
double para_constrast(double** mat, int size) {
  double value = 0;
  int number_of_gray = size;

  for (int n = 0; n < number_of_gray; n++) {
    float sub = 0;

    for (int i = 0; i < number_of_gray; i++) {
      for (int j = 0; j < number_of_gray; j++) {
        if (abs(i - j) == n) {
          sub += mat[i][j];
        }
      }
    }

    value += n * n * sub;
  }

  return value;
}

/**
 * @brief para texture moyenne
 */
double para_mean(double** mat, int size) {
  double mean = 0;

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      mean += i * j * mat[i][j];
    }
  }

  return mean;
}

/**
 * @brief para texture moyenne i
 */
double para_meani(double** mat, int size) {
  // mean
  double meani = 0;

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      meani += i * mat[i][j];

    }
  }

  return meani;
}

/**
 * @brief para texture moyenne j
 */
double para_meanj(double** mat, int size) {
  // mean

  double meanj = 0;

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {

      meanj += j * mat[i][j];
    }
  }

  return meanj;
}

/**
 * @brief para texture variance
 */
double para_vari(double** mat, int size, double meani) {
  // variance
  double vari = 0;

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      vari += mat[i][j] * pow(i - meani, 2);

    }
  }

  return vari;
}

/**
 * @brief para texture variance j
 */
double para_varj(double** mat, int size, double meanj) {
  // variance

  double varj = 0;

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {

      varj += mat[i][j] * pow(j - meanj, 2);
    }
  }

  return varj;
}

/**
 * @brief para texture correlation
 */
double para_correlation(double** mat, int size, double meani, double meanj, double vari, double varj) {

  // correlation
  double corr = 0;

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      corr += mat[i][j] * (i - meani) * (j - meanj) / sqrt(vari * varj);
    }
  }

  return corr;
}

/**
 * @brief para texture correlation
 */
double para_correlation(double** mat, int size) {
  // mean
  double meani = para_meani(mat, size);
  double meanj = para_meanj(mat, size);

  // variance
  double vari = para_vari(mat, size, meani);
  double varj = para_varj(mat, size, meanj);

  // correlation
  double corr = para_correlation(mat, size, meani, meanj, vari, varj);
  return corr;
}

/**
 * @brief para texture sum_of_squares__variance
 */
double para_sum_of_squares__variance(double** mat, int size, double mean) {
  double var = 0;

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      var += pow(i - mean, 2) * mat[i][j];

    }
  }

  return var;
}

/**
 * @brief para texture inverse_difference_moment
 */
void para_inverse_difference_moment(double** mat, int size) {

}

/**
 * @brief para texture average
 */
double para_sum_average(double** mat, int size) {
  int _2size_1 = 2 * size - 1;
  double value = 0;
  double p_i_add_j = 0;

  for (int k = 0; k < _2size_1; k++) {
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        if (i + j == k) {
          p_i_add_j += mat[i][j];
        }
      }
    }

    value += k * p_i_add_j;
  }

  return value;
}

/**
 * @brief para texture variance
 */
double para_sum_variance(double** mat, int size, double sum_e) {
  int _2size_1 = 2 * size - 1;
  double value = 0;
  double p_i_add_j = 0;

  for (int k = 0; k < _2size_1; k++) {
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        if (i + j == k) {
          p_i_add_j += mat[i][j];
        }
      }
    }

    value += pow(k - sum_e, 2) * p_i_add_j;
  }

  return value;
}

/**
 * @brief para texture entropy
 */
double para_sum_entropy(double** mat, int size) {
  int _2size_1 = 2 * size - 1;
  double value = 0;
  double p_i_add_j = 0;

  for (int k = 0; k < _2size_1; k++) {
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        if (i + j == k) {
          p_i_add_j += mat[i][j];
        }
      }
    }

    if (p_i_add_j != 0)
      value += p_i_add_j * log(p_i_add_j);
  }

  return -value;
}

/**
 * @brief para texture entropy
 */
double para_entropy(double** mat, int size) {
  double value = 0;

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (mat[i][j] != 0)
        value += mat[i][j] * log(mat[i][j]);
    }
  }

  return -value;
}

/**
 * @brief para texture variance
 */
double para_difference_variance(double** mat, int size) {
  double value = 0;
  double p_i_sub_j = 0;

  for (int k = 0; k < size; k++) {
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        if (abs(i - j) == k) {
          p_i_sub_j += mat[i][j];
        }
      }
    }

    value += pow(k, 2) * p_i_sub_j;
  }

  return value;
}

/**
 * @brief para texture difference entropy
 */
double para_difference_entropy(double** mat, int size) {
  //int _2size_1 = 2 * size - 1;
  double value = 0;
  double p_i_sub_j = 0;

  for (int k = 0; k < size; k++) {
    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        if (abs(i - j) == k) {
          p_i_sub_j += mat[i][j];
        }
      }
    }

    if (p_i_sub_j != 0)
      value += p_i_sub_j * log(p_i_sub_j);
  }

  return -value;
}

//para_info_measure_of_correlation_1
void para_info_measure_of_correlation_1(double** mat, int size) {

}

//para_info_measure_of_correlation_2
void para_info_measure_of_correlation_2(double** mat, int size) {

}

//para_max_correlation_coeff
void para_max_correlation_coeff(double** mat, int size) {

}

/**
 * @brief para_dissimilarity
 */
double para_dissimilarity(double** mat, int size) {
  double value = 0;

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      value += mat[i][j] * abs(i - j);
    }
  }

  return value;
}

/**
 * @brief para_energy
 */
double para_energy(double** mat, int size) {
  double value = 0;

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      value += pow(mat[i][j], 2);
    }
  }

  return value;
}

/**
 * @brief para_homogenety
 */
double para_homogenety(double** mat, int size) {
  double value = 0;

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      value += (1 / (double)(1 + pow((i - j), 2))) * mat[i][j];
    }
  }

  return value;
}

/**
 * @brief para_shade
 */
double para_shade(double** mat, int size, int meani, int meanj) {
  double value = 0;

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      value += pow(i - meani + j - meanj, 3) * mat[i][j];
    }
  }

  return value;

}

/**
 * @brief para_prominence
 */
double para_prominence(double** mat, int size, int meani, int meanj) {
  double value = 0;

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      value += pow(i - meani + j - meanj, 4) * mat[i][j];
    }
  }

  return value;

}

// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
