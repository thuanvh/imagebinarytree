#include <iostream>
#include <learning/LibSVMAdapter.h>
#include <lib/LibTIM/Common/Image.h>
#include <learning/featureGenerator.h>

using namespace LibTIM;
using namespace std;

int main(int argc, char** argv) {
  LibSVMAdapter svm;
  svm.param.kernel_type = POLY;
  svm.param.C = 1;
  svm.param.gamma = 0.1;
  svm.param.degree=1;

  string learningFileName = "learningcolor2.txt";
  svm.loadProblem(learningFileName.c_str());
  cout << "end load pbl" << endl;

  Image<U8C3> rgb;
  Image<U8C3>::load("0.png",rgb);
  vector<vector<double>*> vectorList;
  vectorList.push_back(new vector<double>());
  featureGenerator fg;
  fg.generate(rgb,vectorList[0]);
  cout<<"input: ";
  for(int i=0; i<vectorList[0]->size(); i++){
    cout<<(*vectorList[0])[i]<<" ";
  }
  cout<<endl;
  
  /// classify the regions
  svm.setTestData(vectorList);
  cout << "classify" << endl;

  svm.classify();
  cout << "end" << endl;
  cout<<"Classes: ";
  for(int i=0; i<vectorList.size(); i++){
    cout<<svm.classIds[i]<<" ";
  }
  cout<<endl;

  return 0;
}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
