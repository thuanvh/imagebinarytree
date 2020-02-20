
#include <fstream>
#include <string>
#include <sstream>
#include <jpeglib.h>
#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include "Image.h"
#include "readpng.h"
#include <iostream>
#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

using namespace std;

namespace LibTIM {

  U8C3* loadppm(const char*filename, int& rows, int& cols) {
    std::ifstream file(filename, std::ios_base::in  | std::ios_base::binary);

    if (!file) {
      std::cerr << "Image file I/O error\n";
      return 0;
    }

    std::string format;

    unsigned int width, height, colormax;
    U8C3* imdata = NULL;
    GImageIO_ReadPPMHeader(file, format, width, height, colormax);

    if (format != "P6" || colormax >= 256) {
      std::cerr << "Error: either type mismatch image type or image is in ASCII .ppm format\n";
      exit(1);
    } else {
      cols = width;
      rows = height;


      imdata = new U8C3 [cols*rows];
      file.read(reinterpret_cast<char *>(imdata), cols*rows*3);
    }

    file.close();

    return imdata;
  }

  U8C3* loadJpeg(const char*filename, int& rows, int& cols) {
    /*
     * Allocate and initialize a JPEG decompression object
     * Specify the source of the compressed data (eg, a file)
     * Call jpeg_read_header() to obtain image info
     * Set parameters for decompression
     * jpeg_start_decompress(...);
     * while (scan lines remain to be read)
     * jpeg_read_scanlines(...);
     * jpeg_finish_decompress(...);
     * Release the JPEG decompression object
     */

    struct jpeg_decompress_struct cinfo;

    struct jpeg_error_mgr jerr;
    FILE *file;
    unsigned char *ligne;
    int i, j;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    if ((file = fopen(filename, "rb")) == NULL) {
      fprintf(stderr, "Erreur : impossible d'ouvrir le fichier .jpg\n");
      exit(1);
    }

    jpeg_stdio_src(&cinfo, file);

    jpeg_read_header(&cinfo, TRUE);




    if (cinfo.jpeg_color_space == JCS_GRAYSCALE) {
      fprintf(stdout, "Erreur : l'image doit etre de type RGB\n");
      exit(1);
    }

    cinfo.out_color_space = JCS_RGB;

    jpeg_start_decompress(&cinfo);

    //unsigned char image[cinfo.image_width*cinfo.image_height*3];
    cols = cinfo.image_width;
    rows = cinfo.image_height;
    U8C3* imdata = new U8C3 [cols*rows];
    ligne = (uch*)imdata;

    while (cinfo.output_scanline < cinfo.output_height) {
      ligne = (uch*)imdata + 3 * cinfo.output_width * cinfo.output_scanline;
      jpeg_read_scanlines(&cinfo, &ligne, 1);
    }

    jpeg_finish_decompress(&cinfo);

    jpeg_destroy_decompress(&cinfo);




    /*RGB* imdata = new RGB [cols*rows];
    unsigned char* ucharimdata=(unsigned char*)imdata;
    memcpy(ucharimdata,image,cols*rows*3);
    */
    /*for (i=0;i<cinfo.image_height;i++){
      for (j=0;j<cinfo.image_width;j++) {
        int n=i*cinfo.image_width*3+j*3;
        //cout<<n<<endl;
        ucharimdata[n+0]=image[n];
        ucharimdata[n+1]=image[n+1];
        ucharimdata[n+2]=image[n+2];
      }
    }*/
    return imdata;
  }

  static double display_exponent;
  U8C3* loadPng(const char* filename, int& rows, int& cols) {
    FILE* infile;
    int rc;
    int have_bg = FALSE;
    double LUT_exponent;               /* just the lookup table */
    double CRT_exponent = 2.2;         /* just the monitor */
    double default_display_exponent;   /* whole display system */

#if defined(NeXT)
    LUT_exponent = 1.0 / 2.2;
    /*
    if (some_next_function_that_returns_gamma(&next_gamma))
        LUT_exponent = 1.0 / next_gamma;
     */
#elif defined(sgi)
    LUT_exponent = 1.0 / 1.7;
    /* there doesn't seem to be any documented function to get the
     * "gamma" value, so we do it the hard way */
    infile = fopen("/etc/config/system.glGammaVal", "r");

    if (infile) {
      double sgi_gamma;

      fgets(tmpline, 80, infile);
      fclose(infile);
      sgi_gamma = atof(tmpline);

      if (sgi_gamma > 0.0)
        LUT_exponent = 1.0 / sgi_gamma;
    }

#elif defined(Macintosh)
    LUT_exponent = 1.8 / 2.61;

    /*
    if (some_mac_function_that_returns_gamma(&mac_gamma))
        LUT_exponent = mac_gamma / 2.61;
     */
#else
    LUT_exponent = 1.0;   /* assume no LUT:  most PCs */

#endif

    /* the defaults above give 1.0, 1.3, 1.5 and 2.2, respectively: */
    default_display_exponent = LUT_exponent * CRT_exponent;


    /* If the user has set the SCREEN_GAMMA environment variable as suggested
     * (somewhat imprecisely) in the libpng documentation, use that; otherwise
     * use the default value we just calculated.  Either way, the user may
     * override this via a command-line option. */
    char *p;

    if ((p = getenv("SCREEN_GAMMA")) != NULL)
      display_exponent = atof(p);
    else
      display_exponent = default_display_exponent;


    int error = 0;

    if (!(infile = fopen(filename, "rb"))) {
      fprintf(stderr, ":  can't open PNG file [%s]\n", filename);
      ++error;
    } else {
      ulg w, h;

      if ((rc = readpng_init(infile, &w, &h)) != 0) {
        ++error;
      } else {
      }

      if (error)
        fclose(infile);

      cols = (int)w;

      rows = (int)h;
    }


    if (error) {
      fprintf(stderr, ":  aborting.\n");
      exit(2);
    }


    /* if the user didn't specify a background color on the command line,
     * check for one in the PNG file--if not, the initialized values of 0
     * (black) will be used */
    /*unsigned char bg_red,bg_green,bg_blue;
    if (have_bg) {
        unsigned r, g, b;   // this approach quiets compiler warnings /

        sscanf(bgstr+1, "%2x%2x%2x", &r, &g, &b);
        bg_red   = (uch)r;
        bg_green = (uch)g;
        bg_blue  = (uch)b;
    } else if (readpng_get_bgcolor(&bg_red, &bg_green, &bg_blue) > 1) {
        readpng_cleanup(TRUE);
        fprintf(stderr,
          ":  libpng error while checking for background color\n");
        exit(2);
    }
    */
    int image_channels = 0;

    ulg image_rowbytes;

    uch* image_data = readpng_get_image(display_exponent, &image_channels,
        &image_rowbytes);


    /* done with PNG file, so clean up to minimize memory usage (but do NOT
     * nuke image_data!) */

    readpng_cleanup(FALSE);

    fclose(infile);

    if (!image_data) {
      fprintf(stderr,  ":  unable to decode PNG image\n");
      exit(3);
    }

    return (U8C3*)image_data;
  }



  int savePNGFile(const Image<U8C3>& bitmap, const char *path) {
    FILE * fp;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    size_t x, y;
    png_byte ** row_pointers = NULL;
    /* "status" contains the return value of this function. At first
     *       it is set to a value which means 'failure'. When the routine
     *       has finished its work, it is set to a value which means
     *       'success'. */
    int status = -1;
    /* The following number is set by trial and error only. I cannot
     *       see where it it is documented in the libpng manual.
     */
    int pixel_size = 3;
    int depth = 8;

    fp = fopen(path, "wb");
    if (! fp) {
      goto fopen_failed;
    }

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
      goto png_create_write_struct_failed;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
      goto png_create_info_struct_failed;
    }

    /* Set up error handling. */

    if (setjmp(png_jmpbuf(png_ptr))) {
      goto png_failure;
    }

    /* Set image attributes. */

    png_set_IHDR(png_ptr,
        info_ptr,
        bitmap.getSizeX(),
        bitmap.getSizeY(),
        depth,
        PNG_COLOR_TYPE_RGB,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);

    /* Initialize rows of PNG. */

    row_pointers = (png_byte**) png_malloc(png_ptr, bitmap.getSizeY() * sizeof(png_byte *));
    for (y = 0; y < bitmap.getSizeY(); ++y) {
      png_byte *row = (png_byte*)
      png_malloc(png_ptr, sizeof(uint8_t) * bitmap.getSizeX() * pixel_size);
      row_pointers[y] = row;
      for (x = 0; x < bitmap.getSizeX(); ++x) {
        U8C3  pixel = bitmap(x, y);
        *row++ = pixel[0];
        *row++ = pixel[1];
        *row++ = pixel[2];
      }
    }

    /* Write the image data to "fp". */

    png_init_io(png_ptr, fp);
    png_set_rows(png_ptr, info_ptr, row_pointers);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    /* The routine has successfully written the file, so we set
     *       "status" to a value which indicates success. */

    status = 0;

    for (y = 0; y < bitmap.getSizeY(); y++) {
      png_free(png_ptr, row_pointers[y]);
    }
    png_free(png_ptr, row_pointers);

  png_failure:
  png_create_info_struct_failed:
    png_destroy_write_struct(&png_ptr, &info_ptr);
  png_create_write_struct_failed:
    fclose(fp);
  fopen_failed:
    return status;
  }
  int savePPMFile(const Image<U8C3>& bitmap, const char *path){
    std::ofstream file(path, std::ios_base::trunc  | std::ios_base::binary);
    
    if (!file) {
      std::cerr << "Image file I/O error\n";
      return 0;
    }
    
    std::string str;
    
    std::string format;
    
    int width = bitmap.getSizeX();
    int height = bitmap.getSizeY();
    
    int buf_size = width * height * 3;
    
    file << "P6\n#CREATOR: GImage \n" << width << " " << height << "\n" << "255\n" ;
    
    U8 *buf = new U8[width*height*3];
    int size=width*height;
    for (int i = 0; i < size; i++) {
      buf[i*3] = bitmap(i)[0];
      buf[i*3+1] = bitmap(i)[1];
      buf[i*3+2] = bitmap(i)[2];
    }
    
    file.write(reinterpret_cast<char *>(buf), buf_size);
    
    file << "\n";
    
    file.close();
    
    delete[] buf;
    
    return 1;
  }
  int saveJPEGFile (const Image<U8C3>& bitmap, const char *path)
  {
    int quality=100;
    /* This struct contains the JPEG compression parameters and pointers to
     * working space (which is allocated as needed by the JPEG library).
     * It is possible to have several such structures, representing multiple
     * compression/decompression processes, in existence at once.  We refer
     * to any one struct (and its associated working data) as a "JPEG object".
     */
    struct jpeg_compress_struct cinfo;
    /* This struct represents a JPEG error handler.  It is declared separately
     * because applications often want to supply a specialized error handler
     * (see the second half of this file for an example).  But here we just
     * take the easy way out and use the standard error handler, which will
     * print a message on stderr and call exit() if compression fails.
     * Note that this struct must live as long as the main JPEG parameter
     * struct, to avoid dangling-pointer problems.
     */
    struct jpeg_error_mgr jerr;
    /* More stuff */
    FILE * outfile;   /* target file */
    JSAMPROW row_pointer[1];  /* pointer to JSAMPLE row[s] */
    int row_stride;   /* physical row width in image buffer */
    
    /* Step 1: allocate and initialize JPEG compression object */
    
    /* We have to set up the error handler first, in case the initialization
     * step fails.  (Unlikely, but it could happen if you are out of memory.)
     * This routine fills in the contents of struct jerr, and returns jerr's
     * address which we place into the link field in cinfo.
     */
    cinfo.err = jpeg_std_error(&jerr);
    /* Now we can initialize the JPEG compression object. */
    jpeg_create_compress(&cinfo);
    
    /* Step 2: specify data destination (eg, a file) */
    /* Note: steps 2 and 3 can be done in either order. */
    
    /* Here we use the library-supplied code to send compressed data to a
     * stdio stream.  You can also write your own code to do something else.
     * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
     * requires it in order to write binary files.
     */
    if ((outfile = fopen(path, "wb")) == NULL) {
      fprintf(stderr, "can't open %s\n", path);
      exit(1);
    }
    jpeg_stdio_dest(&cinfo, outfile);
    
    /* Step 3: set parameters for compression */
    
    /* First we supply a description of the input image.
     * Four fields of the cinfo struct must be filled in:
     */
    cinfo.image_width = bitmap.getSizeX();  /* image width and height, in pixels */
    cinfo.image_height = bitmap.getSizeY();
    cinfo.input_components = 3;   /* # of color components per pixel */
    cinfo.in_color_space = JCS_RGB;   /* colorspace of input image */
    /* Now use the library's routine to set default compression parameters.
     * (You must set at least cinfo.in_color_space before calling this,
     * since the defaults depend on the source color space.)
     */
    jpeg_set_defaults(&cinfo);
    /* Now you can set any non-default parameters you wish to.
     * Here we just illustrate the use of quality (quantization table) scaling:
     */
    jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);
    
    /* Step 4: Start compressor */
    
    /* TRUE ensures that we will write a complete interchange-JPEG file.
     * Pass TRUE unless you are very sure of what you're doing.
     */
    jpeg_start_compress(&cinfo, TRUE);
    
    /* Step 5: while (scan lines remain to be written) */
    /*           jpeg_write_scanlines(...); */
    
    /* Here we use the library's state variable cinfo.next_scanline as the
     * loop counter, so that we don't have to keep track ourselves.
     * To keep things simple, we pass one scanline per call; you can pass
     * more if you wish, though.
     */
    row_stride = bitmap.getSizeX() * 3; /* JSAMPLEs per row in image_buffer */
    U8 *buf = new U8[bitmap.getSizeX()*bitmap.getSizeY()*3];
    int size=bitmap.getSizeX()*bitmap.getSizeY();
    for (int i = 0; i < size; i++) {
      buf[i*3] = bitmap(i)[0];
      buf[i*3+1] = bitmap(i)[1];
      buf[i*3+2] = bitmap(i)[2];
    }
    while (cinfo.next_scanline < cinfo.image_height) {
      /* jpeg_write_scanlines expects an array of pointers to scanlines.
       * Here the array is only one element long, but you could pass
       * more than one scanline at a time if that's more convenient.
       */
      row_pointer[0] = buf+cinfo.next_scanline * row_stride;//& bitmap(cinfo.next_scanline * row_stride);
      (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    
    /* Step 6: Finish compression */
    
    jpeg_finish_compress(&cinfo);
    /* After finish_compress, we can close the output file. */
    fclose(outfile);
    
    /* Step 7: release JPEG compression object */
    
    /* This is an important step since it will release a good deal of memory. */
    jpeg_destroy_compress(&cinfo);
    
    /* And we're done! */
  }
}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;  replace-tabs on;  replace-tabs on;   replace-tabs on;

