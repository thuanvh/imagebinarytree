#include "bptSegmentForm.h"
#include <QFileDialog>
#include <QMessageBox>
#include <algo/tim2qt.h>
#include "imageviewer.h"
#include "imageManuallyClassifier.h"
#include <learning/LibSVMAdapter.h>
#include <learning/featureGenerator.h>
#include <learning/knnClassifier.h>

#include <ui/classificationShowForm.h>
#include <ui/treeGraphicWidget.h>
#include <ui/classificationChooser.h>
#include <algo/segmentationVerifier.h>
#include <algo/segmentColorer.h>
#include <algo/convertImage.h>
#include <utility/filename.h>
#include "treeqtw.h"
#include "treeInspector.h"
#include "treeViewer.h"

BptSegmentForm::BptSegmentForm(QWidget* parent) : QDialog(parent) {
  setupUi(this);
  /*QRegExp regExp("[A-Za-z][1-9][0-9]{0,2}");
  lineEdit->setValidator(new QRegExpValidator(regExp, this));
  connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
  */
  bptsegment = NULL;
  mainClassifier = NULL;
}
BptSegmentForm::~BptSegmentForm() {
  if (bptsegment != NULL)
    delete bptsegment;
  if (mainClassifier != NULL)
    delete mainClassifier;
}

Image<U8C3> loadSimpleImage() {
  /*uchar data[]={
    0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0,
    0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0,
    0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0
  };*/
  /*uchar data[]={
    0,0,0, 1,0,0, 1,0,0, 0,0,0, 0,0,0,
    0,0,0, 1,0,0, 1,0,0, 0,0,0, 0,0,0,
    0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0
  };*/
  /*
  uchar data[]={
    0,0,0, 1,0,0, 0,2,0, 0,0,0, 0,0,0,
    0,0,0, 1,0,0, 0,2,0, 0,0,0, 0,0,0,
    0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0
  };*/
  /*uchar data[] = {
    1, 8, 250, 2, 35, 156, 3, 0, 0, 4, 0, 0, 0, 0, 0,
    3, 0, 0, 4, 0, 0, 4, 0, 0, 4, 0, 0, 7, 0, 0,
    2, 0, 0, 2, 0, 0, 1, 0, 0, 5, 0, 0, 0, 0, 0
  };*/
  uchar data[] = {
    1, 0, 0, 2, 0, 0, 3, 0, 0, 4, 0, 0, 0, 0, 0,
    3, 0, 0, 4, 0, 0, 4, 0, 0, 4, 0, 0, 7, 0, 0,
    2, 0, 0, 2, 0, 0, 1, 0, 0, 5, 0, 0, 0, 0, 0
  };
  /*
  uchar data[] = {
    1, 0, 0, 1, 0, 0, 3, 0, 0,
    4, 0, 0, 4, 0, 0, 4, 0, 0,
    2, 0, 0, 2, 0, 0, 4, 0, 0
  };*/
  TSize size[] = {5, 3, 1};
  TSpacing space[] = {1, 1, 1};
  Image<U8C3>* image = new Image<U8C3> (size, space, (U8C3*) data);
  return *image;
}

void BptSegmentForm::setCoefficient() {
  if (chkTextureWavelet->isChecked())
    bptsegment->coefTextureWavelet = txtTextureWavelet->text().toFloat();
  else
    bptsegment->coefTextureWavelet = 0;
  
  if(chkColorVariance->isChecked())
    bptsegment->coefColorVariance = txtColorVariance->text().toFloat();
  else
    bptsegment->coefColorVariance = 0;

  if (chkHomogeneity->isChecked())
    bptsegment->coefHomogeneity = txtHomogeneity->text().toFloat();
  else
    bptsegment->coefHomogeneity = 0;

  if (chkMeanColor->isChecked())
    bptsegment->coefMeanColor = txtMeanColor->text().toFloat();
  else
    bptsegment->coefMeanColor = 0;

  if (chkRegionSize->isChecked())
    bptsegment->coefRegionSize = txtRegionSize->text().toFloat();
  else
    bptsegment->coefRegionSize = 0;

  if (chkMeanGradient->isChecked())
    bptsegment->coefMeanGradient = txtMeanGradient->text().toFloat();
  else
    bptsegment->coefMeanGradient = 0;

}

void BptSegmentForm::on_btnFileBrowser_clicked() {
  // File browser
  QString fileName = QFileDialog::getOpenFileName(this,
      tr("Open File"), QDir::currentPath());

  if (fileName.isEmpty()) {
    QMessageBox::information(this, tr("Image Viewer"),
        tr("Cannot load %1.").arg(fileName));
    return;
  }

  filename = fileName.toStdString();

  if (bptsegment != NULL)
    delete bptsegment;

  bptsegment = new BPTSegment();

  //bptsegment->regionFirstCount=spinNumberOfInitRegion->value();
  Image<U8C3>::load(filename.c_str(), bptsegment->imageSrc);
  //bptsegment->imageSrc=loadSimpleImage();

  imageSrc = &bptsegment->imageSrc;

  //bptsegment->volvementThreshold=spinEvolvementThreshold->value();
  bptsegment->volvementThreshold = txtEvolvementThreshold->text().toFloat();

  setCoefficient();

  if (ImageViewer::parent == NULL) {
    ImageViewer::parent = new QWidget();
    ImageViewer::parent->setParent(this);
  } else {
    delete ImageViewer::parent;
    ImageViewer::parent = new QWidget();
    ImageViewer::parent->setParent(this);
  }

  showImage(bptsegment->imageSrc, "rgb image");
  bptsegment->segment();
  refreshImage();

  /*
  spinSegmentNumber->setValue(bptsegment->regionFinalCount);
  spinSegmentNumber->setMaximum(bptsegment->regionFirstCount);
  spinSegmentNumber->setMinimum(1);

  slideSegmentNumber->setMaximum(bptsegment->regionFirstCount);
  slideSegmentNumber->setMinimum(1);
  slideSegmentNumber->setValue(bptsegment->regionFinalCount);
  */

  spinNumberOfPruningRegion->setValue(bptsegment->regionFinalCount);
  spinNumberOfPruningRegion->setMaximum(bptsegment->regionFirstCount);
  spinNumberOfPruningRegion->setMinimum(1);

}

void BptSegmentForm::on_btnSegmentStart_clicked() {
  if (bptsegment != NULL)
    delete bptsegment;

  bptsegment = new BPTSegment();

  //bptsegment->regionFirstCount=spinNumberOfInitRegion->value();
  Image<U8C3>::load(filename.c_str(), bptsegment->imageSrc);
  bptsegment->segment();

  refreshImage();

  /*spinSegmentNumber->setValue(bptsegment->regionFinalCount);
  spinSegmentNumber->setMaximum(bptsegment->regionFirstCount);
  spinSegmentNumber->setMinimum(1);
  slideSegmentNumber->setMaximum(bptsegment->regionFirstCount);
  slideSegmentNumber->setMinimum(1);
  slideSegmentNumber->setValue(bptsegment->regionFinalCount);
  */
}

void BptSegmentForm::on_btnView_clicked() {
  int value = spinNumberOfPruningRegion->value();
  //slideSegmentNumber->setMaximum(bptsegment->regionFirstCount);
  //slideSegmentNumber->setMinimum(1);
  //spinSegmentNumber->setValue(value);
  bptsegment->regionFinalCount = value;
  bptsegment->newPruning();
  refreshImage();
}

void BptSegmentForm::on_btnAutoPruning_clicked() {
  setCoefficient();
  int value = spinNumberOfPruningRegion->value();
  //slideSegmentNumber->setMaximum(bptsegment->regionFirstCount);
  //slideSegmentNumber->setMinimum(1);
  //spinSegmentNumber->setValue(value);
  bptsegment->regionFinalCount = value;
  bptsegment->newPruningAuto();
  //bptsegment->volvementThreshold=spinEvolvementThreshold->value();
  bptsegment->volvementThreshold = txtEvolvementThreshold->text().toFloat();
  refreshImage();
}

void BptSegmentForm::on_btnNoThreshold_clicked() {
  setCoefficient();
  int value = spinNumberOfPruningRegion->value();
// slideSegmentNumber->setMaximum(bptsegment->regionFirstCount);
  //slideSegmentNumber->setMinimum(1);
  //spinSegmentNumber->setValue(value);
  bptsegment->regionFinalCount = value;
  bptsegment->newPruningAutoNoThreshold();
  //bptsegment->volvementThreshold=spinEvolvementThreshold->value();
  bptsegment->volvementThreshold = txtEvolvementThreshold->text().toFloat();
  refreshImage();
}


void BptSegmentForm::on_slideSegmentNumber_valueChanged(int value) {
  //slideSegmentNumber->setMaximum(bptsegment->regionFirstCount);
  //slideSegmentNumber->setMinimum(1);
  //spinSegmentNumber->setValue(value);
  bptsegment->regionFinalCount = value;
  bptsegment->newPruning();
  refreshImage();
}

void BptSegmentForm::on_spinSegmentNumber_valueChanged(int value) {
  //slideSegmentNumber->setValue(value);
}

void BptSegmentForm::refreshImage() {
  //QPixmap pix;
  QImage image = getQImageFromMat(*bptsegment->showedFinalSegmentedImage);
  segmentedImage = &bptsegment->finalSegmentedImage;

  lblImage->setPixmap(QPixmap::fromImage(image));
  QRect rect = lblImage->geometry();

  lblImage->setGeometry(QRect(rect.x(), rect.y(), bptsegment->showedFinalSegmentedImage->getSizeX(), bptsegment->showedFinalSegmentedImage->getSizeY()));
}

void BptSegmentForm::on_btnShowManualClassification_clicked() {
  imageManuallyClassifier* imc = new imageManuallyClassifier(this);
  imc->srcImage = imageSrc;
  imc->imageSegment = segmentedImage;
  imc->colorSegmentedImage = this->bptsegment->showedFinalSegmentedImage;
  imc->bptsegment = bptsegment;
  char f[255];
  getFilename(filename.c_str(), f);
  imc->filename = f;
  imc->show();
  imc->loadImage();
  imc->loadClassName();
}

void BptSegmentForm::on_btnLearningFile_clicked() {
  // File browser
  QString fileName = QFileDialog::getOpenFileName(this,
      tr("Open File"), QDir::currentPath());

  if (fileName.isEmpty()) {
    QMessageBox::information(this, tr("Image Viewer"),
        tr("Cannot load %1.").arg(fileName));
  }

  learningFileName = fileName.toStdString();
}

void extractSegmentedImage(Image<U8C3>const & imageSrc, vector<int*> pointList, Image<U8C3> & imageRegion);

void BptSegmentForm::devideImage(Image<U8C3> const & srcImage, map<int, Image<U8C3>*>* segmentedImageMap) {
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

void BptSegmentForm::on_btnClassify_clicked() {

  /// devide the image into some regions (local image)
  map<int, Image<U8C3>*>* segmentedImageMap = new map<int, Image<U8C3>*>();
  devideImage(*imageSrc, segmentedImageMap);
  int sizemap = segmentedImageMap->size();

  /// extract features of regions
  map<int, Image<U8C3>*>::iterator iter;

  featureGenerator fg;
  fg.load(ConfigurationManager::instance());
  
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

//#define SVM
  /*
  #ifdef SVM
    // SVM classification
    LibSVMAdapter lsa;
    /// load the learning file
    lsa.loadProblem(learningFileName.c_str());
    /// classify the regions
    lsa.setTestData(vectorList);
    vector<double>* classes = new vector<double>();
    lsa.classify(classes);

  #else

    // Knn classification
    knnClassifier knn;
    knn.k = 10;
    knn.loadProblem(learningFileName.c_str());
    vector<double>* classes = new vector<double>();
    int size=vectorList.size();

    for (int i = 0; i < size; i++) {
      double distance=0;
      int classid = knn.classify(*vectorList[i],distance);
      classes->push_back((double)classid);
    }
  #endif
  */
  //LibSVMAdapter aclss;//=new LibSVMAdapter();
  cout<<"begin"<<endl;
  classifier* clss = mainClassifier;
  cout<<clss->getInfo()<<endl;
  clss->loadProblem(learningFileName.c_str());
  cout<<"end load pbl"<<endl;
  clss->reset();
  /// classify the regions
  clss->setTestData(vectorList);
  cout<<"classify"<<endl;
  clss->classify();
  cout<<"end"<<endl;
  cout << idList->size() << "vs" << clss->classIds.size();
  
  /// show the results
  ClassificationShowForm* csf = new ClassificationShowForm();
  csf->idList = idList;
  csf->classes = &clss->classIds;
  csf->segmentedImageMap = segmentedImageMap;
  csf->imageSrc = imageSrc;
  csf->show();
  csf->loadClassification();
}
void BptSegmentForm::on_btnSegFileBrowser_clicked() {
  // File browser
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath());
  if (!fileName.isEmpty()) {
    segmentVerifier.setSegFilePath(fileName.toStdString().c_str());
  }
  filename = fileName.toStdString();
}
void BptSegmentForm::on_btnCompaire_clicked() {

  segmentVerifier.compaire(bptsegment->finalSegmentedImage);
  cout << "Result compairaison:" << endl;
  cout << "Precision:\t" << segmentVerifier.getPrecision() << endl;
  cout << "Recall:\t" << segmentVerifier.getRecall() << endl;
  cout << "F-measure:\t" << segmentVerifier.getFMeasure() << endl;
  double h, hinvert, htotal;
  htotal = segmentVerifier.getDistanceHausdorff(h, hinvert);
  cout << "Hausdorff distance: \t" << h << "\t" << hinvert << "\t" << htotal << endl;
  Image<U8>* img1 = segmentVerifier.getContourSegmentImage();
  Image<U8>* img2 = segmentVerifier.getContourSampleImage();
  showImage(*img1, "Segment image");
  showImage(*img2, "Sample image");
  Image<TLabel>* imgColor = segmentVerifier.getSampleSegmentImage();
  Image<U8C3> sample = *segmentColorer::showSegmentationRandomColorFix(*imgColor, 1000);
  showImage(sample, "Sample image color");
  
  /*
  Image<U8C3> rgb;
  Image<U8C3>::load("../build-run/simple.png", rgb);
  Image<U8> u8(rgb.getSize());
  Image<U8> gray(rgb.getSize());
  cvtRGB2Gray(rgb,gray);
  Image<TLabel> lbl(rgb.getSize());
  for(int i=0; i<rgb.getSizeX(); i++){
    for(int j=0; j<rgb.getSizeY(); j++){
      lbl(i,j)=gray(i,j);
    }
  }
  segmentationVerifier::regionSeparation(lbl,u8);
  showImage(gray,"Segment image");
  showImage(u8,"Sample image");
  */
}
void BptSegmentForm::on_btnTreeViewer_clicked() {
  /*  OvenTimer* foo=new OvenTimer(this);
    foo->tree=&this->bptsegment->tree;
    foo->show();
    */
  showTreeViewer(&this->bptsegment->tree, this);
  /*
  treeInspector* treeinspector = new treeInspector(this);
  treeinspector->tree = &this->bptsegment->tree;
  treeinspector->load();
  treeinspector->show();
  */
}
void BptSegmentForm::on_btnTreeInspector_clicked() {
  /*  OvenTimer* foo=new OvenTimer(this);
    foo->tree=&this->bptsegment->tree;
    foo->show();
    */
  //showTreeInspector(&this->bptsegment->tree, this);

  treeInspector* tr = new treeInspector(this);
  tr->tree = &this->bptsegment->tree;
  tr->bptsegment = this->bptsegment;
  tr->srcImage = &this->bptsegment->imageSrc;

  tr->load();
  tr->show();

  /*
  treeInspector* treeinspector = new treeInspector(this);
  treeinspector->tree = &this->bptsegment->tree;
  treeinspector->load();
  treeinspector->show();
  */
}

void BptSegmentForm::on_btnClassSegment_clicked() {
  bptsegment->newPruningByClassification(learningFileName);
  refreshImage();


  /// devide the image into some regions (local image)
  map<int, Image<U8C3>*>* segmentedImageMap = new map<int, Image<U8C3>*>();
  devideImage(*imageSrc, segmentedImageMap);
  int sizemap = segmentedImageMap->size();

  /// extract features of regions
  map<int, Image<U8C3>*>::iterator iter;

  //featureGenerator fg;
  //fg.graySize = 32;
  //vector< vector<double>*> vectorList;
  vector<int>* idList = new vector<int>();
  vector<int>* classes = new vector<int>();
  for (iter = segmentedImageMap->begin(); iter != segmentedImageMap->end(); iter++) {
    int id = iter->first;
    //Image<U8C3>* rgb = iter->second;
    //showImage(*rgb,"aha");
    //vector<double> *vec = new vector<double>();
    //fg.generate(*rgb, vec);
    //vectorList.push_back(vec);
    idList->push_back(id);
    classes->push_back(bptsegment->graph.showedNode[id]->classId);
  }

  /// show the results
  ClassificationShowForm* csf = new ClassificationShowForm();
  csf->idList = idList;
  csf->classes = classes;
  csf->segmentedImageMap = segmentedImageMap;
  csf->imageSrc = imageSrc;
  csf->show();
  csf->loadClassification();
}

void BptSegmentForm::on_btnSaveImage_clicked() {
  // File browser
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QDir::currentPath().append("/untitle.ppm"), tr("Images (*.ppm)"));
  if (!fileName.isEmpty()) {
    string filename = fileName.toStdString();
    bptsegment->showedFinalSegmentedImage->save(filename.c_str());
  }
}

void BptSegmentForm::on_btnClassificationMethod_clicked() {
  ClassificationChooser* cc = new ClassificationChooser(this);
  cc->mainClassifier = mainClassifier;
  cc->show();
  cc->loadClassification();
  setWindowModality(Qt::WindowModal);
  if (cc->exec() == QDialog::Accepted) {
    cout << "ok" << endl;
    //if (mainClassifier != NULL)
      //delete mainClassifier;
    mainClassifier = cc->mainClassifier;
    bptsegment->mainClassifier = mainClassifier;
  } else {
    cout << "cancel" << endl;
  }
}

// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
