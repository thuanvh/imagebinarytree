#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <QWidget>
#include <QDialog>
#include "treeInspector.h"
#include "objecttreemodel.h"
#include <queue>
#include "treeGraphicWidget.h"
#include <QHBoxLayout>
#include "treeGraphicNode.h"
#include <algo/tim2qt.h>
#include <qstandarditemmodel.h>
#include <qtableview.h>
using namespace std;
/*
id;
level;
colors[3];
texture;
numberOfPoint;
children[2];
parent;
isErased;
removeCost;
preserveCost;
childrenRemoved[2];
homogeneity;
isEvolvementConserved;
/// wavelet
LH, HL;
/// color covariance
colorVariance;
/// mean gradient
meanGradient;
pos[2];
graphicNode;
/// bounding
minX;
maxX;
minY;
maxY;
/// generic fourier descriptor
//vector<double> GFD;
classId;
*/
treeInspector::treeInspector(QWidget* parent): QDialog(parent) {
  setupUi(this);
  this->setWindowFlags(this->windowFlags() | Qt::WindowMinMaxButtonsHint);
  
  this->setLayout(vlayMain);
  loadTitle();

}
void treeInspector::loadTitle(){
  QStandardItemModel* model=new QStandardItemModel(23,2);
  QStringList list;
  list<<"Name"<<"Value";
  model->setHorizontalHeaderLabels(list);
  
  QStandardItem *item;
  item=new QStandardItem;
  item->setText("Id");
  model->setItem(0, 0, item);
  item=new QStandardItem;
  item->setText("Size");
  model->setItem(2, 0, item);
  item=new QStandardItem;
  item->setText("Color");
  model->setItem(3, 0, item);
  item=new QStandardItem;
  item->setText("Texture");
  model->setItem(4, 0, item);
  item=new QStandardItem;
  item->setText("level");
  model->setItem(5, 0, item);
  item=new QStandardItem;
  item->setText("children");
  model->setItem(6, 0, item);
  item=new QStandardItem;
  item->setText("parent");
  model->setItem(7, 0, item);
  item=new QStandardItem;
  item->setText("isErased");
  model->setItem(8, 0, item);
  item=new QStandardItem;
  item->setText("removeCost");
  model->setItem(9, 0, item);
  item=new QStandardItem;
  item->setText("preserveCost");
  model->setItem(10, 0, item);
  item=new QStandardItem;
  item->setText("childrenRemoved");
  model->setItem(11, 0, item);
  item=new QStandardItem;
  item->setText("homogeneity");
  model->setItem(12, 0, item);
  item=new QStandardItem;
  item->setText("isEvolvementConserved");
  model->setItem(13, 0, item);
  item=new QStandardItem;
  item->setText("wavelet LH HL");
  model->setItem(14, 0, item);
  item=new QStandardItem;
  item->setText("colorVariance");
  model->setItem(15, 0, item);
  item=new QStandardItem;
  item->setText("meanGradient");
  model->setItem(16, 0, item);
  item=new QStandardItem;
  item->setText("pos");
  model->setItem(17, 0, item);
  item=new QStandardItem;
  item->setText("minX");
  model->setItem(18, 0, item);
  item=new QStandardItem;
  item->setText("maxX");
  model->setItem(19, 0, item);
  item=new QStandardItem;
  item->setText("minY");
  model->setItem(20, 0, item);
  item=new QStandardItem;
  item->setText("maxY");
  model->setItem(21, 0, item);
  item=new QStandardItem;
  item->setText("classId");
  model->setItem(22, 0, item);
  
  
  //QTableView *tblNodeInfo;
  tblNodeInfo->setModel(model);
}
void treeInspector::loadValue(node* dataNode)
{
  QStandardItemModel* model=(QStandardItemModel*)tblNodeInfo->model();
  //QStringList list;
  //list<<"Name"<<"Value";
  //model->setHorizontalHeaderLabels(list);
  
  QStandardItem *item;
  item=new QStandardItem;
  item->setText(tr("%1").arg(dataNode->id));
  model->setItem(0, 1, item);
  item=new QStandardItem;
  item->setText(tr("%1").arg(dataNode->numberOfPoint));
  model->setItem(2, 1, item);
  item=new QStandardItem;
  item->setText(tr("%1,%2,%3").arg(dataNode->colors[0]).arg(dataNode->colors[1]).arg(dataNode->colors[2]));
  model->setItem(3, 1, item);
  item=new QStandardItem;
  item->setText(tr("%1").arg(dataNode->texture));
  model->setItem(4, 1, item);
  item=new QStandardItem;
  item->setText(tr("%1").arg(dataNode->level));
  model->setItem(5, 1, item);
  item=new QStandardItem;
  item->setText(tr("%1,%2").arg(dataNode->children[0]!=NULL?dataNode->children[0]->id:NULL).arg(dataNode->children[1]!=NULL?dataNode->children[1]->id:NULL));
  model->setItem(6, 1, item);
  item=new QStandardItem;
  item->setText(tr("%1").arg(dataNode->parent!=NULL?dataNode->parent->id:NULL));
  model->setItem(7, 1, item);
  item=new QStandardItem;
  item->setText(tr("%1").arg(dataNode->isErased));
  model->setItem(8, 1, item);
  item=new QStandardItem;
  item->setText(tr("%1").arg(dataNode->removeCost));
  model->setItem(9, 1, item);
  item=new QStandardItem;
  item->setText(tr("%1").arg(dataNode->preserveCost));
  model->setItem(10, 1, item);
  item=new QStandardItem;
  item->setText(tr("%1,%2").arg(dataNode->childrenRemoved[0]).arg(dataNode->childrenRemoved[1]));
  model->setItem(11, 1, item);
  item=new QStandardItem;
  item->setText(tr("%1").arg(dataNode->homogeneity));
  model->setItem(12, 1, item);
  item=new QStandardItem;
  item->setText(tr("%1").arg(dataNode->isEvolvementConserved));
  model->setItem(13, 1, item);
  item=new QStandardItem;
  item->setText(tr("%1,%2").arg(dataNode->LH).arg(dataNode->HL));
  model->setItem(14, 1, item);
  item=new QStandardItem;
  item->setText(tr("%1").arg(dataNode->colorVariance));
  model->setItem(15, 1, item);
  item=new QStandardItem;
  item->setText(tr("%1").arg(dataNode->meanGradient));
  model->setItem(16, 1, item);
  item=new QStandardItem;
  item->setText(tr("%1,%2").arg(dataNode->pos[0]).arg(dataNode->pos[1]));
  model->setItem(17, 1, item);
  item=new QStandardItem;
  item->setText(tr("%1").arg(dataNode->minX));
  model->setItem(18, 1, item);
  item=new QStandardItem;
  item->setText(tr("%1").arg(dataNode->maxX));
  model->setItem(19, 1, item);
  item=new QStandardItem;
  item->setText(tr("%1").arg(dataNode->minY));
  model->setItem(20, 1, item);
  item=new QStandardItem;
  item->setText(tr("%1").arg(dataNode->maxY));
  model->setItem(21, 1, item);
  item=new QStandardItem;
  item->setText(tr("%1").arg(dataNode->classId));
  model->setItem(22, 1, item);
  
  
  //QTableView *tblNodeInfo;
  //tblNodeInfo->setModel(model);
}
void extractSegmentedImage(Image<U8C3>const & imageSrc, vector<int*> pointList, Image<U8C3> & imageRegion);
void treeInspector::nodeClickHandler(Node* sender) {
  // load image here
  int id = sender->id;
  /// construct the file
  vector<int> leavesList;
  int leavesCount = bptsegment->graph.leavesCount;

  if (id <= leavesCount)
    leavesList.push_back(id);
  else
    leavesList = (*bptsegment->graph.graphMapToLeaves[id-leavesCount-1]);

  /// leaves
  Image<U8C3> localimage(srcImage->getSize());

  //segmentedImageMap->insert(pair<int, Image<U8C3>*> (id, localimage));

  U8C3 zero;
  zero[0] = zero[1] = zero[2] = 0;
  localimage.fill(zero);

  //}else{
  //  localimage=segmentedImageMap[classname];
  //}


  for (int mapid = 0; mapid < leavesList.size(); mapid++) {
    vector<int*>* pointsList = bptsegment->graph.pointsListOfLeaves[leavesList[mapid]];
    extractSegmentedImage(*srcImage, *pointsList, localimage);
  }

  lblImage->setPixmap(QPixmap::fromImage(getQImageFromMat(localimage)));

  //QTableView *tblNodeInfo;
  //tblNodeInfo->insert()
  loadValue(bptsegment->graph.showedNode[id]);
}
void treeInspector::load() {
  GraphWidget* widget = new GraphWidget(this);
  widget->loadTree(*tree, this);
  //widget->show();
  //layTree->
  //QHBoxLayout layTree;
  layTree->addWidget(widget);
}

/*void treeInspector::load2()
{

  node* root=tree->root;
  for(int i=0; i<root->id; i++){
    nodeList.push_back(NULL);
  }
  queue<node*> q;
  q.push(root);
  QObject* noderoot=new QObject();
  nodeList[root->id]=noderoot;

  while(!q.empty()){
    node* current=q.front();
    cout<<current->id<<endl;
    q.pop();
    QObject* child;
    if(current->children[0]!=NULL){
      child=new QObject(nodeList[current->id]);
      child->setObjectName(QString(current->children[0]->id));
      nodeList[current->children[0]->id]=child;

      child=new QObject(nodeList[current->id]);
      child->setObjectName(QString(current->children[1]->id));
      nodeList[current->children[1]->id]=child;

      q.push(current->children[0]);
      q.push(current->children[1]);
    }
  }
  ObjectTreeModel model( noderoot );

  trvTree->setModel( &model );
  trvTree->show();

}
*/
void showTreeInspector(BinaryTree* tree, QWidget* parent) {
  treeInspector* tr = new treeInspector(parent);
  tr->tree = tree;
  tr->load();
  tr->show();
  //tr->drawEdge=NULL;
  //tr->drawNode=NULL;
  //delete tr;
}

// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;
