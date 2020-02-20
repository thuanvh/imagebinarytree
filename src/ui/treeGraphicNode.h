#ifndef NODE_H
#define NODE_H

#include <QGraphicsItem>
#include <QList>

class Edge;
class GraphWidget;
class QGraphicsSceneMouseEvent;
class NodeClickHandler;
//enum Qt::GlobalColor;
class Node : public QGraphicsItem
{
public:
  int height;
  int width;
  //int x;
  //int y;
  int nrBranches;
  Node* branches[2];
  int level;
  Qt::GlobalColor color;
  Node(GraphWidget *graphWidget);
  
  void addEdge(Edge *edge);
  QList<Edge *> edges() const;
  
  enum { Type = UserType + 1 };
  int type() const { return Type; }
  
  void calculateForces();
  bool advance();
  
  QRectF boundingRect() const;
  QPainterPath shape() const;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
  int id;
  NodeClickHandler * clickHandler;
protected:
  QVariant itemChange(GraphicsItemChange change, const QVariant &value);
  
  void mousePressEvent(QGraphicsSceneMouseEvent *event);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
  
private:
  QList<Edge *> edgeList;
  QPointF newPos;
  GraphWidget *graph;
};

class NodeClickHandler
{
public:
  virtual void nodeClickHandler(Node*) = 0;
};

#endif