#ifndef _HU_MOMENT_H_
#define _HU_MOMENT_H_

/**
 * Get a raw moment Mij of an image
 * @img Image
 * @i i coefficient
 * @j j coefficient
 */
double getRawMoment(Image<U8> const & img, int i, int j);

/**
 * Get value of Nij from the uij
 */
double getNij(double uij, double u00, int i, int j);
/**
 * Get Hu Moments from image
 * @img Image
 * @HuMoment array of return value
 */
void getHuMoment(Image<U8> const & img, double * HuMoment);


#endif