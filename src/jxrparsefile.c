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
#include "jxrdatard.h"


/*
 * Read JPEG-XR file to obtain headers and decompression parameters.
 * This reads file, directory, image, tile and macroblock layer 
 * information. Need only initialize JPEG object and supply a data source
 * before calling.
 *
 */
GLOBAL(int)
jpegxr_file_read_metadata (j_file_ptr finfo)
{
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
  (*finfo->src->seek_input_data) (finfo, (long) finfo->first_ifd_offset); 
  
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
  jpegxr_dir_read_metadata(&dinfo);


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

  fprintf(stdout, "Supported IFD entry values\n");
  for (int i=0; i<16; i++) 
    fprintf(stdout, "pixel format: %x\n", finfo->dirs[0]->pixel_format[i] );
  fprintf(stdout, "image_width: %x\n", finfo->dirs[0]->image_width );
  fprintf(stdout, "image_height: %x\n", finfo->dirs[0]->image_height );
  fprintf(stdout, "image_offset: %x\n", finfo->dirs[0]->image_offset );
  fprintf(stdout, "image_byte_count: %x\n", finfo->dirs[0]->image_byte_count );
  

    
  
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
  
  /* Parse file parameters */
  INPUT_2BYTES(finfo, c2, return FALSE); 
  finfo->fixed_file_header_ii_2bytes = c2;
  INPUT_BYTE(finfo, c, return FALSE);
  finfo->fixed_file_header_0xbc_byte = c;
  INPUT_BYTE(finfo, c, return FALSE);
  finfo->file_version_id = c;
  /* Offset is stored in little endian format */
  INPUT_4BYTES_LE(finfo, c4, return FALSE);
  finfo->first_ifd_offset = c4;
  
  /* TODO return correct code */
  return JPEG_REACHED_SOS;
}
