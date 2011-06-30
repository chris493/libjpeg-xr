/*
 * jxrdapi.c
 *
 * Copyright (C) 2011, Chris Harding.
 * Copyright (C) 1994-1998, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains application interface code for the decompression of
 * JPEG XR files.
 *
 * All of the routines intended to be called directly by an application
 * are in this file.
 */

#define JPEG_INTERNALS
#include "jinclude.h"
#include "jpegxrlib.h"

/* Parsing functions defined in jxrparse.c */
EXTERN(int) jpegxr_parse_image_tile_layer JPP((j_decompress_ptr cinfo));

/*
 * Initialization of a JPEG decompression object.
 * The error manager must already be set up (in case memory manager fails).
 */
GLOBAL(void)
jpegxr_CreateDecompress (j_decompress_ptr cinfo, int version, size_t structsize)
{
  int i;

  /* Guard against version mismatches between library and caller. */
  cinfo->mem = NULL;		/* so jpegxr_destroy knows mem mgr not called */
  if (version != JPEGXR_LIB_VERSION)
    ERREXIT2(cinfo, JERR_BAD_LIB_VERSION, JPEGXR_LIB_VERSION, version);
  if (structsize != SIZEOF(struct jpegxr_decompress_struct))
    ERREXIT2(cinfo, JERR_BAD_STRUCT_SIZE, 
	     (int) SIZEOF(struct jpegxr_decompress_struct), (int) structsize);

  /* For debugging purposes, we zero the whole master structure.
   * But the application has already set the err pointer, and may have set
   * client_data, so we have to save and restore those fields.
   * Note: if application hasn't set client_data, tools like Purify may
   * complain here.
   */
  {
    struct jpeg_error_mgr * err = cinfo->err;
    void * client_data = cinfo->client_data; /* ignore Purify complaint here */
    MEMZERO(cinfo, SIZEOF(struct jpegxr_decompress_struct));
    cinfo->err = err;
    cinfo->client_data = client_data;
  }
  cinfo->is_decompressor = TRUE;

  /* Initialize a memory manager instance for this object */
  jinit_memory_mgr((j_common_ptr) cinfo);

  /* Zero out pointers to permanent structures. */
  cinfo->progress = NULL;
  cinfo->src = NULL;
  
  /* For now we keep records of the header with the decompression object.
   * These must be allocated. */
  cinfo->file_hdr = NULL;
  cinfo->hdr = NULL;
  cinfo->img_plane_hdr = NULL;
  cinfo->alpha_plane_hdr = NULL;
  cinfo->idx_tbl = NULL;

  /* OK, I'm ready */
  cinfo->global_state = DSTATE_START;
}


/*
 * Read the start of a JPEG-XR file to obtain codestream details.
 *
 */
GLOBAL(int)
jpegxr_read_file (j_decompress_ptr cinfo)
{
  int retcode;

  /* Check we are in the correct state. TODO - do we need inheader state? */
  if (cinfo->global_state != DSTATE_START &&
      cinfo->global_state != DSTATE_INHEADER)
    ERREXIT1(cinfo, JERR_BAD_STATE, cinfo->global_state);

  /* For now just parse image and tile layer */
  retcode = jpegxr_parse_file_header(cinfo);


  /* TODO - what new return codes are needed? */
  switch (retcode) {
  case JPEG_REACHED_SOS:
    retcode = JPEG_HEADER_OK;
    break;
  case JPEG_REACHED_EOI:
    /* Reset to start state; it would be safer to require the application to
     * call jpeg_abort, but we can't change it now for compatibility reasons.
     * A side effect is to free any temporary memory (there shouldn't be any).
     */
    //jpeg_abort((j_common_ptr) cinfo); /* sets state = DSTATE_START */
    retcode = JPEG_HEADER_TABLES_ONLY;
    break;
  case JPEG_SUSPENDED:
    /* no work */
    break;
  }

  return retcode;
}

/*
 * Read start of JPEG-XR codestream to obtain decompression parameteres.
 * Need only initialize JPEG object and supply a data source
 * before calling. This simply wraps parsing calls with some additional
 * error management.
 *
 */
GLOBAL(int)
jpegxr_read_header (j_decompress_ptr cinfo, boolean require_image)
{
  int retcode;

  /* Check we are in the correct state. TODO - do we need inheader state? */
  if (cinfo->global_state != DSTATE_START &&
      cinfo->global_state != DSTATE_INHEADER)
    ERREXIT1(cinfo, JERR_BAD_STATE, cinfo->global_state);

  /* For now just parse image and tile layer */
  retcode = jpegxr_parse_image_tile_layer(cinfo);


  /* TODO - what new return codes are needed? */
  switch (retcode) {
  case JPEG_REACHED_SOS:
    retcode = JPEG_HEADER_OK;
    break;
  case JPEG_REACHED_EOI:
    if (require_image)		/* Complain if application wanted an image */
      ERREXIT(cinfo, JERR_NO_IMAGE);
      /* Do we need an equivalent of require image? */
    /* Reset to start state; it would be safer to require the application to
     * call jpeg_abort, but we can't change it now for compatibility reasons.
     * A side effect is to free any temporary memory (there shouldn't be any).
     */
    //jpeg_abort((j_common_ptr) cinfo); /* sets state = DSTATE_START */
    retcode = JPEG_HEADER_TABLES_ONLY;
    break;
  case JPEG_SUSPENDED:
    /* no work */
    break;
  }

  return retcode;
}



/*
 * Destruction of a JPEG decompression object
 */
GLOBAL(void)
jpegxr_destroy_decompress (j_decompress_ptr cinfo)
{
  jpegxr_destroy((j_common_ptr) cinfo); /* use common routine */
}



/*
 * Destruction of a JPEG-XR object.
 *
 * Everything gets deallocated except the master jpeg_compress_struct itself
 * and the error manager struct.  Both of these are supplied by the application
 * and must be freed, if necessary, by the application.  (Often they are on
 * the stack and so don't need to be freed anyway.)
 * Closing a data source or destination, if necessary, is the application's
 * responsibility.
 */
GLOBAL(void)
jpegxr_destroy (j_common_ptr cinfo)
{
  /* We need only tell the memory manager to release everything. */
  /* NB: mem pointer is NULL if memory mgr failed to initialize. */
  if (cinfo->mem != NULL)
    (*cinfo->mem->self_destruct) (cinfo);
  cinfo->mem = NULL;		/* be safe if jpegxr_destroy is called twice */
  cinfo->global_state = 0;	/* mark it destroyed */
}
