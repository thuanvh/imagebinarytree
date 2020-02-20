#include <QtGui/QApplication>
#include "thermaloberver.h"


int main(int argc, char** argv) {
  QApplication app(argc, argv);
  thermaloberver foo;
  foo.show();
  return app.exec();
}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
