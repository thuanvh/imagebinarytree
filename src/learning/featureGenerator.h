#ifndef _FEATURE_GENERATOR_H_
#define _FEATURE_GENERATOR_H_

#include <vector>
#include "../lib/LibTIM/Common/Image.h"
#include <config/ConfigurationManager.h>
#include <algo/SIFT.h>
using namespace std;

using namespace LibTIM;

class featureGenerator {

public:
  featureGenerator();
  ~featureGenerator();
  /// Texture gray size
  int ColorSize;
  int GFDAngle;
  int GFDRadius;
  int TextureGraySize;
  void generate(Image<U8C3> const & image, vector<double>* featureVector);
  bool useColor;
  bool useTexture;
  bool useGFD;
  bool useColorLight;
  bool usePosition;
  bool useSIFT;
  string siftDictFile;
  vector<SIFTPoint*> dict;
  void load(ConfigurationManager* config);
};

#endif
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
