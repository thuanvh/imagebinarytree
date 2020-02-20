#include "featureGenerator.h"
#include "texture.h"
#include "../algo/convertImage.h"
#include "humoment.h"
#include "GFT.h"
#include <Common/Histogram.h>
#include <algo/SIFT.h>
#include <iostream>
#include <fstream>
/**
 * @brief Normalisation the vector by the normal distribution
 */
void normalize(double* feature, int size) {
  // Normalize par ecartype
  double sum = 0;
  for (int i = 0; i < size; i++) {
    //rotate and scale normalisation
    sum += feature[i];
  }

  double avg = sum / size;
  double ecarttype = 0;
  for (int i = 0; i < size; i++) {
    ecarttype += pow(feature[i] - avg, 2);
  }
  ecarttype = sqrt(ecarttype / size);
  // normalise
  for (int i = 0; i < size; i++) {
    feature[i] = (feature[i] - avg) / ecarttype;
  }
}
/**
 * @brief Generate of image and feature vector
 */
void featureGenerator::generate(const LibTIM::Image< U8C3 >& image, std::vector< double >* featureVector) {
  int colid = 0;
  /// generate the textures feature
  Image<U8> gray(image.getSize());
  cvtRGB2Gray(image, gray);
  if(useTexture){
    // initialize list of matrix co-occurence
    double*** concurrenceArray = new double**[NUM_MATRIX];
    
    for (int i = 0; i < NUM_MATRIX; i++) {
      concurrenceArray[i] = new double*[TextureGraySize];
      for (int j = 0; j < TextureGraySize; j++) {
        concurrenceArray[i][j] = new double[TextureGraySize];
        for (int k = 0; k < TextureGraySize; k++) {
          concurrenceArray[i][j][k] = 0;
        }
      }
    }
    double textureVector[NUM_MATRIX * 15];
    int textureVectorSize = 0;
    //cout << "extract texture" << endl;
    extractTexture(gray, TextureGraySize, concurrenceArray, textureVector, textureVectorSize);
    //cout << "end extract texture" << endl;
    freeMatrix(concurrenceArray, TextureGraySize);
    // normalize texture vector
    normalize(textureVector, textureVectorSize);
    for (int i = 0; i < textureVectorSize; i++)
      featureVector->push_back((textureVector)[i]);
  }

  /*
  double huMoments[8];
  getHuMoment(gray,huMoments);
  for(int i=0; i<8; i++){
    featureVector->push_back(huMoments[i]);
  }
  */
  if(useColor){
    // Histogram
    //int colorSize = colorSize;
    //Convertir l'espace de couleur
    Image<U8C3> srclab(image.getSize());
    //cvtColor(dst, srclab, CV_BGR2Lab);
    cvtRGB2Lab(image, srclab);
    //resizeImageColor(srclab, dst, colorSize);
    
    // Calculer Histogramme
    int sizeHist[] = {ColorSize, ColorSize, ColorSize};
    int max[] = {256, 256, 256};
    int* channelsIndex;//=new int[3];{ 0, 1, 2};
    int numberChannels = 0;
    if(useColorLight){
      channelsIndex = new int[3];
      channelsIndex[0]=0;
      channelsIndex[1]=1;//{ 1, 2};
      channelsIndex[2]=2;//{ 1, 2};
      numberChannels = 3;
    }else{
      channelsIndex = new int[2];
      channelsIndex[0]=1;
      channelsIndex[1]=2;//{ 1, 2};
      numberChannels = 2;
    }

    
    Image<U8C3> blackrgb(1,1,1);
    blackrgb.fill(0);
    Image<U8C3> blacklab(1,1,1);
    cvtRGB2Lab(blackrgb,blacklab);
    U8C3 black=blacklab(0,0);
    Histogram hist(srclab, max, sizeHist, channelsIndex, numberChannels,&black);
    hist.normalize();
    hist.write(*featureVector);
    //delete []channelsIndex;
  }

  if(useGFD){
    // GFT
    const int r = GFDRadius;
    const int angle = GFDAngle;
    double gft[r*angle];
    GFT::getGFT(gray, r, angle, gft);
    //cout << "out value:" << gft[0];
    int size = r * angle;
    for (int i = 0; i < size; i++) {
      featureVector->push_back(gft[i]);
    }
    cout << "end of gft" << endl;
  }

  if(usePosition){
    int possize=10;
    int pos2=possize*possize;
    int pos[pos2];
    for(int i=0; i<pos2; i++){
      pos[i]=0;
    }
    int sizex=image.getSizeX();
    int sizey=image.getSizeY();
    for(int x=0; x<sizex; x++){
      for(int y=0; y<sizey; y++){
        int posx=(int)((x/(float)sizex)*possize);
        int posy=(int)((y/(float)sizey)*possize);
        if(image(x,y)!=0){
          pos[posy*possize+posx]=1;
        }
      }
    }
    for(int i=0; i<pos2; i++){
      featureVector->push_back(pos[i]);
    }
    /*
    for(int i=0; i<possize; i++){
      for(int j=0; j<possize; j++){
        cout<<pos[i*possize+j];
      }
      cout<<endl;
    }
    getchar();*/
  }

  if(useSIFT){
    SIFTAdapter sift;
    vector<SIFTPoint*> imagePointList;
    sift.getSiftPoint(gray,imagePointList);
    int dictsize=dict.size();
    double index[dictsize];
    sift.getIndexVectorFromDictionary(dict,imagePointList,index);
    /*for(int i=0; i<dictsize; i++){
      cout<<index[i]<<" ";
    }
    cout<<endl;
    getchar();
    */
    //normalize(index,dictsize);
    for(int i=0; i<dictsize; i++){
      featureVector->push_back(index[i]);
    }
    for(int i=0; i<imagePointList.size(); i++){
      delete imagePointList[i];
    }
  }
}
/**
 * @brief Constructor
 */
featureGenerator::featureGenerator() {
  TextureGraySize = 32;
}
/**
 * @brief Destructor
 */
featureGenerator::~featureGenerator()
{
  for(int i=0; i<dict.size();i++){
    delete dict[i];
  }
}
/**
 * @brief Load of the configuration
 */
void featureGenerator::load(ConfigurationManager* config) {
  this->TextureGraySize = config->TextureGraySize;
  this->ColorSize=config->ColorSize;
  this->GFDAngle=config->GFDAngle;
  this->GFDRadius=config->GFDRadius;
  
  this->useColor = config->UseColor;
  this->useGFD = config->UseGFD;
  this->useTexture=config->UseTexture;
  this->useColorLight=config->UseColorLight;
  this->usePosition=config->UsePosition;
  this->useSIFT=config->UseSIFT;
  this->siftDictFile=config->SIFTDictFile;

  if(useSIFT){
    SIFTAdapter sift;
    ifstream ifs;
    ifs.open(this->siftDictFile.c_str());
    sift.loadListSIFTPoint(ifs,dict,false);
    ifs.close();
  }
}
