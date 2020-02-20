//#include <gvc.h>
#include <graphviz/gvc.h>
#include <graphviz/cgraph.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

using namespace std;


int main1(int argc, char **argv) {
  graph_t *g, *prev = NULL;
  GVC_t *gvc;
  gvc = gvContext();
  gvParseArgs(gvc, argc, argv);
  while ((g = gvNextInputGraph(gvc))) {
    if (prev) {
      gvFreeLayout(gvc, prev);
      agclose(prev);
    }
    gvLayoutJobs(gvc, g);
    gvRenderJobs(gvc, g);
    prev = g;
  }
  return (gvFreeContext(gvc));
}

int main(int argc, char **argv) {
  Agraph_t *g;
  Agnode_t *n, *m;
  Agedge_t *e;
  Agsym_t *a;
  GVC_t *gvc;
  /* set up a graphviz context */
  gvc = gvContext();
  /* parse command line args - minimally argv[0] sets layout engine */
  gvParseArgs(gvc, argc, argv);
  /* Create a simple digraph */
  g = agopen("g", AGDIGRAPH);
  n = agnode(g, "n");
  m = agnode(g, "m");
  e = agedge(g, n, m);
  /* Set an attribute - in this case one that affects the visible rendering */
  agsafeset(n, "color", "red", "");
  gvLayout(gvc, g, "dot");
  /* Compute a layout using layout engine from command line args */
  gvLayoutJobs(gvc, g);
  /* Write the graph according to -T and -o options */
  gvRenderJobs(gvc, g);
  gvRenderFilename(gvc,g,"png","a.png");
  /* Free layout data */
  gvFreeLayout(gvc, g);
  /* Free graph structures */
  agclose(g);
  /* close output file, free context, and return number of errors */
  return (gvFreeContext(gvc));
}


int main2(int argc, char **argv) {
  GVC_t *gvc;
  graph_t *g;
  FILE *fp;
  gvc = gvContext();
  if (argc > 1)
    fp = fopen(argv[1], "r");
  else
    fp = stdin;
  FILE *out = fopen(argv[2], "w");;
  g = agread(fp);
  gvLayout(gvc, g, "dot");
  gvRender(gvc, g, "png", out);
  gvFreeLayout(gvc, g);
  agclose(g);
  return (gvFreeContext(gvc));
}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;
