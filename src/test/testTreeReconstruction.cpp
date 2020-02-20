#include <iostream>
#include <algo/bptTreeReconstruction.h>
using namespace std;

int main(int argc,char** argv){
  
  AdjacentGraph* graph=new AdjacentGraph();
 
  graph->leavesCount=7;
  graph->showedNode.push_back(NULL);
  for(int i=1; i<12; i++)
    graph->showedNode.push_back(new node(i));
  graph->showedNode[7]->children[0]=graph->showedNode[2];
  graph->showedNode[7]->children[1]=graph->showedNode[3];
  graph->showedNode[2]->parent=graph->showedNode[7];
  graph->showedNode[3]->parent=graph->showedNode[7];

  graph->showedNode[8]->children[0]=graph->showedNode[5];
  graph->showedNode[8]->children[1]=graph->showedNode[6];
  graph->showedNode[5]->parent=graph->showedNode[8];
  graph->showedNode[6]->parent=graph->showedNode[8];

  graph->showedNode[9]->children[0]=graph->showedNode[7];
  graph->showedNode[9]->children[1]=graph->showedNode[1];
  graph->showedNode[7]->parent=graph->showedNode[9];
  graph->showedNode[1]->parent=graph->showedNode[9];

  graph->showedNode[10]->children[0]=graph->showedNode[4];
  graph->showedNode[10]->children[1]=graph->showedNode[8];
  graph->showedNode[4]->parent=graph->showedNode[10];
  graph->showedNode[8]->parent=graph->showedNode[10];

  graph->showedNode[11]->children[0]=graph->showedNode[9];
  graph->showedNode[11]->children[1]=graph->showedNode[10];
  graph->showedNode[9]->parent=graph->showedNode[11];
  graph->showedNode[10]->parent=graph->showedNode[11];

  graph->showedNode[11]->parent=NULL;

  BinaryTree * tree=new BinaryTree();
  tree->root=graph->showedNode[11];

  vector<node*> markpruning;
  markpruning.push_back(graph->showedNode[7]);
  markpruning.push_back(graph->showedNode[4]);
  markpruning.push_back(graph->showedNode[8]);
  
  bptTreeReconstruction rec(graph,tree,&markpruning);
  //rec.tree=tree;

  
  vector<node*> a;
  //rec.maxTreeReconstruction(a);
  rec.minTreeReconstruction(a);
  for(int i=0; i<a.size(); i++){
    cout<<a[i]->id<<endl;
  }
  
  return 0;
}