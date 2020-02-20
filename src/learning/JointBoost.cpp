// This is a convert of the method Textonboost C#

#include "JointBoost.h"
#include <stdlib.h>
#include <iostream>
using namespace std;

double fRand(double fMin, double fMax);
/**
 * @brief Constructor
 */
JointBoost::JointBoost(IBoost* iBoost): C(iBoost->C()) {
  this->iBoost = iBoost;
  //rnd = new Random(0);  // Start from same random seed each time
  srand(0);
  iBoost->Initialise();

  this->N = iBoost->N();
  this->D = iBoost->D();
  //this->C = iBoost->C;

  // Initialise thresholds
  thetaStart = iBoost->ThetaStart();
  thetaInc = iBoost->ThetaInc();
  thetaV = new int[iBoost->NumThetas()];
  thetaVLength = iBoost->NumThetas();
  for (int t = 0; t < thetaVLength; t++) {
    thetaV[t] = thetaStart + t * thetaInc;
  }


  // Initialise weights and targets
  targets = new int[N];
  iBoost->GetTargets(targets);
  weights = new double*[N];
  for (int i = 0; i < N; i++)
    weights[i] = new double[C];
  for (int i = 0; i < N; i++) {
    for (int c = 0; c < C; c++) {
      weights[i][c] = 1.0;
    }
  }



  // Initialise scratch space
  pV = new double[C];
  tV = new double[C];
  kV = new double[C];
  featureValues = new int[N];
  qV = new double*[thetaVLength];
  uV = new double*[thetaVLength];
  for (int i = 0; i < thetaVLength; i++) {
    qV[i] = new double[C];
    uV[i] = new double[C];
  }

}
/**
 * @brief Print all the thing
 */
void JointBoost::printAll() {
  cout << endl << "****************************" << endl;
  cout << "pV:" << endl;
  for (int i = 0; i < C; i++)
    cout << pV[i] << " ";
  cout << endl;
  cout << "tV:" << endl;
  for (int i = 0; i < C; i++)
    cout << tV[i] << " ";
  cout << endl;
  cout << "kV:" << endl;
  for (int i = 0; i < C; i++)
    cout << kV[i] << " ";
  cout << endl;
  cout << "qV:" << endl;
  for (int i = 0; i < thetaVLength; i++) {
    for (int j = 0; j < C; j++)
      cout << qV[i][j] << " ";
    cout << endl;
  }


  cout << "uV:" << endl;
  for (int i = 0; i < thetaVLength; i++) {
    for (int j = 0; j < C; j++)
      cout << uV[i][j] << " ";
    cout << endl;
  }

  /*cout<<"weights:"<<endl;
  for(int i=0; i<N; i++){
    for(int j=0; j<C; j++)
      cout<<weights[i][j]<<" ";
    cout<<endl;
  }

  cout<<"Features:"<<endl;
  for(int i=0; i<N; i++)
    cout<<featureValues[i]<<" ";
  cout<<endl;
  */
}
/**
 * @brief Perform each round of boosting
 * @return A weakLearnin selected
 */
WeakLearner* JointBoost::PerformRound() {
  time_t start, end;
  time(&start);

  // Minimum search variables
  WeakLearner* minimum = new WeakLearner();
  minimum->error = HUGE_VAL;//FP_INFINITE;//double.PositiveInfinity;

  // Precompute vectors pV, tV, kV
  CalculateVectorsPTK();
  //printAll();
  bool a = false;
  for (int d = 0; d < D; d++) {
    // Randomisation

    if (fRand(0, 1) > iBoost->AcceptFraction())
      continue;
    //cout << "d=" << d << " ";
    // Calculate feature values
    iBoost->CalculateFeatureValues(featureValues, d);
    /*cout<<"Feature: ";
    for(int i=0; i<N; i++){
      cout<<featureValues[i]<<" ";
    }
    cout<<endl;
    */
    // Generate sums using histograms
    //CalculateVectorsQU(featureValues, qV, uV);
    CalculateVectorsQU(featureValues);
    //printAll();
    // Optimise over sharing
    WeakLearner* optimalSharing = OptimiseSharing(qV, uV);
    /*
    if (optimalSharing->n == 0) {
      cout << "optimize n=" << optimalSharing->n <<":"<< "qV, uV" << endl;
      for (int i = 0; i < thetaVLength; i++) {
        for (int c = 0; c < C;  c++) {
          cout << qV[i][c] << "," << uV[i][c] << " ";
        }
        cout << endl;
      }
      continue;
    }*/
    /*if (optimalSharing->n == 0) {
      continue;
    }*/
    if (optimalSharing->error < minimum->error) {
      delete minimum;
      minimum = optimalSharing;
      minimum->d = d; // Store the feature number
      a = true;
    }
    //cout << "error:" << optimalSharing->error << " ";
  }

  if (isfinite(minimum->error))
    UpdateWeights(*minimum, featureValues);
  //else
  //throw "Failed to find improved weak learner!";
  time(&end);

  minimum->timeTaken = difftime(end, start) * 1000;//miliseconds
  /*if (minimum->n == NULL) {
    getchar();
    cout << a << endl;
  }*/
  return minimum;
}
/**
 * @brief Calculate the vectors P T K
 */
void JointBoost::CalculateVectorsPTK() {
  for (int c = 0; c < C; c++) {
    pV[c] = tV[c] = 0;
    for (int i = 0; i < N; i++) {
      pV[c] += weights[i][c];
      tV[c] += weights[i][c] * (targets[i] == c ? 1.0 : -1.0);
    }
    kV[c] = tV[c] / pV[c];

    if (!isfinite(kV[c]) || !isfinite(tV[c]) || pV[c] == 0) {
      cout << "calculate PTK JB" << endl;
      for (int i = 0; i < N; i++) {
        cout << weights[i][c] << ",";
      }
      cout << endl;
      getchar();
    }

  }

}

/**
 * @brief Calculate vectors QU by histogram
 * @param featureValues Input feature value vector
 * @param qV Q vector output
 * @param uV U vector output
 */
void JointBoost::CalculateVectorsQU2(int* featureValues, double** qV, double** uV) {
  int* fvBase = featureValues;
  int* tBase = targets;
  double * qVBase = *qV, *uVBase = *uV, *wBase = *weights;
  int* fvPtr = fvBase, *tPtr = tBase;
  double* qVPtr = qVBase, *uVPtr = uVBase;
  double* wPtr = wBase;

  // Initialise
  for (int tc = 0; tc < thetaVLength * C; tc++) {
    qVPtr[tc] = 0;
    uVPtr[tc] = 0;
  }
  /*cout << "weight:";
  for (int c = 0; c < C; c++, wPtr++)
    cout << *wPtr << " ";
  cout << endl;
  getchar();
  */
  // Every example votes
  /*
  for (int tc = 0; tc < thetaVLength * C; tc++, qVPtr++, uVPtr++) {
    cout << *qVPtr << ",";
    cout << *uVPtr << " ";
  }
  cout << endl;
  getchar();
  */
  for (int i = 0; i < N; i++, fvPtr++, tPtr++) {
    // Calculate bin number
    int t = (*fvPtr - thetaStart) / thetaInc; //(int)((*fvPtr - thetaStart) / thetaInc);

    if (t > thetaVLength - 1) // Value too large - falls in final bin
      t = thetaVLength - 1;
    else
      if (t < 0) // Value too small - has no influence
        continue;

    int targetVal = *tPtr;

    qVPtr = qVBase + t * C;
    uVPtr = uVBase + t * C;
    for (int c = 0; c < C; c++) {
      //*qVPtr += *wPtr;
      //*uVPtr += *wPtr * (targetVal == c ? 1.0 : -1.0);
      qVPtr[c] += wPtr[c];
      uVPtr[c] += wPtr[c] * (targetVal == c ? 1.0 : -1.0);
    }
  }
  /*
  for (int tc = 0; tc < thetaVLength * C; tc++, qVPtr++, uVPtr++) {
    cout << *qVPtr << ",";
    cout << *uVPtr << " ";
  }

  cout << endl;
  getchar();
  */
  // Compute cumulative histograms
  qVPtr = qVBase + (thetaVLength - 2) * C;
  uVPtr = uVBase + (thetaVLength - 2) * C;
  for (int t = thetaVLength - 2; t >= 0; t--, qVPtr -= 2 * C, uVPtr -= 2 * C) {
    for (int c = 0; c < C; c++, qVPtr++, uVPtr++) {
      *qVPtr += *(qVPtr + C);
      *uVPtr += *(uVPtr + C);
    }
  }
  /*
  for (int tc = 0; tc < thetaVLength * C; tc++, qVPtr++, uVPtr++) {
    cout << *qVPtr << ",";
    cout << *uVPtr << " ";
  }
  cout << endl;
  getchar();
  */
}

/**
 * @brief Calculate the vector QU. Non-optimised version but it works in C++
 * @param featureValues feature value input
 */
void JointBoost::CalculateVectorsQU(int* featureValues) {
  // Initialise
  for (int t = 0; t < thetaVLength; t++)
    for (int c = 0; c < C; c++) {
      qV[t][c] = 0.0;
      uV[t][c] = 0.0;
    }

// Every example votes
  for (int i = 0; i < N; i++) {
    int t = min(thetaVLength - 1, (int)((featureValues[i] - thetaStart) / thetaInc));

    if (t < 0) // Value too small - has no influence
      continue;

    for (int c = 0; c < C; c++) {
      qV[t][c] += weights[i][c];
      //uV[t][c] += weights[i][c] * targets[i][c];
      uV[t][c] += weights[i][c] * (targets[i]==c?1.0:-1.0);
    }
  }

  // Compute cumulative histograms
  for (int t = thetaVLength - 2; t >= 0; t--)
    for (int c = 0; c < C; c++) {
      qV[t][c] += qV[t + 1][ c];
      uV[t][c] += uV[t + 1][ c];
    }
}

/**
 * @brief Optimise the sharing
 */
WeakLearner* JointBoost::OptimiseSharing(double** qV, double** uV) {
  // Quadratic O(C^2) cost algorithm for searching over possible sharing sets (see Torralba et al)

  SharingSet* mask = new SharingSet();
  WeakLearner** testWL = new WeakLearner*[C];
  for (int nBits = 0; nBits < C; nBits++) {
    testWL[nBits] = new WeakLearner();
    testWL[nBits]->error = HUGE_VAL;//FP_INFINITE;
  }

  SharingSet** tempN = new SharingSet*[C];
  for (int c = 0; c < C; c++) {
    tempN[c] = new SharingSet();
  }

  //// Try sharing between zero classes (i.e. only ks used)
  //WeakLearner optimalWLZero = OptimiseWeakLearner(new SharingSet(), qV, uV);
  //if (optimalWLZero.error > 0 && optimalWLZero.error < testWL[0].error)
  //    testWL[0] = optimalWLZero;

  // Optimise over number of bits allowed
  for (int nBits = 0; nBits < C; nBits++) {
    vector<SharingSet*> allowed;
    for (int bit = 0; bit < C; bit++) {
      if (mask->Contains(bit))
        continue;

      allowed.push_back(new SharingSet(*mask, bit));
    }
    for (int i = 0; i < allowed.size(); i++) {
      SharingSet* n = allowed[i];
      //foreach(SharingSet n in allowed) {
      //cout << "n: " << n->n << " ";
      WeakLearner* optimalWL = OptimiseWeakLearner(*n, qV, uV);
      //cout<<"error optimalwl:"<<optimalWL->error<<endl;
      if (optimalWL->error > 0 && optimalWL->error < testWL[nBits]->error) {
        delete testWL[nBits];
        testWL[nBits] = optimalWL;
        delete tempN[nBits];
        tempN[nBits] = n;        
      }else{
        delete n;
        delete optimalWL;
      }
    }

    mask = tempN[nBits];
  }

  // Choose nBits with lowest error
  WeakLearner* min = (testWL[0]);
  for (int nBits = 1; nBits < C; nBits++){
    if (testWL[nBits]->error < min->error){
      delete min;
      min = (testWL[nBits]);
    }      
    else
      delete testWL[nBits];
  }
  for(int i=0; i<C; i++) {
    delete tempN[i];
  }
  /*
    if (min->n == NULL) {
      cout << "optimise sharing" << endl;
      getchar();
    }
  */
  return min;
}
/**
 * @brief Find the weak learner
 */
WeakLearner* JointBoost::OptimiseWeakLearner(SharingSet& n, double** qV, double** uV) {
  WeakLearner* min = new WeakLearner();
  *min->n = n;
  min->k = new double[C];
  min->kLength = C;
  min->error = HUGE_VAL ;//FP_INFINITE;

  for (int tIndex = 0; tIndex < thetaVLength; tIndex++) {
    // Compute particular values of p, q, t, u
    double p = 0.0, q = 0.0, t = 0.0, u = 0.0;
    for (int c = 0; c < C; c++) {
      if (n.Contains(c)) {
        p += pV[c];
        q += qV[tIndex][c];
        t += tV[c];
        u += uV[tIndex][c];
      }
    }
    double a = 0.0, b = 0.0;
    //cout<<"testsolvematrix";
    if (/*!n.IsEmpty && */!SolveMatrix(p, q, t, u, a, b))
      continue;
    //cout<<"solvematrix";
    // Compute Jwse for this a, b, theta
    double Jwse = 0.0;
    for (int c = 0; c < C; c++) {
      double temp = 0;
      if (n.Contains(c))
        temp = pV[c] - 2.0 * a * uV[tIndex][c] - 2.0 * b * tV[c] +
            a * a * qV[tIndex][c] + b * b * pV[c] +
            2.0 * a * b * qV[tIndex][c];
      else
        temp = pV[c] - 2.0 * kV[c] * tV[c] + kV[c] * kV[c] * pV[c];
      Jwse += temp;
      //cout<<"Jwse="<<Jwse;
      if (!isfinite(Jwse) || !isfinite(-Jwse || isinf(Jwse))) {
        //return min;
        std::cout << " (" << temp << "," << a << "," << b << "," << kV[c] << "," << pV[c] << "," << uV[tIndex][c] << "," << qV[tIndex][c] << "," << Jwse << ") ";
        getchar();
      }

    }
    //cout<<endl;
    // If improves then store weak learner
    if (Jwse >= 0 && Jwse < min->error) {
      min->error = Jwse;
      min->theta = thetaV[tIndex];
      min->a = a;
      min->b = b;
      for (int c = 0; c < C; c++)
        min->k[c] = n.Contains(c) ? 0.0 : kV[c];
    }
  }

  /*if (min->n == NULL) {
    cout << "optimise weak leaner" << endl;
    getchar();
  }
  */
  return min;
}
/**
 * @brief Calculate a and b for this method
 */
bool JointBoost::SolveMatrix(double p, double q, double t, double u, double &a, double &b) {
  const double tol = 1e-6;

  double pq = p - q;
  double tu = t - u;

  a = 0.0;
  b = 0.0;

  if (abs(q) < tol || abs(pq) < tol) {

    return false;
  }


  b = tu / pq;
  a = (u / q) - b;

  return true;
}

void JointBoost::ContinueFrom(vector<WeakLearner*> wls) {
  for (int i = 0; i < wls.size(); i++) {
    WeakLearner* wl = wls[i];
    UpdateWeights(*wl, featureValues);
  }
  //foreach(WeakLearner wl in wls)

}
/**
 * @brief Update weights with the new weak learner and the feature values
 */
void JointBoost::UpdateWeights(WeakLearner& wl, int* featureValues) {
  // Calculate feature values
  iBoost->CalculateFeatureValues(featureValues, wl.d);
  double maxValue = 0;
  // Update the weights
  for (int i = 0; i < N; i++) {
    // Calculate confidence value h_m for those classes in sharing set
    double confidence = (featureValues[i] > wl.theta) ? (wl.a + wl.b) : wl.b;

    for (int c = 0; c < C; c++) {
      double a = weights[i][c] * exp(-(targets[i] == c ? 1.0 : -1.0) * (wl.n->Contains(c) ? confidence : wl.k[c]));;
      //if(!isinf(a))
      weights[i][c] = a;
      if (isinf(a)) {
        cout << "inf error at update weight:" << endl;
        cout << a << "," << targets[i] << "," << c << "," << wl.n->Contains(c) << "," << confidence << "," << wl.k[c] << "," << featureValues[i] << "," << wl.theta << "," << wl.a << "," << wl.b << endl;
        getchar();
        //int a=0;
      }
      maxValue = max(abs(weights[i][c]), maxValue);
    }

  }
  // Normalize all weight
  for (int i = 0; i < N; i++) {
    for (int c = 0; c < C; c++) {
      weights[i][c] /= maxValue;
    }
  }
  /*cout<<"Weights:"<<endl;
  for(int i=0; i<N; i++){
    for(int c=0; c<C; c++){
      cout<<weights[i][c]<<",";
    }
    cout<<endl;
  }*/
}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;

