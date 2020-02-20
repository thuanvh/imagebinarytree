#ifndef KMEANS_CLUSTERING_H
#define KMEANS_CLUSTERING_H
#include <vector>
#include <lib/LibTIM/Common/Image.h>
using namespace std;
using namespace LibTIM;
class Kmean{
private:
  void initCentroids(std::vector< std::vector< double >* >& data);  
public:
  /// Condition of stop
  double EPSILON;
  int k;
  int dimension;
  int maxTimes;
  //vector<double>* min;
  //vector<double>* max;
  vector<vector<double>*> centroids;
  
  void clustering(std::vector< std::vector< double >* >& data,vector<TLabel> & result);
  
  Kmean(){
    EPSILON=0.02;
    k=10;
    maxTimes=10;
  }
};

#endif