#include <stdlib.h>
#include <iostream>

using namespace std;
#include "bptsegment.h"
#include "../lib/LibTIM/Common/Image.h"
#include "../lib/LibTIM/Common/FlatSE.h"
// header pour les algos principaux de morphologie (éro, dil, ouv, fer, rec,...)
#include "../lib/LibTIM/Algorithms/Morphology.h"
// header pour l'algo du Watershed
#include "../lib/LibTIM/Algorithms/Watershed.h"
// header pour l'algo de seeded region growing (SRG)
#include "../lib/LibTIM/Algorithms/RegionGrowing.h"
// header pour la gestion des composantes connexes
#include "../lib/LibTIM/Algorithms/ConnectedComponents.h"

using namespace LibTIM;
/**
 * @brief Segmentation of watershed
 */
void watershedSegmentation(const Image< U8C3 >& src, Image< TLabel >& imageSegment, int& segmentCount) {
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

  //Image<U8C3> im(src);
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
  Image <U8> grad(xsize, ysize);

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

#ifdef USEQT
  showImage(grad, "Gradient image");

#endif
  // filtre h-min: suppression des minima du gradient non significatifs (évite la sur-segmentation du watershed)

  // paramètre h déterminant la profondeur maximale des minima éliminés
  // possibilité d'obtenir une pyramide d'images pour chaque h
  int h = 4;//atoi(4);

  //hMinFilter(grad, se, h);

  //grad.save("gradAfterFiltering.pgm");

  // calcul des minima régionaux du gradient filtré
  Image <U8> minima = regionalMinima(grad, connexity);

  // labelisation des minima
  Image <TLabel> minimaLabel1 = labelConnectedComponents(minima, connexity);

  //Image <TLabel> minimaLabel2=minimaLabel1;


  // Algo 1: watershed, algo de Meyer (le résultat est stocké dans l'image Label minimaLabel1)
  watershedMeyer(grad, minimaLabel1, connexity);

  // Algo 2: croissance de régions (avantage: calculé directement sur l'image originale)
  //seededRegionGrowing(im,minimaLabel2,connexity);

  // pour chaque région labélisée, calcule la moyenne de la région dans l'image originale
  // création d'une image mosaïque

  //Image <U8C3> result1=computeMarkerMean(im, minimaLabel1);
  //Image <U8> result2=computeMarkerMean(im, minimaLabel2);

  //result1.save("result1.pgm");
  //result2.save("result2.pgm");
  imageSegment = minimaLabel1;

  segmentCount = imageSegment.getMax();

}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
