#include "filename.h"
#include <dirent.h>
#include <cstring>
#include <vector>
#include <string>
using namespace std;
/**
 * @brief Get extension of file path
 */
int getExtension(const char* filepath, char* extension) {
  int len = strlen(filepath);
  int id = -1;
  //char ext[10] = {0};

  for (int i = len - 1; i >= 0; i--) {
    if (filepath[i] == '.') {
      id = i;
      break;
    }
  }

  int size = len - id;
  int i = 0;
  for (i = 0; i < size; i++) {
    id++;
    extension[i] = filepath[id];
  }
  extension[i] = 0;
  return size;
}
/**
 * @brief Get file name from a file path
 */
int getFilename(const char* filepath, char* filename) {
  int len = strlen(filepath);
  int id = -1;
  //char ext[10] = {0};
  
  for (int i = len - 1; i >= 0; i--) {

    if (filepath[i] == '/') {
      id = i;
      break;
    }
  }

  int size = len - id;

  int i = 0;
  for (i = 0; i < size; i++) {
    id++;
    filename[i] = filepath[id];
  }
  filename[i] = 0;
  return size;
}
/**
 * @brief get file name without extension from a file path
 */
int getFilenameNotExtension(const char* filepath, char* filename)
{
  int len = strlen(filepath);
  int id = -1;
  //char ext[10] = {0};
  int posPoint=-1;
  bool found=false;
  for (int i = len - 1; i >= 0; i--) {
    if(!found)
    if(filepath[i]=='.'){
      posPoint=i;
      found=true;
    }
    if (filepath[i] == '/') {
      id = i;
      break;
    }
  }
  
  int size = len - id;
  if(posPoint!=-1)
    size=posPoint-id-1;
  int i = 0;
  for (i = 0; i < size; i++) {
    id++;
    filename[i] = filepath[id];
  }
  filename[i] = 0;
  return size;
}
/**
 * @brief Get file list in a directory
 */
void getFileList(const char* directory, vector<string>& fileList){
  // read list of file
  DIR *dp;
  struct dirent *ep;
  //vector<string> files;
  //cout<<"read folder "<<folder<<endl;
  dp = opendir(directory);
  if (dp != NULL) {
    while (ep = readdir(dp)) {
      //      puts(ep->d_name);
      if (ep->d_type == DT_REG)
        fileList.push_back((string) ep->d_name);
    }
    (void) closedir(dp);
  }
}

// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
