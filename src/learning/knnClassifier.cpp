#include <fstream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include "knnClassifier.h"
#include <algo/distanceCalculate.h>
#include <map>
using namespace std;
/**
 * @brief struct distance-class used for knn
 */
struct knnnodearray {
  double distance;
  int classId;
};
/**
 * @brief Compare function
 */
bool knncompare(knnnodearray a, knnnodearray b) {
  return a.distance < b.distance;
  /*if (a.distance > b.distance)
    return -1;
  else
    if (a.distance == b.distance)
      return 0;
    else
      return 1;
    */
}
string knnClassifier::getInfo()
{
  char a[255];
  sprintf(a,"Knn k=%d",k);
  return string(a);
}

/**
 * @brief Classify a vector by knn
 * @param item Vector to classify
 * @param classDistance The return value of class distance
 * @return The class id
 */
int knnClassifier::classify(const vector<double >& item, double& classDistance) {
  
  //vector<double> distanceArray;
  //vector<int> classes;
  vector<knnnodearray> knnArray;

  int size = learningItems.size();
  int itemsize = item.size();
  double maxDistance = 0;
  /*for(int i=0; i<itemsize; i++){
    cout<<item[i]<<" ";
  }
  cout<<endl;
  */
  for (int i = 0; i < size; i++) {
    vector<double>* litem = learningItems[i];
    double distance = distanceCalculator::getDistanceEuclidien(*litem, item);
    //cout<<"item "<<i<<" distance"<<distance<<endl;
    knnnodearray kvalue;
    kvalue.distance = distance;
    kvalue.classId = learningClass[i];
    //cout << learningClass[i] << ":" << distance <<": max:"<<maxDistance<< endl;
    if (knnArray.size() == 0) {
      knnArray.push_back(kvalue);
    } else {
      if (distance < maxDistance) {
        vector<knnnodearray>::iterator lowit = lower_bound(knnArray.begin(), knnArray.end(), kvalue, knncompare);
        knnArray.insert(lowit, kvalue);
        //for (int m = 0; m < knnArray.size(); m++) {
        //  cout << knnArray[m].distance << " ";
        //}
        //cout << endl;
        if (knnArray.size() > k) {
          knnArray.erase(knnArray.begin() + knnArray.size() - 1);
        }
      } else {
        if (knnArray.size() < k) {
          knnArray.push_back(kvalue);
        }
      }
    }
    maxDistance = knnArray[knnArray.size()-1].distance;
  }
  map<int, int> classCountMap;
  map<int, double> classDistanceMap;
  // find the principal class
  //cout << "classes ";
  for (int i = 0; i < knnArray.size(); i++) {
    //cout << knnArray[i].classId << " ";
    if (classCountMap.find(knnArray[i].classId) == classCountMap.end()) {
      classCountMap.insert(pair<int, int>(knnArray[i].classId, 1));
      classDistanceMap.insert(pair<int, double>(knnArray[i].classId, knnArray[i].distance));
    } else {
      classCountMap[knnArray[i].classId]++;
    }
  }
  //cout << endl;
  int countMax = 0;
  int classMax = 0;

  for (map<int, int>::iterator it = classCountMap.begin(); it != classCountMap.end(); it++) {
    if (it->second > countMax) {
      countMax = it->second;
      classMax = it->first;
    }
  }
  classDistance = classDistanceMap[classMax];
  //cout<<"class "<<classMax<<" with distance "<<classDistance<<endl;
  return classMax;
  //
}
/**
 * @brief Load problem from the lightSvm format file
 * @param filename File of the lightSvm format
 */
void knnClassifier::loadProblem(const char* filename) {
  ifstream ifs;
  ifs.open(filename);
  string pairvalue;
  learningClass.clear();
  learningItems.clear();
  if (ifs.good())
    ifs >> pairvalue;
  while (ifs.good()) {
    int classid = 0;
    classid = atoi(pairvalue.c_str());
    //cout<<endl<<"line:\""<<pairvalue<<"\" ";
    //ifs>>classid;
    learningClass.push_back(classid);
    vector<double>* valueitem = new vector<double>();
    learningItems.push_back(valueitem);
    while (ifs.good()) {
      ifs >> pairvalue;
      //cout<<"\""<<pairvalue<<"\" ";
      int pos = pairvalue.find(':');
      if (pos != string::npos) {
        string ids = pairvalue.substr(0, pos);
        string vals = pairvalue.substr(pos + 1, pairvalue.length() - pos);
        //cout << ids << " " << vals << endl;
        int id = atoi(ids.c_str())-1;
        double val = atof(vals.c_str());
        for (int i = valueitem->size(); i < id; i++) {
          valueitem->push_back(0);
        }
        if(valueitem->size()==id)
          valueitem->push_back(val);
      } else {
        break;
      }
    }
  }
  ifs.close();
  
  // printing the loaded problem
  /*
  for(int i=0; i<learningClass.size(); i++){
    cout<<learningClass[i]<<"***";
    for(int j=0; j<learningItems[i]->size(); j++){
      cout<<(*learningItems[i])[j]<<" ";
    }
    cout<<endl;
  }*/
}
/**
 * @brief Destructor of knn
 */
knnClassifier::~knnClassifier() {
  for (int i = 0; i < learningItems.size(); i++) {
    delete learningItems[i];
  }
}
/**
 * @brief Classify. Inherit from classifier
 */
void knnClassifier::classify() {
  for(int i=0; i<testData->size(); i++){
    double distance=0;
    int classid=classify(*(*testData)[i],distance);
    classIds.push_back(classid);
    classDistances.push_back(distance);
  }
}
/**
 * @brief Set data. Inherit from classifier
 */
void knnClassifier::setTestData(const std::vector< std::vector< double >* >& testData) {
  this->testData=&testData;
}
/**
 * @brief Constructor.
 */
knnClassifier::knnClassifier() {
  isDistanceSupport=true;  
}
/**
 * @brief Reset the classifier
 */
void knnClassifier::reset()
{
  classIds.clear();
  classDistances.clear();
  
}
