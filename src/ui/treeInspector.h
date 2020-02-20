#ifndef _TREE_INSPECTOR_H_
#define _TREE_INSPECTOR_H_


//#define HAVE_BOOLEAN

#include <QDialog>
#include "ui_treeInspector.h"
#include "../algo/bptBinaryTree.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <QWidget>
#include <QDialog>
#include "algo/bptsegment.h"
#include <queue>
#include "treeGraphicNode.h"

class treeInspector : public QDialog, public Ui::treeInspector,public NodeClickHandler {
  Q_OBJECT
  
public:
  BinaryTree* tree;
  treeInspector(QWidget *parent = 0);
  void load();
  vector<QObject*> nodeList;
  BPTSegment* bptsegment;
  Image<U8C3>* srcImage;
  //void (*drawNode)(Agnode_t *);
  //void (*drawEdge)(Agedge_t *);
  void nodeClickHandler(Node* sender);
  void loadTitle();
  void loadValue(node* item);
};
void showTreeInspector(BinaryTree* tree, QWidget* parent=0);
#endif