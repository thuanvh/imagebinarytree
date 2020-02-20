#ifndef FILE_NAME_H
#define FILE_NAME_H
#include <string>
#include <vector>
using namespace std;
int getExtension(const char* filepath,char* extension);
int getFilename(const char* filepath,char* filename);
int getFilenameNotExtension(const char* filepath,char* filename);
void getFileList(const char* directory, vector<string>& fileList);

#endif