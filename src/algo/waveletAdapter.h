#ifndef _WAVELET_ADAPTER_
#define _WAVELET_ADAPTER_

#include <stdio.h>
#include <math.h>
#include "../lib/wavelet/macros.h"
#include "../lib/wavelet/alloc.h"
#include "../lib/wavelet/wav_trf.h"
#include "../lib/wavelet/wav_filters_extern.h"
#include "../lib/wavelet/wav_gen.h"
#include "../lib/LibTIM/Common/Image.h"

using namespace LibTIM;


#define WAVELET_HAAR      0
#define WAVELET_ANTONINI  1
#define WAVELET_NICK      2
#define WAVELET_NICK_FLIP 3
#define WAVELET_D79       4
#define WAVELET_D8        5
/**
 * @brief class of wavelet transform
 */
class waveletAdapter {

private:
  void roundImageToU8(float **im, int sizeX, int sizeY, Image<U8>& image);

public:
  /// global variable on whether we should grow packets or not.
  waveletAdapter(): PACKET(0) {}

  int PACKET;
  void chooseMethod(int waveletMethod);
  void complxWaveletTransform(Image<U8>const & original, Image<F32>& image);
  void waveletTransform(Image<U8>const & original, Image<F32>& image);
};

#endif
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
