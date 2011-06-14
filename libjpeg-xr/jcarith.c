/*
 * jcarith.c
 *
 * [IJG Copyright notice]
 *
 * This file holds place for arithmetic entropy encoding routines.
 */

#define JPEG_INTERNALS
#include "jinclude.h"
#include "jpeglib.h"


/*
 * Module initialization routine for arithmetic entropy encoding.
 */

GLOBAL(void)
jinit_arith_encoder (j_compress_ptr cinfo)
{
  ERREXIT(cinfo, JERR_ARITH_NOTIMPL);
}
