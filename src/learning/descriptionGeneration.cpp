#include "learning.h"
#include <vector>
#include <map>
#include "texture.h"
#include "humoment.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <time.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h> // for mmap
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <algorithm>
#include "assignmentoptimal.h"
#include <math.h>
#include <cmath>
#include "../lib/LibTIM/Common/Image.h"
#include "../lib/LibTIM/Common/Histogram.h"
#include "../algo/convertImage.h"
#include "featureGenerator.h"
#include <utility/filename.h>
using namespace LibTIM;

using namespace std;

void calcVariance(Image<U8C3> const & image) {

}

void calcCorrelation(Image<U8C3> const & image) {

}

void calcCorrelation2(Image<U8C3> const & image) {

}


/**
 * @brief extraire des caracteristique des images
 */
void extract(const char* dirPath, const char* name, int graySize, int colorSize) {

  // read list of file and its class
  DIR *dp, *dpDetail;

  struct dirent *ep, *epDetail;


  vector<string> dirlists;
  // init list of files
  char outFileName[255];
  sprintf(outFileName, "%s", name);
  ofstream outfile;
  outfile.open(outFileName);

  // affichier l'entete de fichier
  //outfile << dirPath << " " << name << " " << graySize << " " << colorSize << " ";
  //outfile << NUM_MATRIX_ATT * NUM_MATRIX << " " << colorSize * colorSize * colorSize << endl;


  // initialize list of matrix co-occurence
  double*** concurrenceArray = new double**[NUM_MATRIX];

  for (int i = 0; i < NUM_MATRIX; i++) {
    concurrenceArray[i] = new double*[graySize];

    for (int j = 0; j < graySize; j++) {
      concurrenceArray[i][j] = new double[graySize];

      for (int k = 0; k < graySize; k++) {
        concurrenceArray[i][j][k] = 0;
      }
    }
  }

  // lire liste de fichier
  dp = opendir(dirPath);

  if (dp != NULL) {
    while (ep = readdir(dp)) {
      dirlists.push_back((string)ep->d_name);
      string currentdir = ep->d_name;
      int pos = currentdir.find('.');
      int currentid = atoi(currentdir.substr(0, pos).c_str());

      if (currentdir == "" || currentdir == "." || currentdir == "..")
        continue;

      string fulldir;
      fulldir += dirPath;
      fulldir += "/";
      fulldir += currentdir;
      //dpDetail = opendir(fulldir.c_str());

      vector<string> files;
      getFileList(fulldir.c_str(),files);
      /*if (dpDetail != NULL) {
        while (epDetail = readdir(dpDetail)) {
          //      puts(ep->d_name);
          files.push_back((string) epDetail->d_name);
        }

        (void) closedir(dpDetail);
      } else {
        perror(ERR_DIR_OPEN);
      }
      */
      // trier liste de fichier
      sort(files.begin(), files.end());

      for (int i = 0; i < files.size(); i++) {
        string filename = files.at(i);
        Image<U8C3> src;

        if (filename == "" || filename == "." || filename == "..")
          continue;

        string filePathSrc = string(dirPath);

        filePathSrc += "/" + currentdir + "/" + filename;

        cout << filePathSrc << endl;

        Image<U8C3>::load(filePathSrc.c_str(), src);

        outfile << currentid << " ";
        
        int field = 1;
        vector<double> values;
        featureGenerator fg;
        fg.generate(src,&values);
        for(int k=0; k<values.size(); k++){
          if(values[k]!=0)
            outfile<<field<<":"<<values[k]<<" ";
          field++;
        }
        /*
        Image<U8> gray(src.getSize());
        cvtRGB2Gray(src,gray);
        
        // Extraire des caracteristique
        extractTexture(gray, graySize, concurrenceArray, &values);
        
        for(int k=0; k<values.size(); k++){
          if(values[k]!=0)
            outfile<<field<<":"<<values[k]<<" ";
          field++;
        }
        double hu[8];
        getHuMoment(gray, hu);
        for(int k=0; k<8; k++){
          if(values[k]!=0)
            outfile<<field<<":"<<values[k]<<" ";
          field++;
        }      
        */
        //extractHistoColor(src, colorSize, outfile, field);

        outfile << endl;
      }
    }

    (void) closedir(dp);
  }

  // Free memory
  cout << "free memory" << endl;

  freeMatrix(concurrenceArray, graySize);

  outfile.close();
}





/*
 * extract Humoment pour la repertoire
 */
/*
void extractHuMoment(const char* dirPath, const char* name) {
  DIR *dp;
  struct dirent *ep;
  vector<string> files;
  // lire les fichiers
  dp = opendir(dirPath);
  if (dp != NULL) {
    while (ep = readdir(dp)) {
      //      puts(ep->d_name);
      files.push_back((string) ep->d_name);
    }
    (void) closedir(dp);
  } else
    perror(ERR_DIR_OPEN);
  // trier des fichier par nom
    sort(files.begin(), files.end());

    char outFileName[255];
    sprintf(outFileName, "%s", name);
    ofstream outfile;
    outfile.open(outFileName);
    // affichier l'entete de fichier
    outfile << dirPath << " " << name << " " << endl;

    for (int i = 0; i < files.size(); i++) {
      string filename = files.at(i);
      Mat src;
      if (filename == "")
        continue;

      string filePathSrc = string(dirPath);
      filePathSrc += "/" + filename;
      cout << filePathSrc << endl;
      // load image
      if (!(src = imread(filePathSrc, 1)).data)
        continue;

      // extraire caracteristique
        outfile << filename << " ";
        extractHuMomentImage(src, outfile);
        outfile << endl;
    }
    cout << "free memory" << endl;
    outfile.close();
}
*/
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
