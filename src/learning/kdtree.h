#ifndef KDTREE_H
#define KDTREE_H

#include <vector>
#include <cmath>
#include <queue>
using namespace std;

class kdTreeNode {
public:
  bool terminal;

  // Terminal fields
  vector<int>* indices;

  // Non-terminal fields
  int dim;
  double val;
  kdTreeNode* left, *right;

  // Terminal constructor
  kdTreeNode(vector<int>* indices) {
    terminal = true;
    this->indices = indices;
  }

  void MakeNonTerminal(int dim, double val, kdTreeNode* leftBranch, kdTreeNode* rightBranch) {
    this->terminal = false;
    this->indices = NULL;
    this->dim = dim;
    this->val = val;
    this->left = leftBranch;
    this->right = rightBranch;
  }
};

class kdTree {

private:
  vector<vector<double>*> data;
  int D;
  kdTreeNode* rootNode;
  void NearestNeighbour(vector<double>& point, kdTreeNode* node, double currentBestDistance, int & bestIndex, double & bestDistance);
  double EuclideanDistance(vector<double>& p1, vector<double>& p2, double minDistance);
  // Build the kd-tree
public:
  kdTree(vector<vector<double>*> database, int pointsPerBin);
  // Search the tree for a data-point
  int NearestNeighbour(vector<double>& point);


};

#endif
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;  replace-tabs on;
