#ifndef CONFIGURATION_MANAGER_H
#define CONFIGURATION_MANAGER_H

#include "learning/ImageClass.h"
#include <vector>
using namespace std;


#define PARA_TEXTUREWAVELET 1
#define PARA_HOMOGENEITY 2
#define PARA_MEANCOLOR 4
#define PARA_REGIONSIZE 8
#define PARA_MEANGRADIENT 16
#define PARA_COLORCOVARIANCE 32


class ConfigurationManager{
private:
  void loadClassList();
  void removeClassList();
  static ConfigurationManager* Manager;
public:
  ConfigurationManager();
  ~ConfigurationManager();
  void loadConfig(const char* filename);
  vector<ImageClass*>* classList;
  
  static ConfigurationManager* instance();
  string ClassNameFile;//=classname.txt
  string LearningFileName;//=learning.txt
  
  string LearningFolder;//=learning-image
  string LearningClass;//=learning-data
  
  string TestFolder;//=test-image
  string TestClass;//=test-data
  string ResultFolder;//=result-data
  string ResultColor;//=result-color
  //#Test mode
#define BPT_CLASSIFY_AFTER_SEGMENTATION 0
#define BPT_SEGMENTATION_AFTER_CLASSIFIER 1
#define TEXTONBOOST 2
  int TestMode ;//= BPT_CLASSIFY_AFTER_SEGMENTATION
  
  //#Binary Partition Tree

  #define REGIONNUMBER 0
  #define EVOLVEMENT_NO_THRESHOLD 1
  #define EVOLVEMENT_THRESHOLD 2

  int BPTMode;//=EVOLVEMENT_NO_THRESHOLD
  int BPTRegionNumber;//=10
  float BPTEvolvementThreshold;//=0.1
  
#define SVM 0
#define KNN 1
#define SIFTSEARCH 2
  int Classifier;//=SVM

//#define
  int SVMType;//=C_SVM
  
  //#POLY, GAMA, RDF, LINEAR
  int SVMKernelType;//=POLY;
  int SVMDegree;//=3
  float SVMGamma;//=0.2
  int SVMC;//=2
  
  int KNNk;//=10

  string SIFTAllPointFile;
  float SIFTThreshold;
  //#Property
  
  bool UseColor;//=1
  bool UseTexture;//=1
  bool UseGFD;//=1
  bool UseColorLight;
  bool UsePosition;
  bool UseSIFT;
  string SIFTDictFile;
  
  int ColorSize;//=32
  int GFDAngle;//=5
  int GFDRadius;//=4
  int TextureGraySize;

  int BPTProperty;
};
#endif