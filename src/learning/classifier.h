#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include <vector>
#include <string>
using namespace std;
class classifier{
public:
  bool isDistanceSupport;
  virtual void loadProblem(const char* filename)=0;
  virtual void classify()=0;
  virtual void setTestData(const std::vector< std::vector< double >* >& testData)=0;
  virtual string getInfo()=0;
  virtual void reset()=0;
  std::vector<int> classIds;
  std::vector<double> classDistances;
};

#endif