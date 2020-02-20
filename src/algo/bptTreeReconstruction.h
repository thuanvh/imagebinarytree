#ifndef _BPT_TREE_RECONSTRUCTION_H_
#define _BPT_TREE_RECONSTRUCTION_H_
#include <vector>
#include "bptAdjacentGraph.h"
#include "bptBinaryTree.h"
using namespace std;

class bptTreeReconstruction{
private:
  AdjacentGraph* graph;
  vector<node*>* markPruntNodeList;
  BinaryTree* tree;
public:
  bptTreeReconstruction(AdjacentGraph* graph, BinaryTree* tree, vector<node*>* markedNodeList){
    this->graph=graph;
    this->markPruntNodeList=markedNodeList;
    this->tree=tree;
  }
  void minTreeReconstruction(vector<node*>& nodeConservedList);
  void maxTreeReconstruction(vector<node*>& nodeConservedList);
};
#endif