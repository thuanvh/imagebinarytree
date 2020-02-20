#include <learning/SIFTClassifier.h>
#include <iostream>

using namespace std;

#define ERR_DIR_MISSING "Missing directory"


int main(int argc,char** argv){
  //cout<<"start"<<endl;
  char* imageDir;
  char* classDir;
  char* fileout;
  for (int i = 0; i < argc; i++) {
    
    // directory reference
    if (strcmp(argv[i], "-imagedir") == 0) {
      i++;
      
      if (i < argc) {
        imageDir = argv[i];
      } else {
        throw ERR_DIR_MISSING;
      }
      
      continue;
    }
    
    // directory reference
    if (strcmp(argv[i], "-classdir") == 0) {
      i++;
      
      if (i < argc) {
        classDir = argv[i];
      } else {
        throw ERR_DIR_MISSING;
      }
      
      continue;
    }
    // directory reference
    if (strcmp(argv[i], "-fileout") == 0) {
      i++;
      
      if (i < argc) {
        fileout = argv[i];
      } else {
        throw ERR_DIR_MISSING;
      }
      
      continue;
    }
  }
  SIFTClassifier::generateDictionary(imageDir,classDir, fileout);
  
  return 0;
}