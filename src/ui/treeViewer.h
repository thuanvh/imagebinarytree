#ifndef _TREE_VIEWER_H_
#define _TREE_VIEWER_H_


#define HAVE_BOOLEAN

#include <QDialog>
#include "ui_treeViewer.h"
#include "../algo/bptBinaryTree.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <QWidget>
#include <QDialog>

#include <queue>

//#include <graphviz/gvc.h>
//#include <graphviz/graph.h>
class treeViewer : public QDialog, public Ui::treeViewer {
  Q_OBJECT
  
public:
  BinaryTree* tree;
  treeViewer(QWidget *parent = 0);
  void load();
  //void (*drawNode)(Agnode_t *);
  //void (*drawEdge)(Agedge_t *);
};
void showTreeViewer(BinaryTree* tree, QWidget* parent=0);
#endif