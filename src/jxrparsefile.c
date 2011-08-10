/*
 * jxrparsefile.c
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
#ifndef JXR_DATARD_INCLUDED
#include "jxrdatard.h"
#endif


/*
 * Read JPEG-XR file to obtain headers and decompression parameters.
 * This reads file, directory, image, and image plane layer 
 * information. Need only initialize JPEG object and supply a data source
 * before calling.
 *
 */
GLOBAL(int)
jpegxr_file_read_metadata (j_file_ptr finfo)
{
  TRACEMS(finfo,1,JXRTRC_FILE_BEGIN_META);
  
  // TODO sort out error handling etc.
  int retcode = JPEG_REACHED_SOS;

  /* Check we are in the correct state. TODO - more states, e.g. in file
   * header state, directory header state etc. */
  if (finfo->global_state != DSTATE_START &&
      finfo->global_state != DSTATE_INHEADER)
    ERREXIT1(finfo, JERR_BAD_STATE, finfo->global_state);

  /* Parse the file header */
  jpegxr_file_read_header(finfo);
  
  /* Skip forward to the first directory */
  TRACEMS1(finfo,2,JXRTRC_SEEK_DIR,finfo->first_ifd_offset);
  (*finfo->src->seek_input_data) ((j_common_ptr) finfo, (long) finfo->first_ifd_offset); 
  
  /* Create directory object */
  /* These are stored in a linked list throughout the codestream.
   * Currently we only support one (the first) directory. Further
   * directories are ignored. */
  TRACEMS(finfo,2,JXRTRC_CREATE_DIR);
  finfo->num_dirs = 1;
  
  /* Allocate storage for array of directory object pointers */
  finfo->dirs = (*finfo->mem->alloc_small) (
			      (j_common_ptr) finfo,
			      JPOOL_IMAGE,
			      finfo->num_dirs * SIZEOF(j_dir_ptr)
			  );
  
  /* Allocate, initialise and read data for each directory */
  for (unsigned int i=0; i < finfo->num_dirs; i++) {
    /* Allocate storage for directory object */
    finfo->dirs[i] = (*finfo->mem->alloc_small) (
            (j_common_ptr) finfo,
            JPOOL_IMAGE,
            SIZEOF(struct jpegxr_dir_struct)
          );
     
    /* Initialize the JPEG-XR directory decompression object */
    finfo->dirs[i]->err = finfo->err;
    finfo->dirs[i]->mem = finfo->mem;
    finfo->dirs[i]->progress = finfo->progress;
    finfo->dirs[i]->src = finfo->src;

    /* Read the directory header */
    jpegxr_dir_read_metadata( finfo->dirs[i] );
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
 * Read only the file header at the begining of a file input data source.
 *  
 */          
GLOBAL(int)
jpegxr_file_read_header (j_file_ptr finfo)
{
  UINT8 c;
  UINT16 c2;
  UINT32 c4;
  
  INPUT_VARS(finfo);
  
  TRACEMS1(finfo,2,JXRTRC_FILE_BEGIN,idx);
  
  /* Parse fixed length file prefix */
  INPUT_2BYTES(((j_common_ptr)finfo), c2, return FALSE);
  INPUT_BYTE(((j_common_ptr)finfo), c, return FALSE);
  /* check this looks like a .jxr file */
  if (c2 != JXR_FIXED_FILE_HEADER_2BYTE || c != JXR_FIXED_FILE_HEADER_1BYTE )
    ERREXIT4(	finfo, JXRERR_FILE_HEADER,
		c2, c, JXR_FIXED_FILE_HEADER_2BYTE, JXR_FIXED_FILE_HEADER_1BYTE);   
  TRACEMS2(finfo,3,JXRTRC_FILE_HEADER, c2, c);
  
  /* Parse file version ID */
  INPUT_BYTE(((j_common_ptr)finfo), c, return FALSE);
  /* check version matches with this library */
  if (c != JXR_FILE_VERSION)
    TRACEMS2(finfo,0,JXRTRC_FILE_VERSION_DIFF, c, JXR_FILE_VERSION);
  else
    TRACEMS1(finfo,3,JXRTRC_FILE_VERSION_MATCH, c);
  finfo->file_version_id = c;
    
  /* Parse offset of first directory */
  /* Stored in little endian format. */
  INPUT_4BYTES_LE(((j_common_ptr)finfo), c4, return FALSE);
  TRACEMS1(finfo,2,JXRTRC_FILE_OFFSET, c4);
  finfo->first_ifd_offset = c4;
  
  /* TODO return correct code */
  return JPEG_REACHED_SOS;
}
