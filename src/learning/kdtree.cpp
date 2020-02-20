// This is a convert of TextonBoost C#

#include "kdtree.h"

/**
 * @brief Constructor the kdtree from a database
 */
kdTree::kdTree(vector<vector<double>*> database, int pointsPerBin) {
  this->data = database;
  D = database[0]->size();

  // Initialise root node
  vector<int>* indices = new vector<int>(database.size());
  for (int i = 0; i < indices->size(); i++)
    (*indices)[i] = i;
  rootNode = new kdTreeNode(indices);

  // Queue of nodes to process
  queue<kdTreeNode*> toProcess;
  toProcess.push(rootNode);

  // While queue is not empty
  while (!toProcess.empty()) {
    kdTreeNode* node = toProcess.front();
    toProcess.pop();
    int N = node->indices->size();
    if (N > pointsPerBin) {
      // Compute variances of each dimension for this bin
      double vars[D];
      double x[D], x2[D];
      for (int i = 0;i < N;i++)
        for (int d = 0; d < D; d++) {
          x[d] += (*data[(*node->indices)[i]])[d];
          x2[d] += (*data[(*node->indices)[i]])[d] * (*data[(*node->indices)[i]])[d];
        }
      for (int d = 0; d < D; d++) {
        vars[d] = x2[d] / N + x[d] * x[d] / (N * N);
      }


      // Find dimension of highest variance
      int hDim = 0;
      for (int d = 1; d < D; d++) {
        if (vars[d] > vars[hDim])
          hDim = d;
      }


      // Check for all points equal - don't split any further
      if (vars[hDim] == 0)
        continue;

      // Use mean of hDim to split at (should really be median, but saves sorting)
      double split = x[hDim] / N;
      vector<int>* leftIndices = new vector<int>();
      vector<int>* rightIndicies = new vector<int>();
      for (int i = 0; i < N; i++)
        if ((*data[(*node->indices)[i]])[hDim] < split)
          leftIndices->push_back((*node->indices)[i]);
        else
          rightIndicies->push_back((*node->indices)[i]);

      // Make left and right tree nodes
      kdTreeNode* left = new kdTreeNode(leftIndices);
      kdTreeNode* right = new kdTreeNode(rightIndicies);

      // Make the current node a non-terminal and recurse
      node->MakeNonTerminal(hDim, split, left, right);
      toProcess.push(left);
      toProcess.push(right);
    }
  }
}

/**
 * @brief Search the tree for a data-point
 */
int kdTree::NearestNeighbour(vector<double>& point) {
  int bestIndex;
  double bestDistance;
  NearestNeighbour(point, rootNode, HUGE_VAL, bestIndex, bestDistance);
  return bestIndex;
}

/**
 * @brief Search the tree for a data-point
 */
void kdTree::NearestNeighbour(vector<double>& point, kdTreeNode* node, double currentBestDistance, int & bestIndex, double & bestDistance) {
  bestDistance = currentBestDistance;
  if (node->terminal) {
    // Linear search across data points in terminal node
    bestIndex = (*node->indices)[0];
    bestDistance = EuclideanDistance(*data[bestIndex], point, bestDistance);
    for (int i = 1; i < node->indices->size(); i++) {
      int index = (*node->indices)[i];
      double dist = EuclideanDistance(*data[index], point, bestDistance);
      if (dist < bestDistance) {
        bestIndex = index;
        bestDistance = dist;
      }
    }
  } else {
    // Take branch according to split
    if (point[node->dim] < node->val) {
      // Branch left
      NearestNeighbour(point, node->left, bestDistance,  bestIndex,  bestDistance);

      // Check if need to evaluate other branch
      if (node->val - point[node->dim] <= bestDistance) {
        int checkIndex;
        double checkDistance;
        NearestNeighbour(point, node->right, bestDistance,  checkIndex,  checkDistance);
        if (checkDistance < bestDistance) {
          bestIndex = checkIndex;
          bestDistance = checkDistance;
        }
      }
    } else {
      // Branch right
      NearestNeighbour(point, node->right, bestDistance,  bestIndex,  bestDistance);

      // Check if need to evaluate other branch
      if (point[node->dim] - node->val <= bestDistance) {
        int checkIndex;
        double checkDistance;
        NearestNeighbour(point, node->left, bestDistance,  checkIndex,  checkDistance);
        if (checkDistance < bestDistance) {
          bestIndex = checkIndex;
          bestDistance = checkDistance;
        }
      }
    }
  }
}

/**
 * @brief euclide distance
 */
double kdTree::EuclideanDistance(vector<double>& p1, vector<double>& p2, double minDistance) {
  minDistance = minDistance * minDistance; // Square minDistance so can be compared without square-rooting

  double euclideanDistance = 0.0;
  for (int d = 0; d < D; d++) {
    double diff = p1[d] - p2[d];
    euclideanDistance += diff * diff;
    if (euclideanDistance > minDistance)
      return HUGE_VAL; // Stop early if possible
  }
  return sqrt(euclideanDistance);
}

// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
