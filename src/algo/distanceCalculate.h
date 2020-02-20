#ifndef DISTANCE_CALCULATE_H
#define DISTANCE_CALCULATE_H
#include <vector>
using namespace std;
class distanceCalculator{
public:
  static double getDistanceEuclidien(const vector<double>& a,const vector<double>& b);
  static double getDistanceEuclidien(const vector< double >& a, const vector< double >& b, double bestDistance);
  static double getDistanceEuclidien(double* a,double* b, int size);
  static double getDistanceEuclidien(double* a,double* b, int size, double bestDistance);
};
#endif