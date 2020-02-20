#ifndef _CLASSIFICATION_SHOW_FORM_H_
#define _CLASSIFICATION_SHOW_FORM_H_

#include <QtGui/QDialog>
#include <QSignalMapper>
#include "ui_classificationShowForm.h"
#include "../algo/bptsegment.h"

class ClassificationShowForm : public QDialog, public Ui::ClassificationShowForm {
  Q_OBJECT

public:
  ClassificationShowForm(QWidget *parent = 0);
  ~ClassificationShowForm();
  void loadClassification();
  void loadClassname();
  vector<int>* idList;
  vector<int>* classes;
  vector<int> classedId;
  //map<int, string> classesNames;
  map<int, Image<U8C3>*>* segmentedImageMap;
  Image<U8C3>* imageSrc;

private:
  QSignalMapper *mapper;

private slots:
  void checkboxClicked(int state);

};

#endif // GOTOCELLDIALOG_H
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
