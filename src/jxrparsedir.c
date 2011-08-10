/*
 * jxrparsedir.c
 *
 * Copyright (C) 2011, Chris Harding.
 * Copyright (C) 1994-1998, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 * 
 * This file contains algorithms used to parse the directory layer of .jxr
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
 * Cycle through and read IFD entries. Often this is just reading values
 * into fields of the directory object. Sometimes this involves seeking and
 * reading from the stream if num_elements * sizeof(element_type) is
 * larger than 4 bytes. This occurs at least once, for pixel_format.
 * This means that there is no guaruantee of where the read head will be
 * after this operation.
 * 
 * Only a small subset of field value combinations are permitted according
 * to the specification. This function should also ensure that IFD entries
 * are valid.
 *  
 */          
LOCAL(int)
jpegxr_dir_read_ifd_entries (j_dir_ptr dinfo, ifd_entry * ifd_entry_list)
{
  UINT8 c;
  UINT16 c2;
  UINT32 c4;
  ifd_entry * ifde;
  
  TRACEMS(dinfo,2,JXRTRC_DIR_BEGIN_ENTRIES);
    
  INPUT_VARS(dinfo);
  
  /* Define string lookup functions for enums */
  DEFINE_ENUM(JXR_FIELD_TAG,JXR_FIELD_TAG_DEF);
  
  /* For each entry we found */
  for (int i=0; i < dinfo->num_entries; i++) {
    
    /* Current IFD entry */
    ifde = &ifd_entry_list[i];
    
    /* Currently we only support the 5 mandatory fields */
    switch (ifde->field_tag) {
      case JFIELDTAG_PIXEL_FORMAT:
	TRACEMSS(dinfo,3,JXRTRC_DIR_READ_ENTRY, GetString_JXR_FIELD_TAG(ifde->field_tag) );
	/* Always 16 elements so have to read from stream */
	/* Seek to correct offset */
	(*dinfo->src->seek_input_data) (((j_common_ptr)dinfo), (long) ifde->values_or_offset);
	INPUT_RELOAD(dinfo);
	/* Size depends on type */
	switch (ifde->element_type) {
	  case (JELEMTYPE_BYTE):
	    for (int i=0; i<ifde->num_elements; i++) {
	      INPUT_BYTE(((j_common_ptr)dinfo),c,return FALSE);
	      TRACEMS3(dinfo,4,JXRTRC_DIR_READ_ENTRY_MULTIPLE, i+1, ifde->num_elements, c);
	      dinfo->pixel_format[i] = c;
	    }
	    break;
	  default :
	    /* Wrong element type for pixel format, ignore IFD entry */
	    TRACEMS1(dinfo,0,JXRTRC_DIR_IGNORE_ENTRY, ifde->element_type);
	    break;
	}
	break;
      case JFIELDTAG_IMAGE_WIDTH:
	TRACEMSS(dinfo,3,JXRTRC_DIR_READ_ENTRY, GetString_JXR_FIELD_TAG(ifde->field_tag) );
	dinfo->image_width = ifde->values_or_offset;
	break;
      case JFIELDTAG_IMAGE_HEIGHT:
	TRACEMSS(dinfo,3,JXRTRC_DIR_READ_ENTRY, GetString_JXR_FIELD_TAG(ifde->field_tag) );
	dinfo->image_height = ifde->values_or_offset;
	break;
      case JFIELDTAG_IMAGE_OFFSET:
	TRACEMSS(dinfo,3,JXRTRC_DIR_READ_ENTRY, GetString_JXR_FIELD_TAG(ifde->field_tag) );
	dinfo->image_offset = ifde->values_or_offset;
	break;
      case JFIELDTAG_IMAGE_BYTE_COUNT:
	TRACEMSS(dinfo,3,JXRTRC_DIR_READ_ENTRY, GetString_JXR_FIELD_TAG(ifde->field_tag) );
	dinfo->image_byte_count = ifde->values_or_offset;
	break;
      default :
	TRACEMSS(dinfo,0,JXRTRC_DIR_UNSUPPORTED_ENTRY, GetString_JXR_FIELD_TAG(ifde->field_tag) );
	break;
    }
  }
  
  INPUT_SYNC(dinfo);
  
  /* TODO return correct code */
  return JPEG_REACHED_SOS;
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
  
  TRACEMS(dinfo, 2, JXRTRC_DIR_BEGIN);
  
  INPUT_VARS(dinfo);
  
  /* Number of IFD entries the IFD contains */
  INPUT_2BYTES_LE(((j_common_ptr)dinfo), c2, return FALSE);
  if (c2 < JXR_MIN_NUM_IFD_ENTRIES)
    ERREXIT2(dinfo,JXRERR_TOO_FEW_IFD_ENTRIES,c2,JXR_MIN_NUM_IFD_ENTRIES);
  TRACEMS1(dinfo, 3, JXRTRC_DIR_NUM_IFD_ENTRIES, c2);
  dinfo->num_entries = c2;
  
  /* Allocate temporary obect for reading IFD entries */
  ifd_entry * ifd_entry_list = (*dinfo->mem->alloc_small) (
			  (j_common_ptr) dinfo,
			  JPOOL_IMAGE,
			  dinfo->num_entries * SIZEOF(ifd_entry)
			);
			
  /* Define string lookup functions for enums */
  DEFINE_ENUM(JXR_FIELD_TAG,JXR_FIELD_TAG_DEF);
  
  /* Cycle through and fill out IFD entry */
  for (unsigned int i=0; i < dinfo->num_entries; i++) {
    
    /* Field tag determines type of IFD entry */
    /* We check for supported types later */
    INPUT_2BYTES_LE(((j_common_ptr)dinfo), c2, return FALSE);
    TRACEMS2(dinfo, 3, JXRTRC_DIR_FIELD_TAG, i, GetString_JXR_FIELD_TAG(c2) );
    ifd_entry_list[i].field_tag = c2;
    
    /* Element type determines length and format of elements */
    INPUT_2BYTES_LE(((j_common_ptr)dinfo), c2, return FALSE);
    // check for reserved type
    if (c2 < JELEMTYPE_BYTE || c2 > JELEMTYPE_DOUBLE)
      TRACEMS2(dinfo, 0, JXRTRC_DIR_ELEM_TYPE_RESERVED, i, c2);
    else
      TRACEMS2(dinfo, 4, JXRTRC_DIR_ELEM_TYPE, i, c2);
    ifd_entry_list[i].element_type = c2;
    
    /* Number of elements */
    INPUT_4BYTES_LE(((j_common_ptr)dinfo), c4, return FALSE);
    TRACEMS2(dinfo, 4, JXRTRC_DIR_NUM_ELEMS, i, c4);
    ifd_entry_list[i].num_elements = c4;
    
    /* Element values, or offset where they can be found */
    /* Offset used if num_elements * sizeof(element_type) is more that
     * 4-bytes.  */
    INPUT_4BYTES_LE(((j_common_ptr)dinfo), c4, return FALSE);
    TRACEMS2(dinfo, 4, JXRTRC_DIR_VALUES, i, c4);
    ifd_entry_list[i].values_or_offset = c4;
  }
  
  /* Linked list to next directory */
  INPUT_4BYTES_LE(((j_common_ptr)dinfo), c4, return FALSE);
  if (c4 != 0)
    TRACEMS1(dinfo, 2, JXRTRC_DIR_NEXT_IFD, c4);
  else
    TRACEMS(dinfo, 2, JXRTRC_DIR_NO_MORE_IFD);
  dinfo->zero_or_next_ifd_offset = c4;
  
  INPUT_SYNC(dinfo);
  
  /* Process each IFD entry */
  /* This will involve seeking and reading from the stream at least
   * once. After this operation, there is no guarantee of the position
   * of the read head in the source input. */
  jpegxr_dir_read_ifd_entries(dinfo, ifd_entry_list);
  
  /* TODO return correct code */
  return JPEG_REACHED_SOS;
}


/*
 * Read a JPEG-XR directory to obtain headers and decompression
 * parameters. This reads directory, image, and image plane layer 
 * information. The source data should be at the start of the directory
 * header.
 *
 */
GLOBAL(int)
jpegxr_dir_read_metadata (j_dir_ptr dinfo)
{
  int retcode;
  
  TRACEMS(dinfo,1,JXRTRC_DIR_BEGIN_META);
  
  /* Parse the directory header */
  /* This will involve seeking and reading from the stream at least
   * once. After this operation, there is no guarantee of the position
   * of the read head in the source input. */
  retcode = jpegxr_dir_read_header(dinfo);
  
  /* Skip forward to the coded image */
  TRACEMS1(dinfo,2,JXRTRC_SEEK_IMAGE,dinfo->image_offset);
  (*dinfo->src->seek_input_data) (((j_common_ptr)dinfo), (long) dinfo->image_offset); 
  
  /* Create a coded image object */
  /* TODO - currently we support a single coded image. Some directories
   * will contain a second coded image containing the alpha plane.
   * Possibly even more coded images can be contained within a
   * directory, I don't think so though (verify this). */
  TRACEMS(dinfo,2,JXRTRC_CREATE_IMAGE);
  
  /* Allocate memory for image object */
  j_image_ptr iinfo = (*dinfo->mem->alloc_small) (
			  (j_common_ptr) dinfo,
			  JPOOL_IMAGE,
			  SIZEOF(struct jpegxr_image_struct)
			);
  dinfo->image = iinfo;
  
  /* Initialize the JPEG-XR code image object */
  dinfo->image->err = dinfo->err;
  dinfo->image->mem = dinfo->mem;
  dinfo->image->progress = dinfo->progress;
  dinfo->image->src = dinfo->src;
  
  /* Read the coded image header */
  jpegxr_image_read_header(iinfo);
  
  return retcode;
}


