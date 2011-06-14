/*
 * jdarith.c
 *
 * [IJG Copyright notice]
 *
 * This file holds place for arithmetic entropy decoding routines.
 */

#define JPEG_INTERNALS
#include "jinclude.h"
#include "jpeglib.h"


/*
 * Module initialization routine for arithmetic entropy decoding.
 */

GLOBAL(void)
jinit_arith_decoder (j_decompress_ptr cinfo)
{
  ERREXIT(cinfo, JERR_ARITH_NOTIMPL);
}
