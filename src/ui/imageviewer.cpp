#include "imageviewer.h"
#include "../algo/tim2qt.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QBitmap>
ImageViewer::ImageViewer(QWidget* parent): QDialog(parent) {
  label = new QLabel(tr(""));
  QHBoxLayout *mainLayout = new QHBoxLayout;
  mainLayout->addWidget(label);
  setLayout(mainLayout);
}

template<>
void ImageViewer::showImage(const Image<U8C3>& image, const char* title) {
  QImage qimg = getQImageFromMat(image);
  setWindowTitle(QString(title));
  label->setPixmap(QPixmap::fromImage(qimg));
}

template<>
void ImageViewer::showImage(const Image<U8>& image, const char* title) {
  QImage qimg = getQImageFromMat(image);
  setWindowTitle(QString(title));
  //QPixmap px=QPixmap::fromImage(qimg);
  QBitmap px = QBitmap::fromImage(qimg);
  label->setPixmap(px);
}

template<>
ImageViewer& showImage(const Image< U8C3 >& image, const char* title,QWidget *parent) {
  ImageViewer* iv;
  if(parent==NULL)
    iv = new ImageViewer(ImageViewer::parent);
  else
    iv = new ImageViewer(parent);
  
  iv->showImage(image, title);
  iv->show();
  return *iv;
}

template<>
ImageViewer& showImage(const Image< U8 >& image, const char* title,QWidget *parent) {
  ImageViewer* iv ;
  if(parent==NULL)
    iv = new ImageViewer(ImageViewer::parent);
  else
    iv = new ImageViewer(parent);
  
  iv->showImage(image, title);
  iv->show();
  return *iv;
}
QWidget* ImageViewer::parent = NULL;
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
