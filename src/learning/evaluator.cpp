#include "evaluator.h"
#include <iomanip>
/**
 * @brief Evaluation between 2 image
 */
void Evaluator::evaluate(Image< TLabel >& test, Image< TLabel >& truth) {
  int sizex = test.getSizeX();
  int sizey = test.getSizeY();
  double unit = 1.f / (sizex * sizey);
  for (int x = 0; x < sizex; x++) {
    for (int y = 0; y < sizey; y++) {
      int tr = truth(x, y);
      int te = test(x, y);
      confusionMatrix[tr][te] += unit;
    }
  }
  total++;
}
/**
 * @brief Destructor
 */
Evaluator::~Evaluator(){
  //confusionMatrix = new double*[classNumber];
  for (int i = 0; i < classNumber; i++) {
    //confusionMatrix[i] = new double[classNumber];

    delete []confusionMatrix[i];
  }
  delete []confusionMatrix;
}
/**
 * @brief Constructor with allocation of memory
 */
Evaluator::Evaluator(int classNumber) {
  this->classNumber = classNumber;
  confusionMatrix = new double*[classNumber];
  for (int i = 0; i < classNumber; i++) {
    confusionMatrix[i] = new double[classNumber];
    for (int j = 0; j < classNumber; j++) {
      confusionMatrix[i][j] = 0;
    }
  }
  total = 0;
  totalprecis = 0;
  totalrecall = 0;
  totalaccuracy = 0;
}
/**
 * @brief Print the result to the output stream
 */
void Evaluator::print(ostream& os) {
  // calculate the precision and recall
  double rowtotal[classNumber];
  double coltotal[classNumber];
  double recall[classNumber];
  double precis[classNumber];
  double accuracy[classNumber];
  double total=0;
  for (int i = 0; i < classNumber; i++) {
    rowtotal[i] = coltotal[i] = 0;
  }
  for (int i = 0; i < classNumber; i++) {
    for (int j = 0; j < classNumber; j++) {
      rowtotal[i] += confusionMatrix[i][j];
      coltotal[j] += confusionMatrix[i][j];
      total+=confusionMatrix[i][j];
    }
  }

  os << "\t";
  for (int i = 0; i < classNumber; i++) {
    os << (*classList)[i]->name << "\t";
  }
  os << "Recall" << "\t";
  os << endl;
  for (int i = 0; i < classNumber; i++) {
    os << (*classList)[i]->name << "\t";
    for (int j = 0; j < classNumber; j++) {
      os << setprecision(2) << confusionMatrix[i][j]*100 << "\t";      
    }
    // recall
    if (rowtotal[i] != 0) {
      recall[i] = (confusionMatrix[i][i] / rowtotal[i]) * 100;
      os << setprecision(2) << recall[i] << "%";

    } else {
      recall[i] = 0;
      os << "-";
    }

    os << endl;
  }
  os << "Precis" << "\t";
  for (int i = 0; i < classNumber; i++) {
    if (coltotal[i] != 0) {
      precis[i] = (confusionMatrix[i][i] / coltotal[i]) * 100;
      os << setprecision(2) << precis[i] << "%\t";
    } else {
      os << "-\t";
      precis[i] = 0;
    }
  }
  os << endl;

  os << "Accura" << "\t";
  for (int i = 0; i < classNumber; i++) {
    if (coltotal[i] != 0 && rowtotal[i] != 0) {
      accuracy[i] = (total-(coltotal[i] + rowtotal[i]-confusionMatrix[i][i]))/total * 100;
      os << setprecision(2) << accuracy[i] << "%\t";
    } else {
      os << "-\t";
      accuracy[i] = 0;
    }
  }
  os << endl;
  //os<<"Total:"<<total<<endl;

  int ntotal=classNumber;
  for (int i = 0; i < classNumber; i++) {
    totalprecis += precis[i];
    totalrecall += recall[i];
    totalaccuracy += accuracy[i];
    if(precis[i]==0 && recall[i]==0 && accuracy[i]==0){
      ntotal--;
    }
  }
  totalprecis /= ntotal;
  totalrecall /= ntotal;
  totalaccuracy /= ntotal;
  os << "Precis:" << totalprecis << "%" << endl;
  os << "Recall:" << totalrecall << "%" << endl;
  os << "Accuracy:" << totalaccuracy << "%" << endl;
}
/**
 * @brief Normalisation of confusion matrix
 */
void Evaluator::normalise() {
  for (int i = 0; i < classNumber; i++) {
    for (int j = 0; j < classNumber; j++) {
      confusionMatrix[i][j] /= total;
    }
  }
}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;
