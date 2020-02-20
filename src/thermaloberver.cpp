#include "thermaloberver.h"

#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>

thermaloberver::thermaloberver() {
  QLabel* l = new QLabel(this);
  l->setText("Hello World!");
  setCentralWidget(l);
  QAction* a = new QAction(this);
  a->setText("Quit");
  connect(a, SIGNAL(triggered()), SLOT(close()));
  menuBar()->addMenu("File")->addAction(a);
}

thermaloberver::~thermaloberver() {}

#include "thermaloberver.moc"
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
