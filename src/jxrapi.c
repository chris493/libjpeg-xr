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
 * Initialization of a JPEG file decompression object.
 * The error manager must already be set up (in case memory manager fails).
 */
GLOBAL(void)
jpegxr_file_CreateDecompress (j_file_ptr cinfo, int version, size_t structsize)
{
  int i;

  /* Guard against version mismatches between library and caller. */
  cinfo->mem = NULL;		/* so jpegxr_file_destroy knows mem mgr not called */
  if (version != JPEGXR_LIB_VERSION)
    ERREXIT2(cinfo, JERR_BAD_LIB_VERSION, JPEGXR_LIB_VERSION, version);
  if (structsize != SIZEOF(struct jpegxr_file_struct))
    ERREXIT2(cinfo, JERR_BAD_STRUCT_SIZE, 
	     (int) SIZEOF(struct jpegxr_file_struct), (int) structsize);

  /* For debugging purposes, we zero the whole master structure.
   * But the application has already set the err pointer, and may have set
   * client_data, so we have to save and restore those fields.
   * Note: if application hasn't set client_data, tools like Purify may
   * complain here.
   */
  {
    struct jpeg_error_mgr * err = cinfo->err;
    void * client_data = cinfo->client_data; /* ignore Purify complaint here */
    MEMZERO(cinfo, SIZEOF(struct jpegxr_file_struct));
    cinfo->err = err;
    cinfo->client_data = client_data;
  }

  /* Initialize a memory manager instance for this object */
  jinit_memory_mgr((j_common_ptr) cinfo);

  /* Zero out pointers to permanent structures. */
  cinfo->progress = NULL;
  cinfo->src = NULL;

  /* OK, I'm ready */
  cinfo->global_state = DSTATE_START;
}

/*
 * Initialization of a JPEG directory decompression object.
 * The error manager must already be set up (in case memory manager fails).
 */
GLOBAL(void)
jpegxr_dir_CreateDecompress (j_dir_ptr cinfo, int version, size_t structsize)
{
  int i;

  /* Guard against version mismatches between library and caller. */
  cinfo->mem = NULL;		/* so jpegxr_dir_destroy knows mem mgr not called */
  if (version != JPEGXR_LIB_VERSION)
    ERREXIT2(cinfo, JERR_BAD_LIB_VERSION, JPEGXR_LIB_VERSION, version);
  if (structsize != SIZEOF(struct jpegxr_dir_struct))
    ERREXIT2(cinfo, JERR_BAD_STRUCT_SIZE, 
	     (int) SIZEOF(struct jpegxr_dir_struct), (int) structsize);

  /* For debugging purposes, we zero the whole master structure.
   * But the application has already set the err pointer, and may have set
   * client_data, so we have to save and restore those fields.
   * Note: if application hasn't set client_data, tools like Purify may
   * complain here.
   */
  {
    struct jpeg_error_mgr * err = cinfo->err;
    void * client_data = cinfo->client_data; /* ignore Purify complaint here */
    MEMZERO(cinfo, SIZEOF(struct jpegxr_dir_struct));
    cinfo->err = err;
    cinfo->client_data = client_data;
  }

  /* Initialize a memory manager instance for this object */
  jinit_memory_mgr((j_common_ptr) cinfo);

  /* Zero out pointers to permanent structures. */
  cinfo->progress = NULL;
  cinfo->src = NULL;

  /* OK, I'm ready */
  cinfo->global_state = DSTATE_START;
}

/*
 * Read JPEG-XR file to obtain headers and decompression parameters.
 * This reads file, directory, image, tile and macroblock layer 
 * information. Need only initialize JPEG object and supply a data source
 * before calling.
 *
 */
GLOBAL(int)
jpegxr_file_read_header (j_file_ptr finfo)
{
  int retcode;

  /* Check we are in the correct state. TODO - more states, e.g. in file
   * header state, directory header state etc. */
  if (finfo->global_state != DSTATE_START &&
      finfo->global_state != DSTATE_INHEADER)
    ERREXIT1(finfo, JERR_BAD_STATE, finfo->global_state);

  /* Parse the file header */
  /* This also skips forward to the first directory */
  retcode = jpegxr_file_parse_header(finfo);
  
  /* Create file directory object */
  /* These are stored in a linked list throughout the codestream.
   * Currently we only support one (the first) directory. Further
   * directories are ignored. */
  finfo->num_dirs = 1;
  // list of dirs
  j_dir_ptr dirs[finfo->num_dirs];
  struct jpegxr_dir_struct dinfo;
  dirs[0] = &dinfo;
  finfo->dirs = dirs;
  
  /* Initialize the JPEG-XR directory decompression object */
  finfo->dirs[0]->err = finfo->err;
  jpegxr_dir_create_decompress(&dinfo); // has its own mem. manager
  finfo->dirs[0]->progress = finfo->progress;
  finfo->dirs[0]->src = finfo->src;

  /* Read the directory header */
  retcode = jpegxr_dir_parse_header(&dinfo);


  fprintf(stdout, "File header\n");
  fprintf(stdout, "fixed_file_header_ii_2bytes : %x\n", finfo->fixed_file_header_ii_2bytes);
  fprintf(stdout, "fixed_file_header_0xbc_byte : %x\n", finfo->fixed_file_header_0xbc_byte);
  fprintf(stdout, "file_version_id : %x\n", finfo->file_version_id);
  fprintf(stdout, "first_ifd_offset : %x\n\n", finfo->first_ifd_offset);
  
  fprintf(stdout, "Directory\n");
  fprintf(stdout, "num_entries: %x\n", finfo->dirs[0]->num_entries );
  fprintf(stdout, "zero_or_next_ifd_offset: %x\n\n", finfo->dirs[0]->zero_or_next_ifd_offset);


  for (int i=0; i< finfo->dirs[0]->num_entries; i++ ) {
    fprintf(stdout, "IFD entry %d\n", i);
    fprintf(stdout, "field_tag : %x\n",         finfo->dirs[0]->ifd_entry_list[i]->field_tag);
    fprintf(stdout, "element_type : %x\n",      finfo->dirs[0]->ifd_entry_list[i]->element_type);
    fprintf(stdout, "num_elements : %x\n",      finfo->dirs[0]->ifd_entry_list[i]->num_elements);
    fprintf(stdout, "values_or_offset : %x\n\n",  finfo->dirs[0]->ifd_entry_list[i]->values_or_offset);
  }
  
  
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
    //jpeg_abort((j_common_ptr) finfo); /* sets state = DSTATE_START */
    retcode = JPEG_HEADER_TABLES_ONLY;
    break;
  case JPEG_SUSPENDED:
    /* no work */
    break;
  }

  return retcode;
}

/*
 * Read a JPEG-XR directory to obtain headers and decompression
 * parameters. This reads directory, image, tile and macroblock layer 
 * information. Need only initialize JPEG object and supply a data source
 * before calling.
 *
 */
GLOBAL(int)
jpegxr_dir_read_header (j_dir_ptr dinfo)
{
  int retcode;

  /* Check we are in the correct state. TODO - more states, e.g. in file
   * header state, directory header state etc. */
  if (dinfo->global_state != DSTATE_START &&
      dinfo->global_state != DSTATE_INHEADER)
    ERREXIT1(dinfo, JERR_BAD_STATE, dinfo->global_state);

  /* Parse the directory header */
  retcode = jpegxr_dir_parse_header(dinfo);
  
  return retcode;
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
