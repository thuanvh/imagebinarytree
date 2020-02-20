#include <QWidget>
#include <qpainter.h>
#include <QApplication>
#include <QDialog>
#include "../algo/bptBinaryTree.h"
class OvenTimer : public QDialog//QWidget
{
  Q_OBJECT
public:
  OvenTimer(QWidget *parent = 0);
  BinaryTree* tree;
  
  //virtual ~OvenTimer();
  //  void setDuration(int secs);
  //  int duration() const;
  void draw(QPainter *painter);
  //signals:
  //  void timeout();
protected:
  void paintEvent(QPaintEvent *event);
  void mousePressEvent(QMouseEvent *event);
  //private:
  //  QDateTime finishTime;
  //  QTimer *updateTimer;
  //  QTimer *finishTimer;
};