/*
 * jxrdcodetile.c
 *
 * Copyright (C) 2011, Chris Harding.
 * Copyright (C) 1994-1998, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 * 
 * This file contains algorithms used to parse and decode the tile layer
 * of JPEG-XR coded image. We performing parsing and decoding here at the
 * same time, though they are distinct in the specification. This is to
 * permit a single pass implementation.
 */

#define JPEG_INTERNALS
#include "jinclude.h"
#include "jpegxrlib.h"

/* Data read macros */
#ifndef JXR_DATARD_INCLUDED
#include "jxrdatard.h"
#endif

/*
 * Parse and decode a spatially encoded tile.
 *
 */
GLOBAL(void)
jpegxr_decode_tile_spatial (j_image_ptr iinfo, unsigned int index)
{
  
  
  

}

/*
 * Parse and decode a frequency encoded tile.
 *
 */
GLOBAL(void)
jpegxr_decode_tile_frequency (j_image_ptr iinfo, unsigned int index)
{
  
  
  

}
