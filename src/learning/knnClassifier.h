#ifndef KNN_CLASSIFIER_H
#define KNN_CLASSIFIER_H

#include <vector>
#include "classifier.h"
using namespace std;

class knnClassifier : public classifier{
private:
  vector<vector<double>*> learningItems;
  vector<int> learningClass;
  const vector< vector< double >* >* testData;
  int classify(const vector<double>& item,double & classDistance);
public:
  int k;
  void loadProblem(const char* filename);  
  //void loadProblem(const char* filename);
  void classify();
  void setTestData(const std::vector< std::vector< double >* >& testData);
  string getInfo();
  ~knnClassifier();
  knnClassifier();
  void reset();
};

#endif