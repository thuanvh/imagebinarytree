#ifndef BPTSEGMENT_H
#define BPTSEGMENT_H

#include <cstdio>
#include <iostream>
#include <vector>
#include "bptAdjacentGraph.h"
#include "bptBinaryTree.h"
#include "bptNeighborRelation.h"

using namespace std;
#include "../lib/LibTIM/Common/Image.h"
#include <learning/classifier.h>

using namespace LibTIM;

//using namespace opencv2;


#define USING_LAB 0
#define USING_YUV 1
#define USING_RGB 2

#define MERGING_ORDER_GAR98 0
#define MERGING_ORDER_MOYEN 1

#define REGION_MODEL_GAR98 0
#define REGION_MODEL_MOYEN 1

#define _NEIGHBORS_     4
#define _8_NEIGHBORS_   8
#define _4_NEIGHBORS_   4
#define _9_NEIGHBORS_   9
#define _8_NEIGHBORS_DX_  {-1,0,1,0,-1,1,-1,1}
#define _8_NEIGHBORS_DY_  {0,-1,0,1,-1,1,1,-1}
#define _9_NEIGHBORS_DX_  {0,-1,0,1,0,-1,1,-1,1}
#define _9_NEIGHBORS_DY_  {0,0,-1,0,1,-1,1,1,-1}
#define _4_NEIGHBORS_DX_  {-1,0,1,0}
#define _4_NEIGHBORS_DY_  {0,-1,0,1}

#define PRUN_EVOL_THRESHOLD 1
#define PRUN_EVOL_AUTO 0
#define PRUN_EVOL_REGION_COUNT 2

#define MAXINT   10e6
//#define DEBUG

/**
 * \defgroup BinaryPartitionTree
 * \ingroup Algorithms
 */
/**
 * Binary Partition Tree Segmentation
 */
class BPTSegment {

public:
  int methodPruning;
  /// constructor
  BPTSegment();
  ~BPTSegment();
  /// source image
  Image<U8C3> imageSrc;
  /// the main image used to processing. They may be in the Lab color space
  Image<U8C3>* mainImage;
  /// the first segmented image by presegmentation  
  Image<TLabel> firstSegmentedImage;
  /// the final segmented image by pruning
  Image<TLabel> finalSegmentedImage;
  /// the graph
  AdjacentGraph graph;
  /// the tree
  BinaryTree tree;
  /// the colored final segmented image
  Image<U8C3>* showedFinalSegmentedImage;
  /// The colored first segmented image
  Image<U8C3>* showedFirstSegmentedImage;
  /// List of conserved node after pruning tree
  vector<node*> nodeConservedList;
  /// The number of region after presegmentation
  int regionFirstCount;
  /// The number of region after segmentation, pruning tree
  int regionFinalCount;
  /// The threshold used for volvement pruning method 
  float volvementThreshold;

  /// Presegmentation 
  void presegmentation(const Image<U8C3>& imageSrc, Image<TLabel>& imageSegment, int& segmentCount);
  /// Construct the tree
  void merging(const Image<U8C3> & imageSrc,const Image<TLabel>& imageSegment, int segmentCount);
  /// Pruning normal
  void pruning(int regionCount, Image<TLabel> const & imagePreSegment, Image<TLabel>& imageSegment);
  /// Pruning automatic with volvement method
  void pruningAuto(Image<TLabel> const & imagePreSegment, Image<TLabel>& imageSegment);
  /// Pruning automatic with max changement, without any threshold
  void pruningAutoMaxCouvert(Image<TLabel> const & imagePreSegment, Image<TLabel>& imageSegment);
  /// Pruning by classification
  void pruningByClassification(Image<TLabel> const & imagePreSegment, Image<TLabel>& imageSegment,string filename,vector<int>& idList);
  /// Validataion the evolvement
  bool validationEvolvement(node* previous, node* currentNode, int levelsTotal, float sumAll);
  /// Recall to pruning, not repeat calling to the tree construction
  void newPruningAuto();
  /// Recall to pruning with threshold
  void newPruningAutoNoThreshold();
  /// Recall to pruning with classification
  void newPruningByClassification(string filename);
  /// Segmentation
  void segment();
  /// replace all the value of a region in segmented image by the other value
  void replace(Image<TLabel>& imageSegment, int oldvalue, int newvalue);
  //void replaceLUT(Image<TLabel>& imageSegment, map<int,int>& lut);
  /// Replace by Look-up Table that improve the velocity 
  void replaceLUT(Image<TLabel>const & preimageSegment, Image<TLabel>& imageSegment, map<int, int>& lut);
  /// Same region fusion is used to fusion 2 region with the same id separed by a border.
  void sameRegionFusion(Image<TLabel>& imageSegment);
  /// clear the memory
  void clear();
  /// new pruning
  void newPruning();
  /// calcualte the level of each node
  void generateLevel();
  /// Tree construction
  void treeConstruction();
  /// pruning all the evolvement
  int pruningTreeByEvolvementMaxDerived(vector<node*>& conservedList);
  int pruningTreeByEvolvementMediumDerived(vector<node*>& conservedList);
  //void reconstruction();
  /// coefficience of color variance
  float coefColorVariance;
  /// coefficience of texture wavelet
  float coefTextureWavelet;//  
  /// coefficience of homogeneity
  float coefHomogeneity;
  /// coefficience of region size
  float coefRegionSize;
  /// coefficience of mean color
  float coefMeanColor;
  /// coefficience of gradient
  float coefMeanGradient;
  //float coefTextureWavelet;
  /// region model strategy
  int regionModelStrategy;
  /// classifier
  classifier* mainClassifier;
private:

  /**
   * watershed segmentation
   */
  //void watershedSegmentation(const Image<U8C3>& src,Image<TLabel>& imageSegment,int& segmentCount);
  void clearBorder(const Image<U8C3>& src, Image<TLabel>& imageSegment);

  /**
   * show the segmentation
   */
  Image<U8C3>& showSegmentationRandomColorFix(const Image<U8C3>& imagesrc, Image<TLabel>& imageSegment, int segmentCount);
  Image<U8C3>& showSegmentationRandomColorNoFix(const Image<U8C3>& imagesrc, Image<TLabel>& imageSegment, int segmentCount);
  Image<U8C3>& showSegmentationPseudoColor(const Image<U8C3>& imagesrc, Image<TLabel>& imageSegment, int segmentCount);
  /**
   * flat region segmentation
   */
  void initRandomColor(const Image<TLabel>& imageSegment, int segmentCount);
  void addRandomColor(int maxColorId);
  void calculateTheHomogeneity(const Image<U8C3>& imageSrc);
  void calculateMeanGradient(const Image<U8C3>& imageSrc);
  void calculateColorVariance(const Image<U8C3>& imageSrc);
  void pruningTreeByEvolvement(vector<node*>& conservedList);
  float getTotalPropOfNode(node* currentNode);
  void viterbiCorrection();
};

int compare_relation(const neighborRelation *a, const neighborRelation *b);
int compare_relation_invert(const neighborRelation *a, const neighborRelation *b);
void flatRegionSegmentation(Image<U8C3> const & src, Image<TLabel>& imageSegment, int& regionNumber);
void watershedSegmentation(const Image< U8C3 >& src, Image< TLabel >& imageSegment, int& segmentCount);
#endif // BPTSEGMENT_H
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
