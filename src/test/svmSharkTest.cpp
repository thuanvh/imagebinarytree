#include <iostream>
#include <ReClaM/ArtificialDistributions.h>
#include <ReClaM/Dataset.h>
#include <ReClaM/Svm.h>
#include <ReClaM/ClassificationError.h>

using namespace std;
int main(int argc, char** argv) {
  Chessboard chess;
  Dataset dataset;
  dataset.CreateFromSource(chess, 500, 10000);

  double C = 100.0;
  double gamma = 1.0;

  RBFKernel kernel(gamma);
  SVM svm(&kernel);
  C_SVM csvm(&svm, C, C);
  SVM_Optimizer opt;

  opt.init(csvm);

  opt.optimize(svm,
      dataset.getTrainingData(),
      dataset.getTrainingTarget());

  ClassificationError err;
  double e = err.error(svm,
      dataset.getTestData(),
      dataset.getTestTarget());
  cout << e << endl;

  return EXIT_SUCCESS;
}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;  replace-tabs on;
