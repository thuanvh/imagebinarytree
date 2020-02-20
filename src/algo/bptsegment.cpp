
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <queue>
#include <math.h>
#include "bptsegment.h"
#include "colorMorphology.h"
#include "segmentColorer.h"
#ifdef USEQT
#include <ui/imageviewer.h>
#endif

#include <ui/treeInspector.h>
#include "convertImage.h"
#include <learning/GFT.h>
#include <learning/LibSVMAdapter.h>
//#include <ui/treeInspector.h>
#include <learning/featureGenerator.h>
#include <learning/knnClassifier.h>
#include "bptTreeReconstruction.h"
#include <config/ConfigurationManager.h>
#include <learning/SIFTClassifier.h>

using namespace std;

//#define SHOW_IMAGE_OPENCV
/**
 * @brief Segmentation main function of binary partition tree
 */
void BPTSegment::segment() {
  time_t start, end;
  time(&start);

  treeConstruction();
#ifdef DEBUG
  //cout<<"Before prunning";
  tree.printTree();
#endif
  /// Prun the tree by some of method
  cout << "pruning" << endl;
  switch (methodPruning) {
    case PRUN_EVOL_THRESHOLD:
      pruningAuto(firstSegmentedImage, finalSegmentedImage);
      break;
    case PRUN_EVOL_AUTO :
      pruningAutoMaxCouvert(firstSegmentedImage, finalSegmentedImage);
      break;
    case PRUN_EVOL_REGION_COUNT :
      pruning(regionFinalCount, firstSegmentedImage, finalSegmentedImage);
      break;
  }
  //pruning(regionFinalCount,firstSegmentedImage,finalSegmentedImage);
  //pruningAuto(firstSegmentedImage, finalSegmentedImage);
  //pruningAutoMaxCouvert(firstSegmentedImage, finalSegmentedImage);
  //cout<<"After prunning";
#ifdef DEBUG
  tree.printTree();
#endif
  /// Show the result after pruning. That's the segmentation.
  segmentColorer::addRandomColor(tree.root->id);
  showedFinalSegmentedImage = segmentColorer::showSegmentationRandomColorFix(finalSegmentedImage, tree.root->id);
  time(&end);
  double diff = difftime(end, start);
  printf("all segment execution time = %2.1f ms\n", diff);

}
/**
 * @brief Construction of tree
 */
void BPTSegment::treeConstruction() {
  firstSegmentedImage.fill(0);
  Image<U8C3>* imageLab = new Image<U8C3>(imageSrc.getSize());
  cvtRGB2Lab(imageSrc, *imageLab);
  /// Presegmentation
  mainImage = imageLab;
  cout << "presegment" << endl;
  presegmentation(*mainImage, firstSegmentedImage, regionFirstCount);
  /// Merging the region to construct the tree
  cout << "merging" << endl;
  merging(*mainImage, firstSegmentedImage, regionFirstCount);

  finalSegmentedImage = firstSegmentedImage;
}
void BPTSegment::clear() {

}

/**
 * @brief Recall the tree pruning function.
 */
void BPTSegment::newPruning() {
  finalSegmentedImage = firstSegmentedImage;
  pruning(regionFinalCount, firstSegmentedImage, finalSegmentedImage);
  showedFinalSegmentedImage = segmentColorer::showSegmentationRandomColorFix(finalSegmentedImage, tree.root->id);
}

/**
 * @brief Recall the tree pruning function with the pruning auto
 */
void BPTSegment::newPruningAuto() {
  finalSegmentedImage = firstSegmentedImage;
  pruningAuto(firstSegmentedImage, finalSegmentedImage);
  showedFinalSegmentedImage = segmentColorer::showSegmentationRandomColorFix(finalSegmentedImage, tree.root->id);
}
/**
 * @brief Recall the tree pruning function with No threshold
 */
void BPTSegment::newPruningAutoNoThreshold() {
  finalSegmentedImage = firstSegmentedImage;
  pruningAutoMaxCouvert(firstSegmentedImage, finalSegmentedImage);
  showedFinalSegmentedImage = segmentColorer::showSegmentationRandomColorFix(finalSegmentedImage, tree.root->id);
}
/**
 * @brief Recall the tree pruning function by classification
 */
void BPTSegment::newPruningByClassification(string filename) {
  finalSegmentedImage = firstSegmentedImage;
  vector<int> idList;
  pruningByClassification(firstSegmentedImage, finalSegmentedImage, filename, idList);
  showedFinalSegmentedImage = segmentColorer::showSegmentationRandomColorFix(finalSegmentedImage, tree.root->id);
}

/**
 * @brief Merge the flat zone to construct the tree
 */
void BPTSegment::merging(const Image<U8C3> & imageSrc, const Image<TLabel>& imageSegment, int segmentCount) {
  cout << "init graph" << endl;
  time_t start, end, start2, end2;
  time(&start);
  graph.initGraph(imageSrc, imageSegment, segmentCount);
  time(&end);
  //difftime(start,end);
  cout << difftime(end, start) << "ms" << endl;
  graph.caculateRegionDistances();
#ifdef DEBUG
  graph.printGraph();
  graph.printRelationList();
#endif
  //cout<<"a"<<endl;
  time(&start2);
  int i = 0;

  while (true) {
    //cout<<graph.graph.size()<<" ";
    //cout<<i++<<", ";
    /// if there is nothing in the adjacent graph, no region to merge
    if (graph.graph.size() == 1) {
      tree.root = graph.showedNode[(*graph.graph.begin()).first];
      break;
    }

    /// find regions to merge
    node* a = NULL;
    node* b = NULL;

    graph.findMergingNode(a, b);

    /// merge two found regions
    node* newnode;
    graph.merging(a, b, newnode);

#ifdef DEBUG
    graph.printGraph();
    graph.printRelationList();

#endif
    /// update the tree
    tree.merging(a, b, newnode);
  }

  time(&end2);
  cout << "all repeatation:" << difftime(end2, start2) << "ms" << endl;
  generateLevel();
}

/**
 * @brief Presegmentation is used to initialise the list of elementary regions. They are maybe the flat zone or the region of over-segmentation by watershed
 */
void BPTSegment::presegmentation(const Image<U8C3>& imageSrc, Image<TLabel>& imageSegment, int& segmentCount) {

  //cout<<imageSegment.at<int>(0,0)<<endl;
  watershedSegmentation(imageSrc, imageSegment, segmentCount);
  //clearBorder(imageSrc,imageSegment);

  //flatRegionSegmentation(imageSrc, imageSegment, segmentCount);
  //cout<<"Segmented regions: "<<segmentCount<<endl;

  //cout<<imageSegment.at<int>(0,0)<<endl;
  //showedFirstSegmentedImage = showSegmentationRandomColorNoFix(imageSrc, imageSegment, segmentCount);
  showedFirstSegmentedImage = segmentColorer::showSegmentationRandomColorFix(imageSegment, segmentCount);
  //showedFirstSegmentedImage.save("a.ppm");
  //cout<<imageSegment.at<int>(0,0)<<endl;
  cout << "regions:" << segmentCount << endl;
  //showImage(showedFirstSegmentedImage,"watershed");
}
/**
 * @brief calculer le moyen gradient
 */
void BPTSegment::calculateMeanGradient(const Image<U8C3>& imageSrc) {
  /// get gradient image
  Image<U8> grad(imageSrc.getSize());
  colorMorphologicalGradient(imageSrc, grad);

  /// calculate the moyen of gradient
  vector<node*> list = graph.showedNode;
  int size = list.size();
  int i = 0;
  int leavesCount = graph.leavesCount;

  for (i = 0; i < size; i++) {
    node* node = list[i];

    if (node == NULL)
      continue;

    /// calculate the color variance
    float gradmean = 0;

    /// map
    vector<int> leavesList;

    if (node->id <= leavesCount)
      leavesList.push_back(node->id);
    else
      leavesList = (*graph.graphMapToLeaves[node->id-leavesCount-1]);

    /// leaves
    for (int mapid = 0; mapid < leavesList.size(); mapid++) {
      vector<int*> pointsList = *graph.pointsListOfLeaves[leavesList[mapid]];
      //if(leaveslist.size()==0) continue;

      for (int leavemapid = 0; leavemapid < pointsList.size(); leavemapid++) {
        int x = pointsList[leavemapid][0];
        int y = pointsList[leavemapid][1];
        U8 rgb = grad(x, y);
        gradmean += rgb;
      }
    }//end for

    node->meanGradient = gradmean / node->numberOfPoint;
  }
}

/**
 * @brief calculate the color variance
 */
void BPTSegment::calculateColorVariance(const Image<U8C3>& imageSrc) {
  vector<node*> list = graph.showedNode;
  int size = list.size();
  int i = 0;
  int leavesCount = graph.leavesCount;

  for (i = 0; i < size; i++) {
    node* node = list[i];

    if (node == NULL)
      continue;

    /// calculate the color variance
    float var = 0;

    /// map
    vector<int> leavesList;

    if (node->id <= leavesCount)
      leavesList.push_back(node->id);
    else
      leavesList = (*graph.graphMapToLeaves[node->id-leavesCount-1]);

    /// leaves
    for (int mapid = 0; mapid < leavesList.size(); mapid++) {
      vector<int*> pointsList = *graph.pointsListOfLeaves[leavesList[mapid]];
      //if(leaveslist.size()==0) continue;

      for (int leavemapid = 0; leavemapid < pointsList.size(); leavemapid++) {
        int x = pointsList[leavemapid][0];
        int y = pointsList[leavemapid][1];
        U8C3 rgb = imageSrc(x, y);

        if (node->parent != NULL) {
          var += pow(rgb[0] - node->colors[0], 2) +
              pow(rgb[1] - node->colors[1], 2) +
              pow(rgb[2] - node->colors[2], 2);

        }
      }
    }//end for

    node->colorVariance = var / node->numberOfPoint;
  }
}

/**
 * @brief calculate the homogeneity of the vector
 */
void BPTSegment::calculateTheHomogeneity(const Image<U8C3>& imageSrc) {
  vector<node*> list = graph.showedNode;
  int size = list.size();
  int i = 0;
  int leavesCount = graph.leavesCount;

  for (i = 0; i < size; i++) {
    node* node = list[i];

    if (node == NULL)
      continue;

    /// calculate the homogeneity
    if (node->homogeneity != 0)
      continue;

    float h = 0;

    /// map
    vector<int> leavesList;

    if (node->id <= leavesCount)
      leavesList.push_back(node->id);
    else
      leavesList = (*graph.graphMapToLeaves[node->id-leavesCount-1]);

    /// leaves
    for (int mapid = 0; mapid < leavesList.size(); mapid++) {

      vector<int*> pointsList = *graph.pointsListOfLeaves[leavesList[mapid]];
      //if(leaveslist.size()==0) continue;

      for (int leavemapid = 0; leavemapid < pointsList.size(); leavemapid++) {

        int x = pointsList[leavemapid][0];
        int y = pointsList[leavemapid][1];
        U8C3 rgb = imageSrc(x, y);

        if (node->parent != NULL) {
          h += sqrt(pow(rgb[0] - node->parent->children[0]->colors[0], 2) +
              pow(rgb[1] - node->parent->children[0]->colors[1], 2) +
              pow(rgb[2] - node->parent->children[0]->colors[2], 2));

          h += sqrt(pow(rgb[0] - node->parent->children[1]->colors[0], 2) +
              pow(rgb[1] - node->parent->children[1]->colors[1], 2) +
              pow(rgb[2] - node->parent->children[1]->colors[2], 2));
        }
      }
    }//end for

    node->homogeneity = h;
  }
}

/**
 * @brief erase all children of current node
 */
void eraseAllChild(node* currentNode) {
  queue<node*> childlist;
  childlist.push(currentNode);

  while (!childlist.empty()) {
    node* current = childlist.front();

    if (current->children[0] != NULL) {
      current->children[0]->isErased = true;
      current->children[1]->isErased = true;
      childlist.push(current->children[0]);
      childlist.push(current->children[1]);
    }

    childlist.pop();
  }
}

/**
 * @brief Validate the evolvement of the node in a part
 *
 */
bool BPTSegment::validationEvolvement(node* previous, node* currentNode, int levelsTotal, float sumAll) {
  //  node->isPreserve=false;
  /*
    if(node->numberOfPoint<10)
  node->isErased=true;
  */


  if (currentNode->children[0] != NULL && currentNode->parent != NULL) {
    float ecurrent = 0, eparent = 0;
    ecurrent = getTotalPropOfNode(currentNode);
    eparent = getTotalPropOfNode(currentNode->parent);

    float b = ecurrent * levelsTotal / sumAll;
    float a = eparent * levelsTotal / sumAll;

    float s = abs((a - b) / (1 + a * b));

#ifdef DEBUG
    cout << s << endl;
#endif
    //int costh=abs(2*currentNode->homogeneity - previous->homogeneity - currentNode->parent->homogeneity);
    //cout<<s<<" "
    //if(s>tan(volvementThreshold)){

    if (s > volvementThreshold) {
      //cout << s << endl;

      //currentNode->isErased=true;
      // so, erase all the child
      //eraseAllChild(currentNode);
      return true;
    }

    //cout<<"end..."<<endl;
  }

  return false;
}

/**
 * @brief calculate the value of node
 */
float BPTSegment::getTotalPropOfNode(node* currentNode) {
  return
    coefColorVariance*currentNode->colorVariance +
    coefHomogeneity * currentNode->homogeneity +
    coefRegionSize * currentNode->numberOfPoint +
    coefMeanColor * ((currentNode->colors[0] + currentNode->colors[1] + currentNode->colors[2]) / 3.f) +
    coefMeanGradient * currentNode->meanGradient +
    coefTextureWavelet * (currentNode->HL + currentNode->LH);
}
/**
 * @brief pruning the tree by the evolvement method
 */
int BPTSegment::pruningTreeByEvolvementMaxDerived(vector<node*>& conservedList) {
  int size = graph.showedNode.size();

  for (int i = 0; i < size; i++) {
    node* currentnode = graph.showedNode[i];
    if (currentnode == NULL)
      continue;
    currentnode->isErased = false;
    currentnode->isEvolvementConserved = false;
  }

  tree.root->isEvolvementConserved = true;
  tree.root->isErased = false;
  int minlevel = graph.leavesCount * 2;
  int maxlevel = -1;
  vector<node*> maxNodeList;
  for (int i = 0; i < graph.leavesCount; i++) {
    node* currentnode = graph.showedNode[i];
    if (currentnode == NULL)
      continue;
    // decide to pruning in a loop
    double max = -1;
    node* maxnode;
    //int level=0;
    while (currentnode->parent != NULL) {
      //level++;
      node* parent = currentnode->parent;
      if (parent->parent == NULL)
        break;
      double ek1 = getTotalPropOfNode(parent);
      double ek2 = getTotalPropOfNode(currentnode);
      //cout<< parent->id<<":"<<ek1;
      //bool isvalid=false;
      //double ederive = ek1;
      //abs(ek1 - ek2);
      double ederive = abs(ek1 - ek2);
      if (ederive >= max) {
        //isvalid=true;
        max = ederive;
        maxnode = parent;
      }
      //currentnode->isErased = true;
      //cout<<" ("<<currentnode->id<<","<<ek2<<","<<ederive<<") ";
      currentnode = parent;
    }
    minlevel = (minlevel < maxnode->level) ? minlevel : maxnode->level;
    maxlevel = (maxlevel > maxnode->level) ? maxlevel : maxnode->level;

    //maxNodeList.push_back(maxnode);
    maxNodeList.push_back(maxnode->parent->children[0]);
    maxNodeList.push_back(maxnode->parent->children[1]);
    //cout<<"maxnode"<<maxnode->id<<":"<<max<<endl;
    //getchar();
  }

  for (int i = 0; i < maxNodeList.size(); i++) {
    //if (maxNodeList[i]->parent != NULL)
    //maxNodeList[i]->parent->isEvolvementConserved = true;
    maxNodeList[i]->isEvolvementConserved = true;
  }
  cout << "end selection" << endl;
  for (int i = 0; i < graph.leavesCount; i++) {
    node* currentnode = graph.showedNode[i];
    if (currentnode == NULL)
      continue;
    if (currentnode->isErased)
      continue;
    while (currentnode->parent != NULL) {
      if (currentnode->isEvolvementConserved) {
        conservedList.push_back(currentnode);
        //conservedList.push_back(currentnode->parent->children[1]);
        // set leaves to erase
        /*vector<int>* listOfLeavesChildrenId = graph.graphMapToLeaves[currentnode->parent->id-graph.leavesCount-1];
        int numberOfLeavesChidrenId = listOfLeavesChildrenId->size();
        for (int k = 0; k < numberOfLeavesChidrenId; k++) {
          int leaveId = (*listOfLeavesChildrenId)[k];
          graph.showedNode[leaveId]->isErased = true;
        }
        */
        break;
      }
      currentnode = currentnode->parent;
    }
  }
  cout << "end of minimisze" << endl;
#ifdef DEBUG
  for (int i = 0; i < size; i++) {
    if (graph.showedNode[i] != NULL) {
      if (graph.showedNode[i]->isErased) {
        cout << "erase:" << graph.showedNode[i]->id << endl;
      }
    }
  }
#endif

//  return minlevel;
  return maxlevel;
}

/**
 * @brief pruning the tree by the evolvement method
 */
struct nodevaluepair {
  nodevaluepair(double val, node* nptr) {
    value = val;
    nodePtr = nptr;
  }
  double value;
  node* nodePtr;
};
bool nodevaluepairCompaire(nodevaluepair a, nodevaluepair b) {
  return a.value < b.value;
}
/**
 * @brief Cut the tree from the medium change
 */
int BPTSegment::pruningTreeByEvolvementMediumDerived(vector<node*>& conservedList) {

  int size = graph.showedNode.size();

  for (int i = 0; i < size; i++) {
    node* currentnode = graph.showedNode[i];
    if (currentnode == NULL)
      continue;
    currentnode->isErased = false;
    currentnode->isEvolvementConserved = false;
  }

  //tree.root->isEvolvementConserved = false;
  //tree.root->isErased = false;
  int minlevel = graph.leavesCount * 2;
  int maxlevel = -1;
  vector<node*> maxNodeList;
  for (int i = 0; i < graph.leavesCount; i++) {
    node* currentnode = graph.showedNode[i];
    if (currentnode == NULL)
      continue;


    vector<nodevaluepair> distanceList;

    while (currentnode->parent != NULL) {
      //level++;
      node* parent = currentnode->parent;
      if (parent->parent == NULL)
        break;
      double ek1 = getTotalPropOfNode(parent);
      double ek2 = getTotalPropOfNode(currentnode);
      //cout<< parent->id<<":"<<ek1;
      double ederive = abs(ek1 - ek2);

      nodevaluepair a(ederive, currentnode);
      //vector<nodevaluepair>::iterator it = lower_bound(distanceList.begin(), distanceList.end(), a, nodevaluepairCompaire);
      //distanceList.insert(it, a);
      distanceList.push_back(a);

      currentnode = parent;
    }

    double max = distanceList[distanceList.size()-1].value;
    double min = distanceList[0].value;
    double medium = (max + min) / 2;
    //nodevaluepair a(medium,NULL);
    //vector<nodevaluepair>::iterator it = lower_bound(distanceList.begin(), distanceList.end(), a, nodevaluepairCompaire);
    int j = 0;
    for (j = 0; j < distanceList.size(); j++) {
      if (distanceList[j].value > medium) {
        break;
      }
    }

    node* choosenNode = distanceList[j].nodePtr;
    /*
    for (int i = 0; i < distanceList.size(); i++) {
      cout << "(" << distanceList[i].nodePtr->id << "," << distanceList[i].value << ") ";
    }
    cout << "Medium:" << medium << " Choose:" << choosenNode->id;
    cout << endl;
    getchar();
    */
    maxNodeList.push_back(choosenNode->parent->children[0]);
    maxNodeList.push_back(choosenNode->parent->children[1]);
    //cout<<"maxnode"<<maxnode->id<<":"<<max<<endl;
    //getchar();
  }
  bptTreeReconstruction btr(&graph, &tree, &maxNodeList);
  //btr.maxTreeReconstruction(conservedList);
  btr.minTreeReconstruction(conservedList);

#ifdef DEBUG
  for (int i = 0; i < size; i++) {
    if (graph.showedNode[i] != NULL) {
      if (graph.showedNode[i]->isErased) {
        cout << "erase:" << graph.showedNode[i]->id << endl;
      }
    }
  }
#endif

  //  return minlevel;
  return maxlevel;
}

/**
 * @brief pruning the tree by the evolvement method
 */
void BPTSegment::pruningTreeByEvolvement(vector<node*>& conservedList) {
  int size = graph.showedNode.size();

  for (int i = 0; i < size; i++) {
    node* currentnode = graph.showedNode[i];
    if (currentnode == NULL)
      continue;
    currentnode->isErased = false;
    currentnode->isEvolvementConserved = false;
  }

  //vector<node*> deletedNode;
  // read the list of node in tree and mark the erase
  //tree.root->isEvolvementConserved = true;
  //tree.root->isErased = false;
  vector<node*> maxNodeList;
  for (int i = 0; i < graph.leavesCount; i++) {
    node* currentnode = graph.showedNode[i];

    if (currentnode == NULL)
      continue;

    //if(currentnode->isErased) continue;
    float sumAll = 0;
    int levels = 0;
    node* loopnode = currentnode->parent;

    while (loopnode != NULL) {
      sumAll += getTotalPropOfNode(loopnode);
      levels++;
      loopnode = loopnode->parent;
    }

    // decide to pruning in a loop
    while (currentnode->parent != NULL) {
      node* parent = currentnode->parent;
      bool isvalid = validationEvolvement(currentnode, parent, levels, sumAll);
      if (isvalid) {
        maxNodeList.push_back(currentnode->parent->children[0]);
        maxNodeList.push_back(currentnode->parent->children[1]);
      }
      currentnode = parent;
    }
  }
  int sizeold = maxNodeList.size();
  cout << "size pruning nodes list:" << sizeold << endl;
  bptTreeReconstruction btr(&graph, &tree, &maxNodeList);
  btr.maxTreeReconstruction(conservedList);
  int newsize = conservedList.size();
  cout << "size after maxtree:" << newsize << endl;
  int a = 10;
//btr.minTreeReconstruction(conservedList);

  /*for (int i = 0; i < maxNodeList.size(); i++) {
    if (maxNodeList[i]->parent != NULL)
      maxNodeList[i]->parent->isEvolvementConserved = true;
  }
  cout << "end selection" << endl;
  for (int i = 0; i < graph.leavesCount; i++) {
    node* currentnode = graph.showedNode[i];
    if (currentnode == NULL)
      continue;
    if (currentnode->isErased)
      continue;
    while (currentnode->parent != NULL) {
      if (currentnode->parent->isEvolvementConserved) {
        conservedList.push_back(currentnode->parent->children[0]);
        conservedList.push_back(currentnode->parent->children[1]);
        // set leaves to erase
        vector<int>* listOfLeavesChildrenId = graph.graphMapToLeaves[currentnode->parent->id-graph.leavesCount-1];
        int numberOfLeavesChidrenId = listOfLeavesChildrenId->size();
        for (int k = 0; k < numberOfLeavesChidrenId; k++) {
          int leaveId = (*listOfLeavesChildrenId)[k];
          graph.showedNode[leaveId]->isErased = true;
        }
        break;
      }
      currentnode = currentnode->parent;
    }
  }
  cout << "end of minimisze" << endl;
  */
  //for(int i=0; i<deletedNode.size(); i++){
  //  eraseAllChild(deletedNode[i]);
  //}
#ifdef DEBUG
  for (int i = 0; i < size; i++) {
    if (graph.showedNode[i] != NULL) {
      if (graph.showedNode[i]->isErased) {
        cout << "erase:" << graph.showedNode[i]->id << endl;
      }
    }
  }

#endif
}
/**
 * @brief Correct the pruning by the method Viterbi. This method is to minimize the change of pruning in order to conserve the constraint of increasing criterion.
 */
void BPTSegment::viterbiCorrection() {
  return;
  int size = graph.showedNode.size();
  // read the list of node in tree and mark the erase

  for (int i = 0; i < size; i++) {
    node* currentnode = graph.showedNode[i];

    if (currentnode == NULL)
      continue;

    currentnode->preserveCost = currentnode->isErased;
    currentnode->removeCost = !currentnode->isErased;
  }

  // viterbi algo inorder to correct the erasing marks
  // construct a tree of watershed

  for (int i = 0; i < size; i++) {
    node* current = graph.showedNode[i];

    if (current == NULL)
      continue;

    if (current->children[0] == 0)
      continue;

    // cost of preserve
    // child 1
    current->childrenRemoved[0] = current->children[0]->preserveCost > current->children[0]->removeCost;
    current->childrenRemoved[1] = current->children[1]->preserveCost > current->children[1]->removeCost;

    int min1 = current->childrenRemoved[0] ?
        current->children[0]->removeCost : current->children[0]->preserveCost;

    // child 2
    int min2 = current->childrenRemoved[1] ?
        current->children[1]->removeCost : current->children[1]->preserveCost;

    current->preserveCost += min1 + min2;

    // cost of remove
    current->removeCost += current->children[0]->removeCost + current->children[1]->removeCost;
  }

  tree.root->isErased = tree.root->removeCost < tree.root->preserveCost;

  // correct the removing of the tree

  for (int i = size - 1; i >= 0; i--) {
    node* current = graph.showedNode[i];

    if (current == NULL)
      continue;

    if (current->children[0] == 0)
      continue;

    if (current->isErased) {
      current->children[0]->isErased = true;
      current->children[1]->isErased = true;
    } else {
      current->children[0]->isErased = current->childrenRemoved[0];
      current->children[1]->isErased = current->childrenRemoved[1];
    }
  }
}
/**
 * @brief Calculate the level of each node
 */
void BPTSegment::generateLevel() {
  tree.root->level = 0;
  node* current = tree.root;
  queue<node*> nodeTraversList;
  nodeTraversList.push(current);
  while (!nodeTraversList.empty()) {
    current = nodeTraversList.front();
    if (current->children[0] != NULL) {
      current->children[0]->level = current->level + 1;
      current->children[1]->level = current->level + 1;
      nodeTraversList.push(current->children[0]);
      nodeTraversList.push(current->children[1]);
    }
    nodeTraversList.pop();
  }
}
/**
 * @brief automatically pruning the tree
 * @param imagePreSegment The labeled image before pruning
 * @param imageSegment The image after pruning
 */
void BPTSegment::pruningAutoMaxCouvert(Image<TLabel> const & imagePreSegment, Image<TLabel>& imageSegment) {
  //generateLevel();

  calculateTheHomogeneity(*mainImage);
  calculateColorVariance(*mainImage);
  calculateMeanGradient(*mainImage);

  int size = graph.showedNode.size();
  nodeConservedList.clear();
  //int level = pruningTreeByEvolvementMaxDerived(nodeConservedList);
  int level = pruningTreeByEvolvementMediumDerived(nodeConservedList);
  map<int, int> lut;


#ifdef DEBUG
  cout << "node conserve:" << endl;
  for (int i = 0; i < nodeConservedList.size(); i++) {
    node* current = nodeConservedList[i];
    cout << current->id << " ";
  }
  cout << endl;
#endif
  /// Reconstruction des imageSegment
  int sizelist = nodeConservedList.size();

  for (int i = 0; i < sizelist; i++) {
    node* current = nodeConservedList[i];
    int rootid = current->id;

    if (rootid > graph.leavesCount) {
      int id = rootid - graph.leavesCount - 1;

      for (int j = 0; j < graph.graphMapToLeaves[id]->size(); j++) {
        lut.insert(pair<int, int> ((*graph.graphMapToLeaves[id])[j], rootid));
      }
    } else {
      lut.insert(pair<int, int> (rootid, rootid));
    }
  }


  /// mapping from the LUT to segmented image
  replaceLUT(imagePreSegment, imageSegment, lut);
}
/**
 * @brief Pruning the tree by classification. We classify each node of the tree and segment the classified tree.
 */
void BPTSegment::pruningByClassification(Image<TLabel> const & imagePreSegment, Image<TLabel>& imageSegment, string filename, vector<int>& idList) {
  /// calculate the feature
  int size = graph.showedNode.size();
  int leavesCount = graph.leavesCount;
  Image<U8> gray(imageSrc.getSize());
  cvtRGB2Gray(imageSrc, gray);
  vector< vector<double>*> vectorList;
  //vector<int> idList;
  featureGenerator fg;
  fg.load(ConfigurationManager::instance());

  int MaxLevel = 5;
  queue<node*> nodeQueue;
  nodeQueue.push(tree.root);
  //vector<int> classIdList;
  mainClassifier->classIds.clear();
  while (!nodeQueue.empty()) {
    node* current = nodeQueue.front();
    nodeQueue.pop();
    if (current->level > MaxLevel) {
      continue;
    }

    if (current->children[0] != NULL) {
      nodeQueue.push(current->children[0]);
      nodeQueue.push(current->children[1]);
    }

    int id = current->id;
    /// map
    vector<int> leavesList;

    if (current->id <= leavesCount)
      leavesList.push_back(current->id);
    else
      leavesList = (*graph.graphMapToLeaves[current->id-leavesCount-1]);

    /// leaves
    vector<int*> pointsList ;
    Image<U8C3> localimage(imageSrc.getSize());
    localimage.fill(0);
    for (int mapid = 0; mapid < leavesList.size(); mapid++) {
      vector<int*> v = *graph.pointsListOfLeaves[leavesList[mapid]];
      for (int k = 0; k < v.size(); k++) {
        int x = v[k][0];
        int y = v[k][1];
        localimage(x, y) = imageSrc(x, y);
      }
      //pointsList.push_back(v[k]);
    }//end for
    if(ConfigurationManager::instance()->Classifier==KNN || ConfigurationManager::instance()->Classifier==SVM){
      cout << "Extract image " << current->id << endl;
      fg.generate(localimage, &current->descriptor);
      cout << "Extract descriptor " << current->id << endl;
      vectorList.push_back(&current->descriptor);
    }else{
      
      SIFTClassifier* scl=(SIFTClassifier*)mainClassifier;
      int classid=scl->classify(localimage);
      scl->classIds.push_back(classid);
      //classIdList.push_back(classid);
    }
    idList.push_back(current->id);
  }
  cout << "Classify the noeds" << endl;
  //vector<double> classes;
  //vector<double> classDitances;
  if(ConfigurationManager::instance()->Classifier==KNN || ConfigurationManager::instance()->Classifier==SVM){
    /// classify the node of tree
    classifier* clss = mainClassifier;

    //clss=&aclss;
    /// load the learning file
    clss->loadProblem(filename.c_str());
    //lsa.crossValidation();
    /// classify the regions
    clss->reset();
    clss->setTestData(vectorList);
    clss->classify();
    cout << "End of classify noeuds" << endl;
    int itemsize = vectorList.size();
    //classes.insert(classes.begin(), -1);
    for (int i = 0; i < itemsize; i++) {
      cout << idList[i] << " ";
      //getchar();

      graph.showedNode[idList[i]]->classId = clss->classIds[i];
      cout << graph.showedNode[idList[i]]->classId << " ";
      if (clss->isDistanceSupport)
        graph.showedNode[idList[i]]->classDistance = clss->classDistances[i];
    }
  } else {
    int itemsize = idList.size();
    //classes.insert(classes.begin(), -1);
    for (int i = 0; i < itemsize; i++) {
      cout << idList[i] << " ";
      graph.showedNode[idList[i]]->classId = mainClassifier->classIds[i];
    }
  }
  vector<node*> chosenList;
//#ifdef SAME_CLASS
#ifndef SAME_CLASS
  /// pruning by the node of tree
  queue<node*> q;
  q.push(tree.root);
  chosenList.clear();
  while (!q.empty()) {
    node* n = q.front();
    q.pop();
    if (n->classId == -1)
      continue;
    if (n->children[0] != NULL) {
      if (n->children[0]->classId == n->classId && n->children[1]->classId == n->classId) {
        //n->isErased = false;
        //n->isEvolvementConserved=true;
        //eraseAllChild(n);
        chosenList.push_back(n);
      } else {
        q.push(n->children[0]);
        q.push(n->children[1]);
      }
    } else {
      chosenList.push_back(n);
    }
  }
#else
  for (int i = idList.size() - 1; i >= 0; i--) {
    int id = idList[i];
    node* currentNode = graph.showedNode[id];
    if (currentNode->level < MaxLevel) {
      break;
    }
    double minDistance = 10e6;
    node* chosenNode = NULL;
    while (currentNode->parent != NULL) {
      if (currentNode->classDistance < minDistance) {
        minDistance = currentNode->classDistance;
        chosenNode = currentNode;
      }
      currentNode = currentNode->parent;
    }
    chosenList.push_back(chosenNode->parent->children[0]);
    chosenList.push_back(chosenNode->parent->children[1]);
  }
#endif
  cout << "min tree couper" << endl;
  bptTreeReconstruction btr(&graph, &tree, &chosenList);
  //btr.maxTreeReconstruction(conservedList);
  btr.minTreeReconstruction(nodeConservedList);
  cout << "end tree couper" << endl;
  /// showing the node
  /// Reconstruction des imageSegment
  int sizelist = nodeConservedList.size();


  map<int, int> lut;
  for (int i = 0; i < sizelist; i++) {
    node* current = nodeConservedList[i];
    int rootid = current->id;

    if (rootid > graph.leavesCount) {
      int id = rootid - graph.leavesCount - 1;

      for (int j = 0; j < graph.graphMapToLeaves[id]->size(); j++) {
        lut.insert(pair<int, int> ((*graph.graphMapToLeaves[id])[j], rootid));
      }
    } else {
      lut.insert(pair<int, int> (rootid, rootid));
    }
  }
  /// mapping from the LUT to segmented image
  replaceLUT(imagePreSegment, imageSegment, lut);
  /*int xsize=imageSrc.getSizeX();
  int ysize=imageSrc.getSizeY();
  for(int x=0; x<xsize; x++){
    for(int y=0; y<ysize; y++){
      if(imageSegment(x,y)>10){
        cout<<imageSegment(x,y)<<endl;
      }
    }
  }*/
  cout << "end of classification" << endl;
}
/**
 * @brief automatically pruning the tree
 * @param imagePreSegment The labeled image before pruning
 * @param imageSegment The image after pruning
 */
void BPTSegment::pruningAuto(Image<TLabel> const & imagePreSegment, Image<TLabel>& imageSegment) {
  calculateTheHomogeneity(*mainImage);
  calculateColorVariance(*mainImage);
  calculateMeanGradient(*mainImage);

  int size = graph.showedNode.size();
  nodeConservedList.clear();
  map<int, int> lut;
  pruningTreeByEvolvement(nodeConservedList);
  int sizelistold = nodeConservedList.size();
  viterbiCorrection();


#ifdef DEBUG
  cout << "node conserve:" << endl;

  for (int i = 0; i < nodeConservedList.size(); i++) {
    node* current = nodeConservedList[i];
    cout << current->id << " ";
  }

  cout << endl;

#endif
  /// Reconstruction des imageSegment
  int sizelist = nodeConservedList.size();

  for (int i = 0; i < sizelist; i++) {
    node* current = nodeConservedList[i];
    int rootid = current->id;

    if (rootid > graph.leavesCount) {
      int id = rootid - graph.leavesCount - 1;

      for (int j = 0; j < graph.graphMapToLeaves[id]->size(); j++) {
        lut.insert(pair<int, int> ((*graph.graphMapToLeaves[id])[j], rootid));
      }
    } else {
      lut.insert(pair<int, int> (rootid, rootid));
    }
  }


  /// mapping from the LUT to segmented image
  replaceLUT(imagePreSegment, imageSegment, lut);



}
/**
 * @brief Pruning the tree by the number of region
 * @param regionCount The number of region
 * @param imagePreSegment The labeled image before pruning
 * @param imageSegment The labeled image after pruing
 */
void BPTSegment::pruning(int regionCount, Image<TLabel> const & imagePreSegment, Image<TLabel>& imageSegment) {
  int rc = 1;
  queue<node*> nodeTraversList;
  nodeTraversList.push(tree.root);
  nodeConservedList.clear();
  map<int, int> lut;

  /// tree bars par le prunning

  while (rc < regionCount && !nodeTraversList.empty()) {
    node* current = nodeTraversList.front();
    nodeTraversList.pop();

    if (current->children[0] != NULL) {
      nodeTraversList.push(current->children[0]);
      nodeTraversList.push(current->children[1]);
      rc++;
    } else {
      nodeConservedList.push_back(current);
    }
  }

  /// Fusion 2 suit case
  for (int i = 0; i < nodeTraversList.size(); i++) {
    node* current = nodeTraversList.front();
    nodeTraversList.pop();
    i--;
    nodeConservedList.push_back(current);
  }

#ifdef DEBUG
  cout << "node conserve:" << endl;

  for (int i = 0; i < nodeConservedList.size(); i++) {
    node* current = nodeConservedList[i];
    cout << current->id << " ";
  }

  cout << endl;

#endif
  /// Reconstruction des imageSegment

  for (int i = 0; i < nodeConservedList.size(); i++) {
    node* current = nodeConservedList[i];
    int rootid = current->id;
    queue<node*> allchildren;
    allchildren.push(current);
    //cout<<"replace "<<current->id<<":";

    while (!allchildren.empty()) {
      current = allchildren.front();
      allchildren.pop();

      if (current->children[0] != NULL) {
        allchildren.push(current->children[0]);
        allchildren.push(current->children[1]);
      } else {
        //cout<<current->id<<endl;
        //replace(imageSegment,current->id,rootid);
        lut.insert(pair<int, int> (current->id, rootid));
      }
    }

    //cout<<endl;
  }

  /// mapping from the LUT to segmented image
  replaceLUT(imagePreSegment, imageSegment, lut);

  //sameRegionFusion(imageSegment);

  /*for (int i = 0; i < imageSegment.rows; i++ )5
  {
      for (int j = 0; j < imageSegment.cols; j++ )
      {
          cout<<imageSegment.at<int>(i,j);
      }
      cout<<endl;
  }
  */
}

/**
 * @brief Replace the pixel of old by new segment id
 * @param preimageSegment The labeled image before replacing
 * @param imageSegment The labeled image after replacing
 * @param lut The LUT table for replacing
 */
void BPTSegment::replaceLUT(Image<TLabel>const & preimageSegment, Image<TLabel>& imageSegment, map<int, int>& lut) {
  int ysize = imageSegment.getSizeY();
  int xsize = imageSegment.getSizeX();

  for (int i = 0; i < xsize; i++) {
    for (int j = 0; j < ysize; j++) {
      int idx = preimageSegment(i, j);
      imageSegment(i, j) = lut[idx];
    }
  }
}
/**
 * @brief Replace the one value of pixel by another value. This function is expresive when we want to replace many value. In the case of multi-replacing, using the replacLUT is the better choice.
 * @param imageSegment The labeled image for replacing
 * @param oldvalue The old value
 * @param newvalue The new value
 */
void BPTSegment::replace(Image<TLabel>& imageSegment, int oldvalue, int newvalue) {
  int ysize = imageSegment.getSizeY();
  int xsize = imageSegment.getSizeX();

  for (int i = 0; i < xsize; i++) {
    for (int j = 0; j < ysize; j++) {
      int idx = imageSegment(i, j);

      if (idx == oldvalue) {
        imageSegment(i, j) = newvalue;
      }
    }
  }
}
/**
 * @brief Merge the region of the same id
 */
void BPTSegment::sameRegionFusion(Image<TLabel>& imageSegment) {
  //const int voisins=8;
  //int x[voisins]={-1,0,1,0};
  //int y[voisins]={0,1,0,-1};
  int x[_8_NEIGHBORS_] = _8_NEIGHBORS_DX_;
  int y[_8_NEIGHBORS_] = _8_NEIGHBORS_DY_;
  int ysize = imageSegment.getSizeY();
  int xsize = imageSegment.getSizeX();

  for (int i = 0; i < xsize; i++) {
    for (int j = 0; j < ysize; j++) {
      int idx = imageSegment(i, j);

      if (idx == -1) {
        int countRegion = 0;
        int oldregion = -1;
        bool sameRegion = false;

        for (int v = 0; v < _NEIGHBORS_; v++) {
          int newx = i + x[v];
          int newy = i + y[v];

          if (!(newx < 0 || newx >= xsize || newy < 0 || newy >= ysize)) {
            int iseg = imageSegment(newx, newy);

            if (iseg != -1) {
              if (iseg != oldregion) {
                oldregion = iseg;
                countRegion++;
              } else {
                sameRegion = true;
              }
            }
          }
        }

        if (countRegion == 1 && sameRegion) {
          /*for (int v=0; v<voisins; v++) {
              int newrow=i+x[v];
              int newcol=i+y[v];
              if (!(newrow<0 || newrow>=imageSegment.rows || newcol<0 || newcol>=imageSegment.cols)) {
                  cout<<imageSegment.at<int>(newrow,newcol);
              }
          }
          cout<<endl;*/
          imageSegment(i, j) = oldregion;
        }
      }
    }
  }
}


/*
void BPTSegment::clearBorder(const cv::Mat& src, Mat& imageSegment)
{
    //const int voisin=8;
    int x[_8_NEIGHBORS_]=_8_NEIGHBORS_DX_;
    int y[_8_NEIGHBORS_]=_8_NEIGHBORS_DY_;
    for (int i=0; i<src.rows; i++) {
        for (int j=0; j<src.cols; j++) {
            int id=imageSegment.at<int>(i,j);
            if (id==-1) {
                int vnear=0;
                float distanceMin=10e6;
                for (int v=0; v<_NEIGHBORS_; v++) {
                    if (i+x[v]>=0 && i+x[v]<src.rows && j+y[v]>=0 && j+y[v]<src.cols) {
                        float distance=0;
                        Vec3b colors=src.at<Vec3b>(i+x[v],j+y[v]);
                        Vec3b colorMain=src.at<Vec3b>(i,j);
                        for (int u=0; u<3; u++) {
                            distance+=abs(colors[u]-colorMain[u]);
                        }
                        if (distanceMin>distance && imageSegment.at<int>(i+x[v],j+y[v])!=-1) {
                            distanceMin=distance;
                            vnear=v;
                        }
                    }
                }
                imageSegment.at<int>(i,j)=imageSegment.at<int>(i+x[vnear],j+y[vnear]);
            }
        }
    }
}
*/

/**
 * @brief Constructor of binary partition tree
 */
BPTSegment::BPTSegment() {
  regionFinalCount = 3;
  regionFirstCount = 1000;
  volvementThreshold = 0;
  regionModelStrategy = REGION_MODEL_MOYEN;//REGION_MODEL_GAR98;
  mainClassifier = NULL;
  methodPruning = PRUN_EVOL_AUTO;
  showedFinalSegmentedImage = NULL;
  showedFirstSegmentedImage = NULL;
  mainImage = NULL;
  srand(time(NULL));
}
BPTSegment::~BPTSegment() {
  if (mainImage != NULL)
    delete mainImage;
  if (showedFinalSegmentedImage != NULL)
    delete showedFinalSegmentedImage;
  if (showedFirstSegmentedImage != NULL)
    delete showedFirstSegmentedImage;
}

/**
 * @brief compare the relation by decreasing order
 */
int compare_relation_invert(const neighborRelation *a, const neighborRelation *b) {
  return a->distance > b->distance;
}
/**
 * @brief compare the relation by increasing order
 */
int compare_relation(const neighborRelation *a, const neighborRelation *b) {
  return a->distance < b->distance;
}




// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;   replace-tabs on;  replace-tabs on;  replace-tabs on;   replace-tabs on;   replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;
