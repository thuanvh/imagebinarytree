
#include <iostream>
#include <math.h>
#include <stdlib.h>

using namespace std;

#include "bptNode.h"
#include "bptsegment.h"

/**
 * @brief Calculate some color and texture after the initiation
 */
void node::updateAfterInitialisation() {
  for (int i = 0; i < 3; i++) {    
    colors[i] /= numberOfPoint;    
  }

  HL /= numberOfPoint;
  LH /= numberOfPoint;
  //cout << HL << "," << LH << " ";
}

/**
 * @brief Compair with other node
 */
float node::compair(node const & other) {
  int mergingStrategy = MERGING_ORDER_GAR98;
  float distance = 0;

  switch (mergingStrategy) {

    case MERGING_ORDER_MOYEN:
      distance = abs(colors[0] - other.colors[0]) + abs(colors[1] - other.colors[1]) + abs(colors[2] - other.colors[2]);
      break;

    case MERGING_ORDER_GAR98: {
      float mergingcolor[3] = {0};

      if (numberOfPoint > other.numberOfPoint) {
        for (int i = 0; i < 3; i++) {
          mergingcolor[i] = colors[i];
        }
      } else
        if (numberOfPoint < other.numberOfPoint) {
          for (int i = 0; i < 3; i++) {
            mergingcolor[i] = other.colors[i];
          }
        } else {
          for (int i = 0; i < 3; i++) {
            mergingcolor[i] = (colors[i] + other.colors[i]) / 2;
          }
        }

      float a = 0, b = 0;

      for (int i = 0; i < 3; i++) {
        a += pow(colors[i] - mergingcolor[i], 2);
        b += pow(other.colors[i] - mergingcolor[i], 2);
      }

      distance = numberOfPoint * sqrt(a) + other.numberOfPoint * sqrt(b);

      break;
    }

    default:
      break;
  }

  //distance=distance * (abs(HL-other.HL) + abs(LH-other.LH));
  float alpha = 1;

  distance = alpha * distance + (1 - alpha) * (abs(HL - other.HL) + abs(LH - other.LH));

  return distance;
}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
