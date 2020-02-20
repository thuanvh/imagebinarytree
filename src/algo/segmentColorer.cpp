#include "segmentColorer.h"
#include "bptAdjacentGraph.h"

segmentColorer* segmentColorer::me = segmentColorer::instance();

segmentColorer* segmentColorer::instance() {
  me = new segmentColorer();
  return me;
}

/*void segmentColorer::initRandomColor(const Image<TLabel>& imageSegment, int segmentCount) {
  colorTab.clear();
  /// normalise image
  colorMap.clear();
  int colormax = 0;
  int ysize = imageSegment.getSizeY();
  int xsize = imageSegment.getSizeX();

  for (int i = 0; i < xsize; i++) {
    for (int j = 0; j < ysize; j++) {
      int idx = imageSegment(i, j);
      //int colorid=-1;

      if (idx != -1 && idx > 0) {
        if (idx == 8) {
          int a = 0;
        }

        if (colorMap.find(idx) == colorMap.end()) {
          colorMap.insert(pair<int, int> (idx, colormax++));
        } else {
          //colorid=colorMap[idx];
        }
      }
    }
  }

  for (int i = 0; i < segmentCount; i++) {
    Table<U8, 3> a;
    a[0] = rand() % 256;
    a[1] = rand() % 256;
    a[2] = rand() % 256;
    colorTab.push_back(a);
  }
}
*/
void segmentColorer::initRandomColor(int segmentCount) {
  for (int i = 0; i < segmentCount; i++) {
    Table<U8, 3> a;
    a[0] = rand() % 256;
    a[1] = rand() % 256;
    a[2] = rand() % 256;
    me->colorTab.push_back(a);
  }
}

void segmentColorer::addRandomColor(int maxColorId) {
  int add = maxColorId - me->colorTab.size();
  for (int i = 0; i < add; i++) {
    U8C3* a = new U8C3();
    (*a)[0] = rand() % 256;
    (*a)[1] = rand() % 256;
    (*a)[2] = rand() % 256;
    me->colorTab.push_back(*a);
  }
}

Image<U8C3>& segmentColorer::showSegmentationPseudoColor(const AdjacentGraph& graph, Image<TLabel>& imageSegment, int segmentCount) {


  Image<U8C3>* wshed = new Image<U8C3> (imageSegment.getSize());

  /// paint the watershed image

  for (int i = 0; i < imageSegment.getSizeX(); i++) {
    for (int j = 0; j < imageSegment.getSizeY(); j++) {
      int idx = imageSegment(i, j);

      if (idx == -1)
        (*wshed)(i, j)[0] = (*wshed)(i, j)[1] = (*wshed)(i, j)[2] = 255;
      else
        if (idx <= 0)   //|| idx > segmentCount )
          (*wshed)(i, j)[0] = (*wshed)(i, j)[1] = (*wshed)(i, j)[2] = 0;
        else {
          (*wshed)(i, j)[0] = 255 - graph.showedNode[idx]->colors[0];
          (*wshed)(i, j)[1] = graph.showedNode[idx]->colors[1];
          (*wshed)(i, j)[2] = 255 - graph.showedNode[idx]->colors[2];
        }
    }
  }

  /*Mat imgGray3(imagesrc.size(),CV_8UC3);
   *  Mat imgGray1(imagesrc.size(),CV_8UC1);
   *
   *  cvtColor(imagesrc,imgGray1,CV_RGB2GRAY);
   *  cvtColor(imgGray1,imgGray3,CV_GRAY2RGB);
   *
   *  (*wshed) = (*wshed)*0.9 + imgGray3*0.1;
   */
  return *wshed;
}


/*Image<RGB>& segmentColorer::showSegmentationRandomColorNoFix(const Image<TLabel>& imageSegment, int segmentCount) {
  Image<RGB>* wshed = new Image<RGB> (imageSegment.getSize());

  vector<int> idlist;
  initRandomColor(imageSegment, segmentCount);

  /// paint the watershed image
  int ysize = imageSegment.getSizeY();
  int xsize = imageSegment.getSizeX();

  for (int i = 0; i < xsize; i++) {
    for (int j = 0; j < ysize; j++) {
      int idx = imageSegment(i, j);

      if (idx == -1)
        (*wshed)(i, j)[0] = (*wshed)(i, j)[1] = (*wshed)(i, j)[2] = 255;
      else
        if (idx <= 0)   //|| idx > segmentCount )
          (*wshed)(i, j)[0] = (*wshed)(i, j)[1] = (*wshed)(i, j)[2] = 0;
        else {
          int colorid = colorMap[idx];
          (*wshed)(i, j) = colorTab[colorid];
        }
    }
  }

  //Mat imgGray3(imagesrc.size(),CV_8UC3);
  //Mat imgGray1(imagesrc.size(),CV_8UC1);
  //cout<<"painting"<<endl;
  //cvtColor(imagesrc,imgGray1,CV_RGB2GRAY);
  //cvtColor(imgGray1,imgGray3,CV_GRAY2RGB);
  //(*wshed) = (*wshed)*0.9 + imgGray3*0.1;
  return *wshed;
}
*/

Image<U8C3>* segmentColorer::showSegmentationRandomColorFix(Image<TLabel>& imageSegment, int maxId) {
  addRandomColor(maxId);
  Image<U8C3>* wshed = new Image<U8C3> (imageSegment.getSize());

  vector<int> idlist;

  /// paint the watershed image
  int ysize = imageSegment.getSizeY();
  int xsize = imageSegment.getSizeX();

  for (int i = 0; i < xsize; i++) {
    for (int j = 0; j < ysize; j++) {
      int idx = imageSegment(i, j);
      
      if (idx == -1)
        (*wshed)(i, j) = 255;
      else
        if (idx <= 0 || idx > maxId)
          (*wshed)(i, j) = 0;
        else {
          (*wshed)(i, j) = me->colorTab[idx];
        }
    }
  }

  /*
   * Image<RGB> imgGray3(imagesrc.size());
   * Image<RGB> imgGray1(imagesrc.size());
   * //cout<<"painting"<<endl;
   * cvtColor(imagesrc,imgGray1,CV_RGB2GRAY);
   * cvtColor(imgGray1,imgGray3,CV_GRAY2RGB);
   */
#ifdef SHOW_IMAGE_OPENCV
  imshow("im gray", imgGray3);
  imshow("im color", imagesrc);
  imshow("im seg", *wshed);
#endif
  //(*wshed) = (*wshed)*0.9 + imgGray3*0.1;
#ifdef SHOW_IMAGE_OPENCV
  imshow("watershed transform", *wshed);
  waitKey();
#endif

  return wshed;
}

segmentColorer* segmentColorer::load(const char* file)
{
  segmentColorer* a=instance();
  ifstream ifs;
  ifs.open(file);
  a->colorTab.clear();
  while(ifs.good()){
    U8C3 color;
    ifs>>color[0]>>color[1]>>color[2];
    a->colorTab.push_back(color);
  }
  ifs.close();
}
void segmentColorer::save(segmentColorer* colorer, const char* file)
{
  ofstream ofs;
  ofs.open(file);
  for(int i=0; i<colorer->colorTab.size(); i++){
    for(int j=0; j<3; j++)
      ofs<<(int)colorer->colorTab[i][j]<<" ";
    ofs<<endl;
  }
  ofs.close();
}

// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;
