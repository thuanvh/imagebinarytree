#include "SIFT.h"
#include <lib/vl/sift.h>
#include <lib/LibTIM/Common/Image.h>
#include <algo/distanceCalculate.h>
#include <float.h>

using namespace LibTIM;

int SIFTPoint::SIFTSize = 128;
/**
 * @brief Load sift point from a file stream
 * @param ist File stream input
 * @param includeHeading include heading flag
 */
void SIFTPoint::load(istream& ist,bool includedHeading) {
  if (descriptors == NULL)
    descriptors = new double[SIFTSize];
  if(includedHeading){
    ist >> x;
    ist >> y;
    ist >> scale;
    ist >> angle;
  }
  for (int i = 0; i < SIFTSize; i++) {
    ist >> descriptors[i];
  }
}
/**
 * @brief Constructor
 */
SIFTPoint::SIFTPoint() {
  descriptors = new double[SIFTPoint::SIFTSize];
}
/**
 * @brief Destructor
 */
SIFTPoint::~SIFTPoint() {
  if (descriptors != NULL)
    delete descriptors;
}
/**
 * @brief Write some heading
 */
void SIFTPoint::write(ostream& os,bool includedHeading) {
  if(includedHeading){
    os << x << " " << y << " " << scale << " " << angle << " ";
  }
  for (int i = 0; i < SIFTSize; i++) {
    os << descriptors[i] << " ";
  }
  os << endl;
}
/**
 * @brief Get Sift Point from image
 * @param img Input image
 * @param siftarray Array of sift point output
 * This function is a copy of the vl library
 */
void SIFTAdapter::getSiftPoint(const LibTIM::Image< U8 >& img, std::vector< SIFTPoint* >& siftarray) {

  /* algorithm parameters */
  double   edge_thresh  = -1 ;
  double   peak_thresh  = -1 ;
  double   magnif       = -1 ;
  int      O = -1, S = 3, omin = -1 ;

  vl_bool  err    = VL_ERR_OK ;
  char     err_msg [1024] ;
  int      n ;
  int      exit_code          = 0 ;
  int      verbose            = 0 ;
  vl_bool  force_output       = 0 ;
  vl_bool  force_orientations = 0 ;


#define ERRF(msg, arg) {                                        \
    err = VL_ERR_BAD_ARG ;                                      \
    snprintf(err_msg, sizeof(err_msg), msg, arg) ;              \
    break ;                                                     \
  }

#define ERR(msg) {                                              \
    err = VL_ERR_BAD_ARG ;                                      \
    snprintf(err_msg, sizeof(err_msg), msg) ;                   \
    break ;                                                     \
  }








  FILE            *in    = 0 ;
  //vl_uint8        *data  = 0 ;
  vl_sift_pix     *fdata = 0 ;
  

  VlSiftFilt      *filt = 0 ;
  vl_size          q ;
  int              i ;
  vl_bool          first ;

  double           *ikeys = 0 ;
  int              nikeys = 0, ikeys_size = 0 ;



  /* allocate buffer */
  int datasize=img.getBufSize();
  //data  = (vl_uint8*)img.
  fdata = (vl_sift_pix*)malloc(datasize * sizeof(vl_sift_pix)) ;

  if (!fdata) {
    err = VL_ERR_ALLOC ;
    snprintf(err_msg, sizeof(err_msg),
        "Could not allocate enough memory.") ;
    goto done ;
  }


  /* convert data type */
  for (q = 0 ; q < img.getBufSize() ; ++q) {
    fdata [q] = img(q) ;
  }

  /* ...............................................................
   *                                     Optionally source keypoints
   * ............................................................ */

#define WERR(name,op)                                           \
  if (err == VL_ERR_OVERFLOW) {                               \
    snprintf(err_msg, sizeof(err_msg),                        \
        "Output file name too long.") ;                  \
    goto done ;                                               \
  } else if (err) {                                           \
    snprintf(err_msg, sizeof(err_msg),                        \
        "Could not open '%s' for " #op, name) ;          \
    goto done ;                                               \
  }



  filt = vl_sift_new(img.getSizeX(), img.getSizeY(), O, S, omin) ;

  if (edge_thresh >= 0)
    vl_sift_set_edge_thresh(filt, edge_thresh) ;
  if (peak_thresh >= 0)
    vl_sift_set_peak_thresh(filt, peak_thresh) ;
  if (magnif      >= 0)
    vl_sift_set_magnif(filt, magnif) ;

  if (!filt) {
    snprintf(err_msg, sizeof(err_msg),
        "Could not create SIFT filter.") ;
    err = VL_ERR_ALLOC ;
    goto done ;
  }



  /* ...............................................................
   *                                             Process each octave
   * ............................................................ */
  i     = 0 ;
  first = 1 ;
  while (1) {
    VlSiftKeypoint const *keys = 0 ;
    int                   nkeys ;

    /* calculate the GSS for the next octave .................... */
    if (first) {
      first = 0 ;
      err = vl_sift_process_first_octave(filt, fdata) ;
    } else {
      err = vl_sift_process_next_octave(filt) ;
    }

    if (err) {
      err = VL_ERR_OK ;
      break ;
    }



    /* run detector ............................................. */
    if (ikeys == 0) {
      vl_sift_detect(filt) ;

      keys  = vl_sift_get_keypoints(filt) ;
      nkeys = vl_sift_get_nkeypoints(filt) ;
      i     = 0 ;

      if (verbose > 1) {
        printf("sift: detected %d (unoriented) keypoints\n", nkeys) ;
      }
    } else {
      nkeys = nikeys ;
    }

    /* for each keypoint ........................................ */
    for (; i < nkeys ; ++i) {
      double                angles [4] ;
      int                   nangles ;
      VlSiftKeypoint        ik ;
      VlSiftKeypoint const *k ;

      /* obtain keypoint orientations ........................... */
      if (ikeys) {
        vl_sift_keypoint_init(filt, &ik,
            ikeys [4 * i + 0],
            ikeys [4 * i + 1],
            ikeys [4 * i + 2]) ;

        if (ik.o != vl_sift_get_octave_index(filt)) {
          break ;
        }

        k          = &ik ;

        /* optionally compute orientations too */
        if (force_orientations) {
          nangles = vl_sift_calc_keypoint_orientations
              (filt, angles, k) ;
        } else {
          angles [0] = ikeys [4 * i + 3] ;
          nangles    = 1 ;
        }
      } else {
        k = keys + i ;
        nangles = vl_sift_calc_keypoint_orientations
            (filt, angles, k) ;
      }

      /* for each orientation ................................... */
      for (q = 0 ; q < (unsigned) nangles ; ++q) {
        vl_sift_pix descr [128] ;

        /* compute descriptor (if necessary) */
        vl_sift_calc_keypoint_descriptor(filt, descr, k, angles [q]) ;

        SIFTPoint* siftPoint = new SIFTPoint();
        siftPoint->angle = k->sigma;
        siftPoint->x = k->x;
        siftPoint->y = k->y;
        siftPoint->scale = k->s;
        for(int siftindex=0; siftindex<SIFTPoint::SIFTSize; siftindex++){
          siftPoint->descriptors[siftindex]=descr[siftindex];
        }

        siftarray.push_back(siftPoint);
      }
    }
  }

  /* ...............................................................
   *                                                       Finish up
   * ............................................................ */



done :
  /* release input keys buffer */
  if (ikeys) {
    free(ikeys) ;
    ikeys_size = nikeys = 0 ;
    ikeys = 0 ;
  }

  /* release filter */
  if (filt) {
    vl_sift_delete(filt) ;
    filt = 0 ;
  }

  /* release image data */
  if (fdata) {
    free(fdata) ;
    fdata = 0 ;
  }



  /* close files */
  if (in) {
    fclose(in) ;
    in = 0 ;
  }


}

/**
 * @brief Load Sift point
 * 
 */
void SIFTAdapter::loadListSIFTPoint(istream& is, std::vector< SIFTPoint* >& siftarray, bool includedHeading) {
  while (is.good()) {
    SIFTPoint* point = new SIFTPoint();
    point->load(is,includedHeading);
    siftarray.push_back(point);
  }
}
/**
 * @brief Find the matching between 2 collection of SIFT point
 */
void SIFTAdapter::matching(std::vector< SIFTPoint* >& dictionary, std::vector< SIFTPoint* >& siftarrayB, double*  indexvector,double threshold) {
  int size = dictionary.size();
  int itemsize=siftarrayB.size();
  for (int i = 0; i < itemsize; i++) {
    indexvector[i]=-1;
  }
  
  for (int i = 0; i < siftarrayB.size(); i++) {
    SIFTPoint* p = siftarrayB[i];
    double bestDistance = FLT_MAX;
    int bestIndex = 0;
    for (int idic = 0; idic < size; idic++) {
      double distance = distanceCalculator::getDistanceEuclidien(p->descriptors, dictionary[idic]->descriptors, SIFTPoint::SIFTSize, bestDistance);
      if (bestDistance > distance) {
        bestDistance = distance;
        bestIndex = idic;
      }
    }
    if(bestDistance<threshold){
      indexvector[i] = bestIndex;
    }
  }
}
/**
 * @brief Get the index vector of the correspondance between 2 collection of SIFT point
 */
void SIFTAdapter::getIndexVectorFromDictionary(std::vector< SIFTPoint* >& dictionary, std::vector< SIFTPoint* >& siftarray, double*  indexvector) {
  int size = dictionary.size();
  
  for (int i = 0; i < size; i++) {
    indexvector[i]=0;
  }
  int itemsize=siftarray.size();
  for (int i = 0; i < siftarray.size(); i++) {
    SIFTPoint* p = siftarray[i];
    double bestDistance = FLT_MAX;
    int bestIndex = 0;
    for (int idic = 0; idic < size; idic++) {
      double distance = distanceCalculator::getDistanceEuclidien(p->descriptors, dictionary[idic]->descriptors, SIFTPoint::SIFTSize, bestDistance);
      if (bestDistance > distance) {
        bestDistance = distance;
        bestIndex = idic;
      }
    }
    indexvector[bestIndex] ++;
  }
  /*for (int i = 0; i < size; i++) {
    cout<<indexvector[i]<<" ";
  }
  cout<<endl;
  */
}

void SIFTAdapter::getSiftDictionary(std::vector< SIFTPoint* >) {

}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;
