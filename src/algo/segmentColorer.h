#ifndef _SEGMENT_COLORER_H_
#define _SEGMENT_COLORER_H_
#include <vector>
#include "../lib/LibTIM/Common/Image.h"
#include "bptAdjacentGraph.h"

using namespace std;
using namespace LibTIM;

class segmentColorer{
private:
  static segmentColorer* me;
  vector<U8C3> colorTab;
  //map<int, int> colorMap;  
public:
  static segmentColorer* instance();
  static void initRandomColor(int segmentCount);
  //static void initRandomColor(const Image<TLabel>& imageSegment, int segmentCount);
  static Image<U8C3>* showSegmentationRandomColorFix(Image<TLabel>& imageSegment, int maxId);
  static Image<U8C3>& showSegmentationPseudoColor(const AdjacentGraph& graph, Image<TLabel>& imageSegment, int segmentCount);
  static void addRandomColor(int maxColorId);
  static void save(segmentColorer* colorer,const char* file);
  static segmentColorer* load(const char* file);
};

#endif