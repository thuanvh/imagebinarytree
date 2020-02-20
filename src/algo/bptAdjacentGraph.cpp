#include <stdio.h>
#include <iostream>
#include <queue>
#include <algorithm>

using namespace std;
#include "bptAdjacentGraph.h"
#include "bptsegment.h"
#include "bptNode.h"
#include "waveletAdapter.h"
#include "convertImage.h"
/**
 * @brief Constructor 
 */
AdjacentGraph::AdjacentGraph() {
  regionModelStrategy = REGION_MODEL_MOYEN;//REGION_MODEL_GAR98;
}
/**
 * @brief Get list of AdjacentNodes
 */
node* AdjacentGraph::getAdjacentNodes(node& a) {

}
/**
 * @brief Find out the node need to be merged. They are the first nodes of the ascending queue
 * @param a A node found
 * @param b Another found node
 */
void AdjacentGraph::findMergingNode(node*& a, node*& b) {
#ifndef FAH
  while (!relationQueue.empty()) {
    neighborRelation* rel = relationQueue.top();

    if (rel->isValided == false) {
#ifdef DEBUG
      cout << "erasing:" << rel->origineNode->id << "," << rel->neighbor->id << endl;
#endif
      //relationList.erase(relationList.begin());

      delete rel;
      relationQueue.pop();
    } else {
      break;
    }
  }

  neighborRelation* rel = relationQueue.top();
  relationQueue.pop();
#else
  map<int, queue<neighborRelation*> >::iterator iter;
  bool found = false;
  neighborRelation* rel = NULL;
  vector<int> eraseList;
  for (iter = relmap.begin(); iter != relmap.end(); iter++) {
    int distance = (*iter).first;
    queue<neighborRelation*>* q = &(*iter).second;
    while (!q->empty()) {
      neighborRelation* reltest = q->front();
      q->pop();
      if (reltest->isValided == false) {
#ifdef DEBUG
        cout << "erasing:" << reltest << " " << reltest->origineNode->id << "," << reltest->neighbor->id << endl;
#endif

        delete reltest;
        //rel=NULL;
      } else {
        rel = reltest;
        found = true;
        break;
      }
    }
    if (found)
      break;
    if (q->empty())
      eraseList.push_back(distance);
  }
  for (int i = 0; i < eraseList.size(); i++) {
    relmap.erase(eraseList[i]);
  }
#endif
  /*while (!relationList.empty() ){
    neighborRelation* rel= relationList[0];
    if(rel->isValided==false) {
    #ifdef DEBUG
      cout<<"erasing:"<<rel->origineNode->id<<","<<rel->neighbor->id<<endl;
    #endif
      relationList.erase(relationList.begin());
    }else{
      break;
    }
  }
  neighborRelation* rel= relationList[0];
  */
  //getchar();
  a = rel->origineNode;
  b = rel->neighbor;
}

/*void AdjacentGraph::moveRelationShip(neighborRelation* relation)
{
    //cout<<"Move:"<<relation->origineNode->id<<","<<relation->neighbor->id<<endl;
    /// Find the relation
    vector<neighborRelation*>::iterator iter=lower_bound(relationList.begin(),relationList.end(),relation,compare_relation);
    //vector<neighborRelation*>::iterator iter= upper_bound(relationList->begin(),relationList->end(),rel,compare_relation );
    bool found=false;
    while (iter<relationList.end()) {
        neighborRelation* rel=*iter;
        if (rel==relation) {
            found=true;
            break;

        }
        iter++;
    }
    if (found) {
        //cout<<"erase"<<endl;
        relationList.erase(iter);
        /// Move to the new position
        relation->calculateDistance();
        iter=lower_bound(relationList.begin(),relationList.end(),relation,compare_relation);
        relationList.insert(iter,relation);
    }
}
*/
/**
 * @brief Initialize the graph from the source image and segmented image
 * @param imageSrc Source Image
 * @param imageSegment The segmented image
 * @param segmentCount The number of segments
 */
void AdjacentGraph::initGraph(const Image<U8C3> & imageSrc, Image<TLabel> const& imageSegment, int segmentCount) {
  /// wavelet
  waveletAdapter wl;
  Image<U8> u8(imageSegment.getSize());
  cvtRGB2Gray(imageSrc, u8);

  /*for (int i = 0; i < imageSegment.getSizeX(); i++) {
    for (int j = 0; j < imageSegment.getSizeY(); j++) {
      RGB rgb = imageSrc(i, j);
      u8(i, j) = (rgb[0] + rgb[1] + rgb[2]) / 3;
    }
  }*/

  Image<F32> wlval(imageSrc.getSize());

  wl.waveletTransform(u8, wlval);

  leavesCount = segmentCount;
  /// init list of node

  for (int i = 0; i < segmentCount + 1; i++) {
    showedNode.push_back(NULL);
    pointsListOfLeaves.push_back(new vector<int*>());
  }

  int colorScheme = USING_RGB;

  /*Mat imageLab,imageYuv;
  switch (colorScheme) {
  case USING_LAB:
      imageLab=imageSrc.clone();
      cvtColor(imageSrc,imageLab,CV_RGB2Lab);
      break;
  case USING_YUV:
      imageYuv=imageSrc.clone();
      cvtColor(imageSrc,imageYuv,CV_RGB2YUV);
      break;
  }
  */
  /// find the 8-connect neighbor of the pixel
  int ri[_8_NEIGHBORS_] = _8_NEIGHBORS_DX_;
  int ci[_8_NEIGHBORS_] = _8_NEIGHBORS_DY_;

  /// init graph from segmented image
  int sizex = imageSegment.getSizeX();
  int sizey = imageSegment.getSizeY();

  for (int x = 0; x < sizex; x++) {
    for (int y = 0; y < sizey; y++) {
      int segid = imageSegment(x, y);

      if (segid == -1)
        continue;

      node* currentnode = NULL;

      if (showedNode[segid] == NULL) {
        /// create new node
        currentnode = new node(segid);
        showedNode[segid] = currentnode;

        /// add node to new adjacent graph
        vector<neighborRelation*>* vec = new vector<neighborRelation*>();
        graph.insert(pair<int, vector<neighborRelation*>* >(currentnode->id, vec));
      } else {
        currentnode = showedNode[segid];
      }

      /// calculate the property of the zone of node
      currentnode->numberOfPoint++;
      // bounding
      currentnode->maxX = max(currentnode->maxX, x);
      currentnode->minX = min(currentnode->minX, x);
      currentnode->maxY = max(currentnode->maxY, y);
      currentnode->minY = min(currentnode->minY, y);

      /// store the point to listpointof leaves
      int* a = new int[2];
      a[0] = x;
      a[1] = y;

      pointsListOfLeaves[segid]->push_back(a);

      switch (colorScheme) {
        case USING_RGB:
          U8C3 rgb = imageSrc(x, y);
          currentnode->colors[0] += rgb[0];
          currentnode->colors[1] += rgb[1];
          currentnode->colors[2] += rgb[2];
          break;
      }

      /// Wavelet infos
      currentnode->LH += wlval((sizex + x) / 2, y / 2);

      currentnode->HL += wlval(x / 2, (sizey + y) / 2);

      //cout<<"("<<currentnode->LH<<","<<currentnode->HL<<") ";

      //cout<< endl<<segid<< "neighbors:";
      for (int i = 0; i < _NEIGHBORS_; i++) {
        /// if it is outside
        if (!imageSegment.isPosValid(x + ri[i], y + ci[i]))
          continue;

        int neighborid = imageSegment(x + ri[i], y + ci[i]);

        //cout<<neighborid<<",";
        if (neighborid == -1)
          continue;

        if (neighborid != segid) {
          if (showedNode[neighborid] == NULL) {
            // create the new node
            /// create new node
            node* neighbornode = new node(neighborid);
            showedNode[neighborid] = neighbornode;


            /// add node to new adjacent graph
            vector<neighborRelation*>* vec = new vector<neighborRelation*>();
            graph.insert(pair<int, vector<neighborRelation*>* >(neighbornode->id, vec));
          }

          vector<neighborRelation* >* vec = graph[currentnode->id];

          int j = 0;
          /// find the relation

          for (j = 0; j < vec->size(); j++) {
            if ((*vec)[j]->neighbor->id == neighborid)
              break;
          }

          if (j == vec->size()) {
            ///create new relation
            float distance = currentnode->compair(*showedNode[neighborid]);
            neighborRelation* rel = new neighborRelation(showedNode[neighborid], distance);
            rel->origineNode = currentnode;
            relationList.push_back(rel);
            /// add to list of relation graph
            vec->push_back(rel);
            //cout<<"add:"<<rel->neighbor->id<<",";
          }
        }

      }
    }

  }

#ifdef DEBUG
  cout << "point of leaves" << endl;
  for (int i = 0; i < pointsListOfLeaves.size(); i++) {
    cout << i << ":";
    for (int j = 0; j < pointsListOfLeaves[i]->size(); j++) {
      cout << "(" << (*pointsListOfLeaves[i])[j][0] << "," << (*pointsListOfLeaves[i])[j][1] << ") ";
    }
    cout << endl;
  }

#endif

}
/**
 * @brief Merge two nodes into newnode
 * @param a A node
 * @param b Another node
 * @param c A new node created
 */
void AdjacentGraph::merging(node* const& a, node* const& b, node*& newnode) {
  /// create new node
  newnode = new node(showedNode.size());
  /// need some calculate of merging
  newnode->numberOfPoint = a->numberOfPoint + b->numberOfPoint;
  //bounding
  newnode->maxX = max(a->maxX, b->maxX);
  newnode->minX = min(a->minX, b->minX);
  newnode->maxY = max(a->maxY, b->maxY);
  newnode->minY = min(a->minY, b->minY);
  //cout<<a->id<<"+"<<b->id<<"="<<newnode->id<<":";


  switch (regionModelStrategy) {

    case REGION_MODEL_MOYEN:

      for (int i = 0; i < 3; i++) {
        newnode->colors[i] = (a->colors[i] * a->numberOfPoint + b->colors[i] * b->numberOfPoint) / (a->numberOfPoint + b->numberOfPoint);
        //cout<<newnode->colors[i]<<" ";
      }

      //cout<<endl;
      break;

    case REGION_MODEL_GAR98:

      if (a->numberOfPoint > b->numberOfPoint) {
        for (int i = 0; i < 3; i++) {
          newnode->colors[i] = a->colors[i];
        }
      } else
        if (a->numberOfPoint < b->numberOfPoint) {
          for (int i = 0; i < 3; i++) {
            newnode->colors[i] = b->colors[i];
          }
        } else {
          for (int i = 0; i < 3; i++) {
            newnode->colors[i] = (a->colors[i] + b->colors[i]) / 2;
          }
        }

      break;

    default:
      break;
  }

  /// wavelet
  newnode->HL = (a->HL * a->numberOfPoint + b->HL * b->numberOfPoint) / (a->numberOfPoint + b->numberOfPoint);
  newnode->LH = (a->LH * a->numberOfPoint + b->LH * b->numberOfPoint) / (a->numberOfPoint + b->numberOfPoint);

  vector<neighborRelation*>* veca = graph[a->id];
  vector<neighborRelation*>* vecb = graph[b->id];
  vector<neighborRelation*>* vecc = new vector<neighborRelation*>();

  /*for (int i=0; i<veca->size(); i++) {
      cout<<(*veca)[i]->neighbor->id<<" ";
  }
  cout<<endl;
  for (int i=0; i<vecb->size(); i++) {
      cout<<(*vecb)[i]->neighbor->id<<" ";
  }
  cout<<endl;
  */

  for (int i = 0; i < veca->size(); i++) {
    (*veca)[i]->isValided = false;

    if ((*veca)[i]->neighbor->id == b->id)
      continue;

    //(*veca)[i]->origineNode=newnode;
    //(*veca)[i]->calculateDistance(); don't calculate the distance here, after the insertion

    neighborRelation* newrel = new neighborRelation(newnode, (*veca)[i]->neighbor);

    newrel->calculateDistance();

    vecc->push_back(newrel);

    for (int j = 0; j < vecb->size(); j++) {
      if ((*veca)[i]->neighbor->id == (*vecb)[j]->neighbor->id) {
        (*vecb)[j]->isValided = false;
        vecb->erase(vecb->begin() + j);
        //j--;
        break;
      }
    }
  }

  for (int i = 0; i < vecb->size(); i++) {
    (*vecb)[i]->isValided = false;

    if ((*vecb)[i]->neighbor->id == a->id)
      continue;

    //(*vecb)[i]->origineNode=newnode;
    //(*vecb)[i]->calculateDistance();
    (*vecb)[i]->isValided = false;

    neighborRelation* newrel = new neighborRelation(newnode, (*vecb)[i]->neighbor);

    newrel->calculateDistance();

    vecc->push_back(newrel);

    //vecc->push_back((*vecb)[i]);
  }

  /*
  for (int i=0; i<vecc->size(); i++) {
      cout<<(*vecc)[i]->neighbor->id<<" ";
  }
  cout<<endl;
  */

  showedNode.push_back(newnode);

  /// get map of newnode and leaves
  vector<int>* v = new vector<int>();

  if (a->id > leavesCount) {
    int amapid = a->id - leavesCount - 1;
    int asize = graphMapToLeaves[amapid]->size();

    for (int i = 0; i < asize; i++) {
      v->push_back((*graphMapToLeaves[amapid])[i]);
    }
  } else {
    v->push_back(a->id);
  }

  if (b->id > leavesCount) {
    int amapid = b->id - leavesCount - 1;
    int asize = graphMapToLeaves[amapid]->size();

    for (int i = 0; i < asize; i++) {
      v->push_back((*graphMapToLeaves[amapid])[i]);
    }
  } else {
    v->push_back(b->id);
  }

  graphMapToLeaves.push_back(v);

#ifdef DEBUG
  cout << "Graph map to leaves" << endl;

  for (int i = 0; i < graphMapToLeaves.size(); i++) {
    cout << i << ":";

    for (int j = 0; j < (graphMapToLeaves[i])->size();j++) {
      cout << (*graphMapToLeaves[i])[j] << ",";
    }

    cout << endl;
  }

#endif


//    neighborRelation* newRelation=new neighborRelation();
  graph.insert(pair<int, vector<neighborRelation*>* >(newnode->id, vecc));

  vector<neighborRelation*> newRelationList;

  for (int i = 0; i < vecc->size(); i++) {
    neighborRelation* rel = (*vecc)[i];
#ifndef FAH
    //addToSortedRelationList(&newRelationList,rel);
    relationQueue.push(rel);
#else
    relmap[(int)rel->distance].push(rel);
#endif
  }

  /// replace some relations
  //map<node*, vector<neighborRelation* >* >::iterator it;


  for (int veci = 0; veci < vecc->size(); veci++) {
    node* currentnode = (*vecc)[veci]->neighbor;
    vector<neighborRelation* >* relvec = graph[currentnode->id];

    bool found = false;
    //vector<neighborRelation* >* relvec=(*it).second;
    int sizeRelVec = relvec->size();

    for (int i = 0; i < sizeRelVec; i++) {
      if ((*relvec)[i] != NULL) {
        node* neighbor = (*relvec)[i]->neighbor;

        if (neighbor->id == a->id || neighbor->id == b->id) {
          (*relvec)[i]->isValided = false;

          if (!found) {
            neighborRelation* newRel = new neighborRelation(currentnode, newnode);
            newRel->calculateDistance();
            relvec->push_back(newRel);
            //addToSortedRelationList(&newRelationList,newRel);
#ifndef FAH
            relationQueue.push(newRel);
#else
            relmap[(int)newRel->distance].push(newRel);
#endif
            //(*relvec)[i]->neighbor=newnode;
            //(*relvec)[i]->calculateDistance();
            //neighborRelation* rel=(*relvec)[i];
            //relvec->erase(relvec->begin()+i);
            //moveRelationShip(rel);
            found = true;
          } else {
            //delete (*relvec)[i];
            //relvec->erase(relvec->begin()+i);
            //i--;
          }
          //delete relvec->at(i);
          relvec->erase(relvec->begin() + i);
          sizeRelVec--;
          i--;
        }
      }
    }
  }

  vector<neighborRelation*>* dela=graph[a->id];
  vector<neighborRelation*>* delb=graph[b->id];
  graph.erase(a->id);
  graph.erase(b->id);
  delete dela;
  delete delb;
  // update the relationList
  /*for (int i=0; i<relationList.size(); i++) {
      if (relationList[i]->neighbor->id==a->id || relationList[i]->origineNode->id==a->id ||
              relationList[i]->neighbor->id==b->id || relationList[i]->origineNode->id==b->id) {
          delete relationList[i];
          relationList.erase(relationList.begin()+i);
          i--;
      }
  }*/
  //addToSortedRelationList(&relationList,&newRelationList);
  //sort(relationList.begin(),relationList.end(),compare_relation);

  //
}

/*
void AdjacentGraph::addToSortedRelationList(vector<neighborRelation*>* relationList,neighborRelation* relation) {
    vector<neighborRelation*>::iterator iter=lower_bound(relationList->begin(),relationList->end(),relation,compare_relation);
    relationList->insert(iter,relation);
}
*/
/**
 * @brief add a relation to a sorted list
 * @param parentRelationList List of relation
 * @param childRelationList List of small relation
 */
void AdjacentGraph::addToSortedRelationList(std::vector< neighborRelation* >* parentRelationList, std::vector< neighborRelation* >* childRelationList) {
  /*vector<neighborRelation*>::iterator iterbegin=parentRelationList->begin();
  vector<neighborRelation*>::iterator iter;
  int size=childRelationList->size();
  cout<<"child list"<<endl;
  for (int i=0; i<size; i++){
    neighborRelation* child=(*childRelationList)[i];
    cout<<child->distance<<",";
  }
  cout<<endl;
  for (int i=0; i<size; i++) {
      neighborRelation* child=(*childRelationList)[i];
      iter=lower_bound(iterbegin,parentRelationList->end(),child,compare_relation);
      parentRelationList->insert(iter,child);
      iterbegin=iter;
  }*/
  /*vector<neighborRelation*> mergeRelation(parentRelationList->size()+childRelationList->size());
  merge(parentRelationList->begin(),parentRelationList->end(),childRelationList->begin(),childRelationList->end(),mergeRelation.begin(),compare_relation);
  parentRelationList->clear();
  copy(mergeRelation.begin(),mergeRelation.end(),back_inserter(*parentRelationList));
  */
  int size = childRelationList->size();
  int oldsize = parentRelationList->size();
  vector<neighborRelation*>::iterator mid = parentRelationList->begin() + oldsize;
  parentRelationList->resize(parentRelationList->size() + childRelationList->size());
  copy(childRelationList->begin(), childRelationList->end(), parentRelationList->begin() + oldsize);

  inplace_merge(parentRelationList->begin(), parentRelationList->begin() + oldsize, parentRelationList->end(), compare_relation);

  //merge(parentRelationList->begin(),parentRelationList->end(),childRelationList->begin(),childRelationList->end(),mergeRelation.begin(),compare_relation);
  //parentRelationList->clear();

}

/**
 * @brief Destructor of graph
 */
AdjacentGraph::~AdjacentGraph() {
  for (int i = 0; i < showedNode.size(); i++) {
    delete showedNode[i];
  }

  for (int i = 0; i < pointsListOfLeaves.size(); i++) {
    for (int j = 0; j < pointsListOfLeaves[i]->size(); j++) {
      delete(*pointsListOfLeaves[i])[j];
    }

    delete pointsListOfLeaves[i];
  }

  for (int i = 0; i < graphMapToLeaves.size(); i++) {
    delete graphMapToLeaves[i];
  }

}

/**
 * @brief Calculate the distace between the region
 */
void AdjacentGraph::caculateRegionDistances() {
  //cout<<"color:"<<showedNode.size()<<endl;
  for (int i = 0; i < showedNode.size(); i++) {
    //cout<<i<<" ";
    if (showedNode[i] != NULL)
      showedNode[i]->updateAfterInitialisation();
  }

  //cout<<"size:"<<relationList.size();
  for (int i = 0; i < relationList.size(); i++) {
    //relationList[i]->distance=
    relationList[i]->calculateDistance();
#ifndef FAH
    relationQueue.push(relationList[i]);
#else
    relmap[(int)relationList[i]->distance].push(relationList[i]);
#endif
  }

  //sort(relationList.begin(),relationList.end(),compare_relation);
  //make_heap (relationList.begin(),relationList.end(),compare_relation);
  //sort_heap (relationList.begin(),relationList.end(),compare_relation);
  //quick_sort(relationList.begin(),relationList.end(),compare_relation);
  //relationList=new priority_queue<neighborRelation*,vector<neighborRelation*>,neighborRelationComparison >(relationList.begin(),relationList.end(),neighborRelationComparison(false));
}



/**
 * @brief Print the info of graph
 */
void AdjacentGraph::printGraph() {
  map<int, vector<neighborRelation* >* >::iterator it;


  // show content:

  for (it = graph.begin() ; it != graph.end(); it++) {
    cout << (*it).second << ": " << (*it).first << " => " ;
    int size = (*it).second->size();
    for (int i = 0; i < size; i++) {
      cout << (*(*it).second)[i]->neighbor->id << " ";
    }

    cout << endl;
  }

}

/**
 * @brief print the relation list of graph
 */
void AdjacentGraph::printRelationList() {
  /*for (int i=0; i<relationList.size(); i++) {
      cout<<"("<<relationList[i]->origineNode->id<<","<<relationList[i]->neighbor->id<<","<<relationList[i]->distance<<") ";
  }
  cout<<endl;
  */
}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;  replace-tabs on;   replace-tabs on;
