#ifndef _BPT_NODE_H
#define _BPT_NODE_H
#include <vector>
using namespace std;
/**
 * \defgroup BinaryPartitionTree
 * \ingroup Algorithms
 */
class node {

public:
  node() {
    children[0] = children[1] = 0;
    parent = 0;
    numberOfPoint = 0;
    colors[0] = colors[1] = colors[2] = 0;
    homogeneity = 0;
    isEvolvementConserved = false;
    LH = HL = 0;
    colorVariance = meanGradient = 0;
    level = 0;
    pos[0] = pos[1] = 0;
    maxY = maxX = -1;
    minX = minY = 10e6;
    classId=-1;
  };

  node(int newid): isErased(false) {
    children[0] = children[1] = 0;
    parent = 0;
    id = newid;
    numberOfPoint = 0;
    colors[0] = colors[1] = colors[2] = 0;

    //isErased=false;
    removeCost = preserveCost = 0;
    homogeneity = 0;
    isEvolvementConserved = false;
    LH = HL = 0;
    colorVariance = meanGradient = 0;
    level = 0;
    pos[0] = pos[1] = 0;
    maxY = maxX = -1;
    minX = minY = 10e6;
    classId=-1;
  };
  ~node(){
    //delete []GFD;
  }
  /// id of node
  int id;
  /// level
  int level;
  /// average of color of this region
  double colors[3];
  /// texture value
  int texture;
  /// number of point of this node
  long numberOfPoint;
  /// calculate the color average after intitialisation
  void updateAfterInitialisation();
  /// compair with the other
  float compair(node const& other);
  /// 2 children
  node* children[2];
  /// a reference to parent
  node* parent;
  /// is erased
  bool isErased;
  /// remove cost of viterbi algo
  int removeCost;
  /// preserve cost of viterbi algo
  int preserveCost;
  /// children removed mark
  bool childrenRemoved[2];
  /// homogeneity
  float homogeneity;
  /// evolvement
  bool isEvolvementConserved;
  /// wavelet
  float LH, HL;
  /// color covariance
  float colorVariance;
  /// mean gradient
  float meanGradient;
  /// position of this node
  int pos[2];
  /// graphic node
  void* graphicNode;
  /// bounding rectagle
  int minX;
  int maxX;
  int minY;
  int maxY;
  /// descriptor
  vector<double> descriptor;
  /// class id of this node
  int classId;
  /// class distance
  double classDistance;
};
/// Viterbi node is used for determine whether conserver or delete some node
class viterbiNode {
  /// reference to a node
  node* thenode;
  /// erase cost after viterbi loop
  int erasecost;
  /// preserve cost after viterbi pass
  int preservecost;
  /// children
  viterbiNode* children[2];
  /// parent
  viterbiNode* parent;
  /// constructor
  viterbiNode() {
    //thenode=NULL;
    erasecost = preservecost = 0;
  }
};
#endif
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;
