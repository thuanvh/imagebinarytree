#include <QtGui>

#include "ui/treeGraphicWidget.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
  
  GraphWidget widget;
  widget.show();
  return app.exec();
}