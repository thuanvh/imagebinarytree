#include "bptNeighborRelation.h"
/**
 * @brief calculate the distance between the class
 */
void neighborRelation::calculateDistance() {
  distance = neighbor->compair(*origineNode);
#ifdef DEBUG
  cout << "distance:" << neighbor->id << "(" << neighbor->colors[0] << ","
      << neighbor->colors[1] << ","
      << neighbor->colors[2] << ","
      << neighbor->numberOfPoint << ")vs " <<
      origineNode->id << "(" << origineNode->colors[0] << ","
      << origineNode->colors[1] << ","
      << origineNode->colors[2] << ","
      << origineNode->numberOfPoint << ")";
  cout << distance << endl;
#endif
}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
