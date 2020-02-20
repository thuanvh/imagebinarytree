#include "classificationChooser.h"

#include <learning/classifier.h>
#include <learning/knnClassifier.h>
#include <learning/LibSVMAdapter.h>
/**
 * Classification chooser constructor
 */
ClassificationChooser::ClassificationChooser(QWidget* parent): QDialog(parent) {
  setupUi(this);
  cboClassificationMethod->addItem(QString(SVMSTR));
  cboClassificationMethod->addItem(QString(KNNSTR));

  cboSvmKernel->addItem(QString(KERNEL_POLY_STR));
  cboSvmKernel->addItem(QString(KERNEL_LINEAR_STR));
  cboSvmKernel->addItem(QString(KERNEL_GAUSSIAN_STR));
  cboSvmKernel->addItem(QString(KERNER_RBF_STR));    
}
/**
 * load the information of classification to show up
 */
void ClassificationChooser::loadClassification() {
  if (mainClassifier != NULL) {
    if (dynamic_cast<knnClassifier*>(mainClassifier) != NULL) {
      int index = cboClassificationMethod->findText(QString(KNNSTR));
      cboClassificationMethod->setCurrentIndex(index);
      knnClassifier* knn = (knnClassifier*)mainClassifier;
      txtKnnK->setText(tr("%1").arg(knn->k));
    } else
      if (dynamic_cast<LibSVMAdapter*>(mainClassifier) != NULL) {
        int index = cboClassificationMethod->findText(QString(SVMSTR));
        cboClassificationMethod->setCurrentIndex(index);
        LibSVMAdapter* lsa = (LibSVMAdapter*)mainClassifier;
        txtSvmDegree->setText(tr("%1").arg(lsa->param.degree));
        txtSvmGamma->setText(tr("%1").arg(lsa->param.gamma));
      }
  }
}
/**
 * cancel button clicked handler
 */
void ClassificationChooser::on_btnCancel_clicked() {
  //close();
  //rejected();
  reject();
}
/**
 * combobox classification method changes
 */
void ClassificationChooser::on_cboClassificationMethod_currentIndexChanged(const QString& text) {
  if (text.toStdString().compare(SVMSTR)==0) {
    grpKNN->hide();
    grpSVM->show();
  } else
    if (text.toStdString().compare(KNNSTR)==0) {
      grpSVM->hide();
      grpKNN->show();
    }
}
/**
 * button ok classification click
 */
void ClassificationChooser::on_btnOk_clicked() {
  //if(mainClassifier!=NULL)
  //  delete mainClassifier;
  if (cboClassificationMethod->currentText().compare(QString(KNNSTR)) == 0) {
    knnClassifier* knn = new knnClassifier();
    knn->k = txtKnnK->text().toInt();
    mainClassifier = knn;
  } else
    if (cboClassificationMethod->currentText().compare(QString(SVMSTR)) == 0) {
      LibSVMAdapter* lsa = new LibSVMAdapter();
      lsa->param.gamma = txtSvmGamma->text().toDouble();
      lsa->param.degree = txtSvmDegree->text().toDouble();
      
      if(cboSvmKernel->currentText().compare(QString(KERNEL_POLY_STR))==0){
        lsa->param.kernel_type= POLY;//RBF;
      }else if(cboSvmKernel->currentText().compare(QString(KERNEL_LINEAR_STR))==0){
        lsa->param.kernel_type= LINEAR;//RBF;
      }else if(cboSvmKernel->currentText().compare(QString(KERNEL_GAUSSIAN_STR))==0){
        lsa->param.kernel_type= SIGMOID;//RBF;
      }else if(cboSvmKernel->currentText().compare(QString(KERNER_RBF_STR))==0){
        lsa->param.kernel_type= RBF;
      }
      mainClassifier=lsa;
    }
  accept();
  
}

/**
 * destructor
 */
ClassificationChooser::~ClassificationChooser() {

}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
