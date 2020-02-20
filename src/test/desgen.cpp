
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <lib/LibTIM/Common/Image.h>
#include <utility/filename.h>
#include <learning/featureGenerator.h>
#include <utility/imageIOText.h>
#include <config/ConfigurationManager.h>
#include <algo/convertImage.h>
#include <algo/SIFT.h>
#include <learning/kmeanClustering.h>
using namespace std;
using namespace LibTIM;

#define ERR_DIR_MISSING "Missing directory"

void getListOfImage(const Image<U8C3>& colorImage, const Image<TLabel>& segmentImage, map<int, Image<U8C3>*>& classImageMapping) {
  int sizex = colorImage.getSizeX();
  int sizey = colorImage.getSizeY();
  for (int x = 0; x < sizex; x++) {
    for (int y = 0; y < sizey; y++) {
      int classId = segmentImage(x, y);
      Image<U8C3>* eleImage;
      if (classImageMapping.find(classId) == classImageMapping.end()) {
        eleImage = new Image<U8C3>(colorImage.getSize());
        eleImage->fill(0);
        classImageMapping.insert(pair<int, Image<U8C3>*>(classId, eleImage));
      } else {
        eleImage = classImageMapping[classId];
      }
      (*eleImage)(x, y) = colorImage(x, y);
    }
  }
}

int main(int argc, char** argv) {
  //cout<<"start"<<endl;
  char* imageDir;
  char* classDir;
  char* fileout;
  for (int i = 0; i < argc; i++) {

    // directory reference
    if (strcmp(argv[i], "-imagedir") == 0) {
      i++;

      if (i < argc) {
        imageDir = argv[i];
      } else {
        throw ERR_DIR_MISSING;
      }

      continue;
    }

    // directory reference
    if (strcmp(argv[i], "-classdir") == 0) {
      i++;

      if (i < argc) {
        classDir = argv[i];
      } else {
        throw ERR_DIR_MISSING;
      }

      continue;
    }
    // directory reference
    if (strcmp(argv[i], "-fileout") == 0) {
      i++;

      if (i < argc) {
        fileout = argv[i];
      } else {
        throw ERR_DIR_MISSING;
      }

      continue;
    }
  }
  // read list of file and its class
  DIR *dp, *dpDetail;
  struct dirent *ep, *epDetail;
  vector<string> files;
  getFileList(classDir, files);

  // trier liste de fichier
  sort(files.begin(), files.end());

  ConfigurationManager* config = ConfigurationManager::instance();
  config->loadConfig("config.txt");
  
  // dictionary
  if(config->UseSIFT){
    cout<<"Generate sift dictionary:"<<endl;
    //vector<SIFTPoint*> dictionary;
    vector<vector<double>*> data;
    for (int i = 0; i < files.size(); i++) {
      string file = files[i];
      // read file in imagedir
      char filenext[255];
      getFilenameNotExtension(file.c_str(), filenext);
      string colorfullfile(imageDir);
      colorfullfile += "/";
      colorfullfile += filenext;
      cout<<colorfullfile<<endl;
      Image<U8C3> colorImage;
      Image<U8C3>::load(colorfullfile.c_str(), colorImage);

      Image<U8> grayImage(colorImage.getSize());
      cvtRGB2Gray(colorImage, grayImage);

      SIFTAdapter sift;
      vector<SIFTPoint*> pointList;
      sift.getSiftPoint(grayImage,pointList);
      //sift.getSiftPoint(grayImage, dictionary);
      cout<<"get "<<pointList.size()<<" points"<<endl;
      //getchar();
      for (int i = 0; i < pointList.size(); i++) {
        vector<double>* vec = new vector<double>(SIFTPoint::SIFTSize,0);
        for (int j = 0; j < SIFTPoint::SIFTSize; j++) {
          (*vec)[j]=(pointList[i]->descriptors[j]);
        }        
        data.push_back(vec);
      }
      for (int i = 0; i < pointList.size(); i++) {
        delete pointList[i];
      }
    }  


    Kmean kmean;
    kmean.k = 1000;
    kmean.dimension = 128;
    kmean.maxTimes = 20;
    vector<TLabel> result(data.size());
    kmean.clustering(data, result);
    //free data
    for(int i=0; i<data.size(); i++)
      delete data[i];
    cout<<"write to "<<config->SIFTDictFile<<endl;
    ofstream siftofs;
    siftofs.open(config->SIFTDictFile.c_str());
    for (int i = 0; i < kmean.k; i++) {
      for (int j = 0; j < kmean.dimension; j++) {
        siftofs << (*kmean.centroids[i])[j] << " ";
      }
      siftofs << endl;
    }
    siftofs.close();

  }// end of gen dictionary
  featureGenerator fg;
  fg.load(config);
  
  ofstream ofs;
  ofs.open(fileout);
  // read all file in classdir
  for (int i = 0; i < files.size(); i++) {
    string file = files[i];
    // read file in imagedir
    char filenext[255];
    getFilenameNotExtension(file.c_str(), filenext);
    string colorfullfile(imageDir);
    colorfullfile += "/";
    colorfullfile += filenext;
    string grayfullfile(classDir);
    grayfullfile += "/";
    grayfullfile += file;
    cout << colorfullfile << endl;
    cout << grayfullfile << endl;
    Image<U8C3> colorImage;
    Image<U8C3>::load(colorfullfile.c_str(), colorImage);


    //Image<U8> grayImage;
    //Image<U8>::load(grayfullfile.c_str(), grayImage);
    //Image<TLabel> segmentImage = grayImage;
    Image<TLabel> segmentImage;
    getImageText(grayfullfile.c_str(), segmentImage);

    map<int, Image<U8C3>*> maplist;
    cout << "list of image" << endl;
    getListOfImage(colorImage, segmentImage, maplist);
    map<int, Image<U8C3>*>::iterator it;
    cout << "generator" << endl;
    // generate the information
    for (it = maplist.begin(); it != maplist.end(); it++) {
      int classId = (*it).first;
      Image<U8C3>* localImage = (*it).second;
      /*char str[255];
      sprintf(str,"%d.png",classId);
      localImage->save(str);
      */
      vector<double> values;
      fg.generate(*localImage, &values);
      //cout<<"end of generate 1 image"<<endl;
      ofs << classId << " ";
      for (int k = 0; k < values.size(); k++) {
        if (values[k] != NULL)
          ofs << k + 1 << ":" << values[k] << " ";
      }
      ofs << endl;
      /*
      int a=rand();
      char filename[255];
      sprintf(filename,"%d.ppm",a);
      localImage->save(filename);
      */
    }
  }
  ofs.close();

  return 0;
}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;  replace-tabs on;
