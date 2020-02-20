#include "SIFTClassifier.h"
#include <algo/convertImage.h>
#include <config/ConfigurationManager.h>
#include <utility/imageIOText.h>

#include <dirent.h>
#include <algorithm>
#include <graphviz/geom.h>
using namespace std;
/**
 * @brief Generate the dictionary from the list of image in the folder
 * @param foldername the name of folder input
 * @param classDir the folder of class dir
 * @param dictFile the output dictionary file name
 */
void SIFTClassifier::generateDictionary(const char* foldername,const char* classDir, const char* dictFile) {
  // read list of file and its class
  DIR *dp, *dpDetail;
  struct dirent *ep, *epDetail;
  vector<string> files;
  getFileList(foldername, files);

  // trier liste de fichier
  sort(files.begin(), files.end());

  ConfigurationManager* config = ConfigurationManager::instance();
  config->loadConfig("config.txt");


  cout << "Generate sift dictionary:" << endl;
  ofstream ofs;
  ofs.open(dictFile);

  for (int i = 0; i < files.size(); i++) {
    string file = files[i];
    // read file in imagedir
    char filenext[255];
    getFilenameNotExtension(file.c_str(), filenext);
    
    string colorfullfile(foldername);
    colorfullfile += "/";
    colorfullfile += file;
    cout << colorfullfile << endl;
    Image<U8C3> colorImage;
    Image<U8C3>::load(colorfullfile.c_str(), colorImage);

    Image<U8> grayImage(colorImage.getSize());
    cvtRGB2Gray(colorImage, grayImage);

    string segfullfile(classDir);
    segfullfile += "/";
    segfullfile += file+".txt";
    Image<TLabel> segmentedImage(colorImage.getSize());
    getImageText(segfullfile.c_str(),segmentedImage);
    
    SIFTAdapter sift;
    vector<SIFTPoint*> pointList;
    sift.getSiftPoint(grayImage, pointList);
    //sift.getSiftPoint(grayImage, dictionary);
    cout << "get " << pointList.size() << " points" << endl;
    //getchar();
    for (int i = 0; i < pointList.size(); i++) {
      ofs<<segmentedImage(pointList[i]->x,pointList[i]->y)<<" ";
      pointList[i]->write(ofs,false);
    }
    for (int i = 0; i < pointList.size(); i++) {
      delete pointList[i];
    }
  }
  ofs.close();
}
/**
 * @brief Classifier of SIFT
 * @param dictFile The dictionary of dict
 * @param classNumber Number of class
 * @param threshold The threshold of distance between 2 corresponding classes
 */
SIFTClassifier::SIFTClassifier(const char* dictFile,int classNumber,double threshold) {
  SIFTAdapter sift;
  ifstream ifs;
  ifs.open(dictFile);
  while (ifs.good()) {
    int id=0;
    ifs>>id;
    SIFTPoint* point = new SIFTPoint();
    point->load(ifs,false);
    dict.push_back(point);
    idlist.push_back(id);
  }
  
  ifs.close();
  this->classNumber=classNumber;
  this->threshold=threshold;
}
/**
 * @brief Destructor with freeing the memory
 * 
 */
SIFTClassifier::~SIFTClassifier() {
  for (int i = 0; i < dict.size();i++) {
    delete dict[i];
  }
}
/**
 * @brief Classify an image by find the number of each classes points in image. The image belongs to the class which helds the most SIFT points.
 * 
 */
int SIFTClassifier::classify(LibTIM::Image< LibTIM::U8C3 >& image) {

  Image<U8> imggrayb(image.getSize());
  cvtRGB2Gray(image, imggrayb);

  SIFTAdapter sift;
  vector<SIFTPoint*> pointListB;
  sift.getSiftPoint(imggrayb, pointListB);

  int sizeb = pointListB.size();
  double indexb[sizeb];
  sift.matching(dict, pointListB, indexb,threshold);
  
  int classArray[classNumber];
  for(int i=0; i<classNumber; i++){
    classArray[i]=0;
  }
  
  for (int i = 0; i < sizeb; i++) {    
    int aindex = indexb[i];
    if (aindex != -1) {
      bool valid = true;
      for (int j = i + 1; j < sizeb; j++) {
        if (indexb[j] == aindex) {
          indexb[j] = -1;
          valid = false;
        }
      }
      if (valid) {
        // add to list
        //SIFTPoint* pb = pointListB[i];
        int classIndex=idlist[aindex];
        classArray[classIndex]++;
      } else {
        //indexb[i]=-1;
      }
    }
  }
  int bestmax=0;
  int bestindex=0;
  for(int i=0; i<classNumber; i++){
    if(bestmax<classArray[i]){
      bestmax=classArray[i];
      bestindex=i;
    }
  }
  
  for (int i = 0; i < sizeb; i++) {
    delete pointListB[i];
  }
  return bestindex;
}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
