#ifndef _TIM2QT_H_
#define _TIM2QT_H_

#include <QImage>
#include "../lib/LibTIM/Common/Image.h"

using namespace LibTIM;
QImage& getQImageFromMat(const Image<U8C3> & mat);
QImage& getQImageFromMat(const Image<U8> & mat);

#endif
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
