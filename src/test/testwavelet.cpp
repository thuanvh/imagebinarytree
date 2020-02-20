
#include "../algo/waveletAdapter.h"
#include "../lib/LibTIM/Common/Image.h"
#include "../ui/imageviewer.h"

using namespace LibTIM;

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <QApplication>

using namespace std;

int main(int argc, char** argv) {
  QApplication app(argc, argv);

  string file(argv[1]);
  Image<RGB> a;
  Image<RGB>::load(argv[1], a);
  Image<U8> u8(a.getSize());

  showImage(a, "abc");
  a.save("a.ppm");

  for (int i = 0; i < a.getSizeX(); i++) {
    for (int j = 0; j < a.getSizeY(); j++) {
      RGB rgb = a(i, j);
      u8(i, j) = (rgb[0] + rgb[1] + rgb[2]) / 3;
    }
  }

  waveletAdapter wl;

  Image<F32> f(a.getSize());
  wl.waveletTransform(u8, f);

  Image<U8> u8view(a.getSize());

  for (int i = 0; i < a.getSizeX(); i++) {
    for (int j = 0; j < a.getSizeY(); j++) {
      if (f(i, j) > 255) {
        u8view(i, j) = 255;
      } else
        if (f(i, j) < 0) {
          u8view(i, j) = 0;
        } else {
          u8view(i, j) = (unsigned char)(f(i, j) + .5);
        }
    }
  }

  showImage(u8view, "ghj");

  u8view.save("u8v.ppm");
  pause();
  return 0;
}

// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;
