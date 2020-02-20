#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QtGui/QGraphicsView>
#include <algo/bptBinaryTree.h>
class Node;
class NodeClickHandler;
class GraphWidget : public QGraphicsView
{
  Q_OBJECT
  
public:
  GraphWidget(QWidget *parent = 0);
  void loadTree(const BinaryTree& tree,NodeClickHandler* clickHander);
  void itemMoved();
  
protected:
  void keyPressEvent(QKeyEvent *event);
  void timerEvent1(QTimerEvent *event);
  void wheelEvent(QWheelEvent *event);
  //void drawBackground(QPainter *painter, const QRectF &rect);
  
  void scaleView(qreal scaleFactor);
  
private:
  int timerId;
  Node *centerNode;
  vector<Node* > nodeList;
  QGraphicsScene *myscene;
};

#endif