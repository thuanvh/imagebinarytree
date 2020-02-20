#ifndef _IMAGE_IO_ALL_H_
#define _IMAGE_IO_ALL_H_

#include <fstream>
#include <string>
#include <sstream>
#include <jpeglib.h>
#include <png.h>
#include <stdio.h>

namespace LibTIM {
  U8C3* loadppm(const char*filename, int& rows, int& cols);
  U8C3* loadJpeg(const char*filename, int& rows, int& cols);
  U8C3* loadPng(const char* filename, int& rows, int& cols);
}
#endif
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
