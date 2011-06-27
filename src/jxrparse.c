/*
 * jxrparse.c
 *
 * Copyright (C) 2011, Chris Harding.
 * 
 * This file contains algorithms used in the parsing stages of JPEG XR
 * decoding.
 */

#define JPEG_INTERNALS
#include "jinclude.h"
#include "jpegxrlib.h"

/*
 * Parse image/image plane headers and read the tile index table.
 *  
 */          
GLOBAL(int)
jpegxr_parse_image_tile_layer (j_decompress_ptr cinfo, boolean require_image)
{
  
  
  /* TODO return correct code */
  return JPEG_REACHED_SOS;
}
