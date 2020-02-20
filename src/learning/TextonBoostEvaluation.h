// a version of Shotton textonboost C#

#ifndef TEXTON_BOOST_EVALUATION_H
#define TEXTON_BOOST_EVALUATION_H
#include <iostream>
#include <vector>
#include <lib/LibTIM/Common/Image.h>
#include <utility/filename.h>
#include <utility/imageIOText.h>
#include <math.h>
#include "TextonBoost.h"
using namespace std;
using namespace LibTIM;
namespace TextonBoostNS {
  class Evaluate {
    // Other parameters
  public:
    int SubSample;
    int NumRounds;
    string TestSet;


    int numTextons;
    string testFolder;
    string outputFolder;
    string outputColorFolder;
    Evaluate(){
      SubSample = 1;//MainClass.EvaluationSubSample;
      NumRounds = 9;//MainClass.NumRoundsBoosting;
    }
    /**
     * @brief Run the test with TextonBoost
     */
    void DoWork() {
      /// Get parameters

      //TestSet = "Test";//MainClass.TestSet;
      string filterFileName = "ShapeFilter.txt";
      ifstream ifs;
      ifs.open(filterFileName.c_str());
      /// Get list of test data
      vector<string> testDataFilenames;
      getFileList(testFolder.c_str(), testDataFilenames);

      ofstream ofs;
      ofs.open("shapefiltershapetest.txt");
      /// Load shape filters
      vector<ShapeFilter*> shapeFilters;
      for (int round = 0; round < NumRounds; round++) {
        ShapeFilter* sf = new ShapeFilter;
        ShapeFilter::load(ifs, *sf);
        ShapeFilter::write(ofs,*sf);
        shapeFilters.push_back(sf);
      }//shapeFilters.Add((TextonBoost.ShapeFilter) MainClass.Load("ShapeFilter." + round));
      ofs.close();
      ClassList* classList = shapeFilters[0]->classList;

      /// Get number of textons
      //List<KMeans.Cluster> clusters = (List<KMeans.Cluster>) MainClass.Load("Clusters");
      //vector<vector<double>*> clusters;
      //int numTextons = clusters.Count;
      /// loop:
      for (int i = 0; i < testDataFilenames.size(); i++) {
        cout << "Loading images " << testDataFilenames[i] <<endl;
        //Image<byte> image = ImageIO.LoadImage<byte>(testDataFilenames[i].im);

        Image<TLabel> textonMap ;//= (Image<int>) MainClass.Load("TextonMaps." + testDataFilenames[i].im);
        getImageText((testFolder+"/" +testDataFilenames[i]).c_str(), textonMap);

        cout << "  Classifying..." <<endl;
        ///NormalisingOutputMapping<double> normMap = new NormalisingOutputMapping<double>();
        vector<Image<double>*>* classifications = ClassifyImage(textonMap, *classList, shapeFilters, NumRounds, numTextons, SubSample);

        /// Compute mode
        Image<int>* mode = GetMAP(*(*classifications)[classifications->size()- 1]);

        /// Save
        //SaveImage(testDataFilenames[i].im, mode, new IdOutputMapping<int>());
        saveImageText((outputFolder + "/" + testDataFilenames[i]).c_str(), *mode);
        Image<U8C3> colorClassifiedImage(mode->getSize());
        for (int x = 0; x < colorClassifiedImage.getSizeX(); x++) {
          for (int y = 0; y < colorClassifiedImage.getSizeY(); y++) {
            // coloring image
            colorClassifiedImage(x, y)[0] = (*ConfigurationManager::instance()->classList)[(*mode)(x,y)]->color[0];
            colorClassifiedImage(x, y)[1] = (*ConfigurationManager::instance()->classList)[(*mode)(x,y)]->color[1];
            colorClassifiedImage(x, y)[2] = (*ConfigurationManager::instance()->classList)[(*mode)(x,y)]->color[2];
          }
        }

        colorClassifiedImage.save((outputColorFolder + "/" + testDataFilenames[i] + ".jpg").c_str());

        delete mode;
        for (int j = 0; j < classifications->size(); j++) {
          delete (*classifications)[j];
        }
        delete classifications;
      }
      for(int i=0; i<shapeFilters.size(); i++){
        delete shapeFilters[i];
      }
    };

  private:
    /**
     * @brief Classify image texton
     * @param textonMap imageTexton
     * @param classList list of class
     * @param shapeFilters list of random shape filter
     * @param numTextons number of textons
     * @param subSample divide the image in subsample
     */
    vector<Image<double>*>* ClassifyImage(Image<TLabel>& textonMap, ClassList& classList, vector<ShapeFilter*>& shapeFilters, int step, int& numTextons, int& subSample) {
      int xsize = textonMap.getSizeX();
      int ysize = textonMap.getSizeY();
      int ssW = (textonMap.getSizeX() + subSample - 1) / subSample; // Care with off by one errors
      int ssH = (textonMap.getSizeY() + subSample - 1) / subSample; // Care with off by one errors

      // Compute integral images
      Size s(ssW, ssH);//Size(ssW, ssH)
      Image<TLabel>** integralImages = TextonBoost::CalculateIntegralImages(textonMap, numTextons, subSample, s);

      vector<Image< double>* >* classificationFullSizes = new vector<Image<double>*>;
      Image<double> classification(ssW, ssH, classList.NumCompact());

      double* classificationBase = classification.getData();
      {
        // Iteratively classify
        for (int round = 0; round < shapeFilters.size(); round++) {
          ShapeFilter* sf = shapeFilters[round];

          Rectangle r = TextonBoost::SubSample(*sf->r, subSample);

          double* classificationPtr = classificationBase;
          //for (int i = 0; i < classification; i++) {
          for (int x = 0; x < ssW; x++) {
            for (int y = 0; y < ssH; y++) {
              Point p(x, y); //classification.Bounds[i];

              //foreach (Point p in classification.Bounds)
              //{
              double response = TextonBoost::CalculateShapeFilterResponse(*integralImages[sf->t], r, &p);

              double confidenceShared = (response > sf->wl->theta) ? (sf->wl->a + sf->wl->b) : sf->wl->b;

              for (int c = 0; c < classList.NumCompact(); c++, classificationPtr++)
                (classification)(x, y, c) += sf->wl->n->Contains(c) ? confidenceShared : sf->wl->k[c];
            }
          }



          if (round % step == step - 1) {
            // Convert to pdf (soft-max function)
            Image<double> classificationPDF(ssH, ssW, classList.NumCompact());
            for (int x = 0; x < ssW; x++) {
              for (int y = 0; y < ssH; y++) {
                Point p(x, y);
                double sum = 0.0;
                for (int c = 0; c < classList.NumCompact(); c++)
                  sum += exp((classification)(x, y, c));

                for (int c = 0; c < classList.NumCompact(); c++)
                  classificationPDF(x, y, c) = exp((classification)(x, y, c)) / sum;
              }
            }

            // Super-sample (spatially and compact to full)
            Image<double>* classificationFullSize = new Image<double>(textonMap.getSizeX(), textonMap.getSizeY(), classList.NumFull());
            for (int x = 0; x < ssW; x++) {
              for (int y = 0; y < ssH; y++) {
                for (int c = 0; c < classList.NumCompact(); c++)
                  (*classificationFullSize)(x, y, classList.ToFull(c)) = classificationPDF(x / subSample, y / subSample, c);
              }
            }



            classificationFullSizes->push_back(classificationFullSize);
          }
        }
      }
      for(int i=0; i<numTextons; i++){
        delete integralImages[i];
      }
      delete []integralImages;
      
      return classificationFullSizes;
    }

  public:
    /**
     * @brief Divide image in subsample
     * @param classification The image of feature response
     * @param subSample Number of samples
     */
    Image<double>* SubSampleClassification(Image<double>& classification, int subSample) {
      int ssW = (classification.getSizeX() + subSample - 1) / subSample; // Care with off by one errors
      int ssH = (classification.getSizeY() + subSample - 1) / subSample; // Care with off by one errors
      int bands = classification.getSizeZ();
      Image<double>* ssClassification = new Image<double>(ssW, ssH,  bands);
      for (int x = 0; x < ssW; x++) {
        for (int y = 0; y < ssH; y++) {
          for (int c = 0; c < bands; c++)
            (*ssClassification)(x, y, c) = classification(x * subSample, y * subSample, c);
        }
      }
      return ssClassification;
    }
    /**
     * @brief reconstruction of image from the small image
     * @param ssClassification the small image
     * @param subSample the number of sample
     * @param origSize the original size
     */
    Image<double>* SuperSampleClassification(Image<double>& ssClassification, int subSample, Size origSize) {
      int bands = ssClassification.getSizeZ();
      Image<double>* classification = new Image<double>(origSize.Width, origSize.Height, bands);
      for (int x = 0; x < origSize.Width; x++) {
        for (int y = 0; y < origSize.Height; y++) {

          for (int c = 0; c < bands; c++)
            (*classification)(x, y, c) = ssClassification(x / subSample, y / subSample, c);
        }
      }

      return classification;
    }

    /**
     * @brief Map of classification
     */
    Image<int>* GetMAP(Image<double> classification) {
      Image<int>* map = new Image<int>(classification.getSizeX(), classification.getSizeY());
      for (int x = 0; x < map->getSizeX(); x++) {
        for (int y = 0; y < map->getSizeY(); y++) {

          int max = 0;
          double maxP = classification(x, y, max);
          for (int c = 0; c < classification.getSizeZ(); c++)
            if (classification(x, y, c) > maxP) {
              max = c;
              maxP = classification(x, y, c);
            }
          (*map)(x, y) = max;
        }
      }

      return map;
    }
    /**
     * @brief Get entropy of classification
     */
    Image<double>* GetEntropy(Image<double> classification) {
      Image<double>* outImg = new Image<double>(classification.getSizeX(), classification.getSizeY());

      //foreach (Point p in classification.Bounds)
      for (int x = 0; x < outImg->getSizeX(); x++) {
        for (int y = 0; y < outImg->getSizeY(); y++) {
          double entropy = 0.0;
          for (int c = 0; c < classification.getSizeZ(); c++) {
            double prob = classification(x, y, c);
            if (prob > 0.0)
              entropy -= prob * log(prob);
          }

          (*outImg)(x, y) = entropy / log(2.0);
        }
      }

      return outImg;
    }
    /**
     * @brief Get entropy vis 
     */
    static Image<U8C3>* GetEntropyVis(Image<int>& classificationMAP, Image<double>& entropy, int numClasses) {
      Image<U8C3>* outImg = new Image<U8C3>(classificationMAP.getSizeX(), classificationMAP.getSizeY());

      double norm = log(numClasses) / log(2.0);
      for (int x = 0; x < outImg->getSizeX(); x++) {
        for (int y = 0; y < outImg->getSizeY(); y++) {
          //foreach (Point p in outImg.Bounds)
          //Point p;
          /// coloring the output
          U8C3 outColour ;//= IdOutputMapping<byte>.MapToColor(classificationMAP[p]);

          double scale = (1.0 - entropy(x, y) / norm);
          (*outImg)(x, y)[0] = (U8)(outColour[0] * scale);
          (*outImg)(x, y)[1] = (U8)(outColour[1] * scale);
          (*outImg)(x, y)[2] = (U8)(outColour[2] * scale);

        }
      }

      return outImg;
    }


  };
};

#endif
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;
