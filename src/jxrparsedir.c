/*
 * jxrparsedir.c
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
 * Cycle through and read IFD entries. Often this is just reading values
 * into fields of the directory object. Sometimes this involves seeking and
 * reading from the stream if num_elements * element_size (determined by
 * element_type) is larger than 4 bytes. This occurs at least once, for
 * pixel_format. This means that there is no guaruantee of where the
 * read head will be after this operation.
 *  
 */          
GLOBAL(boolean)
jpegxr_dir_read_ifd_entries (j_dir_ptr dinfo)
{
  UINT8 c;
  UINT16 c2;
  UINT32 c4;
  ifd_entry * ifde;
  
  INPUT_VARS(dinfo);
  
  /* For each entry we found */
  for (int i=0; i < dinfo->num_entries; i++) {

    ifde = dinfo->ifd_entry_list[i];
    
    /* Currently we only support the 5 mandatory fields */
    switch (ifde->field_tag) {
      case JFIELDTAG_PIXEL_FORMAT:
	/* Always 16 elements so have to read from stream */
	/* Seek to correct offset */
	(*dinfo->src->seek_input_data) (dinfo, (long) ifde->values_or_offset);
	INPUT_RELOAD(dinfo);
	/* Size depends on type */
	switch (ifde->element_type) {
	  case (JELEMTYPE_BYTE):
	    for (int i=0; i<16; i++) {
	      INPUT_BYTE(dinfo,c,return FALSE);
	      dinfo->pixel_format[i] = c;
	    }
	    break;
	  default :
	    /* TODO - some kind of error here */
	    break;
	}
	break;
      case JFIELDTAG_IMAGE_WIDTH:
	dinfo->image_width = ifde->values_or_offset;
	break;
      case JFIELDTAG_IMAGE_HEIGHT:
	dinfo->image_height = ifde->values_or_offset;
	break;
      case JFIELDTAG_IMAGE_OFFSET:
	dinfo->image_offset = ifde->values_or_offset;
	break;
      case JFIELDTAG_IMAGE_BYTE_COUNT:
	dinfo->image_byte_count = ifde->values_or_offset;
	break;
      default :
	/* TODO - output some message if not supported? */
	break;
    }
  }
  
  INPUT_SYNC(dinfo);
  
  return TRUE;
}


/*
 * Read a JPEG-XR directory to obtain headers and decompression
 * parameters. This reads directory, image, tile and macroblock layer 
 * information. The source data should be at the start of the directory
 * header.
 *
 */
GLOBAL(int)
jpegxr_dir_read_metadata (j_dir_ptr dinfo)
{
  int retcode;

  /* Parse the directory header */
  retcode = jpegxr_dir_read_header(dinfo);
  
  /* Process each IFD entry */
  /* This will involve seeking and reading from the stream at least
   * once. After this operation, there is no guarantee of the position
   * of the read head in the source input. */
  jpegxr_dir_read_ifd_entries(dinfo);
  
  /* Skip forward to the coded image */
  (*dinfo->src->seek_input_data) (dinfo, (long) dinfo->image_offset); 
  
  /* Create a coded image object*/
  /* TODO - currently we support a single coded image. Some directories
   * will contain a second coded image containing the alpha plane.
   * Possibly even more coded images can be contained within a
   * directory, I don't think so though. TODO - verify this. */
  struct jpegxr_image_struct iinfo;
  dinfo->image = &iinfo;
  
  /* Initialize the JPEG-XR code image object */
  dinfo->image->err = dinfo->err;
  jpegxr_image_create_decompress(&iinfo); // has its own mem. manager
  dinfo->image->progress = dinfo->progress;
  dinfo->image->src = dinfo->src;
  
  /* Read the coded image header */
  jpegxr_image_read_header(&iinfo);
  
  return retcode;
}


/*
 * Read only directory header from the source data.
 * The source data should be at the start of the directory header.
 *  
 */          
GLOBAL(int)
jpegxr_dir_read_header (j_dir_ptr dinfo)
{
  UINT8 c;
  UINT16 c2;
  UINT32 c4;
  
  
  INPUT_VARS(dinfo);  
  
  INPUT_2BYTES_LE(dinfo, c2, return FALSE);
  dinfo->num_entries = c2;
  
  /* Allocate storage for list of IFD entries */
  ifd_entry *ifde_list = (*dinfo->mem->alloc_small) (
			      (j_common_ptr) dinfo,
			      JPOOL_IMAGE,
			      dinfo->num_entries * SIZEOF(ifd_entry)
			  );
  ifd_entry **ifde_ptr_list = (*dinfo->mem->alloc_small) (
			      (j_common_ptr) dinfo,
			      JPOOL_IMAGE,
			      dinfo->num_entries * SIZEOF(ifd_entry*)
			  );
  dinfo->ifd_entry_list = ifde_ptr_list;
  /* Cycle through and fill out IFD entry */
  for (int i=0; i < dinfo->num_entries; i++) {
    // list element points to IFD ptr
    dinfo->ifd_entry_list[i] = &ifde_list[i];
    // parse IFD entry fields
    INPUT_2BYTES_LE(dinfo, c2, return FALSE);
    dinfo->ifd_entry_list[i]->field_tag = c2;
    INPUT_2BYTES_LE(dinfo, c2, return FALSE);
    dinfo->ifd_entry_list[i]->element_type = c2;
    INPUT_4BYTES_LE(dinfo, c4, return FALSE);
    dinfo->ifd_entry_list[i]->num_elements = c4;
    INPUT_4BYTES_LE(dinfo, c4, return FALSE);
    dinfo->ifd_entry_list[i]->values_or_offset = c4;
  }
  
  /* Linked list to next directory */
  INPUT_4BYTES_LE(dinfo, c4, return FALSE);
  dinfo->zero_or_next_ifd_offset = c4;
  
  INPUT_SYNC(dinfo);
  
  /* TODO return correct code */
  return JPEG_REACHED_SOS;
}
