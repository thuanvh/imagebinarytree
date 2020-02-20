#include <graphviz/gvc.h>
//#include <graphviz/graph.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <QWidget>
#include <QDialog>
#include "treeViewer.h"
#include <queue>




using namespace std;

treeViewer::treeViewer(QWidget* parent):QDialog(parent)
{
  setupUi(this);
}


void treeViewer::load()
{
  Agraph_t *g;
  //Agnode_t *n, *m;
  //Agedge_t *e;
  Agsym_t *a;
  GVC_t *gvc;
  /* set up a graphviz context */
  gvc = gvContext();
  /* parse command line args - minimally argv[0] sets layout engine */
  //gvParseArgs(gvc, argc, argv);
  /* Create a simple digraph */
  g = agopen("g", Agstrictundirected, 0);
  agattr(g, AGRAPH,"nodesep","0.02");
  agattr(g, AGRAPH,"ranksep","0.02");
  agattr(g, AGNODE, "color", "blue");
  agattr(g, AGNODE, "shape", "point");
  agattr(g, AGRAPH, "size", "7.75,10.25");
  agattr(g, AGEDGE, "weight", "1.2");
  agattr(g, AGNODE, "width", "0.03");
  agattr(g, AGNODE, "height", "0.03");
  //agnodeattr(g, "fixsize", "true");
  
  queue<node*> q;
  q.push(tree->root);
  char strname[255];
  int levelmax=100;
  int count=0;
  while(!q.empty()){
    node* current=q.front();
    count++;
    if(current->level>levelmax)
      break;
    q.pop();
    if(current->children[0]!=0){
      q.push(current->children[0]);
      q.push(current->children[1]);
    }
    
    sprintf(strname,"%d",current->id);
    Agnode_t* gnode=agnode(g,strname,1);
    
    if(current->isErased)
      agsafeset(gnode,"color","red","");
    //if(drawNode!=NULL)
      //drawNode(gnode);
    current->graphicNode=gnode;
    if(current->parent!=NULL){
      Agedge_t *e=agedge(g,gnode,(Agnode_t*)current->parent->graphicNode, NULL, 1);
      //agsafeset(e,"len","2","");
      //if(drawEdge!=NULL)
        //drawEdge(e);
    }    
  }
  //cout<<"Number of node:"<<count<<endl;
  gvLayout(gvc, g, "dot");
  /* Compute a layout using layout engine from command line args */
  //gvLayoutJobs(gvc, g);
  /* Write the graph according to -T and -o options */
  //gvRenderJobs(gvc, g);
  time_t seconds;  
  seconds = time(NULL);
  sprintf(strname,"%d.png",seconds);
  gvRenderFilename(gvc,g,"png",strname);
  /* Free layout data */
  gvFreeLayout(gvc, g);
  /* Free graph structures */
  agclose(g);
  /* close output file, free context, and return number of errors */
  gvFreeContext(gvc);

  QImage image(strname);
  lblTreeImage->setPixmap(QPixmap::fromImage(image));
  lblTreeImage->setGeometry(0,0,image.width(),image.height());
}

void showTreeViewer(BinaryTree* tree, QWidget* parent)
{
  treeViewer* tr=new treeViewer(parent);
  tr->tree=tree;
  tr->load();
  tr->show();
  //tr->drawEdge=NULL;
  //tr->drawNode=NULL;
  //delete tr;
}
void drawClassedImage(Agnode_t* gnode){
  agsafeset(gnode,"color","red","");
}
void showTreeClassified(BinaryTree* tree, QWidget* parent)
{
  treeViewer* tr=new treeViewer(parent);
  tr->tree=tree;
  tr->load();
  tr->show();
  //tr->drawNode=&drawClassedImage;
  //delete tr;
}
