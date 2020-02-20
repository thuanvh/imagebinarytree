#define USEQT

#include <QApplication>
#include "ui/bptSegmentForm.h"
#include "algo/bptsegment.h"
int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  BptSegmentForm *dialog = new BptSegmentForm;
  dialog->show();

  return app.exec();
}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;
