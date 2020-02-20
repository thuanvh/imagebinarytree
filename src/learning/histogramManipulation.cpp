#include "texture.h"
#include "../lib/LibTIM/Common/Histogram.h"
#include "../algo/convertImage.h"

using namespace LibTIM;

/**
 * @brief Extraire Histogramme de l'espace couleur Lab
 */
void extractHistoColorLab(const Image<U8C3> & src, int colorSize, ostream & outfile) {
  // histogram lab
  int histSize[] = {colorSize, colorSize};

  float aranges[] = { -256, 256};
  float branges[] = { -256, 256};

  const float* ranges[] = {aranges, branges};

  // we compute the histogram from the 0-th and 1-st channels

  //from 256 to 32
  Image<U8C3> dst(src);
  resizeImageColor(src, dst, colorSize);

  //Convertir l'espace de couleur
  Image<U8C3> srclab;
  //cvtColor(dst, srclab, CV_BGR2Lab);
  cvtRGB2Lab(src, srclab);

  // Calculer Histogramme
  //calcHist(&srclab, 1, channels, Mat(), hist, 2, histSize, ranges, true, false);
  int size[] = {32, 32, 32};
  int max[] = {256, 256, 256};
  int channelsIndex[] = {0, 1, 2};
  int numberChannels = 3;
  U8C3 black(0);
  Histogram hist(srclab, max, size, channelsIndex, numberChannels,&black);
  hist.normalize();

  hist.write(outfile);
}

/**
 * @brief Extraire Histogramme de l'espace couleur RGB
 */
void extractHistoColorRGB(const Image<U8C3> & src, int colorSize, ostream & outfile, int& colid) {
  // histogram lab
  int histSize[] = {colorSize, colorSize};

  float aranges[] = { -256, 256};
  float branges[] = { -256, 256};

  const float* ranges[] = {aranges, branges};

  // we compute the histogram from the 0-th and 1-st channels

  //from 256 to 32
  Image<U8C3> dst(src);
  resizeImageColor(src, dst, colorSize);


  // Calculer Histogramme
  //calcHist(&srclab, 1, channels, Mat(), hist, 2, histSize, ranges, true, false);
  int size[] = {32, 32, 32};
  int channelsIndex[] = {0, 1, 2};
  int numberChannels = 3;
  int max[] = {256, 256, 256};
  U8C3 black(0);
  Histogram hist(dst, max, size, channelsIndex, numberChannels,&black);
  hist.normalize();

  hist.write(outfile, colid);
}

/**
 * @brief extraire histogramme couleur
 */
void extractHistoColor(const Image<U8C3> & src, int colorSize, ostream & outfile, int& colid) {
  //extractHistoColorRGB(src, colorSize, outfile, colid);
  extractHistoColorLab(src, colorSize,outfile);
}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
