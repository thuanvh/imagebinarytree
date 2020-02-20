#ifndef _COLOR_MORPHOLOGY_
#define _COLOR_MORPHOLOGY_
#include "../lib/LibTIM/Common/Image.h"
#include "../lib/LibTIM/Common/FlatSE.h"
// header pour les algos principaux de morphologie (éro, dil, ouv, fer, rec,...)
#include "../lib/LibTIM/Algorithms/Morphology.h"
#include "../lib/LibTIM/Algorithms/ConnectedComponents.h"

using namespace LibTIM;
/// morphological gradient on the color image
void colorMorphologicalGradient(Image<U8C3> const& src, Image<U8> & grad);

#endif
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
