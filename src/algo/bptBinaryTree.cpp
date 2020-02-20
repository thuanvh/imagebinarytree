#include "bptBinaryTree.h"
#include <iostream>

using namespace std;
/**
 * @brief Merge two nodes
 */
void BinaryTree::merging(node*& a,  node*& b,  node*& newnode) {
  newnode->children[0] = a;
  newnode->children[1] = b;
  a->parent = newnode;
  b->parent = newnode;
}
/**
 * @brief Destructor
 */
BinaryTree::~BinaryTree() {

}

/**
 * @brief Print the tree
 */
void BinaryTree::printTree() {
  printNode(root);
  cout << endl;
}
/**
 * @brief Print the node
 */
void BinaryTree::printNode(node* a) {
  cout << a->id << " ";

  if (a->children[0] != NULL)
    printNode(a->children[0]);

  if (a->children[1] != NULL)
    printNode(a->children[1]);

}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
