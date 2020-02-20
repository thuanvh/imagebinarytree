#ifndef SIFT_CLASSIFIER_H
#define SIFT_CLASSIFIER_H

#include <iostream>
#include <vector>
#include <algo/SIFT.h>
#include <lib/LibTIM/Common/Image.h>
#include <learning/classifier.h>
using namespace LibTIM;
using namespace std;


class SIFTClassifier:public classifier{
public:
  vector<int> idlist;
  vector<SIFTPoint*> dict;
  int classNumber;
  double threshold;
  SIFTClassifier(const char* dictFile,int classNumber,double threshold);
  ~SIFTClassifier();
  int classify(Image<U8C3>& image);
  static void generateDictionary(const char* foldername,const char* classDir, const char* dictFile);

  void loadProblem(const char* filename){};
  void classify(){};
  void setTestData(const std::vector< std::vector< double >* >& testData){};
  string getInfo(){return "SIFT";};
  void reset(){};
  
};

#endif