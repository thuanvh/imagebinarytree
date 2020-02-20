#include <iostream>
#include <stdio.h>
#include <lib/LibTIM/Common/Image.h>
#include <utility/filename.h>
#include <utility/imageIOText.h>
using namespace std;
using namespace LibTIM;
void MapToColor(int id, U8C3 & color);
void getIndexImage(Image<U8C3> &image, Image<TLabel>& lblImage) {
  int xsize = image.getSizeX();
  int ysize = image.getSizeY();
  map<int,U8C3*> classmap;
  int max=10;
  for(int i=0; i<max; i++){
    U8C3* a=new U8C3();
    MapToColor(i,*a);
    classmap.insert(pair<int,U8C3*>(i,a));
  }
  lblImage.fill(0);
  for (int x = 0; x < xsize; x++) {
    for (int y = 0; y < ysize; y++) {
      U8C3 A2 = image(x, y);

      cout<<" "<<(int)(A2)[0]<<" "<<(int)(A2)[1]<<" "<<(int)(A2)[2]<<endl;
      for(map<int,U8C3*>::iterator it=classmap.begin(); it!=classmap.end(); it++){
        int id=(*it).first;
        U8C3 a=*(*it).second;
        if(A2==a){
          //cout<<id<<" ";
          lblImage(x,y)=id;
          break;
        }
      }
    }
  }
  /*
  for (int x = 0; x < xsize; x++) {
    for (int y = 0; y < ysize; y++) {
      U8C3 A2 = image(x, y);
      U8 id = 0;
      U8 valR = (U8) A2[0];
      U8 valG = (U8) A2[1];
      U8 valB = (U8) A2[2];

      for (int j = 0; j < 8; j++)
        id = (id << 3) | (((valR >> j) & 1) << 0) | (((valG >> j) & 1) << 1) | (((valB >> j) & 1) << 2);
      lblImage(x, y) = (TLabel) id;
      
      if(classmap.find(id)==classmap.end()){
        U8C3* a=new U8C3();
        (*a)[0]=valR;
        (*a)[1]=valG;
        (*a)[2]=valB;
        cout<<"find:"<<(int)id<<" "<<(int)valR<<" "<<(int)valG<<" "<<(int)valB<<endl;
        classmap.insert(pair<int,U8C3*>(id,a));
      }
    }
  }
  */
  
  for(map<int,U8C3*>::iterator it=classmap.begin(); it!=classmap.end(); it++){
    int id=(*it).first;
    U8C3 a=*(*it).second;
    cout<<id<<" "<<"class"<<id<<" "<<(int)a[0]<<" "<<(int)a[1]<<" "<<(int)a[2]<<endl;
  }
  
}

void MapToColor(int id, U8C3 & color) {
  U8 valR = 0, valG = 0, valB = 0;
  for (int j = 0; id > 0; j++, id >>= 3) {
    valR |= (U8)(((id >> 0) & 1) << (7 - j));
    valG |= (U8)(((id >> 1) & 1) << (7 - j));
    valB |= (U8)(((id >> 2) & 1) << (7 - j));
  }
  color.el[0] = valR;
  color.el[1] = valG;
  color.el[2] = valB;
}
void getU8C3Image(Image<TLabel> const & lblImage, Image<U8C3> & image) {
  int xsize = image.getSizeX();
  int ysize = image.getSizeY();
  for (int x = 0; x < xsize; x++) {
    for (int y = 0; y < ysize; y++) {
      int id = (int) lblImage(x, y);
      U8 valR = 0, valG = 0, valB = 0;
      for (int j = 0; id > 0; j++, id >>= 3) {
        valR |= (U8)(((id >> 0) & 1) << (7 - j));
        valG |= (U8)(((id >> 1) & 1) << (7 - j));
        valB |= (U8)(((id >> 2) & 1) << (7 - j));
      }
      image(x, y)[0] = (U8) valR;
      image(x, y)[1] = (U8) valG;
      image(x, y)[2] = (U8) valB;
    }
  }
}
int testImageToId(string inputTruth, string outputTruth) {
  //string inputTruth = "shottondata/GroundTruth";
  //string outputTruth = "shottondata/GroundTruthText";
  vector<string> fileList;
  getFileList(inputTruth.c_str(), fileList);
  for (int i = 0; i < fileList.size(); i++) {
    string filename = fileList[i];
    cout<<filename<<endl;
    Image<U8C3> img;
    Image<U8C3>::load((inputTruth+"/"+filename).c_str(), img);
    
    
    Image<TLabel> lblimg(img.getSize());
    getIndexImage(img, lblimg);
    
    string filetext = outputTruth + "/" + filename + ".txt";
    saveImageText(filetext.c_str(), lblimg);
    img.save(filename.c_str());
  }
}
int testIdToImage(string inputTruth, string outputTruth) {
  

  vector<string> fileList;
  getFileList(inputTruth.c_str(), fileList);
  
  for (int i = 0; i < fileList.size(); i++) {
    string filename = fileList[i];
    cout<<filename<<endl;
    Image<TLabel> lblimg;
    getImageText((inputTruth+"/"+filename).c_str(), lblimg);

    Image<U8C3> img(lblimg.getSize());
    getU8C3Image(lblimg, img);
    char filenamenoext[255];
    getFilenameNotExtension(filename.c_str(), filenamenoext);

    string fileimg = outputTruth + "/" + filenamenoext;
    img.save(fileimg.c_str());
  }
}
int main(int argc, char** argv) {
  //string inputTruth = "shottondata/GroundTruthText";
  //string outputTruth = "shottondata/GroundTruthImage";
  
  string inputTruth="learning-data";
  string outputTruth="shotoninput";
  //testIdToImage(inputTruth,outputTruth);
  testImageToId("result2","result-data");
}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
