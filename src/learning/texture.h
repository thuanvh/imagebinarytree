/*
 * File:   texture.h
 * Author: thuanvh
 *
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>

#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>

#define FUNC_EXTRACT 1
#define FUNC_SEARCH 2

#define STR_EXTRACT "extract"
#define STR_SEARCH "search"

#define STR_FEATURE_TYPE_HU "hu"
#define STR_FEATURE_TYPE_TEXTURE_COLOR "texturecolor"

#define FEATURE_TYPE_HU 1
#define FEATURE_TYPE_TEXTURE_COLOR 0


#define ERR_DIR_MISSING "Directory parametre is missing."
#define ERR_DIR_OPEN "Could not open directory"
#define ERR_FILE_MISSING "File parametre is missing."
#define ERR_FILE_OPEN "Could not open file"
#define ERR_FUNC_MISSING "Function parametre is missing."

#define NUM_MATRIX 8
#define NUM_MATRIX_ATT 6
#include "../lib/LibTIM/Common/Image.h"

using namespace std;

using namespace LibTIM;

//void extraitFeatureVector(const char* dirPath, const char* name, int graySize);

/**
 * Reduct the gray number
 */
void reduireNiveauDeGris(Image<U8>& image, int size);
void freeMatrix(double*** concurrenceArray, int graySize);
void calculerMatrixCooccurence(Image<U8> const & image, double*** concurrenceArray, int graySize);
void extraitCaracteristicVector(double** mat, int size, double* vvalue, int& valueSize);

double para_angular_second_moment(double** mat, int size);
double para_constrast(double** mat, int size);
double para_entropy(double** mat, int size);
double para_correlation(double** mat, int size);
double para_correlation(double** mat, int size, double meani, double meanj, double vari, double varj);
double para_meani(double** mat, int size);
double para_meanj(double** mat, int size);
double para_vari(double** mat, int size, double meani);
double para_varj(double** mat, int size, double meanj);
double para_dissimilarity(double** mat, int size);
double para_energy(double** mat, int size);
double para_homogenety(double** mat, int size);
double para_shade(double** mat, int size, int meani, int meanj);
double para_prominence(double** mat, int size, int meani, int meanj);
double para_sum_average(double** mat, int size);
double para_sum_variance(double** mat, int size, double sum_e);
double para_sum_entropy(double** mat, int size);
double para_difference_variance(double** mat, int size);
double para_difference_entropy(double** mat, int size);
double para_sum_of_squares__variance(double** mat, int size, double mean);
double para_mean(double** mat, int size);


void extract(const char* dirPath, const char* name, int graySize, int colorSize);
void search(const char* fileLearn, const char* fileTest, int k, double colorWeight, const char* fileRef, const char* refFileOutDir);
void searchHuMoment(const char* fileLearn, const char* fileTest, int k, const char* fileRef, const char* refFileOutDir);
/**
 * extract the texture value to file
 */
//void extractTexture(const Image<U8C3> & src, int graySize, double*** concurrenceArray, ostream& outfile, int & colid);
/**
 * extract the texture value to vector
 */
void extractTexture(const Image<U8> & src, int graySize, double*** concurrenceArray,double* textureVector, int& vectorSize);

//void extractHuMoment(const char* dirPath, const char* name);
void crossTesting(const char* filename, int percent, int k);
void extractHistoColor(const Image<U8C3> & src, int colorSize, ostream& outfile, int & colid);
void printMatrix(double** dirPath, int size);
double getTextureVectorDistance(const double* learningVector, const double* testingVector, int textureSize);
double getColorHistoDistance(const double* learningVector, const double* testingVector, int colorSize);
double getHuMomentVectorDistance(const double* learningVector, const double* testingVector);
void segmenter(const char* fileImage, const char* name, int graySize, int numberNeighbor, int segmblocksize);
void segmenterCAH(const char* fileImage, const char* name, int graySize, int numberGroup, int segmblocksize);
void setZero(double*** concurrenceArray, int graySize);
void freeVector(vector<int>** vector, int size);
void freeVector(vector<double*> vector, int size);
void freeVector(vector<string*> vector, int size);
void freeMatrix(double** matrix, int size);
void resizeImageColor(const Image<U8C3>& src, Image<U8C3>& dst, int num_color);
#endif /* TEXTURE_H */

// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
