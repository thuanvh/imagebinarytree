#include <learning/textonizer.h>
#include <learning/kmeanClustering.h>
#include <iostream>
#include <algo/convertImage.h>
#include <algo/segmentColorer.h>
#include <learning/TextonBoost.h>
#include <learning/TextonBoostEvaluation.h>
#include <learning/evaluator.h>
using namespace std;
using namespace TextonBoostNS;
void printVector1(vector<double>* a) {
  for (int i = 0; i < a->size(); i++) {
    cout << (*a)[i] << " ";
  }
  cout << endl;
}
void printVector1(vector<vector<double>*>* a) {
  for (int i = 0; i < a->size(); i++) {
    for (int j = 0; j < (*a)[i]->size(); j++) {
      cout << (*(*a)[i])[j] << " ";
    }
    cout << endl;
  }

}
double fRand(double fMin, double fMax);
void testKmean() {
  srand(time(NULL) + 2000);
  int vectorSize = 2;
  double min = 0;
  double max = 200;
  //vector<double> valueMax(vectorSize, max);
  //vector<double> valueMin(vectorSize, min);


  int numberPoint = 100;
  vector<vector<double>*> data(numberPoint);
  for (int i = 0; i < numberPoint; i++) {
    data[i] = new vector<double>(vectorSize);
    for (int j = 0; j < vectorSize; j++) {
      (*data[i])[j] = fRand(min, max);
      cout << (*data[i])[j] << " ";
    }
    cout << endl;
  }
  // clustering
  Kmean km;
  //km.max = &valueMax;
  //km.min = &valueMin;
  km.dimension = vectorSize;
  km.k = 3;
  km.EPSILON = 0.0005;
  km.maxTimes = 200;
  //km.initCentroids();

  //printVector1(&valueMax);
  //printVector1(&valueMin);
  cout << "centre before" << endl;
  printVector1(&km.centroids);

  vector<TLabel> result(data.size(), 0);

  km.clustering(data, result);
  cout << "centre after" << endl;
  printVector1(&km.centroids);

  Image<TLabel> image(max, max, 1);
  image.fill(0);

  for (int i = 0; i < numberPoint; i++) {
    image((*data[i])[0], (*data[i])[1]) = result[i] + 1;
  }

  segmentColorer* sc = segmentColorer::instance();
  sc->initRandomColor(km.k + 1);
  Image<U8C3>* image2 = sc->showSegmentationRandomColorFix(image, km.k + 1);
  for (int i = 0; i < km.centroids.size(); i++) {
    (*image2)((*km.centroids[i])[0], (*km.centroids[i])[1]) = 255;
  }
  image2->save("kmeans.ppm");

  delete image2;
  delete sc;
  for (int i = numberPoint - 1; i >= 0; i--) {
    vector<double>* a = data[i];
    data.pop_back();

    delete a;
  }
}
void testTexton() {
  Textonizer tt;
  tt.inputfolder = "learning-image";
  tt.outputfolder = "texton-data";
  tt.rescale = 2;
  tt.nbCluster = 10;
  tt.outputfolderColor = "texton-color";
  tt.colorFile = "colorTexton.txt";
  tt.centroidsFile = "centroids.txt";
  tt.isLearning = true;

  tt.textonize();
}
void testConvolution() {
  Image<U8> a;
  float data[] = {
    1, 1, 0, 2, 0, 0,
    3, 1, 0, 4, 0, 0,
    3, 1, 0, 4, 0, 0,
    4, 1, 0, 4, 0, 0,
    7, 1, 0, 2, 0, 0,
    2, 1, 0, 1, 0, 0,
    5, 1, 0, 0, 0, 0
  };
  /*
   u char data[] =* {
     1, 0, 0, 1, 0, 0, 3, 0, 0,
     4, 0, 0, 4, 0, 0, 4, 0, 0,
     2, 0, 0, 2, 0, 0, 4, 0, 0
   };*/
  TSize size[] = {6, 7, 1};
  TSpacing space[] = {1, 1, 1};
  Image<float> image(size, space, (float*) data);
  for (int i = 0; i < image.getSizeX(); i++) {
    for (int j = 0; j < image.getSizeY(); j++) {
      cout << image(i, j) << " ";
    }
    cout << endl;
  }
  cout << "=============" << endl;
  Kernel1D<float> k(3, 1);
  for (int i = 0; i < k.size; i++)
    k.data[i] = i;
  k.printData();
  cout << "=============" << endl;
  Image<float> out = k.ConvolveY(image, Extend);
  for (int i = 0; i < out.getSizeX(); i++) {
    for (int j = 0; j < out.getSizeY(); j++) {
      cout << out(i, j) << " ";
    }
    cout << endl;
  }
}
void testConvolutionGauss() {
  Kernel2D<float>* k = Kernel2D<float>::CreateGaussian(10);
  Image<U8C3> a;
  Image<U8C3>::load("56028.jpg", a);
  Image<float> b(a.getSize());
  cvtRGB2Gray<float>(a, b);
  Image<float> out(a.getSize());
  k->Convolve(out, b, Extend);
  Image<U8> gray = out;
  gray.save("56028.gray.pgm");

  Image<float> imgchains[3];
  Image<float> imgouts[3];
  Image<U8> imgoutsu8[3];
  for (int i = 0; i < 3; i++)
    imgchains[i].setSize(a.getSize());
  divide3Chains<float>(a, imgchains);
  for (int i = 0; i < 3; i++) {
    imgouts[i].setSize(imgchains[i].getSize());
    k->Convolve(imgouts[i], imgchains[i], Extend);
    char str[255];
    sprintf(str, "56028.%d.pgm", i);
    imgoutsu8[i] = imgouts[i];
    imgoutsu8[i].save(str);
  }
  Image<U8C3> imgoutcolor(a.getSize());
  merge3Chains<float>(imgoutcolor, imgouts);
  imgoutcolor.save("56028.gauss.ppm");
}
void testTextonBoost(int isLearn) {

  Textonizer tt;
  tt.rescale = 0.7;
  tt.nbCluster = 400;
  tt.kmeansMaxTimes = 20;
  //int isLearn = 2;
  // Textonization of learning image
  switch (isLearn) {
    case 0:
      tt.inputfolder = "learning-image";
      tt.outputfolder = "texton-data";
      tt.outputfolderColor = "texton-color";
      tt.colorFile = "colorTexton.txt";
      tt.centroidsFile = "centroids.txt";
      tt.isLearning = true;
      tt.textonize();
      break;
    case 1:
      tt.inputfolder = "test-image";
      tt.outputfolder = "texton-data-test";
      tt.outputfolderColor = "texton-color-test";
      tt.colorFile = "colorTexton.txt";
      tt.centroidsFile = "centroids.txt";
      tt.isLearning = false;
      tt.textonize();
      break;
    case 2: {
      TextonBoost ttb;
      ttb.numTextons = tt.nbCluster;
      ttb.groundTruthFolder = "learning-data";
      ttb.trainingFolder = "texton-data";
      ttb.DoWork();
      break;
    }
    case 3: {
      Evaluate eval;
      eval.numTextons = tt.nbCluster;
      eval.NumRounds=700;
      eval.SubSample=1;
      eval.testFolder = "texton-data-test";
      eval.outputFolder="result-data";
      eval.outputColorFolder="result-color";
      eval.DoWork();
      break;
    }
    case 4:{
      string folderResult = "result-data";//"result-data";
      string folderTruth = "test-data";;//"learning-data";
      cout << folderResult << endl;
      vector<string> fileList;
      getFileList(folderResult.c_str(), fileList);
      
      int size = ConfigurationManager::instance()->classList->size();
      Evaluator eval(size);
      eval.classList = ConfigurationManager::instance()->classList;
      cout << "Evaluation:" << endl;
      for (int i = 0; i < fileList.size(); i++) {
        string file = fileList[i];
        cout<<file<<endl;
        Image<TLabel> result;
        getImageText((folderResult + "/" + file).c_str(), result);
        
        Image<TLabel> truth;
        getImageText((folderTruth + "/" + file).c_str(), truth);
        cout<<file<<endl;
        eval.evaluate(result, truth);
      }
      eval.normalise();
      eval.print(cout);
    }


  }

  // Textonization of testing image
  /*

  */
  /*

  */
}
int testIntegralImage() {
  Image<U8> a;
  /*int data[] = {
    1, 1, 0, 2, 0, 0,
    3, 1, 0, 4, 0, 0,
    3, 1, 0, 4, 0, 0,
    4, 1, 0, 4, 0, 0,
    4, 1, 0, 2, 0, 0,
    2, 1, 0, 1, 0, 0,
    2, 1, 0, 0, 0, 0
  };*/
  TLabel data[] = {
    1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 2, 0
  };
  /*
   *   u char data[] =* {
   *     1, 0, 0, 1, 0, 0, 3, 0, 0,
   *     4, 0, 0, 4, 0, 0, 4, 0, 0,
   *     2, 0, 0, 2, 0, 0, 4, 0, 0
   };*/
  TSize size[] = {6, 7, 1};
  TSpacing space[] = {1, 1, 1};
  Image<TLabel> image(size, space, (TLabel*) data);
  int subSample = 2;
  int nbCluster = 2;
  Size subSampledSize;
  subSampledSize.Width = (image.getSizeX() + subSample - 1) / subSample; // Care with off by one errors
  subSampledSize.Height = (image.getSizeY() + subSample - 1) / subSample; // Care with off by one errors
  Image<TLabel>** list = TextonBoost::CalculateIntegralImages(image, nbCluster, subSample, subSampledSize);
  for (int i = 0; i < nbCluster; i++) {
    for (int x = 0; x < image.getSizeX(); x++) {
      for (int y = 0; y < image.getSizeY(); y++) {
        cout << (*list[i])(x, y) << ", ";
      }
      cout << endl;
    }
    cout << endl;
  }


}

int main(int argc, char** argv) {
  //testKmean();
  //testConvolution();
  //testConvolutionGauss();
  //testTexton();
  int isLearn=2;
  if(argc>1){
    isLearn=atoi(argv[1]);
  }
  
  testTextonBoost(isLearn);
  //testTextonBoostShotton(isLearn);
  //cout<<2.224e+223 * exp(662.821);

  //testIntegralImage();
  //cout<< (1<<1)<<endl;
  return 0;
}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;
