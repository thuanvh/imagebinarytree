// a version of Shotton textonboost C#

#include "TextonBoost.h"
#include "JointBoost.h"

#include <lib/LibTIM/Common/Image.h>
#include "TextonBoostBounds.h"
#include "TextonBoost.h"
#include "TextonBoostBoundsEnumerator.h"
#include <vector>
#include <string>
#include <utility/filename.h>
#include <utility/imageIOText.h>
using namespace std;
using namespace LibTIM;
double fRand(double fMin, double fMax);

namespace TextonBoostNS {
  /**
   * @brief Constructor of classlist
   * @param classNames list of class name
   */
  ClassList::ClassList(vector<ImageClass*>& classNames) {
    for (int i = 0; i < classNames.size(); i++) {
      this->classNames.push_back(classNames[i]->name);
    }
    //this->classNames = classNames;

    // Generate toCompact
    toCompactLength = classNames.size();
    toCompact = new int[classNames.size()];
    int cCompact = 0;
    for (int i = 0; i < classNames.size(); i++)
      toCompact[i] = ((*classNames[i]).name[0] == '-') ? -1 : (cCompact++);

    // Generate toFull
    toFull = new int[cCompact];
    toFullLength = cCompact;
    cCompact = 0;
    for (int i = 0; i < classNames.size(); i++)
      if (toCompact[i] == cCompact){
        toFull[cCompact++] = i;
      }
       /* 
    for (int i = 0; i < classNames.size(); i++){
      cout<<this->classNames[i]<<" ";
    }
    for (int i = 0; i < classNames.size(); i++){
      cout<<toFull[i]<<" ";
    }
    for (int i = 0; i < classNames.size(); i++){
      cout<<toCompact[i]<<" ";
    }
    */
  }
  /**
   * @brief Get name of a class
   * @param cFull index of class name
   */
  string ClassList::GetName(int cFull) {
    if (!IsValid(cFull))
      throw "Class " + classNames[cFull] + " is not in use!";
    return classNames[cFull];
  }
  /**
   * @brief Get number of compact
   *
   */
  int ClassList::NumFull() {
    return toCompactLength;
  }
  /**
   * @brief Get number of full
   */
  int ClassList::NumCompact() {
    return toFullLength;
  }
  /**
   * @brief Is the index valide
   * @param cFull Index of name
   */
  bool ClassList::IsValid(int cFull) {
    return toCompact[cFull] != -1;
  }
  /**
   * @brief Get value compact.
   * @param cFull index of name
   */
  int ClassList::ToCompact(int cFull) {
    return toCompact[cFull];
  }
  /**
   * @brief Get full value
   * @param cCompact index of name
   */
  int ClassList::ToFull(int cCompact) {
    return toFull[cCompact];
  }

  /**
   * @brief Get shape filter string description
   *
   */
  string ShapeFilter::ToString() {
    stringstream resultstream;
    resultstream << "{\n";

    stringstream kStr;
    kStr << "{ ";
    if (wl->k == NULL)
      kStr << "}";
    else
      for (int i = 0; i < wl->kLength; i++) {
        kStr << (double)wl->k[i];
        kStr << (i < wl->kLength - 1 ? ", " : " }");
      }


    string cString = "{ ";
    for (int i = 0; i < classList->NumCompact(); i++)
      if (wl->n != NULL)
        if (wl->n->Contains(i))
          cString += classList->GetName(classList->ToFull(i)) + " ";
    cString += "}";
    resultstream << "wl = {error=" << wl->error << ", time=" << wl->timeTaken << ", a=" << wl->a << ", b=" << wl->b << ", k[]=" << kStr.str() << ", theta=" << wl->theta << ", classes=" << cString << " }\n";
    resultstream << "r = " << r->toString() << "\n";
    resultstream << "t = " << t << "\n";

    resultstream << "}\n";
    return resultstream.str();
  }

  /**
   * @brief Main function of textonboost
   */
  void TextonBoost::DoWork() {
    // Get list of training data
    //trainingDataFilenames ;///= (vector<ImageFilenamePair>) MainClass.Load("Filenames.Training");

    // Get number of textons

    //vector<vector<double>*> clusters;/// = (List<KMeans.Cluster>) MainClass.Load("Clusters");
    //numTextons = clusters.size();

    // Get class list
    classList = new ClassList(*ConfigurationManager::instance()->classList);



    // Create the booster object
    JointBoost booster(this);// = new JointBoost(this);

    cout << "Boosting from " << NumImages() << " images..." << endl;

    string shapeFilterFileName = "ShapeFilter.txt";
    ofstream ofs(shapeFilterFileName.c_str());
    // Iterate number of rounds
    for (int round = 0; round < numRounds; round++) {
      cout << "  Boosting round " << round << endl;
      if (round == 20) {
        int a = 0;
      }

      ShapeFilter sf;// = new ShapeFilter();
      sf.wl = booster.PerformRound();
      //sf.classList = classList; // NB stored sharing set uses *compact* class list
      sf.classList = new ClassList(*ConfigurationManager::instance()->classList);
      int d = sf.wl->d;
      sf.t = d / numShapes;
      sf.r = &SuperSample(shapes[d % numShapes], subSample);
      if (sf.wl->n == NULL)
        getchar();
      if (sf.wl->n == NULL)
        sf.wl->n = new SharingSet();

      cout << "    ShapeFilter = " << sf.ToString() << endl;

      ///MainClass.Save("ShapeFilter." + round, sf);
      ShapeFilter::write(ofs, sf);

    }
    ofs.close();

  }
  /**
   * @brief Calculate the integral image from the textonImage. We separate the textonMap by each type texton.
   * @param textonMap texton image
   * @param numTextons Number of texton
   * @param subSample A sub sample
   * @param subSampledSize size of a sub snample
   */
  Image<TLabel>** TextonBoost::CalculateIntegralImages(Image<TLabel>& textonMap, int numTextons, int subSample, Size& subSampledSize) {
    int ssW = subSampledSize.Width;
    int ssH = subSampledSize.Height;
    int W = textonMap.getSizeX();
    int H = textonMap.getSizeY();


    Image<TLabel>** integralImages = new Image<TLabel>*[numTextons];

    for (int textonNum = 0; textonNum < numTextons; textonNum++) {
      //sizes
      Image<TLabel>* currentImage = new Image<TLabel>(ssW, ssH, 1); //= new Image<int>(subSampledSize.Width,subSampledSize.Height,1);
      integralImages[textonNum] = currentImage;
      currentImage->fill(0);

      // First, down-sample
      for (int x = 0; x < W; x++)
        for (int y = 0; y < H; y++)
          if (textonMap(x, y) == textonNum) {
            int a = x / subSample;
            int b = y / subSample;
            (*currentImage)(a, b)++;
          }


      // Second, accumulate
      for (int x = 0; x < ssW; x++)
        for (int y = 0; y < ssH; y++)
          (*currentImage)(x, y) += ((x > 0) ? (*currentImage)(x - 1, y) : 0)
              + ((y > 0) ? (*currentImage)(x, y - 1) : 0)
              - ((x > 0 && y > 0) ? (*currentImage)(x - 1, y - 1) : 0);


    }

    return integralImages;
  }


  /**
   * @brief Initialise the shapes of yours
   */
  void TextonBoost::InitialiseShapes() {
    shapes = new Rectangle[numShapes];
    for (int s = 0; s < numShapes; s++) {
      shapes[s].Width = (int)fRand(minShapeSize, maxShapeSize + 1);
      shapes[s].Height = (int)fRand(minShapeSize, maxShapeSize + 1);
      shapes[s].X = (int)fRand(0, maxShapeSize + 1 - shapes[s].Width) - maxShapeSize / 2;
      shapes[s].Y = (int)fRand(0, maxShapeSize + 1 - shapes[s].Height) - maxShapeSize / 2;

      // Sub-sample shape
      shapes[s] = SubSample(shapes[s], subSample);
    }
  }

#define sign(x) ((x)<0?-1:(((x)>0)?1:0))
  /**
   * @brief Get a sous-sample
   */
  Rectangle& TextonBoost::SubSample(Rectangle& input, int subSample) {
    Rectangle* result = new Rectangle();

    result->Width = input.Width / subSample;
    result->Height = input.Height / subSample;

    result->X = ((input.X + sign(input.X) * subSample / 2) / subSample); // NB Care with sign
    result->Y = ((input.Y + sign(input.Y) * subSample / 2) / subSample); // NB Care with sign

    return *result;
  }

  /**
   * @brief Get a super-sample
   * @param input A rectangle input
   * @param subSample A subsample
   */
  Rectangle& TextonBoost::SuperSample(Rectangle& input, int subSample) {
    Rectangle* result = new Rectangle();

    result->Width = input.Width * subSample;
    result->Height = input.Height * subSample;

    result->X = input.X * subSample;
    result->Y = input.Y * subSample;

    return *result;
  }



  /**
   * @brief N
   */
  int TextonBoost::N() {
    return numExamples;

  }

  int TextonBoost::D() {
    return numTextons * numShapes;

  }

  int TextonBoost::C() {
    return classList->NumCompact();

  }

  int TextonBoost::ThetaStart() {
    return thetaStart;

  }

  int TextonBoost::ThetaInc() {
    return thetaInc;

  }

  int TextonBoost::NumThetas() {
    return numThetas;

  }

  double TextonBoost::AcceptFraction() {
    return acceptFraction;

  }

  int TextonBoost::NumImages() {

    return this->trainingDataFilenames.size();

  }
  /**
   * @brief Initialise of textonBoost
   */
  void TextonBoost::Initialise() {
    /// Initialise the shapes
    InitialiseShapes();

    /// Which training images to boost off?
    getFileList(trainingFolder.c_str(), trainingDataFilenames);
    //vector<int> imageList;
    int size = trainingDataFilenames.size();
    //for (int i = 0; i < size; i++) {
    //  imageList.push_back(i);
    //}


    /// Allocate memory
    integralImages = new Image<TLabel>**[size];//[];
    targets = new int*[size];//[];
    //validPoints = new Point*[size];//[];
    subSampledSizes = new Size[size];

    numExamples = 0;

    for (int i = 0; i < size; i++) {
      cout << "Loading training image " << trainingDataFilenames[i] << endl;

      /// Load images

      //Image<U8> u8;
      //Image<U8>::load((trainingFolder + "/" + trainingDataFilenames[i]).c_str(), u8);//= (Image<int>) MainClass.Load("TextonMaps." + trainingDataFilenames[imageList[i]].im);
      Image<TLabel> textonMap;
      getImageText((trainingFolder + "/" + trainingDataFilenames[i]).c_str(), textonMap);

      //= ImageIO.LoadImage<int>(trainingDataFilenames[imageList[i]].gt, new IdInputMapping<int>());
      //Image<U8>::load((groundTruthFolder + "/" + trainingDataFilenames[i]+".txt").c_str(), u8);
      //Image<TLabel>* groundTruth = u8;
      Image<TLabel> groundTruth;
      getImageText((groundTruthFolder + "/" + trainingDataFilenames[i]).c_str(), groundTruth);

      /// Calculate sub sampled image sizes
      subSampledSizes[i].Width = (textonMap.getSizeX() + subSample - 1) / subSample; // Care with off by one errors
      subSampledSizes[i].Height = (textonMap.getSizeY() + subSample - 1) / subSample; // Care with off by one errors

      /// Calculate integral images
      integralImages[i] = CalculateIntegralImages(textonMap, numTextons, subSample, subSampledSizes[i]);

      /// Ignore unused class labels
      vector<Point* >* tempList = new vector<Point*>;

      Bounds b(subSampledSizes[i]);
      //foreach(Point p in new Bounds(subSampledSizes[i])) {
      BoundsEnumerator be(&b);// = b.GetEnumerator();
      do {
        Point* p = new Point;
        *p = be.Current();
        int gtLabel = groundTruth(p->X * subSample, p->Y * subSample);
        if (gtLabel > 0 && classList->IsValid(gtLabel))
          tempList->push_back(p);
      } while (be.MoveNext());

      validPoints.push_back(tempList);;
      numExamples += validPoints[i]->size();

      /// Calculate target values
      int n = 0;
      targets[i] = new int[validPoints[i]->size()];

      //foreach(Point p in validPoints[i]) {
      for (int j = 0; j < validPoints[i]->size(); j++) {
        Point* p = (*validPoints[i])[j];
        int classFull = groundTruth(p->X * subSample, p->Y * subSample);
        targets[i][n++] = classList->ToCompact(classFull);
      }
    }

    /// Precompute allow examples
    cout << "Calculating allow examples..." << endl;
    Rectangle arect(-(maxShapeSize + 1) / 2, -(maxShapeSize + 1) / 2, maxShapeSize + 1, maxShapeSize + 1);
    Rectangle maxRect = SubSample(arect, subSample);
    allowExamples = new BoolArray[numTextons];
    for (int t = 0; t < numTextons; t++) {
      allowExamples[t] = BoolArray(N());

      int n = 0;
      for (int i = 0; i < NumImages(); i++) {
        Image<TLabel>* integralImage = integralImages[i][t];

        //foreach(Point p in validPoints[i])
        for (int j = 0; j < validPoints[i]->size(); j++) {
          Point* p = (*validPoints[i])[j];
          allowExamples[t].set(n++, CalculateShapeFilterResponse(*integralImage, maxRect, p) > 0);
        }

      }
    }
  }
  /**
   * @brief Get all points
   * @param target Get a list point target
   */
  void TextonBoost::GetTargets(int* targets) {
    int n = 0;

    for (int i = 0; i < NumImages(); i++) {
      for (int j = 0; j < this->validPoints[i]->size(); j++)
        targets[n++] = this->targets[i][j];
    }
  }
  /**
   * @brief Calculate the value of feature
   * @param values List of feature values
   * @param d Distance de shapes
   */
  void TextonBoost::CalculateFeatureValues(int* values, int d) {
    int t = d / numShapes;
    Rectangle shape = shapes[d % numShapes];
    //cout << "shape:" << shape.X << "," << shape.Y << "," << shape.Width << "," << shape.Height << endl;
    //getchar();

    int n = 0;

    for (int i = 0; i < NumImages(); i++) {
      Image<TLabel>* integralImage = integralImages[i][t];
      /*
      for (int i = 0; i < integralImage.getSizeX(); i++) {
        for (int j = 0; j < integralImage.getSizeY(); j++)
          cout << integralImage(i, j) << " ";
        cout << endl;
      }
      cout << "End of print integral image" << endl;
      getchar();
      */
      //foreach(Point p in validPoints[i]) {
      for (int j = 0; j < validPoints[i]->size(); j++) {
        Point* p = (*validPoints[i])[j];
        //cout << "(" << p->X << "," << p->Y << ")";
        if (!allowExamples[t].get(n))
          values[n] = 0;
        else
          values[n] = CalculateShapeFilterResponse(*integralImage, shape, p);


        n++;
      }
      //getchar();
    }
    /*
    for (int i = 0; i < n; i++) {
      cout << values[i] << " ";
    }
    cout << endl;*/
    //getchar();
  };

  /**
   * @brief Calculate summed response from integral image
   * @param integralImage An integral image
   * @param shape A shape
   * @param p List of point
   */
  int TextonBoost::CalculateShapeFilterResponse(Image<TLabel>& integralImage, Rectangle& shape, Point* p) {

    Size size;
    size.Height = integralImage.getSizeY();
    size.Width  = integralImage.getSizeX();


    int x1 = p->X + shape.X;
    int y1 = p->Y + shape.Y;
    int x2 = x1 + shape.Width;
    int y2 = y1 + shape.Height;

    // Check if rectangle is at least partially inside image bounds
    if (x1 >= size.Width || y1 >= size.Height || x2 <= 0 || y2 <= 0)
      return 0;

    int maxX = min(size.Width - 1, x2 - 1);
    int maxY = min(size.Height - 1, y2 - 1);

    int m = integralImage(maxX, maxY);
    int mx = x1 > 0 ? integralImage(x1 - 1, maxY) : 0;
    int my = y1 > 0 ? integralImage(maxX, y1 - 1) : 0;
    int mxy = x1 > 0 && y1 > 0 ? integralImage(x1 - 1, y1 - 1) : 0;

    int a = m - mx - my + mxy;
    if (isnan(a) || isnan(-a)) {
      int b = 0;
    }
    return a;
  }
  TextonBoost::TextonBoost() {
    // Other parameters
    numRounds = 700;/// = MainClass.NumRoundsBoosting;
    acceptFraction = 0.003;/// = MainClass.RandomizationFactor;
    subSample = 5;///MainClass.BoostingSubSample;

    numThetas = 20;//25;///MainClass.NumberOfThresholds;
    thetaStart = 5;///MainClass.ThresholdStart;
    thetaInc = 10;//40;///MainClass.ThresholdIncrement;

    numShapes = 100;///MainClass.NumberOfRectangles;
    minShapeSize = subSample;///MainClass.MinimumRectangleSize;
    maxShapeSize = 200;///MainClass.MaximumRectangleSize;
  }

}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;
