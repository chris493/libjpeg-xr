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
 * Parse image file header and skip to the first directory.
 *  
 */          
GLOBAL(int)
jpegxr_file_parse_header (j_file_ptr finfo)
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
  
  /* Seek to the directory */
  INPUT_SYNC(finfo);
  (*finfo->src->seek_input_data) (finfo, (long) finfo->first_ifd_offset);   
  
  /* TODO return correct code */
  return JPEG_REACHED_SOS;
}
