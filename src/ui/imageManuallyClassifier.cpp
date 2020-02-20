#include "imageManuallyClassifier.h"
#include <QGraphicsView>
#include <QGraphicsItem>
#include "../algo/tim2qt.h"
#include "../algo/convertImage.h"
#include <iostream>
#include <fstream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <vector>
#include <dirent.h>
#include <sys/stat.h>
#include <qlabel.h>
#include <learning/ImageClass.h>
#include <config/ConfigurationManager.h>
#include <utility/imageIOText.h>
using namespace std;

/**
 * constructor of classification
 */
imageManuallyClassifier::imageManuallyClassifier(QWidget* parent): QDialog(parent) {
  setupUi(this);
  changeOfClassedImage=false;
  classedImageViewer=NULL;
  mode=MARKING_MODE;
  isMarking=false;
}
/**
 * binding the data to combobox
 */
void imageManuallyClassifier::loadClassName() {
  int size = ConfigurationManager::instance()->classList->size();
  for (int i = 0; i < size; i++) {
    cboClassName->addItem(QString((*ConfigurationManager::instance()->classList)[i]->name.c_str()));
  }
  coeffc1 = 0.3;
  coeffc2 = 0.7;
}
/**
 * load image for classifier
 */
void imageManuallyClassifier::loadImage() {
  //QGraphicsScene* scene=new QGraphicsScene();
  //scene.addText("Hello, world!");
  //this->srcImage
  Image<U8C3> fusionImage(srcImage->getSize());
  mergeRGB(*srcImage, *colorSegmentedImage, coeffc1, coeffc2, fusionImage);
  QImage image = getQImageFromMat(fusionImage);

  //lblImage->setPixmap(QPixmap::fromImage(image));
  lblSegmentImage->setPixmap(QPixmap::fromImage(image));
  lblSegmentImage->show();
  lblSegmentImage->setGeometry(lblSegmentImage->x(), lblSegmentImage->y(), fusionImage.getSizeX(), fusionImage.getSizeY());
  //grvSegmentImage->setScene(scene);
  //grvSegmentImage->show();

  //view.show();
  colorClassedImage.setSize(srcImage->getSize());
  indexClassedImage.setSize(srcImage->getSize());
  colorClassedImage.fill(0);
  indexClassedImage.fill(0);
}
/**
 * from list of point, extract to an regional image
 */
void extractSegmentedImage(Image<U8C3>const & imageSrc, vector<int*> pointList, Image<U8C3> & imageRegion) {
  for (int i = 0; i < pointList.size(); i++) {
    int x = pointList[i][0];
    int y = pointList[i][1];
    imageRegion(x, y) = imageSrc(x, y);
  }
}
void imageManuallyClassifier::calculateTheClassedImage() {
  /// save file
  map<int, int>::iterator it;
  // show content:
  /*time_t rawtime;
  struct tm * timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  char timestr[255];
  sprintf(timestr, "%d.%d.%d.%ld", timeinfo->tm_year, timeinfo->tm_mon, timeinfo->tm_mday, rawtime);
  */
  map<int, Image<U8C3>*> segmentedImageMap;

  for (it = itemLabelMap.begin() ; it != itemLabelMap.end(); it++) {
    //cout << (*it).first << " => " << (*it).second << endl;
    int classId = (*it).second;
    ImageClass imageClass = *(*ConfigurationManager::instance()->classList)[classId];
    int segmentid = (*it).first;
    cout << imageClass.id << endl;
    //string filename;

    //filename+=timestr+"."+id+".ppm";
    //filename+="."+ string(id)+".ppm";


    /// construct the file
    vector<int> leavesList;
    int leavesCount = bptsegment->graph.leavesCount;

    if (segmentid <= leavesCount)
      leavesList.push_back(segmentid);
    else
      leavesList = (*bptsegment->graph.graphMapToLeaves[segmentid-leavesCount-1]);

    /// leaves
    /*Image<U8C3>* localimage;

    if (segmentedImageMap.find(imageClass.id) == segmentedImageMap.end()) {
      localimage = new Image<U8C3>(srcImage->getSize());
      segmentedImageMap.insert(pair<int, Image<U8C3>*>(imageClass.id, localimage));
      U8C3 zero;
      zero[0] = zero[1] = zero[2] = 0;
      localimage->fill(zero);
    } else {
      localimage = segmentedImageMap[imageClass.id];
    }
    */
    for (int mapid = 0; mapid < leavesList.size(); mapid++) {
      vector<int*>* pointsList = bptsegment->graph.pointsListOfLeaves[leavesList[mapid]];
      //extractSegmentedImage(*srcImage, *pointsList, *localimage);
      for (int i = 0; i < pointsList->size(); i++) {
        int x = (*pointsList)[i][0];
        int y = (*pointsList)[i][1];
        // set color
        colorClassedImage(x, y)[0] = (*ConfigurationManager::instance()->classList)[imageClass.id]->color[0];
        colorClassedImage(x, y)[1] = (*ConfigurationManager::instance()->classList)[imageClass.id]->color[1];
        colorClassedImage(x, y)[2] = (*ConfigurationManager::instance()->classList)[imageClass.id]->color[2];
        // set index
        indexClassedImage(x, y) = imageClass.id;
      }
    }
  }
}
/**
 * event handler of save button click
 */
void imageManuallyClassifier::on_btnSave_clicked() {

  calculateTheClassedImage();

  char afname[255];
  sprintf(afname, "learning-color/%s.ppm", filename.c_str());
  colorClassedImage.save(afname);
  cout<<"save "<< afname<<endl;
  sprintf(afname, "learning-data/%s.txt", filename.c_str());
  //indexClassedImage.save(afname);
  saveImageText(afname,indexClassedImage);
  cout<<"save "<< afname<<endl;
  /*
  map<string, Image<U8C3>*>::iterator itlocal;

  for (itlocal = segmentedImageMap.begin() ; itlocal != segmentedImageMap.end(); itlocal++) {
    string classname = (*itlocal).first;
    char afname[255];
    sprintf(afname, "%s.ppm", timestr);
    string filepath = foldername + "/" + classname + "/" + afname;
    mkdir((foldername + "/" + classname).c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
    //cout << (*it).first << " => " << (*it).second << endl;
    Image<U8C3>* localimage = (*itlocal).second;
    localimage->save(filepath.c_str());
  }*/
}
void imageManuallyClassifier::mouseMoveEvent(QMouseEvent* event)
{
    QWidget::mouseMoveEvent(event);
    if(isMarking){
      if (lblSegmentImage->geometry().contains(event->x(), event->y())) {
        mouseImagePositionX = event->x() - lblSegmentImage->x();
        mouseImagePositionY = event->y() - lblSegmentImage->y();
        int id = (*imageSegment)(mouseImagePositionX, mouseImagePositionY);
        showSegmentRegion(id);
        /*if (itemLabelMap.find(id) != itemLabelMap.end()) {
         c boClassName->setCurrentIn*dex(itemLabelMap[id]);;
      } else {
        cboClassName->setCurrentIndex(0);;
      }*/
        //cboClassName->setGeometry(event->x(), event->y(), cboClassName->width(), cboClassName->height());
        //int id = (*imageSegment)(mouseImagePositionX, mouseImagePositionY);
        int newmark;
        if (itemLabelMap.find(id) != itemLabelMap.end()) {
          newmark = itemLabelMap[id];
          itemLabelMap[id]=currentClassIndex;
          //newmark->setText(text);
        } else {
          newmark = currentClassIndex;
          itemLabelMap.insert(pair<int, int>(id, newmark));
        }
        
        calculateTheClassedImage();
        
        Image<U8C3> fusionImage(srcImage->getSize());
        mergeRGB(*srcImage, colorClassedImage, coeffc1, coeffc2, fusionImage);
        QImage image = getQImageFromMat(fusionImage);
        //lblSegmentImage->setPixmap(QPixmap::fromImage(image));
        if (classedImageViewer == NULL)
          classedImageViewer = &showImage(fusionImage, "classed image", this);
        else
          classedImageViewer->showImage(fusionImage, "classed image");
        
        //cboClassName->show();
          changeOfClassedImage = true;
      }
    }
}


void imageManuallyClassifier::mousePressEvent(QMouseEvent* event) {
  QWidget::mousePressEvent(event);
  if (lblSegmentImage->geometry().contains(event->x(), event->y())) {
    isMarking=!isMarking;
    //setMouseTracking(isMarking);
  }
  switch(mode){
    case REPAIR_MODE:
      if (lblSegmentImage->geometry().contains(event->x(), event->y())) {
        
        mouseImagePositionX = event->x() - lblSegmentImage->x();
        mouseImagePositionY = event->y() - lblSegmentImage->y();
        int id = (*imageSegment)(mouseImagePositionX, mouseImagePositionY);
        showSegmentRegion(id);
        if (itemLabelMap.find(id) != itemLabelMap.end()) {
          cboClassName->setCurrentIndex(itemLabelMap[id]);;
        } else {
          cboClassName->setCurrentIndex(0);;
        }
        //cboClassName->setGeometry(event->x(), event->y(), cboClassName->width(), cboClassName->height());
        
        cboClassName->show();
        changeOfClassedImage = true;
      }
      break;
    case MARKING_MODE:
      if (lblSegmentImage->geometry().contains(event->x(), event->y())) {
        
        mouseImagePositionX = event->x() - lblSegmentImage->x();
        mouseImagePositionY = event->y() - lblSegmentImage->y();
        int id = (*imageSegment)(mouseImagePositionX, mouseImagePositionY);
        showSegmentRegion(id);
        /*if (itemLabelMap.find(id) != itemLabelMap.end()) {
          cboClassName->setCurrentIndex(itemLabelMap[id]);;
        } else {
          cboClassName->setCurrentIndex(0);;
        }*/
        //cboClassName->setGeometry(event->x(), event->y(), cboClassName->width(), cboClassName->height());
        //int id = (*imageSegment)(mouseImagePositionX, mouseImagePositionY);
        int newmark;
        if (itemLabelMap.find(id) != itemLabelMap.end()) {
          newmark = itemLabelMap[id];
          itemLabelMap[id]=currentClassIndex;
          //newmark->setText(text);
        } else {
          newmark = currentClassIndex;
          itemLabelMap.insert(pair<int, int>(id, newmark));
        }
        
        calculateTheClassedImage();
        
        Image<U8C3> fusionImage(srcImage->getSize());
        mergeRGB(*srcImage, colorClassedImage, coeffc1, coeffc2, fusionImage);
        QImage image = getQImageFromMat(fusionImage);
        //lblSegmentImage->setPixmap(QPixmap::fromImage(image));
        if (classedImageViewer == NULL)
          classedImageViewer = &showImage(fusionImage, "classed image",this);
        else
          classedImageViewer->showImage(fusionImage, "classed image");
        
        //cboClassName->show();
        changeOfClassedImage = true;
      }
      break;  
  }

}

/*
 * show segmentation region
 */
void imageManuallyClassifier::showSegmentRegion(int segmentId) {
  vector<int> leavesList;
  int leavesCount = bptsegment->graph.leavesCount;

  if (segmentId <= leavesCount)
    leavesList.push_back(segmentId);
  else
    leavesList = (*bptsegment->graph.graphMapToLeaves[segmentId-leavesCount-1]);

  /// leaves
  Image<U8C3> mask = *colorSegmentedImage;

  for (int mapid = 0; mapid < leavesList.size(); mapid++) {
    vector<int*> pointsList = *bptsegment->graph.pointsListOfLeaves[leavesList[mapid]];

    for (int leavemapid = 0; leavemapid < pointsList.size(); leavemapid++) {
      int x = pointsList[leavemapid][0];
      int y = pointsList[leavemapid][1];
      mask(x, y) = (*srcImage)(x, y);
      //gradmean += rgb;
    }
  }

  Image<U8C3> fusionImage(srcImage->getSize());

  mergeRGB(*srcImage, mask, 0.7, 0.3, fusionImage);
  QImage image = getQImageFromMat(fusionImage);
  lblSegmentImage->setPixmap(QPixmap::fromImage(image));
}

void imageManuallyClassifier::on_cboClassName_currentIndexChanged(int index) {
  switch(mode){
    case REPAIR_MODE:
      if (changeOfClassedImage) {
        currentClassIndex=index;
        int id = (*imageSegment)(mouseImagePositionX, mouseImagePositionY);
        int newmark;
        if (itemLabelMap.find(id) != itemLabelMap.end()) {
          newmark = itemLabelMap[id];
          itemLabelMap[id]=currentClassIndex;
          //newmark->setText(text);
        } else {
          newmark = currentClassIndex;
          itemLabelMap.insert(pair<int, int>(id, newmark));
        }
        
        calculateTheClassedImage();
        
        Image<U8C3> fusionImage(srcImage->getSize());
        mergeRGB(*srcImage, colorClassedImage, coeffc1, coeffc2, fusionImage);
        QImage image = getQImageFromMat(fusionImage);
        //lblSegmentImage->setPixmap(QPixmap::fromImage(image));
        if (classedImageViewer == NULL)
          classedImageViewer = &showImage(fusionImage, "classed image",this);
        else
          classedImageViewer->showImage(fusionImage, "classed image");
        //newmark->setGeometry(mouseImagePositionX, mouseImagePositionY, 20, 20);
          changeOfClassedImage = false;
      }
      break;
    case MARKING_MODE:
      currentClassIndex=index;
      break;      
  }

}

void imageManuallyClassifier::on_radMarkingMode_toggled(bool checked)
{
  if(radMarkingMode->isChecked())
    mode=MARKING_MODE;
}

void imageManuallyClassifier::on_radRepairMode_toggled(bool checked)
{
  if(radRepairMode->isChecked())
    mode=REPAIR_MODE;
}

/*
void imageManuallyClassifier::on_cboClassName_currentIndexChanged(const QString& text) {
  //int x=cboClassName->x();
  //int y=cboClassName->y();
  //int dy=mainImageItem->y();
  //int dx=mainImageItem->x();
  int id = (*imageSegment)(mouseImagePositionX, mouseImagePositionY);
  QLabel* newmark;

  if (itemLabelMap.find(id) != itemLabelMap.end()) {
    newmark = itemLabelMap[id];
    newmark->setText(text);
  } else {
    newmark = new QLabel(text);
    itemLabelMap.insert(pair<int, QLabel*>(id, newmark));
  }

  newmark->setGeometry(mouseImagePositionX, mouseImagePositionY, 20, 20);

  //this->addWidget(newmark);

  //newmark->setNum();
  //lblSegmentImage->setZValue(0);
}
*/


/// need a function handled here
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;
