// Onur G. Guleryuz 1995, 1996, 1997,
// University of Illinois at Urbana-Champaign,
// Princeton University,
// Polytechnic University.

#include <stdio.h>
#include <math.h>
#include "macros.h"
#include "alloc.h"
#include "wav_filters.h"
#include "wav_trf.h"
#include "wav_gen.h"

// global variable on whether we should grow packets or not.
// set later.
int PACKET=0;

/***********************************************************/
// file io related

FILE *myfopen(char *str,char *fmt)

{
	FILE *res;
	
	res=fopen(str,fmt);
	check_ptr(res,"myfopen");
	return(res);
}

float **load_image(char *name,int N1,int N2)

{
	int i,j;
	float **res;
	FILE *mf;
	
	res=allocate_2d_float(N1,N2,0);
	mf=myfopen(name,"rb");
	for(i=0;i<N1;i++)
		for(j=0;j<N2;j++) {

			if(feof(mf)) {

				printf("eof reached in loading %4d x%4d image\n",N1,N2);
				printf("at row %d and column %d\n",i,j);
				printf("perhaps you have the dimensions wrong?\n");
				exit(1);
			}
			else
				res[i][j]=(float)fgetc(mf);
		}
	fclose(mf);
	return(res);
}

void save_image(char *name,float **im,int N1,int N2)

{
	int i,j;
	FILE *mf;
	
	mf=myfopen(name,"wb");
	for(i=0;i<N1;i++)
		for(j=0;j<N2;j++)
			fputc((int)im[i][j],mf);
	fclose(mf);
}

/***********************************************************/
// Basic operations for thresholding and clipping.

// round and clip the pixels to comply with a grayscale image.
void round_and_clip_to_0_255(float **im,int N1,int N2)

{
	int i,j;

	for(i=0;i<N1;i++)
		for(j=0;j<N2;j++) {

			if(im[i][j]>255)
				im[i][j]=255;
			else if(im[i][j]<0)
				im[i][j]=0;
			im[i][j]=(float)((int)(im[i][j]+.5));
		}
}

// hard threshold the values in matrix.
int hard_threshold(float **matrix,int Ni,int Nj,float threshold)

{
	int i,j,cnt=0;

	for(i=0;i<Ni;i++)
		for(j=0;j<Nj;j++) {

			if(fabs(matrix[i][j])<=threshold) {

				matrix[i][j]=0;
				cnt++;
			}
		}
	return(cnt);
}

// hard threshold the absolute values of the "complex numbers in matrix".
// This is intended for use on complex wavelet coefficients of the form 
// matrix[0][m][n]+ j matrix[2][m][n] and
// matrix[1][m][n]+ j matrix[3][m][n].
// 
// See complex_wav_pack_forw() in wav_trf.c for details.
int hard_threshold_complex(float ***matrix,int Ni,int Nj,float threshold)

{
	int i,j,m,cnt=0;
	float mythres=threshold*threshold;
	float tmpf;
	
	for(m=0;m<2;m++) {

		for(i=0;i<Ni;i++) {

			for(j=0;j<Nj;j++) {

				tmpf=matrix[m][i][j]*matrix[m][i][j]+matrix[m+2][i][j]*matrix[m+2][i][j];
						
				if(tmpf<mythres) {

					matrix[m][i][j]=matrix[m+2][i][j]=0;
					cnt++;
				}
					
			}
		}
	}

	return(cnt);
}

/***********************************************************/
// Example denoising routines.

void complx_wavelet_denoise(float **noisy,float **denoised,int Ni,int Nj,float threshold)

{
	int i,j;
	float **buffer_t;
	float **ctrf[4],**cim;
	// 4 levels
	const int levs=4;

	// main buffer for calculations.
	buffer_t=allocate_2d_float(Ni,Nj,0);

	// buffer that will hold the complex wavelet transform (4x redundancy)
	for(i=0;i<4;i++)
		ctrf[i]=allocate_2d_float(Ni,Nj,0);

	// Copy "noisy" input to the buffer.
	for(i=0;i<Ni;i++)
		for(j=0;j<Nj;j++)
			buffer_t[i][j]=noisy[i][j];

	if(PACKET) {

		// Forward packet transform. Not tested well.
		complex_wav_pack_forw(buffer_t,ctrf,Ni,Nj,levs);

		// Hard threshold the coefficients.
		hard_threshold_complex(ctrf,Ni,Nj,(float)(sqrt(2)*threshold));

		// Inverse transform.
		// Allocates the temporary result array cim.
		cim=complex_wav_pack_inv(ctrf,Ni,Nj,levs);
	}
	else {
		// Forward transform.
		complex_wav_forw(buffer_t,ctrf,Ni,Nj,levs);

		// Hard threshold.
		hard_threshold_complex(ctrf,Ni,Nj,(float)(sqrt(2)*threshold));

		// Inverse transform.
		// Allocates the temporary result array cim.
		cim=complex_wav_inv(ctrf,Ni,Nj,levs);
	}

	for(i=0;i<Ni;i++)
		for(j=0;j<Nj;j++) {

			// Copy final results into place.
			denoised[i][j]=cim[i][j];
		}

	// Free ctrf components allocated.
	for(i=0;i<4;i++)
		free_2d_float(ctrf[i],Ni);

	// Free buffer.
	free_2d_float(buffer_t,Ni);

	// Free temporary storage.
	free_2d_float(cim,Ni);
}


void wavelet_denoise(float **noisy,float **denoised,int Ni,int Nj,float threshold)

{
	int i,j,k;
	int oc_max,oc_r,oc_c;
	float **buffer_t,**buffer;
	int **normalization;
	int shift_arr_r[MAX_ARR_SIZE],shift_arr_c[MAX_ARR_SIZE];
	// 4 level wavelet transform.
	const int levs=4;
	int Nl,Nh;
	float *lp,*hp;

	// Choose wavelet filter. See wav_filters.h and wav_trf.c.
	choose_filter('D',9);

	// Main buffer for operations.
	buffer_t=allocate_2d_float(Ni,Nj,0);

	// Buffer on which results are accumulated.
	buffer=allocate_2d_float(Ni,Nj,1);

	// To keep track of the number of overcomplete results
	// on each pixel. Legacy stuff, this is really not necessary 
	// when doing things image-wide.
	normalization=allocate_2d_int(Ni,Nj,1);

	// Variable that controls the number of overcomplete computations.
	// oc_max=1<<levs; fully overcomplete, takes a looooooong time.
	// oc_max=2 overcomplete at the first level, then regular (4 x redundant)...
	// see comments below.
	oc_max=2;

	for(oc_r=0;oc_r<oc_max;oc_r++) {

		// At every level of a 1-D wavelet transform	there are two transforms possible
		// obtained by the original bank or its one pixel shifted version.
		// In L levels there are 2^L possibilities with each of these possibilities
		// forming a complete transform (i.e., invertible, etc.). If we evaluate
		// more than one then what we end up with is an overcomplete transform.
		// In 2-D there are 4 possibilities at each level and hence 4^L total
		// possibilities.
		//
		// At level l, this code selects either the "regular" (shift_arr_r[l]=0)
		// or the one shifted bank (shift_arr_r[l]=1) over rows and likewise
		// over columns using shift_arr_c[l].
		//
		// In order to generate the fully overcomplete transform we effectively
		// put the binary representation of numbers from 0 to 2^L-1 (represented
		// by oc_max) in shift_arr_r and shift_arr_c to traverse all 4^L possibilities.
		//
		// In fully overcomplete denoising example, we evaluate each of the 4^L transforms,
		// hard threshold coefficients, inverse transform, and finally average the results.
		
		
		// Convert oc_r into binary and put each digit (0-1) into shift_arr_r.
		// Each digit causes the filters at the resulting wavelet level
		// to be shifted by one (1) or remain unshifted (0).
		// For filter shifts over rows.
		k=oc_r;	
		for(i=levs-1;i>=0;i--) {

			shift_arr_r[i]=k>>i;
			k-=shift_arr_r[i]<<i;
		}
		
		
		for(oc_c=0;oc_c<oc_max;oc_c++) {	
			
			// For filter shifts over columns.
			k=oc_c;	
			for(i=levs-1;i>=0;i--) {

				shift_arr_c[i]=k>>i;
				k-=shift_arr_c[i]<<i;
			}
			
			// Copy the "noisy" region into the buffer.
			for(i=0;i<Ni;i++) {

				for(j=0;j<Nj;j++) {

					buffer_t[i][j]=noisy[i][j];
				}
			}
				
			
			// Select the forward bank of filters.
			lp=MFLP;Nl=Nflp;  
			hp=MFHP;Nh=Nfhp;
				
			if(PACKET) {

				// Packet transform.
				wavpack2d_inpl(buffer_t,Ni,Nj,levs,lp,Nl,hp,Nh,1,shift_arr_r,shift_arr_c);

				// Threshold.
				hard_threshold(buffer_t,Ni,Nj,threshold);
			}
			else {

				// Regular wavelets.
				wav2d_inpl(buffer_t,Ni,Nj,levs,lp,Nl,hp,Nh,1,shift_arr_r,shift_arr_c);

				// Threshold.
				hard_threshold(buffer_t,Ni,Nj,threshold);
			}
			
			// Select the inverse bank of filters.
			lp=MILP;Nl=Nilp;  
			hp=MIHP;Nh=Nihp;
				
			// Inverse transform.
			if(PACKET)
				wavpack2d_inpl(buffer_t,Ni,Nj,levs,lp,Nl,hp,Nh,0,shift_arr_r,shift_arr_c);
			else
				wav2d_inpl(buffer_t,Ni,Nj,levs,lp,Nl,hp,Nh,0,shift_arr_r,shift_arr_c);
				
				
			// Accumulate results.
			for(i=0;i<Ni;i++)
				for(j=0;j<Nj;j++) {

					buffer[i][j]+=buffer_t[i][j];
					normalization[i][j]++;
				}			
		}
	}

	for(i=0;i<Ni;i++)
		for(j=0;j<Nj;j++) {
			
			// Copy final results into place.
			if(normalization[i][j])
				denoised[i][j]=buffer[i][j]/normalization[i][j];
			else
				denoised[i][j]=noisy[i][j];
		}

	// Free buffers.
	free_2d_float(buffer_t,Ni);
	free_2d_float(buffer,Ni);
	free_2d_int(normalization,Ni);
}

/***********************************************************/

void add_uniform_noise(float **orig,int Ni,int Nj,float **noisy,float mean,float stdev)

{
	int i,j;
	static int kilroy=0;
	// The way uniform noise is generated has variance 1/12.
	// Multiply with sqrt(12)*stdev to generate noise with variance=stdev^2.
	float scaler=(float)(sqrt(12)*stdev);

	if(!kilroy) {
		kilroy=1;
		srand(0);
	}

	for(i=0;i<Ni;i++)
		for(j=0;j<Nj;j++)
			noisy[i][j]=orig[i][j]+(float)((rand()/(RAND_MAX+1.0)-.5)*scaler+mean);
}

/***********************************************************/

// Play with levs above for the number of wavelet levels
// (and oc_max to change the "amount" of overcompletion
// for wavelets)
int main(int argc,char **argv)

{
	int i,j;
	float **original,**noisy,**denoised;
	const int Ni=512,Nj=512;
	const float noise_stdev=6;
	const float threshold=12;
	float mse=0;

	// This is some version of peppers that I lifted off the Internet.
	// I don't know if it is the "standard" peppers.
	original=load_image("peppers.raw",Ni,Nj);

	// You can also try uncommenting this checkerboard pattern
	// to see if packets are working better.
/*	for(i=0;i<512;i++)
		for(j=0;j<512;j++) {

			if((i/4+j/4)%2)
				original[i][j]=192;
			else
				original[i][j]=64;
		}*/

	// Generate noisy image.
	noisy=allocate_2d_float(Ni,Nj,0);

	// Change this routine for gaussian noise.
	add_uniform_noise(original,Ni,Nj,noisy,0,noise_stdev);

	// Round and clip to grayscale.
	round_and_clip_to_0_255(noisy,Ni,Nj);

	save_image("noisy.raw",noisy,Ni,Nj);
	printf("\nThe noisy image is saved to \"noisy.raw\"\n");

	// Calculate mse.
	mse=0;
	for(i=0;i<Ni;i++)
		for(j=0;j<Nj;j++) {

			mse+=(original[i][j]-noisy[i][j])*(original[i][j]-noisy[i][j]);
		}
	mse/=Ni*Nj;

	printf("\nThe mse of noisy image is: \n");
	printf("\t%8.4f, ( %8.4f dB)\n ",mse,10*log10(255*255/mse));


	// Allocate denoised image.
	denoised=allocate_2d_float(Ni,Nj,0);


	// PACKET=0, no packets, PACKET=1, packets.
	// For packets try an image with high frequencies
	// (try uncommenting the checkerboard pattern above).
	// wavelet (complex wavelet) packets should beat 
	// regular wavelets (complex wavelets).
	PACKET=0;

	// Wavelet denoising.
	printf("-------------------------------\n");
	printf("\nTesting wavelet transform denoising:\n");
	printf("\t(Check source to see which banks are being utilized.)\n");

	wavelet_denoise(noisy,denoised,Ni,Nj,threshold);

	// Round and clip to grayscale.
	round_and_clip_to_0_255(denoised,Ni,Nj);

	save_image("wavelet_denoised.raw",denoised,Ni,Nj);
	printf("The denoised image is saved to \"wavelet_denoised.raw\"\n");

	mse=0;
	for(i=0;i<Ni;i++)
		for(j=0;j<Nj;j++) {

			mse+=(original[i][j]-denoised[i][j])*(original[i][j]-denoised[i][j]);
		}
	mse/=Ni*Nj;

	printf("\nThe mse after wavelet denoising is: \n");
	printf("\t%8.4f, ( %8.4f dB)\n ",mse,10*log10(255*255/mse));


	// Complex wavelet denoising.
	printf("-------------------------------\n");
	printf("\nTesting complex wavelet transform denoising:\n");

	complx_wavelet_denoise(noisy,denoised,Ni,Nj,threshold);

	// Round and clip to grayscale.
	round_and_clip_to_0_255(denoised,Ni,Nj);

	save_image("complx_wavelet_denoised.raw",denoised,Ni,Nj);
	printf("The denoised image is saved to \"complx_wavelet_denoised.raw\"\n");

	mse=0;
	for(i=0;i<Ni;i++)
		for(j=0;j<Nj;j++) {

			mse+=(original[i][j]-denoised[i][j])*(original[i][j]-denoised[i][j]);
		}
	mse/=Ni*Nj;

	printf("\nThe mse after complex wavelet denoising is: \n");
	printf("\t%8.4f, ( %8.4f dB)\n ",mse,10*log10(255*255/mse));

	return(1);
}
