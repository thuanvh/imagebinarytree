#ifndef _CLASSIFICATION_CHOOSER_H_
#define _CLASSIFICATION_CHOOSER_H_

#include <QtGui/QDialog>
#include <QSignalMapper>
#include "ui_classificationChooser.h"
#include "../algo/bptsegment.h"
#include <learning/classifier.h>

#define SVMSTR "SVM"
#define KNNSTR  "KNN"

#define KERNEL_POLY_STR "Poly"
#define KERNEL_LINEAR_STR "Linear"
#define KERNEL_GAUSSIAN_STR "Gaussian"
#define KERNER_RBF_STR "RBF"

class ClassificationChooser : public QDialog, public Ui::ClassificationChooser {
  Q_OBJECT
  
public:
  ClassificationChooser(QWidget *parent = 0);
  ~ClassificationChooser();
  void loadClassification();
  classifier* mainClassifier;
  
private slots:
  void on_btnOk_clicked();
  void on_btnCancel_clicked();
  void on_cboClassificationMethod_currentIndexChanged(const QString& text);
};

#endif // GOTOCELLDIALOG_H
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;
