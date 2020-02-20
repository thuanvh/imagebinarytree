#include "distanceCalculate.h"
#include <cmath>
#include <iostream>
using namespace std;
/**
 * @brief Calculate the distance
 */
double distanceCalculator::getDistanceEuclidien(const vector< double >& a, const vector< double >& b)
{
  double distance = 0;
  int itemsize=a.size();
  int itemsizeb=b.size();
  itemsize=min(itemsize,itemsizeb);
  // distance euclidien
  for (int j = 0; j <itemsize; j++) {
    //cout<<"("<<a[j]<<"-"<<b[j]<<")^2+";
    distance += pow(a[j] - b[j], 2);
  }
  distance = sqrt(distance);
  return distance;
}
/**
 * @brief Calculate the distance with the view of the minimal distance
 */
double distanceCalculator::getDistanceEuclidien(const vector< double >& a, const vector< double >& b, double bestDistance)
{
  double distance = 0;
  double max=bestDistance*bestDistance;
  int itemsize=a.size();
  int itemsizeb=b.size();
  itemsize=min(itemsize,itemsizeb);
  // distance euclidien
  for (int j = 0; j <itemsize; j++) {
    distance += pow(a[j] - b[j], 2);
    if(distance>max)
      return bestDistance+1;
  }
  distance = sqrt(distance);
  return distance;
}
/**
 * @brief Calculate the distance
 */
double distanceCalculator::getDistanceEuclidien(double* a, double* b, int size)
{
  double distance = 0;

  // distance euclidien
  for (int j = 0; j <size; j++) {
    //cout<<"("<<a[j]<<"-"<<b[j]<<")^2+";
    distance += pow(a[j] - b[j], 2);
  }
  distance = sqrt(distance);
  return distance;
}
/**
 * @brief Calculate the distance with the view of the minimal distance
 */
double distanceCalculator::getDistanceEuclidien(double* a, double* b, int size, double bestDistance)
{
  double distance = 0;
  double max=bestDistance*bestDistance;

  // distance euclidien
  for (int j = 0; j <size; j++) {
    distance += pow(a[j] - b[j], 2);
    if(distance>max)
      return bestDistance+1;
  }
  distance = sqrt(distance);
  return distance;
}
