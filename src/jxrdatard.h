/*
 * jxrdatard.h
 *
 * Copyright (C) 2011, Chris Harding.
 * Copyright (C) 1994-1998, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 * 
 * This file contains macros for fetching data from the data source module.
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
		  
/* Get y bits from a byte, starting from index x */
/* Might want to change this depending on architecture/compiler */
#define GETBITS(c,x,y)	((((c << x) & 0xff) >> (8 - y)) & 0xff)

