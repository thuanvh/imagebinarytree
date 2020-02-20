#include "textonizer.h"
#include <algo/convolution.h>
#include <dirent.h>
#include <algo/convertImage.h>
#include "kdtree.h"
#include "kmeanClustering.h"
#include <algo/segmentColorer.h>
#include <algo/distanceCalculate.h>
#include <utility/filename.h>
#include <utility/imageIOText.h>
#include <list>
void printVector(vector<double>* a) {
  for (int i = 0; i < a->size(); i++) {
    cout << (*a)[i] << " ";
  }
  cout << endl;
}
void printVector(vector<vector<double>*>* a) {
  for (int i = 0; i < a->size(); i++) {
    for (int j = 0; j < (*a)[i]->size(); j++) {
      cout << (*(*a)[i])[j] << " ";
    }
    cout << endl;
  }

}
/**
 * @brief Initiation of textonisation. This function collects all the files images in a folder.
 */
void Textonizer::init() {
  fileList.clear();
  getFileList(inputfolder.c_str(), fileList);
}
/**
 * @brief All work of textonize. It includes the initiation, the creation of filter bank, the convolution and clustering
 */
void Textonizer::textonize() {
  cout << "Initialisation" << endl;
  init();
  if (isLearning){
    initFilterBank();
    SaveFilterBank(filterBank,"filterbank.txt");
  }else{
    LoadFilterBank(filterBank,"filterbank.txt");
  }
    
  // convolution
  cout << "Convolution" << endl;
  convolution();
  cout << "Clustering and labeling" << endl;
  if (isLearning) {
    // kmeans and labeling
    textonClustering();

  } else {
    vector<vector<double>*> centroids;
    loadCentroids(centroids);
    Labeling(&labelresponses,centroids);
    textonLabeling(centroids);
  }
  // save image
  cout << "Save image" << endl;
  saveTextonImage();
}
/**
 * @brief Load centroids
 * @param centroids The value container of centroids
 */
void Textonizer::loadCentroids(vector<vector<double>*>& centroids) {
  // load centroids
  ifstream ifs;
  ifs.open(centroidsFile.c_str());
  int dimension;
  //km.dimension<<ifs;
  ifs >> dimension;
  //bool initVector=true;
  vector<double>* aVector;
  int itemCount = 0;
  while (ifs.good()) {
    if (itemCount % dimension == 0) {
      aVector = new vector<double>;
      centroids.push_back(aVector);
      itemCount = 0;
    }
    double a;
    ifs >> a;
    aVector->push_back(a);
    itemCount++;
  }
}
void Textonizer::Labeling(vector<TLabel>*  labelresponses, vector<vector<double>*>& centroids){
  labelresponses->clear();
  for (int i = 0; i < responses->size(); i++) {
    vector<double>* avec = (*responses)[i];
    //int id = tree.NearestNeighbour(*avec);
    int id = 0;
    double minDistance = std::numeric_limits<double>::max();;
    for (int j = 0; j < nbCluster; j++) {
      vector<double>* cen = centroids[j];
      double dist = distanceCalculator::getDistanceEuclidien(*avec, *cen);
      if (dist < minDistance) {
        minDistance = dist;
        id = j;
      }
    }
    labelresponses->push_back(id);
    //(*labelresponses)[i]=id;
  }
}
/**
 * @brief Texton labeling
 */
void Textonizer::textonLabeling(vector<vector<double>*>& centroids) {
  cout << "Labeling" << endl;
// get list of label image
  /*
    kdTree tree(centroids, 3);
    for (int i = 0; i < responses->size(); i++) {
      vector<double>* avec = (*responses)[i];
      int id = tree.NearestNeighbour(*avec);
      labelresponses.push_back(id);
    }
    */
  
  /*
    for (int i = 0; i < responses->size(); i++) {
      vector<double>* avec = (*responses)[i];
      //int id = tree.NearestNeighbour(*avec);
      int id = 0;
      double minDistance = std::numeric_limits<double>::max();;
      for (int j = 0; j < nbCluster; j++) {
        vector<double>* cen = centroids[j];
        double dist = distanceCalculator::getDistanceEuclidien(*avec, *cen);
        if (dist < minDistance) {
          minDistance = dist;
          id = j;
        }
      }
      labelresponses[i]=id;
    }
  */
  
  //vector<TLabel>::iterator it = labelresponses.begin();
  int resid = 0;
  for (int i = 0; i < imageOriginalList.size(); i++) {
    int sizex = imageOriginalList[i]->getSizeX();
    int sizey = imageOriginalList[i]->getSizeY();
    Image<TLabel>* labelimg = new Image<TLabel>(imageOriginalList[i]->getSize());
    labelimg->fill(0);
    for (int x = 0; x < sizex; x++) {
      for (int y = 0; y < sizey; y++) {
        (*labelimg)(x, y) = labelresponses[resid];//*it;
        //it++;
        resid++;
      }
    }
    /*
    char s[255];
    sprintf(s, "%d.org.ppm", i);
    imageOriginalList[i]->save(s);
    sprintf(s, "%d.out.pgm", i);
    Image<U8> a = *labelimg;
    scaleImageRange<U8>(a, a, 255, 0);
    a.save(s);
    */
    imageLabelList.push_back(labelimg);
    
  }
}
Textonizer::Textonizer() {
  nbCluster = 10;
}
Textonizer::~Textonizer() {

}

/**
 * @brief Clustering the responses of convoluton to find the texton
 */
void Textonizer::textonClustering() {
  // clustering
  Kmean km;
  //km.max = valueMax;
  //km.min = valueMin;
  km.dimension = vectorSize;
  km.k = nbCluster;
  km.maxTimes= kmeansMaxTimes;
  //km.initCentroids();
  printVector(valueMax);
  printVector(valueMin);
  cout << "centre before" << endl;
  printVector(&km.centroids);
  labelresponses.resize(responses->size());
  km.clustering(*responses, labelresponses);
  cout << "centre after" << endl;
  printVector(&km.centroids);





  // save centroids
  ofstream ofs;
  ofs.open(centroidsFile.c_str());
  ofs << km.dimension << endl;
  for (int i = 0; i < km.centroids.size(); i++) {
    for (int j = 0; j < km.centroids[i]->size(); j++)
      ofs << (*km.centroids[i])[j] << " ";
    ofs << endl;
  }
  ofs.close();

  textonLabeling(km.centroids);


  Image<TLabel> image((int)(*valueMax)[0] + 1, (int)(*valueMax)[1] + 1, 1);
  image.fill(0);
  for (int i = 0; i < labelresponses.size(); i++) {
    image((*(*responses)[i])[0], (*(*responses)[i])[1]) = labelresponses[i] + 1;
  }

  segmentColorer* sc = segmentColorer::instance();
  sc->initRandomColor(km.k + 1);
  Image<U8C3>* image2 = sc->showSegmentationRandomColorFix(image, km.k + 1);
  for (int i = 0; i < km.centroids.size(); i++) {
    (*image2)((*km.centroids[i])[0], (*km.centroids[i])[1]) = 255;
  }
  image2->save("kmeans.ppm");
}
/**
 * @brief Save label image to disk
 */
void Textonizer::saveTextonImage() {
  int size = imageLabelList.size();
  int dimension = (*responses)[0]->size();
  // load coloring
  if (isLearning) {
    segmentColorer* s = segmentColorer::instance();
    s->initRandomColor(dimension);
    segmentColorer::save(s, colorFile.c_str());
  } else {
    segmentColorer* s = segmentColorer::load(colorFile.c_str());
  }

  for (int i = 0; i < size; i++) {
    Image<TLabel>* alabelimage = imageLabelList[i];
    string filename = outputfolder + "/" + fileList[i] + ".txt";
    //Image<U8> gray = *alabelimage;
    //Image<U8> gray255(gray.getSize());
    //scaleImageRange<U8>(gray, gray255, 255, 0);
    saveImageText(filename.c_str(),*alabelimage);
    //gray255.save(filename.c_str());


    string filenamecolor = outputfolderColor + "/" + fileList[i] + ".ppm";
    Image<U8C3>* colorImage = segmentColorer::instance()->showSegmentationRandomColorFix(*alabelimage, dimension);
#ifdef DEBUG
    for (int a = 0; a < colorImage.getSizeX(); a++) {
      for (int b = 0; b < colorImage.getSizeX(); b++) {
        cout << (*alabelimage)(a, b) << " ";
        /*cout<<"(";
        for(int c=0; c<3; c++)
          cout<< (int)colorImage(a,b)[c];
        cout<<") ";*/
      }
      cout << endl;
    }
#endif
    colorImage->save(filenamecolor.c_str());

    imageOriginalList[i]->save(string(outputfolderColor + "/" + fileList[i]).c_str());
    delete colorImage;
  }

}
Image<U8C3>* loadSimpleImage2() {
  U8 data[] = {
    1, 0, 0, 2, 0, 0, 3, 0, 0, 4, 0, 0, 0, 0, 0,
    3, 0, 0, 4, 0, 0, 4, 0, 0, 4, 0, 0, 7, 0, 0,
    2, 0, 0, 2, 0, 0, 1, 0, 0, 5, 0, 0, 0, 0, 0
  };

  TSize size[] = {5, 3, 1};
  TSpacing space[] = {1, 1, 1};
  Image<U8C3>* image = new Image<U8C3> (size, space, (U8C3*) data);
  return image;
}
/**
 * @brief Convolution of all input images
 *
 */
void Textonizer::convolution() {
  long sizeall = 0;
  vectorSize = filterBank.size();
  for (int i = 0; i < fileList.size(); i++) {
    string filepath = inputfolder + "/" + fileList[i];
    cout << filepath << endl;
    Image<U8C3>* img = new Image<U8C3>();
    Image<U8C3>::load(filepath.c_str(), *img);
    //Image<U8C3>* img =loadSimpleImage2();
    imageOriginalList.push_back(img);
    sizeall += img->getBufSize();
  }
  //cout<<"all size:"<<sizeall;
  //responses.resize(sizeall,0);
  responses = new vector<vector<double>*>(sizeall, 0);
  for (int i = 0; i < responses->size(); i++) {
    (*responses)[i] = new vector<double>(vectorSize, 0);
  }
  valueMax = new vector<double>(vectorSize, -1);
  valueMin = new vector<double>(vectorSize, 10e10);
  int idresponse = 0;
  for (int i = 0; i < fileList.size(); i++) {
    //vector<vector<double>*> responseItem;
    Image<U8C3>* img = imageOriginalList[i];
    convolutionItem(*img, *responses, idresponse);
  }
}
/**
 * @brief Convolution of each image
 */
void Textonizer::convolutionItem(Image<U8C3>& image, vector<vector<double>*> & valueResponse, int &id) {

  Image<float> imageBands[3];
  for (int i = 0; i < 3; i++)
    imageBands[i] = Image<float>(image.getSize());

  Image<float> imageGray(image.getSize());
  cvtRGB2Gray(image, imageGray);
  Image<U8C3> lab(image.getSize());
  cvtRGB2Lab(image, lab);
  divide3Chains(lab, imageBands);


  vector<vector<double>*>::iterator beginIter = valueResponse.begin() + id;
  //cout<<image.getSizeX()<<"x"<<image.getSizeY()<<"="<<image.getBufSize()<<endl;
  int imagesize = image.getBufSize();
  //valueResponse.resize(valueResponse.size()+imagesize,NULL);
  //list<list<double>*> abc;
  //for (int i = 0; i < imagesize; i++) {
  //list<double>* a=new list<double>(vectorSize);
  //abc.push_back(a);
  //}

  //cout<<valueResponse.size()<<endl;
  //vector<double> a;//=new vector<double>();//vectorSize,0);;
  //for (int i = 0; i < imagesize; i++) {
  //cout<<i<<" ";
  //a=new vector<double>(vectorSize,0);
  //valueResponse.push_back(a);
  //*(beginIter+i)=a;
  //*(beginIter+i)=a;//new vector<double>();
  //}

  int sizex = image.getSizeX();
  int sizey = image.getSizeY();

  for (int i = 0; i < vectorSize; i++) {
    FilterBankElement* ele = filterBank[i];
    Kernel2D<float>* filter = ele->filter;
    int band = ele->colourBand;
    bool isGray = ele->isGray;
    Image<float> imageOut(image.getSize());
    if (ele->isGray) {
      filter->Convolve(imageOut, imageGray, Extend);
    } else {
      filter->Convolve(imageOut, imageBands[band], Extend);
    }
    for (int x = 0; x < sizex; x++) {
      for (int y = 0; y < sizey; y++) {
        int pos = x * sizey + y;//y * sizex + x;
        //valueResponse[pos][i]=imageOut(x,y);
        vector<double>* a = *(beginIter + pos);
        (*a)[i] = imageOut(x, y);
        (*valueMax)[i] = max((*valueMax)[i], (*a)[i]);
        (*valueMin)[i] = min((*valueMin)[i], (*a)[i]);
      }
    }
  }
  id += imagesize;
}
/**
 * @brief Creatation of filter bank
 */
void Textonizer::initFilterBank() {
  // Gaussians (applied to all colour channels)
  /*
  filterBank.push_back(new FilterBankElement(Kernel2D<float>::CreateGaussian(1 * rescale), 0, false));
  filterBank.push_back(new FilterBankElement(Kernel2D<float>::CreateGaussian(2 * rescale), 0, false));
  */


  for (int band = 0; band < 3; band++) {
    for (double sigma = 1.0; sigma <= 4.0; sigma *= 2.0)
      filterBank.push_back(new FilterBankElement(Kernel2D<float>::CreateGaussian(sigma * rescale), band, false));
  }
  // Laplacians (applied to just greyscale)
  for (double sigma = 1.0; sigma <= 8.0; sigma *= 2.0)
    filterBank.push_back(new FilterBankElement(Kernel2D<float>::CreateLaplacian(sigma * rescale), 0, true));

  // Derivatives of Gaussians (appiled to just greyscale)
  for (double sigma = 2.0; sigma <= 4.0; sigma *= 2.0) {
    filterBank.push_back(new FilterBankElement(Kernel2D<float>::CreateGaussianDerivativeX(sigma * rescale, 3.0 * sigma * rescale), 0, true)); // d/dx
    filterBank.push_back(new FilterBankElement(Kernel2D<float>::CreateGaussianDerivativeY(3.0 * sigma * rescale, sigma * rescale), 0, true)); // d/dy
  }
  /*
    // Print filter bank
    for (int i = 0; i < filterBank.size(); i++) {
      cout << i << ": " << endl;
      Kernel2D<float>* a = filterBank[i]->filter;
      if (dynamic_cast<SeparableKernel2D<float>*>(a) != NULL) {
        cout << "SeparableKernel2D" << endl;
        SeparableKernel2D<float>* k = (SeparableKernel2D<float>*)a;
        k->kernelX->printData();
        k->kernelY->printData();
      }
      if (dynamic_cast<LoGKernel2D<float>*>(a) != NULL) {
        cout << "LoGKernel2D" << endl;
        LoGKernel2D<float>* k = (LoGKernel2D<float>*)a;
        k->kernel2ndDerivative->printData();
        k->kernelGaussian->printData();
      }

    }
  */
}
int getType(Kernel2D<float>* k){
  if(dynamic_cast<SeparableKernel2D<float>* >(k)!=NULL)
    return 0;
  else if(dynamic_cast<NonSeparableKernel2D<float>* >(k)!=NULL)
    return 1;
  else if(dynamic_cast<LoGKernel2D<float>* >(k)!=NULL)
    return 2;
}
/**
 * @brief Save all filter in a file
 */
void Textonizer::SaveFilterBank(vector<FilterBankElement*>& bank, const char* filename) {
  ofstream ofs;
  ofs.open(filename);
  ofs<<bank.size()<<endl;
  for (int i = 0; i < bank.size(); i++) {
    FilterBankElement* filter = bank[i];
    Kernel2D<float>* k = filter->filter;
    int type=getType(k);
    ofs << type <<endl;
    k->save(ofs);
    ofs <<endl;
    ofs << filter->colourBand <<endl;
    ofs << filter->isGray <<endl;
  }
  ofs.close();
}
/**
 * @brief Load all filter from a file
 */
void Textonizer::LoadFilterBank(vector<FilterBankElement*>& bank, const char* filename){
  ifstream ifs;
  ifs.open(filename);
  int size=0;
  ifs>>size;
  int k=0;
  while((k++)<size){
    FilterBankElement* fbe=new FilterBankElement();
    int type=0;
    ifs>>type;
    Kernel2D<float>* kernel;
    switch(type){
      case 0:
        kernel=(Kernel2D<float>*)new SeparableKernel2D<float>;        
        break;
      case 1:
        kernel=(Kernel2D<float>*)new NonSeparableKernel2D<float>;
        break;
      case 2:
        kernel=(Kernel2D<float>*)new LoGKernel2D<float>;
        break;        
    }
    kernel->load(ifs);
    fbe->filter=kernel;
    ifs >> fbe->colourBand;
    ifs >> fbe->isGray;
    bank.push_back(fbe);
    
  }
  ifs.close();
  
}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;  replace-tabs on;   replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;
