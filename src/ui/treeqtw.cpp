#include <QWidget>
#include <qpainter.h>
#include <QApplication>
#include <QMouseEvent>

#include "treeqtw.h"
#include <queue>
using namespace std;

OvenTimer::OvenTimer(QWidget* parent): QDialog(parent)
{
  
}

void OvenTimer::draw(QPainter* painter)
{
  queue<node*> q;
  tree->root->pos[0]=width() / 2;
  tree->root->pos[1]=0;
  int w=15;
  int w2=w/2;
  int h=15;
  q.push(tree->root);
  while(!q.empty()){
    node* n=q.front();
    q.pop();
    if(n->children[0]!=NULL){
      q.push(n->children[0]);
      n->children[0]->pos[0]=n->pos[0]-w2;
      n->children[0]->pos[1]=n->pos[1]+h;
      
      q.push(n->children[1]);
      n->children[1]->pos[0]=n->pos[0]+w2+1;
      n->children[1]->pos[1]=n->pos[1]+h;
    }
    painter->drawPoint(n->pos[0],n->pos[1]);
    if(n->parent!=NULL){
      painter->drawLine(n->pos[0],n->pos[1],n->parent->pos[0],n->parent->pos[1]);
    }
  }
  /*
  painter->drawPoint(20,0);
  painter->drawPoint(30,20);
  painter->drawLine(30,20,20,0);
  painter->drawPoint(10,20);
  painter->drawLine(10,20,20,0);
  */
}

void OvenTimer::mousePressEvent(QMouseEvent* event)
{
  QWidget::mousePressEvent(event);
  int a=0;  
}

void OvenTimer::paintEvent(QPaintEvent* event)
{
  QWidget::paintEvent(event);
  int a=0;
  QPainter painter(this);
  //painter.setRenderHint(QPainter::Antialiasing, true);
  int side = qMin(width(), height());
  //painter.setViewport((width() - side) / 2, (height() - side) / 2, side, side);
  //painter.setWindow(-50, -50, 100, 100);
  draw(&painter);  
}
