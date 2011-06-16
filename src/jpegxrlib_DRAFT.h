/*
 * jpegxrlib.h
 *
 * Copyright (C) 2011, Chris Harding.
 * Copyright (C) 1991-1998, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file defines the application interface for the JPEG library.
 * Most applications using the library need only include this file,
 * and perhaps jerror.h if they want to know the exact error codes.
 */
 
 #ifndef JPEGLIBXR_H
#define JPEGLIBXR_H

/*
 * First we include the configuration files that record how this
 * installation of the JPEG library is set up.  jconfig.h can be
 * generated automatically for many systems.  jmorecfg.h contains
 * manual configuration options that most people need not worry about.
 */

#ifndef JCONFIG_INCLUDED	/* in case jinclude.h already did */
#include "jconfig.h"		/* widely used configuration options */
#endif
#include "jmorecfg.h"		/* seldom changed options */


/* Version ID for the JPEG library.
 * Might be useful for tests like "#if JPEGXR_LIB_VERSION >= 60".
 */
#define JPEGXR_LIB_VERSION  11	/* Version 1a */


/* Types for JPEG-XR header information */

/* Image header */
typedef struct {} IMAGE_HEADER;

/* Image plane header, for both image and alpha planes */
typedef struct {} IMAGE_PLANE_HEADER;

/* Title index table */
typedef struct {} INDEX_TABLE_TILES;



/* Common fields between JPEG compression and decompression master structs. */

#define jpeg_common_fields \
  struct jpeg_error_mgr * err;	/* Error handler module */\
  struct jpeg_memory_mgr * mem;	/* Memory manager module */\
  struct jpeg_progress_mgr * progress; /* Progress monitor, or NULL if none */\
  void * client_data;		/* Available for use by application */\
  boolean is_decompressor;	/* So common code can tell which is which */\
  int global_state		/* For checking call sequence validity */

/* Routines that are to be used by decompression and compression
 * (currently not implemented)  halves of the library are declared to
 * receive a pointer to this structure.  There are no actual instances
 * of jpeg_common_struct, only of jpegxr_decompress_struct.
 */
struct jpeg_common_struct {
  jpeg_common_fields;		/* Fields common to both master struct types */
  /* Additional fields follow in an actual jpegxr_decompress_struct.
   * All three structs must agree on these initial fields!  (This would
   * be a lot cleaner in C++.)
   */
};

typedef struct jpeg_common_struct * j_common_ptr;
typedef struct jpegxr_decompress_struct * j_decompress_ptr;


/* Master record for a decompression instance */

struct jpegxr_decompress_struct {
  jpeg_common_fields;		/* Fields shared with jpeg_compress_struct */

  /* Source of compressed data */
  struct jpeg_source_mgr * src;
  
  /* Image header */
	IMAGE_HEADER image_header;
  
  /* Plane headers for image and alpha planes */
	IMAGE_PLANE_HEADER image_plane_header;
	IMAGE_PLANE_HEADER alpha_plane_header;

  /* Tile index table */
  /* Tiles need not be in order and there may be codestream segments of
   * unspecified content between the tiles. The tile index table is used
   * to locate the data that corresponds to a particular tile */
	INDEX_TABLE_TILES index_table_tiles;

  /* RESERVED_A_BYTE */
  /* The use of this syntax element is reserved for future specification
   * by ITU-T | ISO/IEC. This decoder is based on ITU-T Rec. T832
   * (03/2009). When present, the value of this syntax element is
   *  ignored. */
	UINT8 reserved_a_byte;
  
};






#endif /* JPEGLIBXR_H */
