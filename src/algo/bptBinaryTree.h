#ifndef _BPT_BINARY_TREE_H
#define _BPT_BINARY_TREE_H
#include "bptNode.h"
/**
 * \defgroup BinaryPartitionTree
 * \ingroup Algorithms
 */
/**
 * Binary tree
 */
class BinaryTree {

public:
  /// root
  node* root;
  /// merge 2 nodes into a new node
  void merging(node*& a,  node*& b,  node*& newnode);
  ~BinaryTree();
  void printTree();

private:
  void printNode(node* a);
};
#endif
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
