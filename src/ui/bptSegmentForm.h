#ifndef _BPTSEGMENTFORM_H
#define _BPTSEGMENTFORM_H

#include <QtGui/QDialog>
#include "ui_bptSegmentForm.h"
#include "../algo/bptsegment.h"
#include "../algo/segmentationVerifier.h"
#include <learning/classifier.h>

class BptSegmentForm : public QDialog, public Ui::BptSegmentForm {
  Q_OBJECT

public:
  BptSegmentForm(QWidget *parent = 0);
  ~BptSegmentForm();
  BPTSegment* bptsegment;
  string filename;
  string learningFileName;
  Image<U8C3>* imageSrc;
  Image<TLabel>* segmentedImage;
  segmentationVerifier segmentVerifier;
  classifier* mainClassifier;
  
private slots:
  void on_btnFileBrowser_clicked();
  void on_btnSegmentStart_clicked();
  void on_btnView_clicked();
  void on_btnAutoPruning_clicked();
  void on_btnNoThreshold_clicked();
  void on_btnShowManualClassification_clicked();
  void on_slideSegmentNumber_valueChanged(int value);
  void on_spinSegmentNumber_valueChanged(int value);
  void on_btnClassify_clicked();
  void refreshImage();
  void setCoefficient();
  void devideImage(Image<U8C3> const & srcImage, map<int, Image<U8C3>*>* segmentedImageMap);
  void on_btnLearningFile_clicked();
  void on_btnSegFileBrowser_clicked();
  void on_btnCompaire_clicked();
  void on_btnTreeInspector_clicked();
  void on_btnTreeViewer_clicked();
  void on_btnClassSegment_clicked();
  void on_btnSaveImage_clicked();
  void on_btnClassificationMethod_clicked();
};

#endif // GOTOCELLDIALOG_H
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
