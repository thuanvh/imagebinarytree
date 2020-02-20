#ifndef IMAGE_CLASS_H
#define IMAGE_CLASS_H

#include <string>
#include <vector>
#include <iostream>
using namespace std;

class ImageClass {
public:
  ImageClass() {
    id = -1;
    color[0] = color[1] = color[2] = 0;
  }
  int id;
  string name;
  unsigned char color[3];
  ~ImageClass(){};
  static vector<ImageClass*>* loadImageClasses(const char * filename);
};

#endif
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
