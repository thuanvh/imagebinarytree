
#include <stdlib.h>
#include <stdio.h>
#include "kmeanClustering.h"
#include <cmath>
#include <algo/distanceCalculate.h>
#include <lib/LibTIM/Common/Image.h>
#include <time.h>
using namespace LibTIM;
//#include "random"
double fRand(double fMin, double fMax);
/**
 * @brief Clustering the data and get the list of final gravity vector output
 * 
 */
void Kmean::clustering(std::vector< std::vector< double >* >& data, vector<TLabel> & result) {
  int nClasses = k;

  double distanceClass;
  double bestDistance;
  TLabel bestClass;
  double doubleMax = std::numeric_limits<double>::max();


  //Algorithm
  bool stop = false;
  initCentroids(data);

  vector<vector<double>*> oldCentroids(centroids.size());
  for(int i=0; i<centroids.size(); i++){
    oldCentroids[i]=new vector<double>(dimension,0);
  }
  //int res[data.size()];
  int time = 0;
  int datasize=data.size();
  
  double sumElements[nClasses][dimension];
  int nbElements[nClasses];

    
  
  while (!stop) {
    cout << "Repeat:" << time << endl;
    for(int i=0; i<nClasses; i++){
      (*oldCentroids[i])=(*centroids[i]);
    }
    //Init sum of element
    for (int i = 0; i < nClasses; i++){
      nbElements[i]=0;
      for(int j=0; j<dimension; j++)
        sumElements[i][j] = 0;
    }
        
    //Update clusters
    for (int i = 0; i < datasize; i++) {
      vector<double>* value = data[i];
      bestDistance = doubleMax;
      bestClass = 0;

      for (int c = 0; c < nClasses; c++) {
        distanceClass = distanceCalculator::getDistanceEuclidien(*value, *centroids[c],bestDistance);// fabs(value - centroids[c]);
        //cout << "(" << c << "," << distanceClass << ")";
        if (distanceClass < bestDistance) {
          bestDistance = distanceClass;
          bestClass = (TLabel)c;
        }
      }
      //update the class
      result[i] = bestClass;
      //update the number of class
      nbElements[bestClass]++;
      //update the sum of class
      for (int a = 0; a < dimension; a++) {
        sumElements[bestClass][a] += (double)(*data[i])[a];
      }
    }

    for (int c = 0; c < nClasses; c++) {
      if(nbElements[c]!=0){
        for (int a = 0; a < dimension; a++) {
          (*centroids[c])[a] = (double)sumElements[c][a] / nbElements[c];
        }
      }
    }
    
    //Test convergence
    stop = true;

    for (int c = 0; c < nClasses; c++) {
      double distance=distanceCalculator::getDistanceEuclidien(*centroids[c] , *oldCentroids[c]);

      if ( distance> EPSILON){
        stop = false;
        break;
      }
    }
    if (time++ > maxTimes)
      stop = true;
  }
}
double fRand(double fMin, double fMax) {
  double f = (double)rand() / RAND_MAX;
  return fMin + f * (fMax - fMin);
}

/**
 * @brief Init the gravity 
 */
void Kmean::initCentroids(std::vector< std::vector< double >* >& data) {
  srand(time(NULL));
  int size=data.size();
  for (int i = 0; i < k; i++) {
    vector<double>* vec = new vector<double>(dimension);
    int id=rand()%size;
    for (int j = 0; j < dimension; j++) {
      (*vec)[j] = (*data[id])[j];//fRand((*min)[i], (*max)[i]);
    }
    centroids.push_back(vec);
  }
}

// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;  replace-tabs on;
