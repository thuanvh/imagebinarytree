// a version of Shotton textonboost C#

#ifndef TEXTON_BOOST_H
#define TEXTON_BOOST_H

#include "JointBoost.h"
#include "TextonBoostBoolArray.h"

#include <vector>
#include <string>
#include <lib/LibTIM/Common/Image.h>
#include <config/ConfigurationManager.h>
#include "TextonBoostBounds.h"
using namespace std;
using namespace LibTIM;

namespace TextonBoostNS {


  class Rectangle {
  public:
    /// Constructor
    Rectangle() {};
    /// Constructor
    Rectangle(int x, int y, int w, int h) {
      X = x;
      Y = y;
      Width = w;
      Height = h;
    }
    ///X
    int X;
    ///Y
    int Y;
    /// width
    int Width;
    /// height
    int Height;
    /// get text
    string toString() {

      stringstream str;
      str << "{" << X << "," << Y << "," << Width << "," << Height << "}";
      return str.str();
    }
    static void load(istream& is, Rectangle& rect) {
      is >> rect.X;
      is >> rect.Y;
      is >> rect.Width;
      is >> rect.Height;
    }
    static void write(ostream& os, Rectangle& rect) {
      os << rect.X << " ";
      os << rect.Y << " ";
      os << rect.Width << " ";
      os << rect.Height << " ";
    }
  };

  /**
   * ClassList
   */
  class ClassList {
  private:
    vector<string> classNames;
    int* toCompact;
    int* toFull;
    int toCompactLength;
    int toFullLength;
  public:
    ClassList() {};
    ClassList(vector<ImageClass*>& classes);
    ~ClassList(){
      //for(int i=0; i<toCompactLength; i++)
      //  delete toCompact[i];
      //for(int i=0; i<toFullLength; i++)
      //  delete toFull[i];
      delete []toCompact;
      delete []toFull;
    }
    string GetName(int cFull);

    int NumFull();

    int NumCompact();
    bool IsValid(int cFull);

    int ToCompact(int cFull);

    int ToFull(int cCompact);
    static void load(istream& is, ClassList& cl) {
      is >> cl.toCompactLength;
      is >> cl.toFullLength;
      cl.toCompact = new int[cl.toCompactLength];
      cl.toFull = new int[cl.toFullLength];
      for (int i = 0; i < cl.toCompactLength; i++) {
        is >> cl.toCompact[i];
      }
      for (int i = 0; i < cl.toFullLength; i++) {
        is >> cl.toFull[i];
      }
      int classNameSize = 0;
      is >> classNameSize;
      for (int i = 0; i < classNameSize; i++) {
        string a;
        is >> a;
        cl.classNames.push_back(a);
      }
    }
    static void write(ostream& os, ClassList& cl) {
      os << cl.toCompactLength << " ";
      os << cl.toFullLength << " ";

      for (int i = 0; i < cl.toCompactLength; i++) {
        os << cl.toCompact[i] << " ";
      }
      for (int i = 0; i < cl.toFullLength; i++) {
        os << cl.toFull[i] << " ";
      }

      os << cl.classNames.size() << " ";
      for (int i = 0; i < cl.classNames.size(); i++) {
        os << cl.classNames[i] << " ";
      }
    }
  };
  /**
   * Shapes filter
   *
   */
  struct ShapeFilter {
  public:
    WeakLearner* wl; // The weak learner
    Rectangle* r; // The rectangle
    int t; // The texton index
    ClassList* classList; // The class list - needed to map full class numbers to compact ones
    ShapeFilter(){
      wl=NULL;
      r=NULL;
      classList=NULL;
    }
    ~ShapeFilter(){
      if(wl!=NULL){
        delete wl;
      }
      if(r!=NULL){
        delete r;
      }
      if(classList!=NULL){
        delete classList;
      }
    }
    string ToString();
    static void load(istream& is, ShapeFilter& sf) {
      sf.wl = new WeakLearner();
      sf.r = new Rectangle();
      sf.classList = new ClassList();
      is >> sf.t;
      WeakLearner::load(is, *sf.wl);
      ClassList::load(is, *sf.classList);
      Rectangle::load(is, *sf.r);
    }
    static void write(ostream& os, ShapeFilter& sf) {
      os << sf.t << " ";
      WeakLearner::write(os, *sf.wl);
      ClassList::write(os, *sf.classList);
      Rectangle::write(os, *sf.r);
      os <<endl;
    }
  };
  /**
   * Texton boost
   */
  class TextonBoost : IBoost {

    // Threshold parameters
  private:
    int numThetas;
    int thetaStart, thetaInc;

    // Shape parameters
    int minShapeSize, maxShapeSize, numShapes;

    // Misc parameters
    int subSample;
    double acceptFraction;

    int numRounds;

    // Fields
    int numExamples;

    BoolArray* allowExamples;
    Rectangle* shapes;
    Image<TLabel>*** integralImages;
    int** targets;
    Size* subSampledSizes;

    vector<vector<Point*>*> validPoints;

    ClassList* classList;
    vector<string> trainingDataFilenames;

    void InitialiseShapes();

  public:
    string trainingFolder;
    string groundTruthFolder;
    int numTextons;
    void DoWork();
    TextonBoost();
    static Image<TLabel>** CalculateIntegralImages(Image<TLabel>& textonMap, int numTextons, int subSample, Size& subSampledSize);
    static Rectangle& SubSample(Rectangle& input, int subSample);
    static Rectangle& SuperSample(Rectangle& input, int subSample);

    int N();
    int D();
    int C();
    int ThetaStart();
    int ThetaInc();
    int NumThetas();
    double AcceptFraction();
    int NumImages();
    void Initialise();
    void GetTargets(int* targets);
    void CalculateFeatureValues(int* values, int d);
    // Calculate summed response from integral image
    static int CalculateShapeFilterResponse(Image<TLabel>& integralImage, Rectangle& shape, Point* p);
  };


};
#endif

// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;
