//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

//#define _DEBUG_PNG8
#include "stdafx.h"
#include "RendererStyles.h"
#include "AGGImageIO.h"
#include "AGGRenderer.h"
#include "png.h"
#include "RS_ByteData.h"
#include "agg_context.h"

#include "gd.h" //for image read support other than PNG (which means JPEG and GIF)
//NOTE: We do not use gd for reading or writing PNG since internally gd drops a bit
//from the alpha channel, which is not desirable for high quality output

#include "GDUtils.h"
#include "MapQuantization.h"
#include <assert.h>

#ifdef _DEBUG_PNG8
#define myassert(COND,L,F) if (!(COND)){ printf("(%d) failed assertion in %d %s", GetCurrentThreadId(), L,F); throw new exception(); }
#else
#define myassert(COND,L,F) (COND)
#endif

#pragma warning(disable : 4611)

//enum MS_RETURN_VALUE {MS_SUCCESS, MS_FAILURE, MS_DONE};

// in-memory switch to permit disabling colormapping at runtime
// (could be in debug ifdefs but will be optimized away in release build anyway)
static bool s_bUseColorMap = true;

struct png_write_context
{
    unsigned char* buf;
    size_t used;
    size_t len;
};

void png_write_cb(png_structp png, png_bytep data, png_size_t sz)
{
    png_write_context* cxt = (png_write_context*)png->io_ptr;

    if (cxt->used + sz > cxt->len)
    {
        size_t nsz = (cxt->len) ? cxt->len : 1;
        while (nsz < cxt->used + sz) nsz *= 2;

        unsigned char* ndata = new unsigned char[nsz];
        memcpy(ndata, cxt->buf, cxt->used);
        delete [] cxt->buf;
        cxt->buf = ndata;
        cxt->len = nsz;
    }

    memcpy(cxt->buf + cxt->used, data, sz);
    cxt->used += sz;
}

void png_read_cb(png_structp png, png_bytep data, png_size_t sz)
{
    png_write_context* cxt = (png_write_context*)png->io_ptr;
    memcpy(data, cxt->buf + cxt->used, sz);
    cxt->used += sz;
}


/* write a png file */
int write_png(png_write_context* cxt, unsigned int* pix, int width, int height, double /*gamma*/, bool drop_alpha)
{
   png_structp png_ptr;
   png_infop info_ptr;

   /* Create and initialize the png_struct with the desired error handler
    * functions.  If you want to use the default stderr and longjump method,
    * you can supply NULL for the last three parameters.  We also check that
    * the library version is compatible with the one used at compile time,
    * in case we are using dynamically linked libraries.  REQUIRED.
    */
   png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

   if (png_ptr == NULL)
   {
      return -1;
   }

   /* Allocate/initialize the image information data.  REQUIRED */
   info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL)
   {
      png_destroy_write_struct(&png_ptr,  png_infopp_NULL);
      return -1;
   }

   /* Set error handling.  REQUIRED if you aren't supplying your own
    * error handling functions in the png_create_write_struct() call.
    */
   if (setjmp(png_jmpbuf(png_ptr)))
   {
      /* If we get here, we had a problem reading the file */
      png_destroy_write_struct(&png_ptr, &info_ptr);
      return -1;
   }

   /* One of the following I/O initialization functions is REQUIRED */
#ifdef streams /* I/O initialization method 1 */
   /* set up the output control if you are using standard C streams */
   png_init_io(png_ptr, fp);
#else /* I/O initialization method 2 */
   /* If you are using replacement read functions, instead of calling
    * png_init_io() here you would call */
   png_set_write_fn(png_ptr, (void *)cxt, png_write_cb, NULL);
   /* where user_io_ptr is a structure you want available to the callbacks */
#endif /* only use one initialization method */

#ifdef hilevel
   /* This is the easy way.  Use it if you already have all the
    * image info living info in the structure.  You could "|" many
    * PNG_TRANSFORM flags into the png_transforms integer here.
    */
   png_write_png(png_ptr, info_ptr, png_transforms, png_voidp_NULL);
#else
   /* This is the hard way */

   /* Set the image information here.  Width and height are up to 2^31,
    * bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
    * the color_type selected. color_type is one of PNG_COLOR_TYPE_GRAY,
    * PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
    * or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
    * PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
    * currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE. REQUIRED
    */
   if (drop_alpha)
   {
       png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB,
          PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
   }
   else
   {
       png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGBA,
          PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
   }
   /* optional significant bit chunk */
   /* if we are dealing with a grayscale image then */
   //sig_bit.gray = true_bit_depth;
   /* otherwise, if we are dealing with a color image then */
   png_color_8 sig_bit;
   sig_bit.gray = 0;
   sig_bit.red = 8;
   sig_bit.green = 8;
   sig_bit.blue = 8;
   /* if the image has an alpha channel then */
   sig_bit.alpha = 8;
   png_set_sBIT(png_ptr, info_ptr, &sig_bit);


   png_set_compression_level(png_ptr, 6);
   /* Optional gamma chunk is strongly suggested if you have any guess
    * as to the correct gamma of the image.
    */
   //TODO: enable this once we allow user setting of gamma
   //png_set_gAMA(png_ptr, info_ptr, gamma);

   /* Optionally write comments into the image */
   /*
   png_text text_ptr[3];
   text_ptr[0].key = "";
   text_ptr[0].text = "";
   text_ptr[0].compression = PNG_TEXT_COMPRESSION_NONE;
   text_ptr[1].key = "";
   text_ptr[1].text = "";
   text_ptr[1].compression = PNG_TEXT_COMPRESSION_NONE;
   text_ptr[2].key = "";
   text_ptr[2].text = "";
   text_ptr[2].compression = PNG_TEXT_COMPRESSION_zTXt;
#ifdef PNG_iTXt_SUPPORTED
   text_ptr[0].lang = NULL;
   text_ptr[1].lang = NULL;
   text_ptr[2].lang = NULL;
#endif
   png_set_text(png_ptr, info_ptr, text_ptr, 3);
   */

   /* other optional chunks like cHRM, bKGD, tRNS, tIME, oFFs, pHYs, */
   /* note that if sRGB is present the gAMA and cHRM chunks must be ignored
    * on read and must be written in accordance with the sRGB profile */

   /* Write the file header information.  REQUIRED */
   png_write_info(png_ptr, info_ptr);

   /* If you want, you can write the info in two steps, in case you need to
    * write your private chunk ahead of PLTE:
    *
    *   png_write_info_before_PLTE(write_ptr, write_info_ptr);
    *   write_my_chunk();
    *   png_write_info(png_ptr, info_ptr);
    *
    * However, given the level of known- and unknown-chunk support in 1.1.0
    * and up, this should no longer be necessary.
    */

   /* Once we write out the header, the compression type on the text
    * chunks gets changed to PNG_TEXT_COMPRESSION_NONE_WR or
    * PNG_TEXT_COMPRESSION_zTXt_WR, so it doesn't get written out again
    * at the end.
    */

   /* set up the transformations you want.  Note that these are
    * all optional.  Only call them if you want them.
    */

   /* invert monochrome pixels */
   //png_set_invert_mono(png_ptr);

   /* Shift the pixels up to a legal bit depth and fill in
    * as appropriate to correctly scale the image.
    */
   //png_set_shift(png_ptr, &sig_bit);

   /* pack pixels into bytes */
   //png_set_packing(png_ptr);

   /* swap location of alpha bytes from ARGB to RGBA */
   //png_set_swap_alpha(png_ptr);

   /* Get rid of filler (OR ALPHA) bytes, pack XRGB/RGBX/ARGB/RGBA into
    * RGB (4 channels -> 3 channels). The second parameter is not used.
    */
   //png_set_filler(png_ptr, 0, PNG_FILLER_BEFORE);

   /* flip BGR pixels to RGB */
   //png_set_bgr(png_ptr);

   /* swap bytes of 16-bit files to most significant byte first */
   //png_set_swap(png_ptr);

   /* swap bits of 1, 2, 4 bit packed pixel formats */
   //png_set_packswap(png_ptr);

   int number_passes;
   /* turn on interlace handling if you are not using png_write_image() */
   //if (interlacing)
   //   number_passes = png_set_interlace_handling(png_ptr);
   //else
      number_passes = 1;

   /* The easiest way to write the image (you may have a different memory
    * layout, however, so choose what fits your needs best).  You need to
    * use the first method if you aren't handling interlacing yourself.
    */
   png_bytep* row_pointers = (png_bytep*)alloca(height * sizeof(png_bytep));

   if (height > PNG_UINT_32_MAX/png_sizeof(png_bytep))
     png_error (png_ptr, "Image is too tall to process in memory");

   for (int k = 0; k < height; k++)
     row_pointers[k] = (png_bytep)(pix + k*width);

   /* One of the following output methods is REQUIRED */
#ifdef entire /* write out the entire image data in one call */
   png_write_image(png_ptr, row_pointers);

   /* the other way to write the image - deal with interlacing */

#else  /* write out the image data by one or more scanlines */
   /* The number of passes is either 1 for non-interlaced images,
    * or 7 for interlaced images.
    */
   for (int pass = 0; pass < number_passes; pass++)
   {
       if (!drop_alpha)
       {
           /* Write a few rows at a time. */
           png_write_rows(png_ptr, &row_pointers[0], height);
       }

       else
       {
           unsigned char* rgb = (unsigned char*)alloca(width * 3);

           /* If you are only writing one row at a time, this works */
           for (int y = 0; y < height; y++)
           {
               //strip alpha from the row if needed (ARGB->RGB)
               unsigned char* src = row_pointers[y];
               unsigned char* dst = rgb;
               unsigned char* end = src + width * 4;

               while (src != end)
               {
                   *dst++ = *src++;
                   *dst++ = *src++;
                   *dst++ = *src++;
                   src++;
               }

               //write the RGB
               png_write_rows(png_ptr, &rgb, 1);
           }

       }
   }
#endif  /* use only one output method */

   /* You can write optional chunks like tEXt, zTXt, and tIME at the end
    * as well.  Shouldn't be necessary in 1.1.0 and up as all the public
    * chunks are supported and you can use png_set_unknown_chunks() to
    * register unknown chunks into the info structure to be written out.
    */

   /* It is REQUIRED to call this to finish writing the rest of the file */
   png_write_end(png_ptr, info_ptr);
#endif //hilevel

   /* If you png_malloced a palette, free it here (don't free info_ptr->palette,
      as recommended in versions 1.0.5m and earlier of this example; if
      libpng mallocs info_ptr->palette, libpng will free it).  If you
      allocated it with malloc() instead of png_malloc(), use free() instead
      of png_free(). */
   //png_free(png_ptr, palette);


   /* Similarly, if you png_malloced any data that you passed in with
      png_set_something(), such as a hist or trans array, free it here,
      when you can be sure that libpng is through with it. */
//   png_free(png_ptr, trans);
//   trans=NULL;

   /* clean up after the write, and free any memory allocated */
   png_destroy_write_struct(&png_ptr, &info_ptr);

   /* that's it */
   return 0;
}

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

/* Read a PNG file.  You may want to return an error code if the read
 * fails (depending upon the failure).  There are two "prototypes" given
 * here - one where we are given the filename, and we need to open the
 * file, and the other where we are given an open file (possibly with
 * some or all of the magic bytes read - see comments above).
 *
 * NOTE: the returned buffer contains the *inverted* image
 */
int read_png(png_write_context* cxt, int& retwidth, int& retheight, unsigned char*& data)  /* file is already open */
{
   png_structp png_ptr;
   png_infop info_ptr;
   png_uint_32 width, height;
   int bit_depth, color_type, interlace_type;

   /* Create and initialize the png_struct with the desired error handler
    * functions.  If you want to use the default stderr and longjump method,
    * you can supply NULL for the last three parameters.  We also supply the
    * the compiler header file version, so that we know if the application
    * was compiled with a compatible version of the library.  REQUIRED
    */
   png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

   if (png_ptr == NULL)
   {
      return -1;
   }

   /* Allocate/initialize the memory for image information.  REQUIRED. */
   info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL)
   {
      png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
      return -1;
   }

   /* Set error handling if you are using the setjmp/longjmp method (this is
    * the normal method of doing things with libpng).  REQUIRED unless you
    * set up your own error handlers in the png_create_read_struct() earlier.
    */

   if (setjmp(png_jmpbuf(png_ptr)))
   {
      /* Free all of the memory associated with the png_ptr and info_ptr */
      png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
      /* If we get here, we had a problem reading the file */
      return -1;
   }

   /* If you are using replacement read functions, instead of calling
    * png_init_io() here you would call:
    */
   png_set_read_fn(png_ptr, (void *)cxt, png_read_cb);
   /* where user_io_ptr is a structure you want available to the callbacks */

   /* If we have already read some of the signature */
   //png_set_sig_bytes(png_ptr, sig_read);

#ifdef hilevel
   /*
    * If you have enough memory to read in the entire image at once,
    * and you need to specify only transforms that can be controlled
    * with one of the PNG_TRANSFORM_* bits (this presently excludes
    * dithering, filling, setting background, and doing gamma
    * adjustment), then you can read the entire image (including
    * pixels) into the info structure with this call:
    */
   png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_BGR | PNG_TRANSFORM_EXPAND, png_voidp_NULL);
#else

   /* OK, you're doing it the hard way, with the lower-level functions */

   /* The call to png_read_info() gives us all of the information from the
    * PNG file before the first IDAT (image data chunk).  REQUIRED
    */
   png_read_info(png_ptr, info_ptr);

   png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
       &interlace_type, int_p_NULL, int_p_NULL);

 /*
 * Set up the data transformations you want.  Note that these are all
 * optional.  Only call them if you want/need them.  Many of the
 * transformations only work on specific types of images, and many
 * are mutually exclusive.
 */

   /* tell libpng to strip 16 bit/color files down to 8 bits/color */
   png_set_strip_16(png_ptr);

   /* Strip alpha bytes from the input data without combining with the
    * background (not recommended).
    */
   //png_set_strip_alpha(png_ptr);

   /* Extract multiple pixels with bit depths of 1, 2, and 4 from a single
    * byte into separate bytes (useful for paletted and grayscale images).
    */
   png_set_packing(png_ptr);

   /* Change the order of packed pixels to least significant bit first
    * (not useful if you are using png_set_packing). */
   //png_set_packswap(png_ptr);

   /* Expand paletted colors into true RGB triplets */
   if (color_type == PNG_COLOR_TYPE_PALETTE)
       png_set_palette_to_rgb(png_ptr);

   /* Expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel */
   if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
      png_set_gray_1_2_4_to_8(png_ptr);

   /* Expand paletted or RGB images with transparency to full alpha channels
    * so the data will be available as RGBA quartets.
    */
   if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
      png_set_tRNS_to_alpha(png_ptr);

   /* Set the background color to draw transparent and alpha images over.
    * It is possible to set the red, green, and blue components directly
    * for paletted images instead of supplying a palette index.  Note that
    * even if the PNG file supplies a background, you are not required to
    * use it - you should use the (solid) application background if it has one.
    */
   /*
   png_color_16 my_background, *image_background;

   if (png_get_bKGD(png_ptr, info_ptr, &image_background))
      png_set_background(png_ptr, image_background,
                         PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
   else
      png_set_background(png_ptr, &my_background,
                         PNG_BACKGROUND_GAMMA_SCREEN, 0, 1.0);
                         */

   /* Some suggestions as to how to get a screen gamma value */

   /* Note that screen gamma is the display_exponent, which includes
    * the CRT_exponent and any correction for viewing conditions */

   //if (/* We have a user-defined screen gamma value */)
   //{
   //   screen_gamma = user-defined screen_gamma;
   //}
   ///* This is one way that applications share the same screen gamma value */
   //else if ((gamma_str = getenv("SCREEN_GAMMA")) != NULL)
   //{
   //   screen_gamma = atof(gamma_str);
   //}
   ///* If we don't have another value */
   //else
   //{
   //   screen_gamma = 2.2;  /* A good guess for a PC monitors in a dimly
   //                           lit room */
   //   screen_gamma = 1.7 or 1.0;  /* A good guess for Mac systems */
   //}

   /* Tell libpng to handle the gamma conversion for you.  The final call
    * is a good guess for PC generated images, but it should be configurable
    * by the user at run time by the user.  It is strongly suggested that
    * your application support gamma correction.
    */
   //int intent;

   //if (png_get_sRGB(png_ptr, info_ptr, &intent))
   //   png_set_gamma(png_ptr, screen_gamma, 0.45455);
   //else
   //{
   //   double image_gamma;
   //   if (png_get_gAMA(png_ptr, info_ptr, &image_gamma))
   //      png_set_gamma(png_ptr, screen_gamma, image_gamma);
   //   else
   //      png_set_gamma(png_ptr, screen_gamma, 0.45455);
   //}

   ///* Dither RGB files down to 8 bit palette or reduce palettes
   // * to the number of colors available on your screen.
   // */
   //if (color_type & PNG_COLOR_MASK_COLOR)
   //{
   //   int num_palette;
   //   png_colorp palette;

   //   /* This reduces the image to the application supplied palette */
   //   if (/* we have our own palette */)
   //   {
   //      /* An array of colors to which the image should be dithered */
   //      png_color std_color_cube[MAX_SCREEN_COLORS];

   //      png_set_dither(png_ptr, std_color_cube, MAX_SCREEN_COLORS,
   //         MAX_SCREEN_COLORS, png_uint_16p_NULL, 0);
   //   }
   //   /* This reduces the image to the palette supplied in the file */
   //   else if (png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette))
   //   {
   //      png_uint_16p histogram = NULL;

   //      png_get_hIST(png_ptr, info_ptr, &histogram);

   //      png_set_dither(png_ptr, palette, num_palette,
   //                     max_screen_colors, histogram, 0);
   //   }
   //}

   /* invert monochrome files to have 0 as white and 1 as black */
   //png_set_invert_mono(png_ptr);

   /* If you want to shift the pixel values from the range [0,255] or
    * [0,65535] to the original [0,7] or [0,31], or whatever range the
    * colors were originally in:
    */
   //if (png_get_valid(png_ptr, info_ptr, PNG_INFO_sBIT))
   //{
   //   png_color_8p sig_bit;

   //   png_get_sBIT(png_ptr, info_ptr, &sig_bit);
   //   png_set_shift(png_ptr, sig_bit);
   //}

   /* flip the RGB pixels to BGR (or RGBA to BGRA) */
   if (color_type & PNG_COLOR_MASK_COLOR)
      png_set_bgr(png_ptr);

   /* swap the RGBA or GA data to ARGB or AG (or BGRA to ABGR) */
   //png_set_swap_alpha(png_ptr);

   /* swap bytes of 16 bit files to least significant byte first */
   //png_set_swap(png_ptr);

   /* Add filler (or alpha) byte (before/after each RGB triplet) */
   png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);

   /* Turn on interlace handling.  REQUIRED if you are not using
    * png_read_image().  To see how to handle interlacing passes,
    * see the png_read_row() method below:
    */
   int number_passes = png_set_interlace_handling(png_ptr);
   (void)number_passes;

   /* Optional call to gamma correct and add the background to the palette
    * and update info structure.  REQUIRED if you are expecting libpng to
    * update the palette for you (ie you selected such a transform above).
    */
   png_read_update_info(png_ptr, info_ptr);

   /* Allocate the memory to hold the image using the fields of info_ptr. */

   /* The easiest way to read the image: */
   retwidth = width;
   retheight = height;
   png_bytep* row_pointers = (png_bytep*)alloca(sizeof(png_bytep) *height);
   data = new unsigned char[height * png_get_rowbytes(png_ptr, info_ptr)];

   for (unsigned row = 0; row < height; row++)
   {
      // configure the row pointers so that the returned buffer contains the inverted image
      row_pointers[row] = (png_bytep)&data[(height - row - 1) * png_get_rowbytes(png_ptr, info_ptr)];
   }

   /* Now it's time to read the image.  One of these methods is REQUIRED */
//#ifdef entire /* Read the entire image in one go */
   png_read_image(png_ptr, row_pointers);

//#else no_entire /* Read the image one or more scanlines at a time */
//   /* The other way to read images - deal with interlacing: */
//
//   for (pass = 0; pass < number_passes; pass++)
//   {
//#ifdef single /* Read the image a single row at a time */
//      for (y = 0; y < height; y++)
//      {
//         png_read_rows(png_ptr, &row_pointers[y], png_bytepp_NULL, 1);
//      }
//
//#else no_single /* Read the image several rows at a time */
//      for (y = 0; y < height; y += number_of_rows)
//      {
//#ifdef sparkle /* Read the image using the "sparkle" effect. */
//         png_read_rows(png_ptr, &row_pointers[y], png_bytepp_NULL,
//            number_of_rows);
//#else no_sparkle /* Read the image using the "rectangle" effect */
//         png_read_rows(png_ptr, png_bytepp_NULL, &row_pointers[y],
//            number_of_rows);
//#endif no_sparkle /* use only one of these two methods */
//      }
//
//      /* if you want to display the image after every pass, do
//         so here */
//#endif no_single /* use only one of these two methods */
//   }
//#endif no_entire /* use only one of these two methods */

   /* read rest of file, and get additional chunks in info_ptr - REQUIRED */
   png_read_end(png_ptr, info_ptr);

#endif //hilevel

   /* At this point you have read the entire image */

   /* clean up after the read, and free any memory allocated - REQUIRED */
   png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);

   /* that's it */
   return 0;
}

/*
 * sort a given list of rgba entries so that all the opaque pixels are at the end
 */
int remapPaletteForPNG(unsigned char *apixels,int height,int width,rgbaPixel *palette,int palette_num_entries,unsigned int palette_scaling_maxval, rgbPixel *rgb, unsigned char *a, int *num_a) {
    int bot_idx, top_idx, x;
    int remap[256];
    unsigned int maxval = palette_scaling_maxval; 


    /*
     ** remap the palette colors so that all entries with
     ** the maximal alpha value (i.e., fully opaque) are at the end and can
     ** therefore be omitted from the tRNS chunk.  Note that the ordering of
     ** opaque entries is reversed from how they were previously arranged
     ** --not that this should matter to anyone.
     */

    for (top_idx = palette_num_entries-1, bot_idx = x = 0;  x < palette_num_entries;  ++x) {
        if (palette[x].a == maxval)
            remap[x] = top_idx--;
        else
            remap[x] = bot_idx++;
    }
    /* sanity check:  top and bottom indices should have just crossed paths */
    if (bot_idx != top_idx + 1) {
        //msSetError(MS_MISCERR,"quantization sanity check failed","createPNGPalette()");
        return MS_FAILURE;
    }

    *num_a = bot_idx;

    for(x=0;x<width*height;x++)
        apixels[x] = remap[apixels[x]];


    for (x = 0; x < palette_num_entries; ++x) {
        if (maxval == 255) {
            a[remap[x]] = palette[x].a;
            rgb[remap[x]].r = palette[x].r;
            rgb[remap[x]].g = palette[x].g;
            rgb[remap[x]].b = palette[x].b;
        } else {
            /* rescale palette */
            rgb[remap[x]].r = (palette[x].r * 255 + (maxval >> 1)) / maxval;
            rgb[remap[x]].g = (palette[x].g * 255 + (maxval >> 1)) / maxval;
            rgb[remap[x]].b = (palette[x].b * 255 + (maxval >> 1)) / maxval;
            a[remap[x]] = (palette[x].a * 255 + (maxval >> 1)) / maxval;
        }
        if (a[remap[x]] != 255) {
            /* un-premultiply pixels */
            double da = 255.0/a[remap[x]];
            rgb[remap[x]].r *=  da;
            rgb[remap[x]].g *=  da;
            rgb[remap[x]].b *=  da;
        }
    }

    return MS_SUCCESS;
}

int savePalettePNG(unsigned char *apixels,int height,int width,rgbaPixel *palette,int palette_num_entries,unsigned int palette_scaling_maxval, png_write_context* cxt, int compression) 
{
    png_infop info_ptr;
    rgbPixel rgb[256];
    unsigned char a[256];
    int num_a;
    int row,sample_depth;
    png_structp png_ptr = png_create_write_struct(
        PNG_LIBPNG_VER_STRING, NULL,NULL,NULL);
    
    if (!png_ptr)
        return (MS_FAILURE);

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_write_struct(&png_ptr,
            (png_infopp)NULL);
        return (MS_FAILURE);
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return (MS_FAILURE);
    }

    png_set_write_fn(png_ptr, (void *)cxt, png_write_cb, NULL);

    png_set_compression_level(png_ptr, compression);

    if (palette_num_entries <= 2)
        sample_depth = 1;
    else if (palette_num_entries <= 4)
        sample_depth = 2;
    else if (palette_num_entries <= 16)
        sample_depth = 4;
    else
        sample_depth = 8;
   
    png_set_IHDR(png_ptr, info_ptr, width, height,
                sample_depth, PNG_COLOR_TYPE_PALETTE,
                0, PNG_COMPRESSION_TYPE_DEFAULT,
                PNG_FILTER_TYPE_DEFAULT);
   
    remapPaletteForPNG(apixels,height,width,palette,palette_num_entries,palette_scaling_maxval,rgb,a,&num_a);

    png_set_PLTE(png_ptr, info_ptr, (png_colorp)(rgb),palette_num_entries);
    if(num_a)
        png_set_tRNS(png_ptr, info_ptr, a,num_a, NULL);

    png_write_info(png_ptr, info_ptr);
    png_set_packing(png_ptr);

    for(row=0;row<height;row++) {
        unsigned char *rowptr = &(apixels[row*width]);
        png_write_row(png_ptr, rowptr);
    }
    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return MS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************
 * mapgd.c 7803 2008-07-09 05:17:40Z sdlime
 * Project:  MapServer
 * Purpose:  GD rendering and other GD related functions.
 * Author:   Steve Lime and the MapServer team.
 ******************************************************************************/
/// copied from mapserver source tree --------> mapgd.c line 3248 - 3463, UV  25.02.2009
 // copy src to dst using dst's palette
 // src must be a truecolor image
 // dst must be a paletted image
 // method is to fine tune the caching used to lookup color values in the palette:
 //  -0 is the default method, which allocates cache memory when needed
 //  -1 is a memory conservative method, that uses very little caching but is much slower
 //  -2 is a memory hungry caching method (allocates 32MB on the heap) but is the fastest for large images
 // see bug #2422 for some benchmark timings of these methods
 //
static int ImageCopyForcePaletteGD(gdImagePtr src, gdImagePtr dst, int method)
{
#ifdef _DEBUG_PNG8
    long icfp = GetTickCount();  // for performance evaluation
#endif
    int x, y;
    int w, h;
    int c, r, g, b,color;
    if(!src || !dst) return MS_FAILURE;
    // input images have different sizes
    if(gdImageSX(src) != gdImageSX(dst) || gdImageSY(src) != gdImageSY(dst)) return MS_FAILURE;
    if(!gdImageTrueColor(src) || gdImageTrueColor(dst)) return MS_FAILURE; /* 24-bit to 8-bit */
    w = gdImageSX(src);
    h = gdImageSY(src);
    if(method==0) {
        /*default caching strategy*/
        unsigned short ***cols=(unsigned short***)calloc(256,sizeof(unsigned short**));

        /*pointer to cache, pointed by red component, indexed by green component*/
        unsigned short **data=(unsigned short**)calloc(256*256,sizeof(unsigned short*));

       for(r=0;r<256;r++) {
        /*populate the cache*/
        cols[r]=&(data[r*256]);

        for (y = 0; (y < h); y++) {
            for (x = 0; (x < w); x++) {
                c = gdImageGetPixel(src, x, y);
                r = gdTrueColorGetRed(c);
                g = gdTrueColorGetGreen(c);
                b = gdTrueColorGetBlue(c);

                if(cols[r][g]==NULL) {
                    /*this is the first time we see this r,g couple.
                     *allocate bytes for the blue component*/
                    cols[r][g]=(unsigned short*)calloc(256,sizeof(unsigned short));
                }

                if(!cols[r][g][b]) {
                    /*this r,g,b triplet was never seen before
                     * compute its index in the palette and cache the result
                     */
                    color = gdImageColorClosest(dst, r, g, b);
                    dst->pixels[y][x] = color;
                    /*the cache data was calloc'ed to avoid initialization
                     * store 'index+1' so we are sure the test for cols[r][g][b]
                     * returns true, i.e that this color was cached*/
                    cols[r][g][b]=color+1;
                }
                else {
                    /*the cache data was calloc'ed to avoid initialization
                     * the cache contains 'index+1' so we must subtract
                     * 1 to get the real color index*/
                    dst->pixels[y][x] = cols[r][g][b]-1;
                } // if
            } // for x
        } // for y
       } // for r
        for(r=0;r<256;r++)
            for(g=0;g<256;g++)
                if(cols[r][g]) /*only free the row if it was used*/
                    free(cols[r][g]);
        free(data);
        free(cols);
    }
    else if(method==1) {
        /*memory conservative method, does not allocate mem on the heap*/
        int R[10], G[10], B[10], C[10];
        int i, color, nCache = 0, iCache =0, maxCache=10;

        for (y = 0; (y < h); y++) {
            for (x = 0; (x < w); x++) {
                c = gdImageGetPixel(src, x, y);
                r =  gdTrueColorGetRed(c);
                g = gdTrueColorGetGreen(c);
                b = gdTrueColorGetBlue(c);
                color = -1;

                /* adding a simple cache to keep colors instead of always calling gdImageColorClosest
               seems to reduce significantly the time passed in this function
               spcially with large images (bug 2096)*/
                for (i=0; i<nCache; i++)
                {
                    if (R[i] == r)
                    {
                        if (G[i] == g && B[i] == b)
                        {
                            color = C[i];
                            break;
                        }
                    }
                }

                if (color == -1)
                {
                    color = gdImageColorClosest(dst, r, g, b);
                    R[iCache] = r;
                    G[iCache] = g;
                    B[iCache] = b;
                    C[iCache] = color;
                    nCache++;
                    if (nCache >= maxCache)
                        nCache = maxCache;

                    iCache++;
                    if (iCache == maxCache)
                        iCache = 0;
                }

                gdImageSetPixel(dst, x, y, color);
            }
        }
    }
    else if(method==2) {
        /*memory hungry method, fastest for very large images*/

        /*use a cache for every possible r,g,b triplet
         * this is usually a full 32MB (when short is 2 bytes) */
        short *cache=(short*)calloc(16777216,sizeof(short));

        for (y = 0; (y < h); y++) {
            for (x = 0; (x < w); x++) {
                int index;
                c = gdImageGetPixel(src, x, y);
                index=c&0xFFFFFF; /*use r,g,b for the cache index, i.e. remove alpha*/
                if(!cache[index]) {
                    r =  gdTrueColorGetRed(c);
                    g = gdTrueColorGetGreen(c);
                    b = gdTrueColorGetBlue(c);
                    color = gdImageColorClosest(dst, r, g, b);
                    cache[index]=color+1;
                    dst->pixels[y][x] = color;
                }
                else
                    dst->pixels[y][x] = cache[index]-1;
            }
        }
        free(cache);
    }
#ifdef _DEBUG_PNG8
    printf("\n(%d) ########################### ImageCopyForcePaletteGD: Time:%6.4f (s)\n", GetCurrentThreadId(), (GetTickCount()-icfp)/1000.0);
#endif

    if(gdImageTrueColor(dst)) return MS_FAILURE; /* result is not 8-bit */
    return MS_SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////////////
// helper function to create an empty image with a palette provided in std::list<MgColor>
// inParam: 24bit image, filename of palette file, imagesixeX, imagesizeY
// return: 8bit image using given palette if provided ...
static gdImagePtr CreateGdImageWithPalette(gdImagePtr img24, RS_ColorVector* baseColorPalette, int sx, int sy)
{
    myassert(sx > 0 && sy > 0, __LINE__, __WFILE__);

    /// allow empty default parameter
    int ncolors = baseColorPalette? baseColorPalette->size() : 0;

    // create new empty palette image
    gdImagePtr gdPPalette = gdImageCreatePalette(sx, sy);
    myassert(gdPPalette, __LINE__, __WFILE__);

    // if the palette file contains less than 256, fill the rest with colors generated
    // by gd - those can include the base colors
    if (ncolors < 256)
    {
        gdImagePtr gdPPaletteFromImg24 = gdImageCreatePaletteFromTrueColor(img24, 1, 255-ncolors);
        myassert(gdPPaletteFromImg24, __LINE__, __WFILE__);

        // copy colors from palette into new empty gdpalette
        for (int c = 0; c < gdPPaletteFromImg24->colorsTotal; c++)
        {
            gdImageColorAllocate(gdPPalette,gdPPaletteFromImg24->red[c],
                                            gdPPaletteFromImg24->green[c],
                                            gdPPaletteFromImg24->blue[c]);
        }
        gdImageDestroy(gdPPaletteFromImg24);
    }
#ifdef _DEBUG_PNG8
    printf("\n(%d) ########################### CreateGdImageWithPalette colors: %d\n", GetCurrentThreadId(), ncolors);
#endif

    if (baseColorPalette)
    {
        // add colors from the colorpalette (duplicate color entries make the used palette less than 256)
        RS_ColorVector::iterator it;
        for (it = baseColorPalette->begin();it != baseColorPalette->end(); ++it)
        {
            gdImageColorAllocate(gdPPalette, (*it).red(), (*it).green(), (*it).blue());
        }
    }

    myassert(gdImageTrueColor(gdPPalette) == 0, __LINE__, __WFILE__);   // result is not a 8-bit paletted image
    return gdPPalette;  // this is an empty image with the right color palette
}


////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
// create an image using the other Save method and store it at filename
bool AGGImageIO::Save(const RS_String& filename, const RS_String& format,
                 unsigned int* src, int src_width, int src_height,
                 int dst_width, int dst_height, RS_Color& bgColor)
{
    //get the in-memory image stream
    RS_ByteData* data = Save(format, src, src_width, src_height, dst_width, dst_height, bgColor);

    if (data == NULL)
        return false;

    //We write to a file to avoid sending a FILE* across DLL
    //boundary, which is not always safe.

    //TODO: take care of this string conversion -- shouldn't matter
    //since it is only a temp file name anyway, and they are ascii
    char mbfilename[1024];
    wcstombs(mbfilename, filename.c_str(), 1024);

    FILE* out = fopen(mbfilename, "wb");
    _ASSERT(out);

    if (out)
    {
        unsigned char* bytes = data->GetBytes();
        unsigned int numBytes = data->GetNumBytes();
        if (fwrite(bytes, 1, numBytes, out) != numBytes)
            _ASSERT(false);
    }

    fclose(out);

    data->Dispose();

    return true;
}


////////////////////////////////////////////////////////////////////////////////////////
// convert imagebuffer src into the desired format using the provided baseColorPalette for 8bit
// (the baseColorPalette is a default parameter NULL)
RS_ByteData* AGGImageIO::Save(const RS_String& format,
                  unsigned int* src, int src_width, int src_height,
                  int dst_width, int dst_height, RS_Color& bgColor,
                  RS_ColorVector* baseColorPalette)
{
    bool drop_alpha = bgColor.alpha() == 255;

    if (dst_width <= 0)
        dst_width = 1;
    if (dst_height <= 0)
        dst_height = 1;

    //do we need to stretch to non-square pixels?
    if (src_width != dst_width || src_height != dst_height)
    {
        agg_context* aggcxt = new agg_context(NULL, dst_width, dst_height);
        aggcxt->ren.clear(agg::argb8_packed(0));

        //passing in negative height makes sure the image is not inverted
        //by the renderer again -- it already is facing the correct way since we rendered
        //it using the inverting renderer
        AGGRenderer::DrawScreenRaster(aggcxt, (unsigned char*)src, src_width * src_height * 4, RS_ImageFormat_NATIVE, src_width, src_height,
            dst_width * 0.5, dst_height * 0.5, dst_width, -dst_height, 0);

        RS_ByteData* data = Save(format, aggcxt->m_rows, dst_width, dst_height, dst_width, dst_height, bgColor);

        delete aggcxt;

        return data;
    }
    else
    {
        double gamma = 1.0; //TODO: will use this once we allow user setting of gamma for AGG gendering

        

        //NOTE: We do not use gd for reading or writing PNG since internally gd drops a bit
        //from the alpha channel, which is not desirable for high quality output
        if (format == L"PNG")
        {
            UnmultiplyAlpha(src, src_width * src_height);

            png_write_context cxt;
            memset(&cxt, 0, sizeof(cxt));
            write_png(&cxt, src, dst_width, dst_height, gamma, drop_alpha);

            RS_ByteData* byteData = (cxt.used > 0)? new RS_ByteData(cxt.buf, (unsigned int)cxt.used) : NULL;
            delete [] cxt.buf;
            return byteData;
        }
        //NOTE: We do not use gd for reading or writing PNG8 for the same reason as for the PNG
        else if (format == L"PNG8")
        {
            // copy the src data into the rgbaPixel array
            rgbaPixel *apixelsIn=(rgbaPixel*)malloc(dst_height*dst_width*sizeof(rgbaPixel));
            unsigned int* ptr = src;
            for (int i=0; i<dst_height*dst_width; i++)
            {
                unsigned int c = *ptr++;
                rgbaPixel *pixel = &(apixelsIn[i]);
                pixel->a = c >> 24;
                pixel->b = (c >> 16) & 0xff;  // some simple optimization ;-)
                pixel->g = (c >> 8) & 0xff;
                pixel->r = c & 0xff;
            }
            int ncolors = baseColorPalette ? baseColorPalette->size() : 0;
            //jng: Incorporate the colors that have been handed down to us into the final palette
            bool bWithinPalette = (ncolors < 256 && ncolors > 0);
            unsigned int reqcolor = (bWithinPalette) ? (256 - ncolors) : 256;
            unsigned int palette_scaling_maxval = 255;
            rgbaPixel *palette=(rgbaPixel*)malloc(256*sizeof(rgbaPixel));
            msQuantizeRasterBuffer(apixelsIn,dst_height,dst_width, &reqcolor, palette, &palette_scaling_maxval);
            
            if (bWithinPalette)
            {
                //A [256 - #colors] color palette has been generated. So fill in the rest with the colors
                //handed down from the baseColorPalette
                unsigned int added = 0;
                for (int i = reqcolor; i < 256; i++)
                {
                    //Requested Colors + baseColorPalette size may not actually reach 256, so guard
                    //against overrun
                    if ((i - reqcolor) >= baseColorPalette->size())
                        break;

                    RS_Color c = baseColorPalette->at(i - reqcolor);
                    //bscott: If the palette_scaling_maxval is lower than 255 we have to scale down all the map palette 
                    //colors so the resulting png palette will be remap correctly.
                    palette[i].a = (c.alpha() * (palette_scaling_maxval) + (255) / 2 ) / (255); 
                    palette[i].r = (c.red() * (palette_scaling_maxval) + (255) / 2 ) / (255); 
                    palette[i].g = (c.green() * (palette_scaling_maxval) + (255) / 2 ) / (255); 
                    palette[i].b = (c.blue() * (palette_scaling_maxval) + (255) / 2 ) / (255); 
                    added++;
                }
                reqcolor += added;
            }

            unsigned char *apixelsOut=(unsigned char *)malloc(dst_height*dst_width);
            msClassifyRasterBuffer(apixelsIn,dst_height,dst_width, palette,reqcolor,apixelsOut) ;

            png_write_context cxt;
            memset(&cxt, 0, sizeof(cxt));
            savePalettePNG(apixelsOut,dst_height,dst_width,palette,reqcolor,palette_scaling_maxval,&cxt,-1);
            RS_ByteData* byteData = (cxt.used > 0)? new RS_ByteData(cxt.buf, (unsigned int)cxt.used) : NULL;
            delete [] cxt.buf;

            free(apixelsIn);
            free(palette);
            free(apixelsOut);

            return byteData;
        }
        else if (format == L"JPG" || format == L"GIF")
        {
            UnmultiplyAlpha(src, src_width * src_height);
            // gdimg24 contains the 24bit image
            gdImagePtr gdimg24 = gdImageCreateTrueColor(dst_width, dst_height);

            int bgc = ConvertColor(gdimg24, bgColor);

            // initialize the destination image to the bg color (temporarily turn
            // off alpha blending so the fill has the supplied alpha)
            gdImageAlphaBlending(gdimg24, 0);
            gdImageFilledRectangle(gdimg24, 0, 0, gdImageSX(gdimg24)-1, gdImageSY(gdimg24)-1, bgc);

            // set any transparent color
            if (bgColor.alpha() != 255)
                gdImageColorTransparent(gdimg24, bgc);

            // copy the src data into the gdImage
            unsigned int* ptr = src;
            for (int j=0; j<dst_height; j++)
            {
                for(int i=0; i<dst_width; i++)
                {
                    //TODO: can be optimized
                    unsigned int c = *ptr++;
                    int a = c >> 24;
                    // skip any fully transparent pixels so a transparent
                    // background color will show through
                    if (a != 0)
                    {
                        int b = (c >> 16) & 0xff;  // some simple optimization ;-)
                        int g = (c >> 8) & 0xff;
                        int r = c & 0xff;

                        int gdc = gdImageColorAllocateAlpha(gdimg24, r, g, b, a);
                        gdImageSetPixel(gdimg24, i, j, gdc);
                    }
                }
            }

            gdImageAlphaBlending(gdimg24, 1);

            // Make output image non-interlaced --- it's a little faster to compress that way.
            gdImageInterlace(gdimg24, 0);

            // Make sure the alpha values get saved -- but only if required
            // it is faster not to save them and makes a smaller PNG
            gdImageSaveAlpha(gdimg24, (bgColor.alpha() != 255)? 1 : 0);

            // convert to 256 color paletted image for PNG8, GIF
            gdImagePtr gdImgPalette = NULL;
            if (format == L"GIF")
            {
                /// skip color quantization if no palette given or empty
                if (baseColorPalette && !baseColorPalette->empty() && s_bUseColorMap) // memory based switch
                {
                    gdImgPalette = CreateGdImageWithPalette(gdimg24, baseColorPalette,
                                                            gdImageSX(gdimg24), gdImageSY(gdimg24));
                    myassert(gdImgPalette, __LINE__, __WFILE__);

                    // methods are described above - we use method 1 as default????? TODO what's best???
                    myassert(ImageCopyForcePaletteGD(gdimg24, gdImgPalette, 1) == MS_SUCCESS, __LINE__, __WFILE__);

                    // forced palette GD image now in gdimgPalette
                }
                else
                {
                    gdImageTrueColorToPalette(gdimg24, 0, 256);  // in place conversion
                    gdImgPalette = gdimg24;
                }
            }

            //get an in-memory image stream
            int size = 0;
            unsigned char* data = NULL;

            if (format == L"GIF")       // MgImageFormats::Gif
                data = (unsigned char*)gdImageGifPtr(gdImgPalette, &size);
            else if (format == L"JPG")  // MgImageFormats::Jpeg
                data = (unsigned char*)gdImageJpegPtr(gdimg24, &size, 75);

            std::auto_ptr<RS_ByteData> byteData;
            byteData.reset((NULL == data)? NULL : new RS_ByteData(data, size));

            gdFree(data);

            if (gdimg24 == gdImgPalette)
                gdimg24 = NULL;     // reset pointer so destructor is not called twice -> exception!!!
            else    //if we allocated a temporary image to stretch-blit, destroy it
                gdImageDestroy(gdimg24);
            //if we allocated a paletted image, destroy it (very likely that is)
            if(NULL != gdImgPalette)
                gdImageDestroy(gdImgPalette);

            return byteData.release();
        }
    }

    // shouldn't get here
    _ASSERT(false);
    return NULL;
}

//---------------------------------------------------------------------------

//TODO: This routine should be rewritten to use agg to blend PNGs more accurately
void AGGImageIO::Combine(const RS_String& src1, const RS_String& src2, const RS_String& dst)
{
    char mbfileIn1[1024];
    char mbfileIn2[1024];
    char mbfileOut[1024];
    wcstombs(mbfileIn1, src1.c_str(), 1024);
    wcstombs(mbfileIn2, src2.c_str(), 1024);
    wcstombs(mbfileOut, dst.c_str(), 1024);

    // read in the images
    FILE* fIn1 = fopen(mbfileIn1, "rb");
    FILE* fIn2 = fopen(mbfileIn2, "rb");

    gdImagePtr img1 = gdImageCreateFromPng(fIn1);
    gdImagePtr img2 = gdImageCreateFromPng(fIn2);

    fclose(fIn1);
    fclose(fIn2);

    // composite the 2nd image onto the 1st
    gdImageSetTile(img1, img2);
    gdImageFilledRectangle(img1, 0, 0, gdImageSX(img1)-1, gdImageSY(img1)-1, gdTiled);
    gdImageSaveAlpha(img1, 1);

    // save the merged image
    FILE* out = fopen(mbfileOut, "wb");
    gdImagePng(img1, out);
    fclose(out);

    // clean up
    gdImageDestroy(img1);
    gdImageDestroy(img2);
}


// NOTE: the returned buffer contains the *inverted* image
unsigned int* AGGImageIO::DecodePNG(const unsigned char* src, size_t len, int& width, int& height)
{
    png_write_context cxt;
    cxt.buf = (unsigned char*)src;
    cxt.len = len;
    cxt.used = 0;

    unsigned char* data = NULL;

    read_png(&cxt, width, height, data);

    return (unsigned int*)data;
}

unsigned int* AGGImageIO::DecodeJPEG(const unsigned char* src, size_t len, int& width, int& height)
{
    //this logic is inefficient due to allocating twice the space
    //needed for the image and copying from gd pixel format to ARGB.
    //We assume that only small images will
    //be handled, for example in symbol definitions and the like.

    gdImagePtr im = gdImageCreateFromJpegPtr((int)len, (void*)src);

    if (!im)
    {
        width = 0;
        height = 0;
        return NULL;
    }

    width = gdImageSX(im);
    height = gdImageSY(im);

    unsigned int* ret = new unsigned int[width * height];
    unsigned int* ptr = ret;

    for (int j=0; j<height; j++)
    {
        for (int i=0; i<width; i++)
        {
            int gdc = gdImageGetPixel(im, i, j);
            unsigned int a = gdImageAlpha(im, gdc);
            unsigned int r = gdImageRed(im, gdc);
            unsigned int g = gdImageGreen(im, gdc);
            unsigned int b = gdImageBlue(im, gdc);
            *ptr++ = (a << 24) | (r << 16) | (g << 8) | b;
        }
    }

    return ret;
}


void AGGImageIO::UnmultiplyAlpha(unsigned int* argb, int len)
{
    unsigned int* end = argb + len;

    do
    {
        unsigned c = *argb;
        unsigned a = c >> 24;

        if (a == 0)
        {
            *argb = 0;
        }
        else if (a != 255)
        {
            unsigned r = (c >> 16) & 0xff;
            unsigned g = (c >> 8) & 0xff;
            unsigned b = c & 0xff;

            r = 0xFF & (r * 255 / a);
            g = 0xFF & (g * 255 / a);
            b = 0xFF & (b * 255 / a);

            *argb = (a << 24) | (r << 16) | (g << 8) | b;
        }

        argb++;
    }
    while (argb != end);
}
