#include <stdlib.h>
#include <iostream>

using namespace std;

#include "colorMorphology.h"
#include "../lib/LibTIM/Common/Image.h"
#include "../lib/LibTIM/Common/FlatSE.h"
// header pour les algos principaux de morphologie (éro, dil, ouv, fer, rec,...)
#include "../lib/LibTIM/Algorithms/Morphology.h"

using namespace LibTIM;
/**
 * @brief Morphologie gradient of the color
 */
void colorMorphologicalGradient(Image<U8C3> const& src, Image<U8> & grad) {
  Image<U8> srcR(src.getSize()), srcG(src.getSize()), srcB(src.getSize());
  int ysize = src.getSizeY();
  int xsize = src.getSizeX();

  for (int i = 0; i < xsize; i++) {
    for (int j = 0; j < ysize; j++) {
      U8C3 val = src(i, j);
      srcR(i, j) = val[0];
      srcG(i, j) = val[1];
      srcB(i, j) = val[2];
    }
  }

  //src.save("src.ppm");
  //srcR.save("r.pgm");
  //srcG.save("g.pgm");
  //srcB.save("b.pgm");
  //pause();
  //showImage(srcR,"R image src");
  //showImage(srcG,"G image src");
  //showImage(srcB,"B image src");
  // Image est une classe générique paramétrée par le type des points contenus dans l'image
  // FlatSE est la classe stockant un élément structurant
  FlatSE connexity;

  // initialisation de l'élément structurant 'connexity' à un 8-voisinage (l'élément ne contient pas l'origine)
  //   X X X
  //   X . X
  //   X X X
  connexity.make2DN8();

  FlatSE se;

  // initialisation à un 9-voisinage (l'élément contient l'origine)
  // X X X
  // X X X
  // X X X
  se.make2DN9();

  //Image<RGB> im(src);
  ////////////////////////////////////////////////////
  // exemple érosion/dilatation                     //
  ////////////////////////////////////////////////////

  // érosion de l'image im par l'élément structurant se
  Image <U8> imEro = erosion(srcG, se);

  //showImage(imEro,"src B erosion");
  // dilatation de l'image im par l'élément structurant se
  Image <U8> imDil = dilation(srcB, se);

  //showImage(imDil,"src B dilation");

  // écriture des images résultat au format .pgm
  //imEro.save("imEro.pgm");
  //imDil.save("imDil.pgm");

  ////////////////////////////////////////
  // exemple d'utilisation du watershed //
  ////////////////////////////////////////

  // calcul du gradient morphologique
  Image <U8> gradB = morphologicalGradient(srcB, se);

  Image <U8> gradR = morphologicalGradient(srcR, se);

  Image <U8> gradG = morphologicalGradient(srcG, se);

#ifdef USEQT
  showImage(gradB, "B grad");

  showImage(gradR, "B grad");

  showImage(gradG, "B grad");

#endif

  //grad.save("gradBeforeFiltering.pgm");
  //Image <U8> grad(xsize,ysize);
  int max = -1;

  for (int i = 0; i < xsize; i++) {
    for (int j = 0; j < ysize; j++) {
      max = -1;
      max = max > gradB(i, j) ? max : gradB(i, j);
      max = max > gradG(i, j) ? max : gradG(i, j);
      max = max > gradR(i, j) ? max : gradR(i, j);
      grad(i, j) = max;
    }
  }
}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
