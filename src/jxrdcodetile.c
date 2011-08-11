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
jpegxr_decode_tile_spatial (j_image_ptr iinfo)
{
  
  INPUT_VARS(iinfo);
    
  printf("Decoding spatial tile at offset 0x%x\n", idx);
  
  

}

/*
 * Parse and decode the DC component of a coded tile.
 *
 */
GLOBAL(void)
jpegxr_decode_tile_dc (j_image_ptr iinfo)
{
  
  INPUT_VARS(iinfo);
    
  printf("Decoding tile DC at offset 0x%x\n", idx);
  

}

/*
 * Parse and decode the low pass component of a coded tile.
 *
 */
GLOBAL(void)
jpegxr_decode_tile_lp (j_image_ptr iinfo)
{
  
  INPUT_VARS(iinfo);
    
  printf("Decoding tile LP at offset 0x%x\n", idx);

}

/*
 * Parse and decode the high pass component of a coded tile.
 *
 */
GLOBAL(void)
jpegxr_decode_tile_hp (j_image_ptr iinfo)
{
  
  INPUT_VARS(iinfo);
    
  printf("Decoding tile HP at offset 0x%x\n", idx);

  
}


/*
 * Parse and decode the flexbits of a coded tile.
 *
 */
GLOBAL(void)
jpegxr_decode_tile_flexbits (j_image_ptr iinfo)
{
  
  INPUT_VARS(iinfo);
    
  printf("Decoding tile flexbits at offset 0x%x\n", idx);
  

}
