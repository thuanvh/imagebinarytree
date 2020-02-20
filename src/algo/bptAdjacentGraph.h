#ifndef _BPT_ADJACENT_GRAPH_H
#define _BPT_ADJACENT_GRAPH_H
#include "bptNode.h"
#include "bptNeighborRelation.h"

#include <cstdio>
#include <iostream>

using namespace std;
#include "../lib/LibTIM/Common/Image.h"

//File d'Attente Hiérarchique
#define FAH

using namespace LibTIM;
/** \defgroup BinaryPartitionTree
 \ingroup Algorithms
 **/
/**
 * @brief The comparison of relation
 */
class neighborRelationComparison {
  bool reverse;

public:
  neighborRelationComparison(const bool& revparam = false) {
    reverse = revparam;
  }

  bool operator()(neighborRelation* & lhs,  neighborRelation* &rhs) const {
    //if (reverse)
    return (lhs->distance > rhs->distance);
    //else
    //return (lhs->distance<rhs->distance);
  }
};
/** \defgroup BinaryPartitionTree
 \ingroup Algorithms
 **/
/**
 * @brief Adjacent graph presents the relation between the region of segmented image
 */
class AdjacentGraph {

public:
  /// Constructor
  AdjacentGraph();
  /// Destructor
  ~AdjacentGraph();
  /// All of node list of graph
  vector<node*> showedNode;
  /// The adjacent graph which maps the id of a node to a list of edge
  map<int, vector<neighborRelation* >* > graph;
  /// The list of relation
  vector<neighborRelation*> relationList;
  /// Strategy of region model. It maybe one of  REGION_MODEL_MOYEN (value of new region is average of children) or REGION_MODEL_GAR98 (value of new region is maximal of children)
  int regionModelStrategy;// = REGION_MODEL_MOYEN;//REGION_MODEL_GAR98;
  //priority_queue<neighborRelation*> relationList(neighborRelationComparison());
  //priority_queue<neighborRelation*>* relationQueue;
  
#ifndef FAH
  /// Priority queue 
  typedef priority_queue<neighborRelation*, vector<neighborRelation*>, neighborRelationComparison > relqueue;
  relqueue relationQueue;
#else
  /// (fil de l'attend hierarchie ). The map between the value of relation and a list of relation with this same value
  map<int,queue<neighborRelation*> > relmap;
#endif

  /// memory list of point of images sorted by plat zone
  vector<vector<int*>* > pointsListOfLeaves;
  /// a map between each node of graph to list of leaves. That's used to rapidly find the list of point of each node on tree
  vector<vector<int>* > graphMapToLeaves;
  int leavesCount;
  /**
   * init adjacent graph from an segmented image
   */
  void initGraph(const Image<U8C3> & imageSrc, const Image<TLabel>& imageSegment, int segmentCount);
  /**
   * merging 2 nodes to construct the new adjacent graph
   */
  void merging(node*const& a, node*const& b, node*& newnode);
  /**
   * get list of adjacent nodes
   */
  node* getAdjacentNodes(node& a);
  /**
   * Find in the adjacent graph the merging node
   */
  void findMergingNode(node*& a, node*& b);
  /**
   * Print adjacent graphe
   */
  void printGraph();
  /**
   * Print relation list
   */
  void printRelationList();
  /**
   * Caculate distances of the relationlist
   */
  void caculateRegionDistances();
  /// Move the relation ship 
  void moveRelationShip(neighborRelation* relation);
  /// Merge some relation list
  void addToSortedRelationList(vector<neighborRelation*>* relationList, neighborRelation* relation);
  void addToSortedRelationList(vector<neighborRelation*>* parentRelationList, vector<neighborRelation*>* childRelationList);
};
#endif
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
