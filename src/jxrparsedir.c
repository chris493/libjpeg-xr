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


/*
 * Macros for fetching data from the data source module.
 *
 * At all times, cinfo->src->next_input_byte and ->bytes_in_buffer reflect
 * the current restart point; we update them only when we have reached a
 * suitable place to restart if a suspension occurs.
 */

/* Declare and initialize local copies of input pointer/count */
#define INPUT_VARS(cinfo)  \
	struct jpeg_source_mgr * datasrc = (cinfo)->src;  \
	const JOCTET * next_input_byte = datasrc->next_input_byte;  \
	size_t bytes_in_buffer = datasrc->bytes_in_buffer;  \
	long idx = datasrc->idx

/* Unload the local copies --- do this only at a restart boundary */
#define INPUT_SYNC(cinfo)  \
	( datasrc->next_input_byte = next_input_byte,  \
	  datasrc->bytes_in_buffer = bytes_in_buffer,	\
	  datasrc->idx = idx	)

/* Reload the local copies --- used only in MAKE_BYTE_AVAIL */
#define INPUT_RELOAD(cinfo)  \
	( next_input_byte = datasrc->next_input_byte,  \
	  bytes_in_buffer = datasrc->bytes_in_buffer,	\
	  idx = datasrc->idx	)

/* Internal macro for INPUT_BYTE and INPUT_2BYTES: make a byte available.
 * Note we do *not* do INPUT_SYNC before calling fill_input_buffer,
 * but we must reload the local copies after a successful fill.
 */
#define MAKE_BYTE_AVAIL(cinfo,action)  \
	if (bytes_in_buffer == 0) {  \
	  if (! (*datasrc->fill_input_buffer) (cinfo))  \
	    { action; }  \
	  INPUT_RELOAD(cinfo);  \
	}

/* Read a byte into variable V.
 * If must suspend, take the specified action (typically "return FALSE").
 */
#define INPUT_BYTE(cinfo,V,action)  \
	MAKESTMT( MAKE_BYTE_AVAIL(cinfo,action); \
		  bytes_in_buffer--; \
		  idx++; \
		  V = GETJOCTET(*next_input_byte++); )

/* Read two bytes interpreted as an unsigned 16-bit integer.
 * V should be declared UINT16
 */
#define INPUT_2BYTES(cinfo,V,action)  \
	MAKESTMT( MAKE_BYTE_AVAIL(cinfo,action); \
		  bytes_in_buffer--; \
		  idx++; \
		  V = ((unsigned int) GETJOCTET(*next_input_byte++)) << 8; \
		  MAKE_BYTE_AVAIL(cinfo,action); \
		  bytes_in_buffer--; \
		  idx++; \
		  V += ((unsigned int) GETJOCTET(*next_input_byte++)) << 0; )
      
/* As above, but read two bytes interpreted as an unsigned 16-bit
 * integer in little endian format. V should be declared UINT16.
 */
#define INPUT_2BYTES_LE(cinfo,V,action)  \
	MAKESTMT( MAKE_BYTE_AVAIL(cinfo,action); \
		  bytes_in_buffer--; \
		  idx++; \
		  V = ((unsigned int) GETJOCTET(*next_input_byte++)) << 0; \
		  MAKE_BYTE_AVAIL(cinfo,action); \
		  bytes_in_buffer--; \
		  idx++; \
		  V += ((unsigned int) GETJOCTET(*next_input_byte++)) << 8; )
      
/* Read four bytes interpreted as an unsigned 32-bit integer.
 * V should be declared UINT32
 */
#define INPUT_4BYTES(cinfo,V,action)  \
	MAKESTMT( MAKE_BYTE_AVAIL(cinfo,action); \
		  bytes_in_buffer--; \
		  idx++; \
		  V = ((UINT32) GETJOCTET(*next_input_byte++)) << 24; \
		  MAKE_BYTE_AVAIL(cinfo,action); \
		  bytes_in_buffer--; \
		  idx++; \
		  V += ((UINT32) GETJOCTET(*next_input_byte++)) << 16; \
		  MAKE_BYTE_AVAIL(cinfo,action); \
		  bytes_in_buffer--; \
		  idx++; \
		  V += ((UINT32) GETJOCTET(*next_input_byte++)) << 8;  \
		  MAKE_BYTE_AVAIL(cinfo,action); \
		  bytes_in_buffer--; \
		  idx++; \
		  V += ((UINT32) GETJOCTET(*next_input_byte++)) << 0; )
      
/* As above, but read four bytes interpreted as an unsigned 32-bit
 * integer in little endian format. V should be declared UINT32
 */
#define INPUT_4BYTES_LE(cinfo,V,action)  \
	MAKESTMT( MAKE_BYTE_AVAIL(cinfo,action); \
		  bytes_in_buffer--; \
		  idx++; \
		  V = ((UINT32) GETJOCTET(*next_input_byte++))  << 0; \
		  MAKE_BYTE_AVAIL(cinfo,action); \
		  bytes_in_buffer--; \
		  idx++; \
		  V += ((UINT32) GETJOCTET(*next_input_byte++)) << 8; \
		  MAKE_BYTE_AVAIL(cinfo,action); \
		  bytes_in_buffer--; \
		  idx++; \
		  V += ((UINT32) GETJOCTET(*next_input_byte++)) << 16;  \
		  MAKE_BYTE_AVAIL(cinfo,action); \
		  bytes_in_buffer--; \
		  idx++; \
		  V += ((UINT32) GETJOCTET(*next_input_byte++)) << 24; )

/*
 * Process an IFD entry. This will involve seeking and reading from the
 * stream if num_elements * element_size (determined by element_type)
 * is larger than 4 bytes.
 *  
 */          
LOCAL(boolean)
process_ifd (j_dir_ptr dinfo, ifd_entry* ifde)
{
  UINT8 c;
  UINT16 c2;
  UINT32 c4;
  
  INPUT_VARS(dinfo);
  
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
  
  INPUT_SYNC(dinfo);
  
  return TRUE;
}


/*
 * Parse image directory header.
 *  
 */          
GLOBAL(int)
jpegxr_dir_parse_header (j_dir_ptr dinfo)
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
  
  /* Process each IFD entry */
  for (int i=0; i < dinfo->num_entries; i++) {
    process_ifd(dinfo, dinfo->ifd_entry_list[i]);
  }
  
  /* TODO return correct code */
  return JPEG_REACHED_SOS;
}
