/*
 * jxrfile.c
 *
 * Copyright (C) 2011, Chris Harding.
 * Copyright (C) 1994-1998, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 * 
 * This file contains algorithms used to parse the file layer of .jxr
 * input file.
 */

#define JPEG_INTERNALS
#include "jinclude.h"
#include "jpegxrlib.h"

/* Data read macros */
#include "jxrdatard.h"


/*
 * Read the header of a coded image from the source data, which should
 * be at the start of the codestream.
 *  
 */          
GLOBAL(int)
jpegxr_image_read_header (j_image_ptr iinfo)
{
  
  /* TODO return correct code */
  return JPEG_REACHED_SOS;
}


