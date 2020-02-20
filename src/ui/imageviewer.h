#ifndef _IMAGE_VIEWER_H_
#define _IMAGE_VIEWER_H_

#include <QDialog>
#include "../lib/LibTIM/Common/Image.h"

using namespace LibTIM;

class QCheckBox;

class QLabel;

class QLineEdit;

class QPushButton;

class ImageViewer : public QDialog {
  Q_OBJECT

public:
  ImageViewer(QWidget *parent = 0);
  template <class T>
  void showImage(Image<T> const & image, const char* title);
  static QWidget* parent;

private:
  QLabel *label;
};

template <class T>
ImageViewer& showImage(Image<T> const & image, const char* title,QWidget *parent = 0);
#endif
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
