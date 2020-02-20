#ifndef _IMAGE_MANUALLY_CLASSIFIER_H_
#define _IMAGE_MANUALLY_CLASSIFIER_H_

#include <QtGui/QDialog>
#include "ui_imageManuallyClassifier.h"
#include "imageviewer.h"
#include "../algo/bptsegment.h"
#include <QMouseEvent>
#include <QGraphicsTextItem>

class imageManuallyClassifier : public QDialog, public Ui::imageManuallyClassifier {
  Q_OBJECT

public:
  imageManuallyClassifier(QWidget *parent = 0);
  string filename;
  Image<U8C3>* srcImage;
  Image<TLabel>* imageSegment;
  Image<U8C3>* colorSegmentedImage;
  Image<U8C3> colorClassedImage;
  Image<TLabel> indexClassedImage;
  BPTSegment* bptsegment;
  void loadImage();
  float coeffc1, coeffc2;
  string classFileName;
  string foldername;
  void loadClassName();
  
private slots:
  //void on_btnFileBrowser_clicked();
  void on_btnSave_clicked();
  void on_radRepairMode_toggled(bool checked);
  void on_radMarkingMode_toggled(bool checked);
  //void on_cboClassName_currentIndexChanged(const QString& text);
  void on_cboClassName_currentIndexChanged(int index);
  //void on_grvSegmentImage_mousePressed();
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void showSegmentRegion(int segmentId);
  // recalculate the classed image
  void calculateTheClassedImage();
private:
  // Map between the item segment id and class id
  map<int, int> itemLabelMap;
  int mouseImagePositionX, mouseImagePositionY;
  int currentClassIndex;
#define REPAIR_MODE 0
#define MARKING_MODE 1
  int mode;
  ImageViewer* classedImageViewer;
  bool changeOfClassedImage;
  bool isMarking;
};

#endif // _IMAGE_MANUALLY_CLASSIFIER_H_
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
