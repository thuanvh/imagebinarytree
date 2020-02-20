#ifndef TEXTONIZER_H
#define TEXTONIZER_H

#include <vector>
#include <string>
#include <lib/LibTIM/Common/Image.h>
#include "algo/convolution.h"
using namespace std;
using namespace LibTIM;
class FilterBankElement
{
public:
  Kernel2D<float>* filter;
  int colourBand;
  bool isGray;
  FilterBankElement(){}
  FilterBankElement(Kernel2D<float>* filter, int colourBand, bool isGray)
  {
    this->filter = filter;
    this->colourBand = colourBand;
    this->isGray=isGray;
  }
};

class Textonizer{
private:

  
  vector<string> fileList;
  vector<Image<TLabel>*> imageLabelList;
  vector<Image<U8C3>*> imageOriginalList;
  vector<vector<double>* >* responses;
  vector<TLabel> labelresponses;
  vector<FilterBankElement*> filterBank;
  vector<double>* valueMax,*valueMin;

  
  void initFilterBank();
  void convolution();
  void convolutionItem(Image<U8C3>& image,vector<vector<double>*> & valueResponse, int &id);
  void init();
  
  void textonClustering();
  void textonLabeling(vector<vector<double>*>& centroids);
  void saveTextonImage();
  void loadCentroids(vector<vector<double>*>& centroids);
public:
  int vectorSize;
  int nbCluster;
  int kmeansMaxTimes;
  string inputfolder;
  string centroidsFile;
  string outputfolder;
  string outputfolderColor;
  string colorFile;
  double rescale;
  bool isLearning;
  
  void textonize();
  Textonizer();
  ~Textonizer();
  void SaveFilterBank(vector<FilterBankElement*>& bank, const char* filename);
  void LoadFilterBank(vector<FilterBankElement*>& bank, const char* filename);
  //void Labeling(vector<vector<double>* >& label, vector<vector<double>*>& centroids);
  void Labeling(vector<TLabel>* label, vector<vector<double>*>& centroids);
};

#endif