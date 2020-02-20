#ifndef JOINT_BOOST_H
#define JOINT_BOOST_H

#include <stdio.h>
#include <cmath>
#include <math.h>
#include <vector>
#include <float.h>
#include <time.h>
#include <iosfwd>
#include <iostream>
#include <stdlib.h>

using namespace std;
/**
 * @brief Interface of boosting
 */
class IBoost {
public:
  virtual void Initialise() = 0;

  virtual int N() = 0;
  virtual int D() = 0;
  virtual int C() = 0;
  virtual int ThetaStart() = 0;
  virtual int ThetaInc() = 0;
  virtual int NumThetas() = 0;
  virtual double AcceptFraction() = 0;

  virtual void CalculateFeatureValues(int* values, int d) = 0;
  virtual void GetTargets(int* targets) = 0;
};
/**
 * @brief Set of sharing classes. It contains only a integer number with each bit for the present of each class
 */
struct SharingSet {


public:
  int n;
  SharingSet() {n=0;};
  SharingSet(const SharingSet& from){
    n=from.n;
  }
  SharingSet(const SharingSet& from, int cNew) {
    n = from.n;
    n |= (1 << cNew);
  }
  void operator=(const SharingSet& from){
    n=from.n;
  }
  bool IsEmpty() {
    return n == 0;
  }

  bool Contains(int c) {
    /*if(c==0){
      cout<<"asking 0";
      getchar();
    }*/
    int a = 1 << c;
    return (n & a) != 0;
  }
  static void load(istream& ist, SharingSet& ss) {
    ist>>ss.n;
  }
  static void write(ostream& os, SharingSet& ss) {
    os << ss.n << " ";
  }
};

/**
 * @brief WeakLearner is the weak learner used to classify a subset of classes with the other
 */
struct WeakLearner {
public:
  int d; // Feature number
  double error; // Training error
  double a, b; // Confidence weights
  int theta; // Threshold
  SharingSet* n; // Sharing set
  double* k; // Constants for classes not in the sharing set
  double timeTaken; // Time taken to optimise this weak learner (milliseconds)
  int kLength;
  WeakLearner(){
    k=NULL;
    n=new SharingSet();
  }
  ~WeakLearner(){
    if(k!=NULL)
      delete[] k;
    if(n!=NULL)
      delete n;
  }
  /**
   * @brief Load weak learner from input stream
   */
  static void load(istream& is, WeakLearner& wl) {
    is >> wl.d;
    is >> wl.error;
    is >> wl.a;
    is >> wl.b;
    is >> wl.theta;
    is >> wl.kLength;
    is >> wl.timeTaken;
    wl.k = new double[wl.kLength];
    for (int i = 0; i < wl.kLength; i++) {
      is >> wl.k[i];
    }
    wl.n = new SharingSet();
    SharingSet::load(is, *wl.n);
  }
  /**
   * @brief Write weak learner to output stream
   */
  static void write(ostream& os, WeakLearner& wl) {
    os << wl.d << " ";
    os << wl.error << " ";
    os << wl.a << " ";
    os << wl.b << " ";
    os << wl.theta << " ";
    os << wl.kLength << " ";
    os << wl.timeTaken << " ";
    //wl.k=new double[kLength];
    for (int i = 0; i < wl.kLength; i++) {
      os << wl.k[i] << " ";
    }

    SharingSet::write(os, *wl.n);

  }
};
/**
 * @brief The boosting classifier
 */
class JointBoost {

private:
  IBoost* iBoost;
  //Random rnd;
public:
  int N; // Num Examples
  int D; // Num Features
  const int C; // Num Classes
  int thetaStart, thetaInc;
  int thetaVLength;
  int* thetaV; // Thresholds
  double** weights; // Weights
  int* targets; // Target values

  // Scratch-space
  double* pV, *tV, * kV;
  int* featureValues;
  double** qV, ** uV;

  JointBoost(IBoost* iBoost);
  WeakLearner* PerformRound();
  void CalculateVectorsPTK();
  void CalculateVectorsQU2(int* featureValues, double** qV, double** uV);
  void CalculateVectorsQU(int* featureValues);
  WeakLearner* OptimiseSharing(double** qV, double** uV);
  WeakLearner* OptimiseWeakLearner(SharingSet& n, double** qV, double** uV);
  static bool SolveMatrix(double p, double q, double t, double u, double &a, double &b);
  void ContinueFrom(vector<WeakLearner*> wls);
  void UpdateWeights(WeakLearner& wl, int* featureValues);
  void printAll();
};
#endif

// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;  replace-tabs on;
