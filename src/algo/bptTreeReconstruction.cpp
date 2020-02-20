#include "bptTreeReconstruction.h"
#include <iostream>
#include <stack>
#include "bptBinaryTree.h"
using namespace std;
/**
 * @brief Maximise the tree
 */
void bptTreeReconstruction::maxTreeReconstruction(std::vector<node* >& nodeConservedList) {
  
  for (int i = 0; i < graph->showedNode.size(); i++) {
    node* currentnode = graph->showedNode[i];
    if (currentnode == NULL)
      continue;
    currentnode->isEvolvementConserved=false;
    currentnode->isErased=false;
  }
  tree->root->isEvolvementConserved=true;
  for (int i = 0; i < markPruntNodeList->size(); i++) {
    (*markPruntNodeList)[i]->isEvolvementConserved = true;
  }
  for (int i = 0; i < graph->leavesCount; i++) {
    node* currentnode = graph->showedNode[i];
    if (currentnode == NULL)
      continue;
    while (currentnode->parent != NULL) {
      if (currentnode->isEvolvementConserved) {
        // marking conserved
        node* parentconserved=currentnode;
        //parentconserved->isEvolvementConserved=true;
        while(parentconserved->parent!=NULL){          
          parentconserved->parent->children[0]->isEvolvementConserved=true;
          parentconserved->parent->children[1]->isEvolvementConserved=true;
          parentconserved=parentconserved->parent;
        }
        break;
      }
      currentnode = currentnode->parent;
    }
  }
  for (int i = 0; i < graph->leavesCount; i++) {
    node* currentnode = graph->showedNode[i];
    if (currentnode == NULL)
      continue;
    if (currentnode->isErased)
      continue;
    while (currentnode->parent != NULL) {
      if (currentnode->isEvolvementConserved) {
        nodeConservedList.push_back(currentnode);
        // erase all children
        node* nodeloop=currentnode;
        queue<node*> q;
        q.push(nodeloop);
        while(!q.empty()){
          node* erasenode=q.front();
          q.pop();
          if(erasenode->children[0]!=NULL){
            erasenode->children[0]->isErased=true;
            erasenode->children[1]->isErased=true;
            q.push(erasenode->children[0]);
            q.push(erasenode->children[1]);
          }
        }
        break;
      }
      currentnode = currentnode->parent;
    }
  }
}

/**
 * @brief minimise tree reconstruction
 */
void bptTreeReconstruction::minTreeReconstruction(std::vector<node* >& nodeConservedList)
{
  for (int i = 0; i < graph->showedNode.size(); i++) {
    node* currentnode = graph->showedNode[i];
    if (currentnode == NULL)
      continue;
    currentnode->isEvolvementConserved=false;
    currentnode->isErased=false;
  }
  for (int i = 0; i < markPruntNodeList->size(); i++) {
    (*markPruntNodeList)[i]->isEvolvementConserved = true;
  }
  stack<node*> nodeList;
  node* root=tree->root;
  nodeList.push(root);
  while(!nodeList.empty()){
    node* nodetop=nodeList.top();    
    nodeList.pop();
    if(nodetop->isEvolvementConserved )
      nodeConservedList.push_back(nodetop);          
    else{
      if(nodetop->children[0]!=NULL){
        nodeList.push(nodetop->children[0]);
        nodeList.push(nodetop->children[1]);
      }else{
        nodeConservedList.push_back(nodetop);
      }
    }
  }
}

