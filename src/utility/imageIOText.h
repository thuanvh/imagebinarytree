#ifndef IMAGE_IO_TEXT_H
#define IMAGE_IO_TEXT_H

#include <lib/LibTIM/Common/Image.h>
#include <iostream>
#include <fstream>
using namespace std;
using namespace LibTIM;
/**
 * @brief Save label image to textfile
 */
template<class T>
void saveImageText(const char* filename, Image<T> & image) {
  ofstream ofs;
  ofs.open(filename);
  ofs << image.getSizeX() << " ";
  ofs << image.getSizeY() << " ";
  ofs << endl;

  int sizex = image.getSizeX();
  int sizey = image.getSizeY();
  for (int x = 0; x < sizex; x++) {
    int y = 0;
    T val = image(x, y);
    int count = 1;
    //int total=0;
    bool isbreak=false;
    while (y++ < sizey) {
      if(y==sizey){
        isbreak=true;
      }else
      if (val != image(x, y) ) {
        isbreak=true;
      }
      if(isbreak){
        ofs << val << ":" << count << " ";
        //cout << val << ":" << count << ":"<<y<<" " ;
        //total+=count;
        count = 0;
        if(y!=sizey)
          val = image(x, y);
        isbreak=false;
      }
      count++;
    }
    /*if(total != sizey){
      cout<<":"<<y<<endl;
      getchar();
    }*/
    ofs << endl;
  }
}
/**
 * @brief Load label image from a textfile
 */
template<class T>
void getImageText(const char* filename, Image<T> & image) {
  ifstream ifs;
  ifs.open(filename);
  int sizex = 0;
  int sizey = 0;
  ifs >> sizex;
  ifs >> sizey;
  //cout << sizex << "*" << sizey << endl;
  image.setSize(sizex, sizey, 1);
  for (int x = 0; x < sizex; x++) {
    int y = 0;
    //cout << endl;
    while (y < sizey) {
      string a;
      ifs >> a;
      int pos = a.find(':');

      if (pos != a.npos) {
        string valstr = a.substr(0, pos);
        string nbstr = a.substr(pos + 1, a.length() - pos);
        int val = atoi(valstr.c_str());
        int nb = atoi(nbstr.c_str());
        //cout << val << ":" << nb << " ";
        for (int i = 0; i < nb; i++, y++)
          image(x, y) = val;
      }
    }
  }
}


#endif
