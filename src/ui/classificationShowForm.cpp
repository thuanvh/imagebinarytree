#include "classificationShowForm.h"
#include <QSignalMapper>
#include "../algo/tim2qt.h"
#include <config/ConfigurationManager.h>
ClassificationShowForm::ClassificationShowForm(QWidget* parent): QDialog(parent) {
  setupUi(this);
}

ClassificationShowForm::~ClassificationShowForm() {
  // free the memory

}

void ClassificationShowForm::checkboxClicked(int checkboxId) {
  //cout<<"abc"<<endl;
  //=new QSignalMapper(this);

  //show the selected checkbox

  //QObject* o=mapper->mapping(checkboxId);
  //QCheckBox* cb=dynamic_cast<QCheckBox*> (o);
  //cout<< cb->text().toStdString() <<endl;

  Image<U8C3> total(imageSrc->getSize());
  U8C3 zero(0);
  total.fill(0);

  for (int ckid = 0; ckid < classedId.size(); ckid++) {
    int cbid = classedId[ckid];
    QObject* o = mapper->mapping(cbid);
    QCheckBox* cb = dynamic_cast<QCheckBox*>(o);

    if (cb->isChecked()) {
      string classname = (*ConfigurationManager::instance()->classList)[cbid]->name;

      for (int i = 0; i < idList->size(); i++) {
        if ((int)(*classes)[i] == cbid) {
          Image<U8C3>* rgb = (*segmentedImageMap)[(*idList)[i]];
          total += *rgb;
        }
      }
    }
  }

  QImage image = getQImageFromMat(total);

  lblImage->setPixmap(QPixmap::fromImage(image));
}

void ClassificationShowForm::loadClassification() {
  //loadClassname();

  mapper = new QSignalMapper(this);
  vector<int> a;

  for (int i = 0; i < classes->size(); i++)
    a.push_back((int)(*classes)[i]);

  make_heap(a.begin(), a.end());
  sort_heap(a.begin(), a.end());

  int currentid = -1;
  for (int i = 0; i < a.size(); i++) {
    if (currentid == a[i])
      continue;
    else
      currentid = a[i];

    //QVBoxLayout q=layCheckbox;
    QCheckBox* cb = new QCheckBox();
    cb->setText(QString( (*ConfigurationManager::instance()->classList)[a[i]]->name.c_str()));
    //qcheck.
    layCheckbox->addWidget(cb);
    connect(cb, SIGNAL(stateChanged(int)), mapper, SLOT(map()));
    mapper->setMapping(cb, currentid);
    classedId.push_back(currentid);
  }

  connect(mapper, SIGNAL(mapped(int)), this, SLOT(checkboxClicked(int)));
}
/*
void ClassificationShowForm::loadClassname() {
  string classFileName = "classname.txt";
  string foldername = ".";
  ifstream ifs;
  ifs.open(classFileName.c_str());
  string classname;

  while (!ifs.eof()) {
    ifs >> classname;
    int pos = classname.find('.');
    int a = atoi(classname.substr(0, pos).c_str());
    string b = classname.substr(pos + 1);
    //cout<<classname<<",";

    if (classname != "")
      classesNames.insert(pair<int, string>(a, b));
  }
}*/
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;
