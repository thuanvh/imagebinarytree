#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <utility/filename.h>
#include <algo/bptsegment.h>
#include <learning/featureGenerator.h>
#include <learning/knnClassifier.h>
#include <learning/LibSVMAdapter.h>
#include <utility/imageIOText.h>
#include <learning/evaluator.h>
#include <config/ConfigurationManager.h>
#include <cmath>
using namespace std;


#define TEST 1
#define EVALUATION 2


void setCoefficient(BPTSegment* bptsegment, int paras, float* parasValue);
void loadClassifier(classifier*& clss);
Image<TLabel>* classify(BPTSegment* bptsegment, classifier* clss, string learningFileName, Image<U8C3>* imageSrc, Image<U8C3>& colorImageOutput);
void deleteClassifier(classifier* clss);
void showResult(BPTSegment* bptsegment, Image<U8C3>* imageSrc, vector<int>* idList, vector<int>& classIds, Image<TLabel>* classifiedImage, Image<U8C3>& colorImageOutput);
Image<TLabel>* segmentByClassify(BPTSegment* bptsegment, string learningFileName, Image<U8C3>* imageSrc, Image<U8C3>& colorImageOutput);

int main(int argc, char** argv) {
  ConfigurationManager* config = ConfigurationManager::instance();
  int mode = TEST | EVALUATION;

  if (argc > 1)
    config->loadConfig(argv[1]);
  else
    config->loadConfig("config.txt");



  string inputfolder = config->TestFolder;//"learning-image";//"learning-image";//"test-image-one";
  string outputfolder = config->ResultFolder;//"result-data";
  string outputcolor = config->ResultColor;//"result-color";

  vector<string> inputFileList;
  getFileList(inputfolder.c_str(), inputFileList);
  const int volvementThreshold = config->BPTEvolvementThreshold;
  string learningFileName = config->LearningFileName;

  const int cmax = 16;
  double g[] = {0.0078125, 0.03125, 0.125};
  
  double totalprecis[3*cmax];
  double totalrecall[3*cmax];
  double totalaccuracy[3*cmax];

  for(int i=0; i<3*cmax; i++){
    totalprecis[i]=0;
    totalaccuracy[i]=0;
    totalrecall[i]=0;
  }
  int totalindex=0;
  for (int c = 0; c < cmax; c++) {
    for (int gi = 0; gi < 3; gi++, totalindex++) {
      config->SVMC = pow(2, c);
      config->SVMGamma = g[gi];
      

      if (mode & TEST) {

        int paras = config->BPTProperty;//PARA_MEANCOLOR;
        float parasvalue[6] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5};

        for (int i = 0; i < inputFileList.size(); i++) {
          // classification
          classifier* clss;
          loadClassifier(clss);
          //cout << "begin" << endl;
          cout << clss->getInfo() << endl;
          clss->loadProblem(learningFileName.c_str());
          //cout << "end load pbl" << endl;

          string testFile = inputFileList[i];
          cout << "Testing file " << testFile << endl;

          BPTSegment bptsegment;
          //bptsegment->regionFirstCount=spinNumberOfInitRegion->value();
          Image<U8C3>::load((inputfolder + "/" + testFile).c_str(), bptsegment.imageSrc);
          //bptsegment->imageSrc=loadSimpleImage();
          Image<U8C3>* imageSrc = &bptsegment.imageSrc;

          //bptsegment->volvementThreshold=spinEvolvementThreshold->value();
          bptsegment.regionFinalCount = config->BPTRegionNumber;
          bptsegment.methodPruning = config->BPTMode;//PRUN_EVOL_AUTO;//PRUN_EVOL_THRESHOLD; PRUN_EVOL_AUTO,  PRUN_EVOL_REGION_COUNT
          bptsegment.volvementThreshold = volvementThreshold;//txtEvolvementThreshold->text().toFloat();
          setCoefficient(&bptsegment, paras, parasvalue);
          Image<U8C3> colorOutput(imageSrc->getSize());
          colorOutput.fill(0);
          Image<TLabel>* classifiedImage;
          if (config->TestMode == BPT_CLASSIFY_AFTER_SEGMENTATION) {

            bptsegment.segment();
            string a = testFile + ".ppm";
            bptsegment.showedFinalSegmentedImage->save(a.c_str());
            classifiedImage = classify(&bptsegment, clss, learningFileName, imageSrc, colorOutput);

          } else
            if (config->TestMode == BPT_SEGMENTATION_AFTER_CLASSIFIER) {
              bptsegment.treeConstruction();
              bptsegment.mainClassifier = clss;
              classifiedImage = segmentByClassify(&bptsegment, config->LearningFileName, imageSrc, colorOutput);
            }
          // save image here
          string clsstxtfile = outputfolder + "/" + testFile + ".txt";
          saveImageText(clsstxtfile.c_str(), *classifiedImage);
          string clsscolorfile = outputcolor + "/" + testFile + ".ppm";
          colorOutput.save(clsscolorfile.c_str());
          deleteClassifier(clss);
          delete classifiedImage;
        }

      }

      if (mode & EVALUATION) {
        string folderResult = config->ResultFolder;//"result-data";
        string folderTruth = config->TestClass;//"learning-data";
        cout << folderResult << endl;
        vector<string> fileList;
        getFileList(folderResult.c_str(), fileList);

        int size = ConfigurationManager::instance()->classList->size();
        Evaluator eval(size);
        eval.classList = ConfigurationManager::instance()->classList;
        cout << "Evaluation:" << endl;
        for (int i = 0; i < fileList.size(); i++) {
          string file = fileList[i];

          Image<TLabel> result;
          getImageText((folderResult + "/" + file).c_str(), result);

          Image<TLabel> truth;
          getImageText((folderTruth + "/" + file).c_str(), truth);

          eval.evaluate(result, truth);
        }
        eval.normalise();
        eval.print(cout);
        totalaccuracy[totalindex]=eval.totalaccuracy;
        totalprecis[totalindex]=eval.totalprecis;
        totalrecall[totalindex]=eval.totalrecall;
      }
    }
  }
  totalindex=0;
  double maxacc=0;
  double maxc=0;
  double maxg=0;
  for (int c = 0; c < cmax; c++) {
    for (int gi = 0; gi < 3; gi++, totalindex++) {
      cout<<"c="<<pow(2,c)<<"\tg="<<g[gi]<<"\taccu="<<totalaccuracy[totalindex]<<"\tprecis="<<totalprecis[totalindex]<<"\trecall="<<totalrecall[totalindex]<<endl;
      if(maxacc<totalaccuracy[totalindex]){
        maxacc=totalaccuracy[totalindex];
        maxc=pow(2,c);
        maxg=g[gi];
      }
    }
  }
  cout<<"Max accuracy:"<<maxacc<<" with c="<<maxc<<" and g="<<maxg<<endl;
  return 0;
}
void deleteClassifier(classifier* clss) {
  delete clss;
}
void loadClassifier(classifier*& clss) {

  ConfigurationManager* config = ConfigurationManager::instance();
  int type = config->Classifier; //SVM;
  switch (type) {
    case KNN: {

      knnClassifier* knn = new knnClassifier();
      knn->k = config->KNNk;
      clss = knn;
      break;

    }
    case SVM: {

      LibSVMAdapter* lsa = new LibSVMAdapter();
      lsa->param.gamma = config->SVMGamma;//0.5;//txtSvmGamma->text().toDouble();
      lsa->param.degree = config->SVMDegree;//3;//txtSvmDegree->text().toDouble();
      lsa->param.kernel_type = config->SVMKernelType;//POLY;//RBF;
      lsa->param.C = config->SVMC;
      lsa->param.svm_type = config->SVMType;
      /*
       *      if(cboSvmKernel->currentText().compare(QString(KERNEL_POLY_STR))==0){
       *        lsa->param.kernel_type= POLY;//RBF;
       }else if(cboSvmKernel->currentText().compare(QString(KERNEL_LINEAR_STR))==0){
         lsa->param.kernel_type= LINEAR;//RBF;
       }else if(cboSvmKernel->currentText().compare(QString(KERNEL_GAUSSIAN_STR))==0){
         lsa->param.kernel_type= SIGMOID;//RBF;
       } */
      clss = lsa;
      break;

    }
    default:
      break;
  }

}
void extractSegmentedImage(Image<U8C3>const & imageSrc, vector<int*> pointList, Image<U8C3> & imageRegion);
void devideImage(BPTSegment* bptsegment, Image<U8C3> const & srcImage, map<int, Image<U8C3>*>* segmentedImageMap) {
  vector<node*> list = bptsegment->nodeConservedList;
  int size = list.size();

  for (int i = 0; i < size; i++) {
    int id = list[i]->id;

    /// construct the file
    vector<int> leavesList;
    int leavesCount = bptsegment->graph.leavesCount;

    if (id <= leavesCount)
      leavesList.push_back(id);
    else
      leavesList = (*bptsegment->graph.graphMapToLeaves[id-leavesCount-1]);

    /// leaves
    Image<U8C3>* localimage;

    //if(segmentedImageMap.find(classname)==segmentedImageMap.end()){
    localimage = new Image<U8C3> (srcImage.getSize());

    segmentedImageMap->insert(pair<int, Image<U8C3>*> (id, localimage));

    U8C3 zero;
    zero[0] = zero[1] = zero[2] = 0;
    localimage->fill(zero);

    //}else{
    //  localimage=segmentedImageMap[classname];
    //}


    for (int mapid = 0; mapid < leavesList.size(); mapid++) {
      vector<int*>* pointsList = bptsegment->graph.pointsListOfLeaves[leavesList[mapid]];
      extractSegmentedImage(srcImage, *pointsList, *localimage);
    }
  }
}

Image<TLabel>* classify(BPTSegment* bptsegment, classifier* clss, string learningFileName, Image<U8C3>* imageSrc, Image<U8C3>& colorImageOutput) {
  ConfigurationManager* config = ConfigurationManager::instance();
  /// devide the image into some regions (local image)
  map<int, Image<U8C3>*>* segmentedImageMap = new map<int, Image<U8C3>*>();
  devideImage(bptsegment, *imageSrc, segmentedImageMap);
  int sizemap = segmentedImageMap->size();
  cout << sizemap << " segments" << endl;
  /// extract features of regions
  map<int, Image<U8C3>*>::iterator iter;

  featureGenerator fg;
  fg.load(config);

  vector< vector<double>*> vectorList;
  vector<int>* idList = new vector<int>();

  for (iter = segmentedImageMap->begin(); iter != segmentedImageMap->end(); iter++) {
    int id = iter->first;
    Image<U8C3>* rgb = iter->second;
    //showImage(*rgb,"aha");
    vector<double> *vec = new vector<double>();
    fg.generate(*rgb, vec);
    vectorList.push_back(vec);
    idList->push_back(id);
  }

  clss->reset();
  /// classify the regions
  clss->setTestData(vectorList);
  cout << "classify" << endl;
  clss->classify();
  cout << "end" << endl;
  cout << idList->size() << "vs" << clss->classIds.size();

  Image<TLabel>* classifiedImage = new Image<TLabel>(imageSrc->getSize());
  for (int i = 0; i < idList->size(); i++) {
    int id = (*idList)[i];
    int classid = clss->classIds[i];
    cout << classid << " ";
  }
  showResult(bptsegment, imageSrc, idList, clss->classIds, classifiedImage, colorImageOutput);
  delete idList;
  for(int i=0; i<vectorList.size(); i++){
    delete vectorList[i];
  }
  for (iter = segmentedImageMap->begin(); iter != segmentedImageMap->end(); iter++) {
    delete iter->second;
  }
  delete segmentedImageMap;
  return classifiedImage;
}
void showResult(BPTSegment* bptsegment, Image<U8C3>* imageSrc, vector<int>* idList, vector<int>& classIds, Image<TLabel>* classifiedImage, Image<U8C3>& colorImageOutput) {
  /// show the results

  int xsize = imageSrc->getSizeX();
  int ysize = imageSrc->getSizeY();
  for (int i = 0; i < idList->size(); i++) {
    int id = (*idList)[i];
    int classid = classIds[i];
    cout << classid << " ";
    vector<int> nodeList;

    if (id > bptsegment->graph.leavesCount) {
      vector<int>* vleaves = bptsegment->graph.graphMapToLeaves[id-bptsegment->graph.leavesCount-1];
      for (int j = 0; j < vleaves->size(); j++) {
        nodeList.push_back((*vleaves)[j]);
      }
    } else {
      nodeList.push_back(id);;
    }
    for (int v = 0; v < nodeList.size(); v++) {
      int leaf = nodeList[v];
      for (int p = 0; p < bptsegment->graph.pointsListOfLeaves[leaf]->size(); p++) {
        int x = (*bptsegment->graph.pointsListOfLeaves[leaf])[p][0];
        int y = (*bptsegment->graph.pointsListOfLeaves[leaf])[p][1];
        (*classifiedImage)(x, y) = classid;

        // coloring image
        colorImageOutput(x, y)[0] = (*ConfigurationManager::instance()->classList)[classid]->color[0];
        colorImageOutput(x, y)[1] = (*ConfigurationManager::instance()->classList)[classid]->color[1];
        colorImageOutput(x, y)[2] = (*ConfigurationManager::instance()->classList)[classid]->color[2];
      }
    }

  }
}
void setCoefficient(BPTSegment* bptsegment, int paras, float* parasValue) {
  if (paras & PARA_TEXTUREWAVELET)
    bptsegment->coefTextureWavelet = parasValue[0];
  else
    bptsegment->coefTextureWavelet = 0;


  if (paras & PARA_HOMOGENEITY)
    bptsegment->coefHomogeneity = parasValue[1];
  else
    bptsegment->coefHomogeneity = 0;

  if (paras & PARA_MEANCOLOR)
    bptsegment->coefMeanColor = parasValue[2];
  else
    bptsegment->coefMeanColor = 0;

  if (paras & PARA_REGIONSIZE)
    bptsegment->coefRegionSize = parasValue[3];
  else
    bptsegment->coefRegionSize = 0;

  if (paras & PARA_MEANGRADIENT)
    bptsegment->coefMeanGradient = parasValue[4];
  else
    bptsegment->coefMeanGradient = 0;

  if (paras & PARA_COLORCOVARIANCE)
    bptsegment->coefColorVariance = parasValue[5];
  else
    bptsegment->coefColorVariance = 0;
}


Image<TLabel>* segmentByClassify(BPTSegment* bptsegment, string learningFileName, Image<U8C3>* imageSrc, Image<U8C3>& colorImageOutput) {
  bptsegment->finalSegmentedImage.fill(0);
  vector<int> idList;
  bptsegment->pruningByClassification(bptsegment->firstSegmentedImage, bptsegment->finalSegmentedImage, learningFileName, idList);

  //refreshImage();

  /*/// devide the image into some regions (local image)
   *  map<int, Image<U8C3>*>* segmentedImageMap = new map<int, Image<U8C3>*>();
   *  devideImage(bptsegment, *imageSrc, segmentedImageMap);
   *  int sizemap = segmentedImageMap->size();
   *
   *  /// extract features of regions
   *  map<int, Image<U8C3>*>::iterator iter;
   */
  //featureGenerator fg;
  //fg.graySize = 32;
  //vector< vector<double>*> vectorList;
  /*vector<int> idList = new vector<int>();
   *  vector<int> classes = new vector<int>();
   *  for (iter = segmentedImageMap->begin(); iter != segmentedImageMap->end(); iter++) {
   *    int id = iter->first;
   *    //Image<U8C3>* rgb = iter->second;
   *    //showImage(*rgb,"aha");
   *    //vector<double> *vec = new vector<double>();
   *    //fg.generate(*rgb, vec);
   *    //vectorList.push_back(vec);
   *    idList->push_back(id);
   *    classes->push_back(bptsegment->graph.showedNode[id]->classId);
   *    cout<<id<<":"<<bptsegment->graph.showedNode[id]->classId<<" ";
   }
   */
  /*/// show the results
   *  ClassificationShowForm* csf = new ClassificationShowForm();
   *  csf->idList = idList;
   *  csf->classes = classes;
   *  csf->segmentedImageMap = segmentedImageMap;
   *  csf->imageSrc = imageSrc;
   *  csf->show();
   *  csf->loadClassification();
   */

  Image<TLabel>* classifiedImage = new Image<TLabel>(imageSrc->getSize());
  showResult(bptsegment, imageSrc, &idList, bptsegment->mainClassifier->classIds, classifiedImage, colorImageOutput);
  return classifiedImage;
  /*int xsize=imageSrc->getSizeX();
   *  int ysize=imageSrc->getSizeY();
   *  for(int x=0; x<xsize; x++){
   *    for(int y=0; y<ysize; y++){
   *      int classid=bptsegment->finalSegmentedImage(x,y);
   *      if(classid>10){
   *        cout<<classid<<" ";
   *        //pause();
   }

   // coloring image
   colorImageOutput(x, y)[0] = (*ConfigurationManager::instance()->classList)[classid]->color[0];
   colorImageOutput(x, y)[1] = (*ConfigurationManager::instance()->classList)[classid]->color[1];
   colorImageOutput(x, y)[2] = (*ConfigurationManager::instance()->classList)[classid]->color[2];
   }
   }

   return &bptsegment->finalSegmentedImage;
   */
}

// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;    replace-tabs on;

