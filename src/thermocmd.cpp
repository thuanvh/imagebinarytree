#undef USEQT
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;
#include "algo/bptsegment.h"

using namespace LibTIM;
//#include <QApplication>

int main(int argc, char** argv) {
  string filename(argv[1]);
  //QApplication app;
  BPTSegment* bptsegment = new BPTSegment();
  bptsegment->regionFirstCount = 10;
  Image<RGB>::load(filename.c_str(), bptsegment->imageSrc);
  //bptsegment->imageSrc=loadSimpleImage();


  bptsegment->segment();
  delete bptsegment;
  return 0;
}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
