#ifndef _NEIGHBOUR_RELATION_H_
#define _NEIGHBOUR_RELATION_H_
#include "bptNode.h"
/**
 * \defgroup BinaryPartitionTree
 * \ingroup Algorithms
 */
/**
 * Relation between node in the weighted adjacent graph
 */
struct neighborRelation {
  neighborRelation(node* newOrigineNode, node* newneighbor, float newdistance = 0) {
    origineNode = newOrigineNode;
    neighbor = newneighbor;
    distance = newdistance;
    isValided = true;
  };

  neighborRelation(node* newneighbor, float newdistance) {
    neighbor = newneighbor;
    distance = newdistance;
    isValided = true;
  };
  /// Calculate the distance between 2 node of this relation
  void calculateDistance();
  /// The origine node
  node* origineNode;
  /// neighbor
  node* neighbor;
  /// distance between 2 nodes
  float distance;
  /// valide
  bool isValided;
};

#endif
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
