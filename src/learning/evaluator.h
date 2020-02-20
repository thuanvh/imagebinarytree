#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <lib/LibTIM/Common/Image.h>
#include <learning/ImageClass.h>
#include <iostream>
using namespace std;
using namespace LibTIM;

class Evaluator{  
public:
  int classNumber;
  double total;
  double** confusionMatrix;

  double totalprecis ;
  double totalrecall ;
  double totalaccuracy;
  
  vector<ImageClass*>* classList;
  Evaluator(int classNumber);
  ~Evaluator();
  void evaluate(Image<TLabel>& test,Image<TLabel>& truth);
  void print(ostream& os);
  void normalise();
  
};

#endif