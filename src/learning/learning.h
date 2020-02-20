#ifndef _LEARNING_H_
#define _LEARNING_H_
#include <stdlib.h>
#include "../lib/LibTIM/Common/Image.h"

using namespace LibTIM;

class descripteur {

public:
#define DESCRIPTEUR_LENGTH 5
  double valeurs[DESCRIPTEUR_LENGTH];
};

void descriptionGenerateur(Image<U8C3> image);



#endif
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
