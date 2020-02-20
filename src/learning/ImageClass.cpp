#include "ImageClass.h"
#include <fstream>
using namespace std;
/**
 * @brief Load the list of classes from the file of class configuration
 * @param filename The file path of classname
 */
vector<ImageClass*>* ImageClass::loadImageClasses(const char* filename) {
  ifstream ifs;
  ifs.open(filename);
  vector<ImageClass*>* classList = new vector<ImageClass*>();
  while (ifs.good()) {
    ImageClass* ic = new ImageClass();
    ifs >> ic->id;
    ifs >> ic->name;
    int a, b, c;
    ifs >> a;
    ic->color[0] = a;
    ifs >> b;
    ic->color[1] = b;
    ifs >> c;
    ic->color[2] = c;
    //cout << ic->name << endl;
    if (ic->id == -1)
      break;
    classList->push_back(ic);
  }
  ifs.close();
  return classList;
}

// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;
