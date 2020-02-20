#ifndef _LIB_SVM_ADAPTER_
#define _LIB_SVM_ADAPTER_
#include <vector>

using namespace std;
#include "../lib/LibSVM/svm.h"
#include "classifier.h"
class LibSVMAdapter : public classifier{

private:

  
  struct svm_problem prob;    // set by read_problem
  struct svm_model *model;
  struct svm_node *x_space;
  int cross_validation;
  int nr_fold;
  vector<svm_node*> testNodeList;

public:
  struct svm_parameter param;   // set by parse_command_line
  void readProblem(const char* file);
  void setTestData(const vector<vector<double>* >& testData);
  string getInfo();
  void classify();
  void training();
  void crossValidation();
  void loadProblem(const char *filename);
  LibSVMAdapter();
  ~LibSVMAdapter();
  void reset();
};

#endif
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
