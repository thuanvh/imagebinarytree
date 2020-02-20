#include <QPainter>

#include "treeGraphicEdge.h"
#include "treeGraphicNode.h"

#include <math.h>

static const double Pi = 3.14159265358979323846264338327950288419717;
static double TwoPi = 2.0 * Pi;

Edge::Edge(Node *sourceNode, Node *destNode)
    : arrowSize(1) {
  setAcceptedMouseButtons(0);
  source = sourceNode;
  dest = destNode;
  drawArrow=false;
  source->addEdge(this);
  dest->addEdge(this);
  //adjust();
  
}

Node *Edge::sourceNode() const {
  return source;
}

Node *Edge::destNode() const {
  return dest;
}

void Edge::adjust() {
  if (!source || !dest)
    return;

  QLineF line(mapFromItem(source, 0, 0), mapFromItem(dest, 0, 0));
  qreal length = line.length();

  prepareGeometryChange();

  //if (length > qreal(5.)) {
    //QPointF edgeOffset((line.dx() * 10) / length, (line.dy() * 10) / length);
  QPointF edgeOffset(0,0);
    sourcePoint = line.p1() + edgeOffset;
    destPoint = line.p2() - edgeOffset;
  //} else {
  //  sourcePoint = destPoint = line.p1();
  //}
}

QRectF Edge::boundingRect() const {
  if (!source || !dest)
    return QRectF();

  qreal penWidth = 1;
  qreal extra = 0;//(penWidth + arrowSize) / 2.0;

  return QRectF(sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(),
      destPoint.y() - sourcePoint.y()))
      .normalized()
      .adjusted(-extra, -extra, extra, extra);
}

void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
  if (!source || !dest)
    return;

  QLineF line(sourcePoint, destPoint);
  if (qFuzzyCompare(line.length(), qreal(0.)))
    return;

  // Draw the line itself
  painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  painter->drawLine(line);

  if (drawArrow) {
/*
    // Draw the arrows
    double angle = ::acos(line.dx() / line.length());
    if (line.dy() >= 0)
      angle = TwoPi - angle;

    QPointF sourceArrowP1 = sourcePoint + QPointF(sin(angle + Pi / 3) * arrowSize,
        cos(angle + Pi / 3) * arrowSize);
    QPointF sourceArrowP2 = sourcePoint + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
        cos(angle + Pi - Pi / 3) * arrowSize);
    QPointF destArrowP1 = destPoint + QPointF(sin(angle - Pi / 3) * arrowSize,
        cos(angle - Pi / 3) * arrowSize);
    QPointF destArrowP2 = destPoint + QPointF(sin(angle - Pi + Pi / 3) * arrowSize,
        cos(angle - Pi + Pi / 3) * arrowSize);

    painter->setBrush(Qt::black);
    painter->drawPolygon(QPolygonF() << line.p1() << sourceArrowP1 << sourceArrowP2);
    painter->drawPolygon(QPolygonF() << line.p2() << destArrowP1 << destArrowP2);
*/
  }
}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
