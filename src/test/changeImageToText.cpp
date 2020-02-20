#include <lib/LibTIM/Common/Image.h>
#include <iostream>
#include <utility/filename.h>
#include <utility/imageIOText.h>
#include <algo/convertImage.h>
using namespace std;
using namespace LibTIM;

int main(int argc,char** argv){
  string folder="learning-data";
  vector<string> fileList;
  getFileList(folder.c_str(),fileList);
  for(int i=0; i<fileList.size(); i++){
    string filename=fileList[i];
    char filenamenoext[255];
    getFilenameNotExtension(filename.c_str(),filenamenoext);
//#define image2text
#ifdef image2text
    Image<U8> imgu8;
    Image<U8>::load((string("abctest/")+filenamenoext+".pgm").c_str(),imgu8);
    Image<TLabel> imglbl=imgu8;
    string ftxt= folder+"/"+ filenamenoext+".txt";    
    saveImageText(ftxt.c_str(),imglbl);
#else
    Image<TLabel> imglbl;
    string ftxt= folder+"/"+ filenamenoext+".txt";
    getImageText(ftxt.c_str(),imglbl);
    Image<U8> imgu8=imglbl;
       
    scaleImageRange(imgu8,imgu8,255,0);
    imgu8.save((folder+"/"+filenamenoext+".pgm").c_str());
#endif
  }
  return 0;
}