#include "treeGraphicWidget.h"
#include "treeGraphicEdge.h"
#include "treeGraphicNode.h"

#include <QtGui>

#include <math.h>
#include <queue>

GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent), timerId(0) {
  myscene = new QGraphicsScene(this);
  myscene->setItemIndexMethod(QGraphicsScene::NoIndex);
  //myscene->setSceneRect(-200, -200, 400, 400);

  setScene(myscene);
  setCacheMode(CacheBackground);
  setViewportUpdateMode(BoundingRectViewportUpdate);
  setRenderHint(QPainter::Antialiasing);
  setTransformationAnchor(AnchorUnderMouse);
  scale(qreal(0.8), qreal(0.8));
  //setMinimumSize(400, 400);
  setWindowTitle(tr("Elastic Nodes"));

}
void doShapeTree(Node* tree, int yPosition, int last[], int * newPosition, int position);
/**
 * load tree to display
 */
void GraphWidget::loadTree(const BinaryTree& tree, NodeClickHandler* clickHandler) {
  node* root = tree.root;
  for (int i = 0; i < root->id; i++)
    nodeList.push_back(NULL);
  queue<node*> q;
  q.push(root);
  int maxLevel=0;
  Node* rootNode=NULL;
  while (!q.empty()) {
    node* current = q.front();
    q.pop();
    Node* cn = new Node(this);
    cn->id = current->id;
    cn->clickHandler = clickHandler;
    if(current->isErased)
      cn->color=Qt::red;
    nodeList[current->id] = cn;
    myscene->addItem(cn);
    //int dx = 10;
    //int dy = 10;
    //int randx = rand() % (dx / 2);
    if (current->parent != NULL) {
      Node* parentNode = nodeList[current->parent->id];
      cn->level=parentNode->level+1;
      myscene->addItem(new Edge(cn, parentNode));
      parentNode->branches[parentNode->nrBranches++]=cn;
      
      //if (current == current->parent->children[0]) {
      //  cn->setPos(parentNode->x() + dx + randx, parentNode->y() + dy);
      //} else {
      //  cn->setPos(parentNode->x() - dx - randx, parentNode->y() + dy);
      //}
    } else {
      cn->setPos(0, -200);
      cn->level=0;
      rootNode=cn;
    }
    if (current->children[0] != NULL) {      
      q.push(current->children[0]);
      q.push(current->children[1]);
    }
    maxLevel=max(maxLevel,cn->level);
  }
  maxLevel++;
  int last[maxLevel];
  for(int i=0; i<maxLevel; i++)
    last[i]=0;
  int newPosition=0;
  doShapeTree(rootNode,10,last,&newPosition,0);
}
/**
 * layout the tree
 * @param tree The node that needs to be drawn
 * @param yPosition The raster that the node lies on
 * @param last The last position occupied by a node at each raster
 * @param newPosition The position where the node is placed
 * @param position The leftmost legal position for the node
 */
void doShapeTree(Node* tree, int yPosition, int last[], int * newPosition, int position) {
#define Y_SEPARATION 6
#define MIN_X_SEPARATION 1
  int branchPosition;
  int i;
  int leftPosition;
  int rightPosition;
  int width;
  if (tree == (Node*)NULL)
    *newPosition = position;
  else {
    /* place subtree*/
    // ensure the nominal position of the node is right of any other node
    
    //for (i = yPosition - Y_SEPARATION; i < yPosition + tree->height; i++)
    //  position = max(position, last[i] + MIN_X_SEPARATION + tree->width / 2);
    position=max(position,last[tree->level]+ MIN_X_SEPARATION + tree->width / 2);

    if (tree->nrBranches >= 1) {
      if (tree->nrBranches > 1) {
        width = (tree->branches[0]->width + tree->branches[tree->nrBranches-1]->width) / 2 +
            (tree->nrBranches - 1) * MIN_X_SEPARATION;
        for (i = 1; i < tree->nrBranches - 1; i++)
          width += tree->branches[i]->width;
      } else
        width = 0;
      branchPosition = position - width / 2;
      // Position far left branch
      doShapeTree(tree->branches[0], yPosition + tree->height + Y_SEPARATION, last, & leftPosition, branchPosition);
      // Position the other branches if they exist
      rightPosition = leftPosition;
      for (i = 1; i < tree->nrBranches; i++) {
        branchPosition += MIN_X_SEPARATION + (tree->branches[i-1]->width + tree->branches[i]->width) / 2;
        doShapeTree(tree->branches[i], yPosition + tree->height + Y_SEPARATION, last, & rightPosition, branchPosition);
      }
      position = (leftPosition + rightPosition) / 2;
    }
  }// tree->nrBranches >=1;
  // Add node to last
  //for (i = yPosition - Y_SEPARATION; i < yPosition + tree->height; i++)
  //  last[i] = position + (tree->width + 1) / 2;
  last[tree->level]=position + (tree->width + 1) / 2;
  //tree->x = position;
  //tree->y = yPosition;
  tree->setPos(position,yPosition);
  *newPosition = position;
}
void GraphWidget::itemMoved() {
  if (!timerId)
    timerId = startTimer(1000 / 25);
}

void GraphWidget::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
    case Qt::Key_Up:
      centerNode->moveBy(0, -20);
      break;
    case Qt::Key_Down:
      centerNode->moveBy(0, 20);
      break;
    case Qt::Key_Left:
      centerNode->moveBy(-20, 0);
      break;
    case Qt::Key_Right:
      centerNode->moveBy(20, 0);
      break;
    case Qt::Key_Plus:
      scaleView(qreal(1.2));
      break;
    case Qt::Key_Minus:
      scaleView(1 / qreal(1.2));
      break;
    case Qt::Key_Space:
    case Qt::Key_Enter:
      foreach(QGraphicsItem *item, scene()->items()) {
        if (qgraphicsitem_cast<Node *>(item))
          item->setPos(-150 + qrand() % 300, -150 + qrand() % 300);
      }
      break;
    default:
      QGraphicsView::keyPressEvent(event);
  }
}

void GraphWidget::timerEvent1(QTimerEvent *event) {
  Q_UNUSED(event);

  QList<Node *> nodes;
  foreach(QGraphicsItem *item, scene()->items()) {
    if (Node *node = qgraphicsitem_cast<Node *>(item))
      nodes << node;
  }

//  foreach(Node *node, nodes)
//  node->calculateForces();

  bool itemsMoved = false;
  foreach(Node *node, nodes) {
    if (node->advance())
      itemsMoved = true;
  }

  if (!itemsMoved) {
    killTimer(timerId);
    timerId = 0;
  }
}

void GraphWidget::wheelEvent(QWheelEvent *event) {
  scaleView(pow((double)2, -event->delta() / 240.0));
}
/*
void GraphWidget::drawBackground(QPainter *painter, const QRectF &rect) {
  Q_UNUSED(rect);

  // Shadow
  QRectF sceneRect = this->sceneRect();
  QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
  QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
  if (rightShadow.intersects(rect) || rightShadow.contains(rect))
    painter->fillRect(rightShadow, Qt::darkGray);
  if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
    painter->fillRect(bottomShadow, Qt::darkGray);

  // Fill
  QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
  gradient.setColorAt(0, Qt::white);
  gradient.setColorAt(1, Qt::lightGray);
  painter->fillRect(rect.intersect(sceneRect), gradient);
  painter->setBrush(Qt::NoBrush);
  painter->drawRect(sceneRect);

  // Text
  QRectF textRect(sceneRect.left() + 4, sceneRect.top() + 4,
      sceneRect.width() - 4, sceneRect.height() - 4);
  QString message(tr("Click and drag the nodes around, and zoom with the mouse "
      "wheel or the '+' and '-' keys"));

  QFont font = painter->font();
  font.setBold(true);
  font.setPointSize(14);
  painter->setFont(font);
  painter->setPen(Qt::lightGray);
  painter->drawText(textRect.translated(2, 2), message);
  painter->setPen(Qt::black);
  painter->drawText(textRect, message);
}
*/
void GraphWidget::scaleView(qreal scaleFactor) {
  qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
  if (factor < 0.07 || factor > 100)
    return;

  scale(scaleFactor, scaleFactor);
}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;   replace-tabs on;  replace-tabs on;
