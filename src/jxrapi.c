/*
 * jxrapi.c
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

/*
 * Common initialisation routines for all JPEG-XR decompression objects.
 * The error manager must already be set up (in case memory manager fails).
 */
LOCAL(void)
common_initialisation (j_common_ptr cinfo)
{
  /* Initialize a memory manager instance for this object */
  jinit_memory_mgr((j_common_ptr) cinfo);

  /* Zero out pointers to permanent structures. */
  cinfo->progress = NULL;
  cinfo->src = NULL;

  /* OK, I'm ready */
  cinfo->global_state = DSTATE_START;
}


/*
 * Initialization of a JPEG file decompression object.
 * The error manager must already be set up (in case memory manager fails).
 */
GLOBAL(void)
jpegxr_file_CreateDecompress (j_file_ptr finfo, int version, size_t structsize)
{
  int i;

  /* Guard against version mismatches between library and caller. */
  finfo->mem = NULL;		/* so jpegxr_file_destroy knows mem mgr not called */
  if (version != JPEGXR_LIB_VERSION)
    ERREXIT2(finfo, JERR_BAD_LIB_VERSION, JPEGXR_LIB_VERSION, version);
  if (structsize != SIZEOF(struct jpegxr_file_struct))
    ERREXIT2(finfo, JERR_BAD_STRUCT_SIZE, 
	     (int) SIZEOF(struct jpegxr_file_struct), (int) structsize);

  /* For debugging purposes, we zero the whole master structure.
   * But the application has already set the err pointer, and may have set
   * client_data, so we have to save and restore those fields.
   * Note: if application hasn't set client_data, tools like Purify may
   * complain here.
   */
  {
    struct jpeg_error_mgr * err = finfo->err;
    void * client_data = finfo->client_data; /* ignore Purify complaint here */
    MEMZERO(finfo, SIZEOF(struct jpegxr_file_struct));
    finfo->err = err;
    finfo->client_data = client_data;
  }
  
  /* For now we support a single directory, which we can therefore allocate
   * now. */
   struct jpegxr_dir_struct first_and_only_directory;
   finfo->dirs = &first_and_only_directory;

  /* Common to all three objects */
  common_initialisation( (j_common_ptr) finfo );
}


/*
 * Initialization of a JPEG directory decompression object.
 * The error manager must already be set up (in case memory manager fails).
 */
GLOBAL(void)
jpegxr_dir_CreateDecompress (j_dir_ptr dinfo, int version, size_t structsize)
{
  int i;

  /* Guard against version mismatches between library and caller. */
  dinfo->mem = NULL;		/* so jpegxr_dir_destroy knows mem mgr not called */
  if (version != JPEGXR_LIB_VERSION)
    ERREXIT2(dinfo, JERR_BAD_LIB_VERSION, JPEGXR_LIB_VERSION, version);
  if (structsize != SIZEOF(struct jpegxr_dir_struct))
    ERREXIT2(dinfo, JERR_BAD_STRUCT_SIZE, 
	     (int) SIZEOF(struct jpegxr_dir_struct), (int) structsize);

  /* For debugging purposes, we zero the whole master structure.
   * But the application has already set the err pointer, and may have set
   * client_data, so we have to save and restore those fields.
   * Note: if application hasn't set client_data, tools like Purify may
   * complain here.
   */
  {
    struct jpeg_error_mgr * err = dinfo->err;
    void * client_data = dinfo->client_data; /* ignore Purify complaint here */
    MEMZERO(dinfo, SIZEOF(struct jpegxr_dir_struct));
    dinfo->err = err;
    dinfo->client_data = client_data;
  }

  /* Common to all three objects */
  common_initialisation( (j_common_ptr) dinfo );
}


/*
 * Initialization of a JPEG image decompression object.
 * The error manager must already be set up (in case memory manager fails).
 */
GLOBAL(void)
jpegxr_image_CreateDecompress (j_image_ptr iinfo, int version, size_t structsize)
{
  int i;

  /* Guard against version mismatches between library and caller. */
  iinfo->mem = NULL;		/* so jpegxr_image_destroy knows mem mgr not called */
  if (version != JPEGXR_LIB_VERSION)
    ERREXIT2(iinfo, JERR_BAD_LIB_VERSION, JPEGXR_LIB_VERSION, version);
  if (structsize != SIZEOF(struct jpegxr_image_struct))
    ERREXIT2(iinfo, JERR_BAD_STRUCT_SIZE, 
	     (int) SIZEOF(struct jpegxr_image_struct), (int) structsize);

  /* For debugging purposes, we zero the whole master structure.
   * But the application has already set the err pointer, and may have set
   * client_data, so we have to save and restore those fields.
   * Note: if application hasn't set client_data, tools like Purify may
   * complain here.
   */
  {
    struct jpeg_error_mgr * err = iinfo->err;
    void * client_data = iinfo->client_data; /* ignore Purify complaint here */
    MEMZERO(iinfo, SIZEOF(struct jpegxr_image_struct));
    iinfo->err = err;
    iinfo->client_data = client_data;
  }

  /* Common to all three objects */
  common_initialisation( (j_common_ptr) iinfo );
}



/*
 * Destruction of a JPEG-XR file decompression object
 */
GLOBAL(void)
jpegxr_file_destroy (j_file_ptr finfo)
{
  jpegxr_destroy((j_common_ptr) finfo); /* use common routine */
}
/*
 * Destruction of a JPEG-XR directory decompression object
 */
GLOBAL(void)
jpegxr_dir_destroy (j_dir_ptr finfo)
{
  jpegxr_destroy((j_common_ptr) finfo); /* use common routine */
}
/*
 * Destruction of a JPEG-XR image decompression object
 */
GLOBAL(void)
jpegxr_image_destroy (j_image_ptr finfo)
{
  jpegxr_destroy((j_common_ptr) finfo); /* use common routine */
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
