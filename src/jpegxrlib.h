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

/* The following include helps diagnostics by allowing us to lookup
 * enum names as string constants. */
#ifndef JXR_ENUM_FACTORY_INCLUDED
#define JXR_ENUM_FACTORY_INCLUDED
#include "jxrefact.h"
#endif

/* Version ID for the JPEG library.
 * Might be useful for tests like "#if JPEGXR_LIB_VERSION >= 11".
 */
#define JPEGXR_LIB_VERSION  11	/* Version 1a */


/* TODO - remove these */
/* Various constants determining the sizes of things.
 * All of these are specified by the JPEG standard, so don't change them
 * if you want to be compatible.
 */
#define DCTSIZE		    8	/* The basic DCT block is 8x8 samples */
#define DCTSIZE2	    64	/* DCTSIZE squared; # of elements in a block */
#define NUM_QUANT_TBLS      4	/* Quantization tables are numbered 0..3 */
#define NUM_HUFF_TBLS       4	/* Huffman tables are numbered 0..3 */
#define NUM_ARITH_TBLS      16	/* Arith-coding tables are numbered 0..15 */
#define MAX_COMPS_IN_SCAN   4	/* JPEG limit on # of components in one scan */
#define MAX_SAMP_FACTOR     4	/* JPEG limit on sampling factors */
/* Unfortunately, some bozo at Adobe saw no reason to be bound by the standard;
 * the PostScript DCT filter can emit files with many more than 10 blocks/MCU.
 * If you happen to run across such a file, you can up D_MAX_BLOCKS_IN_MCU
 * to handle it.  We even let you do this from the jconfig.h file.  However,
 * we strongly discourage changing C_MAX_BLOCKS_IN_MCU; just because Adobe
 * sometimes emits noncompliant files doesn't mean you should too.
 */
#define C_MAX_BLOCKS_IN_MCU   10 /* compressor's limit on blocks per MCU */
#ifndef D_MAX_BLOCKS_IN_MCU
#define D_MAX_BLOCKS_IN_MCU   10 /* decompressor's limit on blocks per MCU */
#endif

/* Constants for JPEG-XR */
/* Fixed length prefix for a .jxr file */
#define JXR_FIXED_FILE_HEADER_2BYTE  0x4949
#define JXR_FIXED_FILE_HEADER_1BYTE  0xBC
#define JXR_FILE_VERSION  1
#define JXR_MIN_NUM_IFD_ENTRIES  5
#define JXR_GDI_SIG_HI 0x574D5048
#define JXR_GDI_SIG_LO 0x4F544F00
#define JXR_INDEX_TABLE_STARTCODE 0x0001

/* Data structures for images (arrays of samples and of DCT coefficients).
 * On 80x86 machines, the image arrays are too big for near pointers,
 * but the pointer arrays can fit in near memory.
 * TODO - rewrite these for JPEG-XR.
 */
typedef JSAMPLE FAR *JSAMPROW;	/* ptr to one image row of pixel samples. */
typedef JSAMPROW *JSAMPARRAY;	/* ptr to some rows (a 2-D sample array) */
typedef JSAMPARRAY *JSAMPIMAGE;	/* a 3-D sample array: top index is color */

typedef JCOEF JBLOCK[8];	/* one block of coefficients */
typedef JBLOCK FAR *JBLOCKROW;	/* pointer to one row of coefficient blocks */
typedef JBLOCKROW *JBLOCKARRAY;		/* a 2-D array of coefficient blocks */
typedef JBLOCKARRAY *JBLOCKIMAGE;	/* a 3-D array of coefficient blocks */

typedef JCOEF FAR *JCOEFPTR;	/* useful in a couple of places */



/* Common fields between JPEG-XR objects */
/* These are shared between file, directory and codestream objects. A
 * file contains mutliple directories, each of which may contain
 * multiple codestreams. A codestream or directory can also be decoded
 * in isolation. All objects therefore share these fields and share
 * instances with their child objects.
 */
#define jpegxr_common_fields \
  struct jpeg_error_mgr * err;	      /* Error handler module */\
  struct jpeg_memory_mgr * mem;	      /* Memory manager module */\
  struct jpeg_progress_mgr * progress;/* Progress monitor, or NULL if none */\
  struct jpeg_source_mgr * src;       /* Source of compressed data */\
  void * client_data;		              /* Available for use by application */\
  int global_state		                /* For checking call sequence validity */
  

/* Routines that are to be used by decompression common to files,
 * directories and image are declared to receive a pointer to this
 * structure.  There are no actual instances of jpeg_common_struct, only
 * of jpegxr_image_struct, jpegxr_dir_struct,
 * jpegxr_file_struct.
 */
struct jpegxr_common_struct {
	jpegxr_common_fields;		/* Fields common to all master struct types */
	/* Additional fields follow in an actual structs: jpegxr_image_struct,
   * jpegxr_dir_struct, jpegxr_file_struct. All three structs must agree
   * on these initial fields (this would be a lot cleaner in C++).
	*/
};

typedef struct jpegxr_common_struct * j_common_ptr;
/* It is safe to cast these to j_common_ptr */
typedef struct jpegxr_image_struct * j_image_ptr;
typedef struct jpegxr_dir_struct * j_dir_ptr;
typedef struct jpegxr_file_struct * j_file_ptr;


/* Data types for JPEG-XR coded image instances */

/* Spatial transform fill */
/* Indicates the location of the [0][0] image sample coordinate position
 * after application of the requested transformation. */
typedef enum {
		/* X Y -> row 0 represents the X edge of the image and column 0
		 * represents the Y edge of the image. */
		JXFILL_TL,	/* Top Left */
		JXFILL_BL,	/* Bottom Left */
		JXFILL_TR,	/* Top Right */
		JXFILL_BR,	/* Bottom Right */
		JXFILL_RT,	/* Right Top */
		JXFILL_RB,	/* Right Bottom */
		JXFILL_LT,	/* Left Top */
		JXFILL_LB	  /* Left Bottom */
} JXR_SPATIAL_XFRM_FILL;

/* Spatial transform subordinate */
/* Prefered spatial transform details, in the absence of any over-riding
 * indication as determined by the application or by a file format usage
 * context. */ 
typedef struct {
		/* Rotate output image 90 degrees clockwise */
		boolean rcw;
		/* Horizontal flip of output image */
		boolean flip_h;
		/* Vertical flip of output image */
		boolean flip_v;
		/* Spatial transform fill */
		JXR_SPATIAL_XFRM_FILL fill;
} jxr_spatial_xfrm_subordinate;

/* Output color formats */
#define JXR_OUTPUT_CLR_FMT_DEF(XX) \
		XX(JOUTCOL_YONLY,) \
		XX(JOUTCOL_YUV420,) \
		XX(JOUTCOL_YUV422,) \
		XX(JOUTCOL_YUV444,) \
		XX(JOUTCOL_CMYK,) \
		XX(JOUTCOL_CMYKDIRECT,) \
    XX(JOUTCOL_NCOMPONENT,) \
		XX(JOUTCOL_RGB,) \
		XX(JOUTCOL_RGBE,) \
		XX(JOUTCOL_RESERVED,)
DECLARE_ENUM(JXR_OUTPUT_CLR_FMT,JXR_OUTPUT_CLR_FMT_DEF)


/* Output bit depths */
#define JXR_OUTPUT_BIT_DEPTH_DEF(XX) \
		XX(JOUTDEP_BD1WHITE1,) \
		XX(JOUTDEP_BD8,      ) \
		XX(JOUTDEP_BD16,     ) \
		XX(JOUTDEP_BD16S,    ) \
		XX(JOUTDEP_BD16F,    ) \
		XX(JOUTDEP_RESERVED1,) \
		XX(JOUTDEP_BD32S,    ) \
		XX(JOUTDEP_BD32F,    ) \
		XX(JOUTDEP_BD5,      ) \
		XX(JOUTDEP_BD10,     ) \
		XX(JOUTDEP_BD565,    ) \
		XX(JOUTDEP_RESERVED2,) \
		XX(JOUTDEP_BD1BLACK1,)
DECLARE_ENUM(JXR_OUTPUT_BIT_DEPTH,JXR_OUTPUT_BIT_DEPTH_DEF)


/* Image header. Fields are presented in the order they appear in the codestream. */
typedef struct {

  /* Reserved for future specifications */
  /* Equal to 0x1 if compatible with this decoder */
  UINT8 reserved_b; /* 4 bits */
  /* If TRUE, overlap filtering is not performed across tile boundaries */
  boolean hard_tiling_flag;
  /* Reserved for future specifications */
  /* Ignore, to enable future definition of backwards compatabile usage. */
  UINT8 reserved_c; /* 3 bits */
  /* If TRUE, then num_ver_tiles_minus1 and num_hor_tiles_minus1 are
   * present in the codestream. Otherwise, the number of tiles is equal
   * to 1*/
  boolean tiling_flag;
  /* TRUE if tiles are in frequency mode, rather than spatial mode */
  boolean frequency_mode_codestream_flag;
  
  /* Prefered spatial transform details, in the absence of any
   * over-riding indication as determined by the application or by a
   * file format usage context */ 
  jxr_spatial_xfrm_subordinate spatial_xfrm_subordinate;
  
  /* If TRUE, the index table is present in the codestream. If
   * frequency_mode_codestream_flag is equal to TRUE, or there is more
   * than one row or column of tiles it is a requirement of codestream
   * conformance that this is TRUE. */
  boolean index_table_present_flag;
  /* Overlap processing mode */
  UINT8 overlap_mode; /* 2 bits */
  /* If TRUE, width_minus1, height_minus1, tile_width_in_mb[n] and
   * tile_height_in_mb[n] (when present) are half their normal lengths. */
  boolean short_header_flag;
  /* If TRUE decoding process variables are 32-bit, rather than 16-bit */
  boolean long_word_flag;
  /* Specifies if windowing dimensions are present in codestream */
  boolean windowing_flag;
  /* Specifies if trim_flexbits element is present in codestream */
  boolean trim_flexbits_flag;
  /* Reserved for future specifications */
  /* Ignore, to enable future definition of backwards compatability */
  UINT8 reserved_d; /* 3 bits */
  /* Specifies if an alpha image plane is present in codestream */
  boolean alpha_image_plane_flag;
  
  /* Colour format and bit depth of the output image */
  JXR_OUTPUT_CLR_FMT output_clr_fmt;
  JXR_OUTPUT_BIT_DEPTH output_bitdepth; /* 4 bits */
  
  /* Output image dimensions */
  UINT32 width_minus1; 	/* 16 bits if short_header_flag = TRUE */
  UINT32 height_minus1;  /* 16 bits if short_header_flag = TRUE */

  /* Tile partitioning sequence lengths */
  /* Set to zero, unless tiling_flag = TRUE */
  UINT16 num_ver_tiles_minus1; /* 12 bits */
  UINT16 num_hor_tiles_minus1; /* 12 bits */

  /* Tile partitioning sequences, in macroblocks */
  UINT16 * tile_width_in_mb; /* 8 bits if short_header_flag = TRUE */
  UINT16 * tile_height_in_mb; /* 8 bits if short_header_flag = TRUE */

  /* Output window margins */
  /* Set to zero, unless windowing_flag = TRUE */
  UINT8 top_margin; 	/* 6 bits */
  UINT8 left_margin; 	/* 6 bits */
  UINT8 bottom_margin; 	/* 6 bits */
  UINT8 right_margin; 	/* 6 bits */
    
} jxr_image_header;

/* Internal color formats */
#define JXR_INTERNAL_CLR_FMT_DEF(XX) \
		XX(JINTCOL_YONLY, 		) \ 
		XX(JINTCOL_YUV420, 	  ) \
		XX(JINTCOL_YUV422, 	  ) \
		XX(JINTCOL_YUV444, 	  ) \
		XX(JINTCOL_YUVK,      ) \
		XX(JINTCOL_RESERVED1, ) \
		XX(JINTCOL_NCOMPONENT,) \ 
		XX(JINTCOL_RESERVED2, )
DECLARE_ENUM(JXR_INTERNAL_CLR_FMT,JXR_INTERNAL_CLR_FMT_DEF)

/* Frequency bands present */
#define JXR_BANDS_PRESENT_DEF(XX) \
    XX(JBANDS_ALL,		   ) \ 
    XX(JBANDS_NOFLEXBITS,) \	
    XX(JBANDS_NOHIGHPASS,) \	
    XX(JBANDS_DCONLY,		 ) \ 
    XX(JBANDS_RESERVED,  )
DECLARE_ENUM(JXR_BANDS_PRESENT,JXR_BANDS_PRESENT_DEF)

/* Component mode, specifies whether the same QP set should be used 
 * across components. */
 #define JXR_COMPONENT_MODE_DEF(XX) \
      XX(JCOMPMODE_UNIFORM,      =0)	\
      XX(JCOMPMODE_SEPARATE,       )	\
      XX(JCOMPMODE_INDEPENDENT,    )	\
      XX(JCOMPMODE_RESERVED,	   )
DECLARE_ENUM(JXR_COMPONENT_MODE,JXR_COMPONENT_MODE_DEF)

/* QP (quantisation parameter) set */
typedef struct {
  
 /* Component mode, defaults to UNIFORM if NumComponents = 1 */
  JXR_COMPONENT_MODE component_mode;
  
  /* If component mode is UNIFORM */
  UINT8 quant;
  /* If component_mode is SEPARATE */
  UINT8 quant_luma;
  UINT8 quant_chroma;
  /* If component_mode is INDEPENDENT */
  /* Head of a list of quantisation parameters, one for each component. */
  UINT8 * quant_ch;

} jxr_qp_set;

/* Image plane header, for both image and alpha planes */
typedef struct {
  
  /* Colour format of the plane */
  JXR_INTERNAL_CLR_FMT internal_clr_fmt;
  /* Is scaling performed in the output formatting stage? */
  boolean scaled_flag;
  /* Which frequency bands are present in the codestream */
  JXR_BANDS_PRESENT bands_present;
  
  /* Chroma sampling grid alignment */
  /* When not present, set to default value of 0. Use is not required
   * for decoder conformance so we ignore in this decoder, though they
   * could be useful when upsampling. */
  /* X-aligment, only present when internal_clr_fmt is YUV420 or YUV422 */
  UINT8 chroma_centering_x; /* 4 bits */
  /* Y-alignment, only present when internal_clr_fmt is YUV420 */
  UINT8 chroma_centering_y; /* 4 bits */
  
  /* If the colour space is generic n-component, these specify the
   * number of components. */
  UINT8 num_components_minus1; /* 4 bits */
  /* Used if 16 or more components */
  UINT16 num_components_extended_minus16; /* 12 bits */

  /* Fields for output formatting */
  /* Present if output_bitdepth is BD16, BD16S or BD32S */
  UINT8 shift_bits;
  /* Present if output_bitdepth is BD32F */
  UINT8 len_mantissa;
  INT8 exp_bias;

  /* Do we use a single QP (quantisation parameter) set for the DC
   * bands of all macroblocks in this plane? */
  boolean dc_image_plane_uniform_flag;
  /* If so, specify the QP set. Otherwise a QP set will be specified in
   * each tile block. */
  jxr_qp_set dc_qp;
  
  /* If low pass bands are present, do we use a single QP (quantisation
   * parameter) set for the LP bands of all macroblocks in this plane? */
  boolean lp_image_plane_uniform_flag;
  /* If so, specify a single QP set. Otherwise QP sets will be specified in
   * each tile block, possibly more than one per tile. */
  jxr_qp_set lp_qp;
  
  /* If high pass bands are present, do we use a single QP (quantisation
   * parameter) set for the HP bands of all macroblocks in this plane? */
  boolean hp_image_plane_uniform_flag;
  /* If so, specify a single QP set. Otherwise QP sets will be specified in
   * each tile block, possibly more than one per tile. */
  jxr_qp_set hp_qp;  
  
  /* Reserved for future specifications */
  /* Ignored, to enable future definition of backwards compatible usage. */
  /* Only present when internal_clr_fmt is YUV420 or YUV422 */
  boolean reserved_e_bit;  
  /* Only present when internal_clr_fmt is YUV444 */
  UINT8 reserved_f; /* 4 bits */
  /* Only present when internal_clr_fmt is YUV420 */
  boolean reserved_g_bit;
  /* Only present when internal_clr_fmt is YUV444, YUV422 or NCOMPONENT */
  UINT8 reserved_h; /* 4 bits */
  /* Only present when bands_present is not DCONLY */
  boolean reserved_i_bit;
  /* Only present when bands_present is not DCONLY or NOHIGHPASS */
  boolean reserved_j_bit;

} jxr_image_plane_header;

/* Image variables */
typedef struct {
	
	/* ith entry specifies the extended image width of component i. */
	UINT32 * extended_width;
	/* ith entry holds the extended image height of component i. */
	UINT32 * extended_height;
	
	/* nth entry specifies the offset of the n-th tile packet from the
	 * start of the coded image data in bytes. */
	UINT16 * index_offset_tile;

	/* For each specific triple (i, x, y), where:
	 * 	0 <= i < NumComponents
	 * 	0 <= x < ExtendedWidth[i]
	 * 	0 <= y < ExtendedHeight[i]
	 * the associated variable image_primary[i][x][y] holds the image
	 * plane sample values associated with the component i, located at
	 * the sample position indicated by the values x and y, for the 
	 * primary image plane.  */
	UINT16 *** image_primary;
	/* For each specific triple (i, x, y), where:
	 * 		i = 0, 0 <= x < extended_width[i]
	 * 		0 <= y < extended_height[i]
	 * this variable holds the image plane sample value, at the sample
	 * position determined by the values x and y, for the alpha image
	 * plane. */
	UINT16 *** image_alpha;

	/* Holds the value associated with the number of vertical macroblock 
	 * partitions. */
	UINT16 mb_height;
	/* Holds the value associated with the number of horizontal macroblock 
	 * partitions. */
	UINT16 mb_width;

	/* Holds the value associated with the number of tile partitions in 
	 * the image horizontally. */
	UINT16 num_tile_cols;

	/* Holds the value associated with the number of tile partitions in 
	 * the image vertically. */
	UINT16 num_tile_rows;

	/* ith entry holds the value associated with the macroblock index of
	 * the top macroblock row
	of the i-th tile row. */
	UINT16 * top_mb_index_of_tile;
	/* ith entry holds the value associated with the macroblock index of 
	 * the left macroblock
	column of the j-th tile column. */
	UINT16 * left_mb_index_of_tile;
	/* nth entry holds the value associated with the number of macroblocks
	 * in the n-th tile. */
	UINT16 * num_mb_in_tile;

	/* Holds the value associated with the value of num_bands (defined
	 * in for the primary image plane.  */
	UINT16 num_bands_of_primary;

	/* Holds the value associated with the number of bytes of subsequent
	 * data that precede the coded_tiles syntax element and follow the
	 * image plane headers and the tiles index table.  */
	UINT16 subsequent_bytes;

} jxr_image_vars;

/* Image plane variables */
typedef struct {
	/* Equal to TRUE if the current plane that is being
	 * parsed or decoded is the alpha image plane; otherwise, this variable
	 * is equal to FALSE. Also used to specify which set of image
	 * plane variables, tile variables, and macroblock variables are being
	 * referenced. */
	UINT16 is_curr_plane_alpha_flag;

	/* Holds the value associated with the number of color
	 * components present in the codestream for the current image plane. For
	 * the primary image plane, its value can be obtained by calling
	 * DetermineNumComponents( ) (subclause 8.4.11). For the alpha image
	 * plane, its value is equal to 1. */
	UINT16 num_components;

	/* Holds the value associated with the number of frequency
	 * bands present in the codestream for the current image plane. Its value
	 * can be obtained by calling DetermineNumBands( ) (subclause 8.4.4). */
	UINT16 num_bands;

	/* Holds the value associated with the number of low pass
	 * QP sets. This variable may have a constant value over an image plane
	 * or it may vary from tile to tile. */
	UINT16 num_lp_qps;

	/* Holds the value associated with the number of high pass
	 * QP sets. This variable may have a constant value over an image plane
	 * or it may vary from tile to tile. */
	UINT16 num_hp_qps;

	/* mb_qp_index_lp[MBx][MBy] holds the QP index into the table of
	 * quantization parameters for LP coefficients, corresponding to the
	 * macroblock indexed by MBx and MBy. The same index applies for all
	 * color components. */
	UINT16 ** mb_qp_index_lp;

	/* mb_qp_index_hp[MBx][MBy] holds the QP index into the table of
	 * quantization parameters for HP coefficients, corresponding to the
	 * macroblock indexed by MBx and MBy. The same index applies for all
	 * color components. */
	UINT16 ** mb_qp_index_hp;

	/* Given entry mb_dc_lp[MBx][MBy][i][j]: when j is equal to 0, this variable
	 * holds the DC transform coefficient for the macroblock indexed by MBx
	 * and MBy, associated with the color component indexed by i. For non-zero
	 * values of the index j, this variable holds the j-th LP transform
	 * coefficient for the macroblock indexed by MBx and MBy, and associated
	 * with the color component indexed by i. The index j ranges from 0 to 15
	 * for luma components of all color formats and chroma components of all
	 * color formats except YUV 4:2:0 and YUV 4:2:2. In the YUV 4:2:0 chroma
	 * component case, j ranges from 0 to 3, and in the YUV 4:2:2 chroma
	 * component case, j ranges from 0 to 7. */
	UINT16 **** mb_dc_lp;

	/* Given entry mb_buffer[MBx][MBy][i][j]: this variable holds the j-th
	 * transform coefficient - associated with the color component i − for
	 * the macroblock indexed by MBx and MBy. The index j ranges from 0 to 255.
	 * The ordering of the 256 transform coefficients in the macroblock is as
	 * follows: let iBlkIndex represent the block index of a 4×4 block of
	 * component i in the macroblock, indexed in raster scan order, with
	 * iBlkIndex ranging from 0 to 15. Then the 16 transform coefficients
	 * for this block (indexed in raster scan order in the block) are
	 * represented by the values of MBBuffer[MBx][MBy][i][j], where j ranges
	 * from (16*iBlkIndex + 0) to (16*iBlkIndex + 15), inclusive. */
	UINT16 **** mb_buffer;

	/* Given entry mb_cbp_hp[MBx][MBy][i]: this variable holds the coded block
	 * status for the macroblock indexed by MBx and MBy, associated with the
	 * color component indexed by i. The association of a bit of this variable
	 * to the block in the macroblock is specified in subclause 8.7.17.1, and
	 * a bit takes the value 1 if the corresponding block has non-zero HP
	 * transform coefficients to be scanned. */
	UINT16 *** mb_cbp_hp;

	/* Given entry pred_dc_lp[MBx][MBy][i][j]: this variable holds the predicted
	 * DC and LP coefficient values, for the macroblock indexed by MBx and MBy,
	 * associated with the color component indexed by i; the index j ranges
	 * from 0 to 6. The predicted DC value corresponds to index 0. */
	UINT16 **** pred_dc_lp;

	/* Given entry model_bits_mb_hp[MBx][MBy][i]: this variable holds the
	 * value of the member variable MBits[i], associated with the data structure
	 * ModelHP as defined in subclause 5.5.6, for the macroblock indexed by
	 * MBx and MBy. The index i ranges from 0 to 1. For each macroblock, two
	 * values are stored: one value for the luma component, and one value for
	 * the chroma components. NOTE – The values ModelBitsMBHP[MBx][MBy][i]
	 * are used in the parsing process to communicate the state between the
	 * parsing of the syntax structures MB_HP( ) and MB_FLEXBITS( ). */
	UINT16 *** model_bits_mb_hp;

	/* Given entry image_plane[i][x][y]: this variable holds the sample value
	 * associated with the color component i, located at the position
	indicated by the values x and y, where:
	* 		0 <= i < num_components,
	* 		0 <= x < extended_width[i]
	* 		0 <= y extended_height[i]
	* for the current image plane being decoded. */
	UINT16 *** image_plane;

}  jxr_image_plane_vars;

/* Tile variables */
typedef struct {
	
} jxr_tile_vars;

/* Macroblock variables */
typedef struct {

} jxr_mb_vars;

/* Master record for a JPEG-XR coded image decompression instance */
struct jpegxr_image_struct {
  jpegxr_common_fields;		/* Fields shared with jpegxr_compress_struct */

  /* Start of instance in data source */
  unsigned int offset;
  
  /* Image variables */
  jxr_image_vars 	* vars;
  
  /* Headers and variables for each (image and alpha) plane */
  jxr_image_plane_vars * image_plane_vars;
  jxr_image_plane_vars * alpha_plane_vars; /* NULL if no alpha plane */

  /* Tile variables */
  jxr_tile_vars * tile_vars;
  
  /* Macroblock variables */
  jxr_mb_vars * mb_vars;
  
};


/* Data types for JPEG-XR directory instances */

/* Field tags for IFD entries */
#define JXR_FIELD_TAG_DEF(XX) \
    XX(JFIELDTAG_DOCUMENT_NAME,=0x010D) \
    XX(JFIELDTAG_IMAGE_DESCRIPTION,=0x010E) \
    XX(JFIELDTAG_EQUIPMENT_MAKE,=0x010F) \
    XX(JFIELDTAG_EQUIPMENT_MODEL,=0x0110) \
    XX(JFIELDTAG_PAGE_NAME,=0x011D) \
    XX(JFIELDTAG_PAGE_NUMBER,=0x0129) \
    XX(JFIELDTAG_SOFTWARE_NAME_VERSION,=0x0131) \
    XX(JFIELDTAG_DATE_TIME,=0x0132) \
    XX(JFIELDTAG_ARTIST_NAME,=0x013B) \
    XX(JFIELDTAG_HOST_COMPUTER,=0x013C) \
    XX(JFIELDTAG_COPYRIGHT_NOTICE,=0x8298) \
    XX(JFIELDTAG_COLOR_SPACE,=0xA001) \
    XX(JFIELDTAG_PIXEL_FORMAT,=0xBC01) \
    XX(JFIELDTAG_SPATIAL_XFRM_PRIMARY,=0xBC02) \
    XX(JFIELDTAG_IMAGE_TYPE,=0xBC04) \
    XX(JFIELDTAG_PTM_COLOR_INFO,=0xBC05) \
    XX(JFIELDTAG_PROFILE_LEVEL_CONTAINER,=0xBC06) \
    XX(JFIELDTAG_IMAGE_WIDTH,=0xBC80) \
    XX(JFIELDTAG_IMAGE_HEIGHT,=0xBC81) \
    XX(JFIELDTAG_WIDTH_RESOLUTION,=0xBC82) \
    XX(JFIELDTAG_HEIGHT_RESOLUTION,=0xBC83) \
    XX(JFIELDTAG_IMAGE_OFFSET,=0xBCC0) \
    XX(JFIELDTAG_IMAGE_BYTE_COUNT,=0xBCC1) \
    XX(JFIELDTAG_ALPHA_OFFSET,=0xBCC2) \
    XX(JFIELDTAG_ALPHA_BYTE_COUNT,=0xBCC3) \
    XX(JFIELDTAG_IMAGE_BAND_PRESENCE,=0xBCC4) \
    XX(JFIELDTAG_ALPHA_BAND_PRESENCE,=0xBCC5) \
    XX(JFIELDTAG_PADDING_DATA,=0xEA1C)
DECLARE_ENUM(JXR_FIELD_TAG,JXR_FIELD_TAG_DEF)

/* Element type for IFD entries */
typedef enum {
      JELEMTYPE_BYTE=1,
      JELEMTYPE_UTF8,
      JELEMTYPE_USHORT,
      JELEMTYPE_ULONG,
      JELEMTYPE_URATIONAL,
      JELEMTYPE_SBYTE,
      JELEMTYPE_UNDEFINED,
      JELEMTYPE_SSHORT,
      JELEMTYPE_SLONG,
      JELEMTYPE_SRATIONAL,
      JELEMTYPE_FLOAT,
      JELEMTYPE_DOUBLE
} JXR_ELEMENT_TYPE;


/* Image file directory entry */
typedef struct {
    JXR_FIELD_TAG field_tag;
    JXR_ELEMENT_TYPE element_type;
    UINT32 num_elements;
    UINT32 values_or_offset;
} ifd_entry;

/* Master record for a JPEG-XR directory decompression instance */
struct jpegxr_dir_struct {
  jpegxr_common_fields;		/* Fields shared with jpegxr_compress_struct */
  
  /* Start of instance in data source */
  unsigned int offset;
  
  /* Number of entries in the directory */
  UINT16 num_entries;
  
  /* Required entry values */
  /* Currently these are the only entries supported. All directories
   * must contain at least these entries. TODO - Intepret pixel
   * format. */
  UINT8 pixel_format[16];
  UINT32 image_width; 
  UINT32 image_height;
  UINT32 image_offset; // where the coded image appears
  UINT32 image_byte_count; // length of coded image
  
  /* Coded image instances */
  /* Currently we only support one per directory, but there could be at
   * least an alpha image also. */
  j_image_ptr image;
  //j_image_ptr alpha_image;
  
  /* Next directory in codestream, or zero if none. */
  UINT32 zero_or_next_ifd_offset;
  
};



/* Data types for JPEG-XR file instances */

/* Master record for a JPEG-XR file decompression instance */
struct jpegxr_file_struct {
  jpegxr_common_fields;	/* Fields shared with jpegxr_compress_struct */
  
  /* File version ID */
  /* Should be equal to (or greater than) JXR_FILE_VERSION */
  UINT8 file_version_id;
  /* Position of first image file directory, from start of file. */
  UINT32 first_ifd_offset;

  /* Image file directories */
  unsigned int num_dirs;
  /* pointer to head of list */
  j_dir_ptr * dirs; // currently only one supported
  
};




/* "Object" declarations for JPEG modules that may be supplied or called
 * directly by the surrounding application.
 * As with all objects in the JPEG library, these structs only define the
 * publicly visible methods and state variables of a module.  Additional
 * private fields may exist after the public ones.
 */

/* Error handler object */
struct jpeg_error_mgr {
  /* Error exit handler: does not return to caller */
  JMETHOD(void, error_exit, (j_common_ptr cinfo));
  /* Conditionally emit a trace or warning message */
  JMETHOD(void, emit_message, (j_common_ptr cinfo, int msg_level));
  /* Routine that actually outputs a trace or error message */
  JMETHOD(void, output_message, (j_common_ptr cinfo, int indent_level));
  /* Format a message string for the most recent JPEG error or message */
  JMETHOD(void, format_message, (j_common_ptr cinfo, char * buffer));
#define JMSG_LENGTH_MAX  200	/* recommended size of format_message buffer */
  /* Reset error state variables at start of a new image */
  JMETHOD(void, reset_error_mgr, (j_common_ptr cinfo));
  
  /* The message ID code and any parameters are saved here.
   * A message can have one string parameter or up to 8 int parameters.
   */
  int msg_code;
#define JMSG_STR_PARM_MAX  80
  union {
    int i[8];
    char s[JMSG_STR_PARM_MAX];
  } msg_parm;
  
  /* Standard state variables for error facility */
  
  int trace_level;		/* max msg_level that will be displayed */
  
  /* For recoverable corrupt-data errors, we emit a warning message,
   * but keep going unless emit_message chooses to abort.  emit_message
   * should count warnings in num_warnings.  The surrounding application
   * can check for bad data by seeing if num_warnings is nonzero at the
   * end of processing.
   */
  long num_warnings;		/* number of corrupt-data warnings */

  /* These fields point to the table(s) of error message strings.
   * An application can change the table pointer to switch to a different
   * message list (typically, to change the language in which errors are
   * reported).  Some applications may wish to add additional error codes
   * that will be handled by the JPEG library error mechanism; the second
   * table pointer is used for this purpose.
   *
   * First table includes all errors generated by JPEG library itself.
   * Error code 0 is reserved for a "no such error string" message.
   */
  const char * const * jpeg_message_table; /* Library errors */
  int last_jpeg_message;    /* Table contains strings 0..last_jpeg_message */
  /* Second table can be added by application (see cjpeg/djpeg for example).
   * It contains strings numbered first_addon_message..last_addon_message.
   */
  const char * const * addon_message_table; /* Non-library errors */
  int first_addon_message;	/* code for first string in addon table */
  int last_addon_message;	/* code for last string in addon table */
};

/* Progress monitor object */
struct jpeg_progress_mgr {
  JMETHOD(void, progress_monitor, (j_common_ptr cinfo));

  long pass_counter;		/* work units completed in this pass */
  long pass_limit;		/* total number of work units in this pass */
  int completed_passes;		/* passes completed so far */
  int total_passes;		/* total number of passes expected */
};

/* Data source object for decompression */
struct jpeg_source_mgr {
  const JOCTET * next_input_byte; /* => next byte to read from buffer */
  size_t bytes_in_buffer;	/* # of bytes remaining in buffer */
  long idx;	/* Index from the start of the source stream  */
  unsigned char bit_idx;

  JMETHOD(void, init_source, (j_common_ptr cinfo));
  JMETHOD(boolean, fill_input_buffer, (j_common_ptr cinfo));
  JMETHOD(void, skip_input_data, (j_common_ptr cinfo, long num_bytes));
  JMETHOD(void, rewind_input_data, (j_common_ptr cinfo, long num_bytes));
  JMETHOD(void, seek_input_data, (j_common_ptr cinfo, long offset));
  JMETHOD(boolean, resync_to_restart, (j_common_ptr cinfo, int desired));
  JMETHOD(void, term_source, (j_common_ptr cinfo));
};


/* Memory manager object.
 * Allocates "small" objects (a few K total), "large" objects (tens of K),
 * and "really big" objects (virtual arrays with backing store if needed).
 * The memory manager does not allow individual objects to be freed; rather,
 * each created object is assigned to a pool, and whole pools can be freed
 * at once.  This is faster and more convenient than remembering exactly what
 * to free, especially where malloc()/free() are not too speedy.
 * NB: alloc routines never return NULL.  They exit to error_exit if not
 * successful.
 */

#define JPOOL_PERMANENT	0	/* lasts until master record is destroyed */
#define JPOOL_IMAGE	1	/* lasts until done with image/datastream */
#define JPOOL_NUMPOOLS	2

typedef struct jvirt_sarray_control * jvirt_sarray_ptr;
typedef struct jvirt_barray_control * jvirt_barray_ptr;

struct jpeg_memory_mgr {
  /* Method pointers */
  JMETHOD(void *, alloc_small, (j_common_ptr cinfo, int pool_id,
				size_t sizeofobject));
  JMETHOD(void FAR *, alloc_large, (j_common_ptr cinfo, int pool_id,
				     size_t sizeofobject));
  JMETHOD(JSAMPARRAY, alloc_sarray, (j_common_ptr cinfo, int pool_id,
				     JDIMENSION samplesperrow,
				     JDIMENSION numrows));
  JMETHOD(JBLOCKARRAY, alloc_barray, (j_common_ptr cinfo, int pool_id,
				      JDIMENSION blocksperrow,
				      JDIMENSION numrows));
  JMETHOD(jvirt_sarray_ptr, request_virt_sarray, (j_common_ptr cinfo,
						  int pool_id,
						  boolean pre_zero,
						  JDIMENSION samplesperrow,
						  JDIMENSION numrows,
						  JDIMENSION maxaccess));
  JMETHOD(jvirt_barray_ptr, request_virt_barray, (j_common_ptr cinfo,
						  int pool_id,
						  boolean pre_zero,
						  JDIMENSION blocksperrow,
						  JDIMENSION numrows,
						  JDIMENSION maxaccess));
  JMETHOD(void, realize_virt_arrays, (j_common_ptr cinfo));
  JMETHOD(JSAMPARRAY, access_virt_sarray, (j_common_ptr cinfo,
					   jvirt_sarray_ptr ptr,
					   JDIMENSION start_row,
					   JDIMENSION num_rows,
					   boolean writable));
  JMETHOD(JBLOCKARRAY, access_virt_barray, (j_common_ptr cinfo,
					    jvirt_barray_ptr ptr,
					    JDIMENSION start_row,
					    JDIMENSION num_rows,
					    boolean writable));
  JMETHOD(void, free_pool, (j_common_ptr cinfo, int pool_id));
  JMETHOD(void, self_destruct, (j_common_ptr cinfo));

  /* Limit on memory allocation for this JPEG object.  (Note that this is
   * merely advisory, not a guaranteed maximum; it only affects the space
   * used for virtual-array buffers.)  May be changed by outer application
   * after creating the JPEG object.
   */
  long max_memory_to_use;

  /* Maximum allocation request accepted by alloc_large. */
  long max_alloc_chunk;
};


/* Declarations for routines called by application.
 * The JPP macro hides prototype parameters from compilers that can't cope.
 * Note JPP requires double parentheses.
 */
#ifdef HAVE_PROTOTYPES
#define JPP(arglist)	arglist
#else
#define JPP(arglist)	()
#endif


/* Default error-management setup */
EXTERN(struct jpeg_error_mgr *) jpeg_std_error
	JPP((struct jpeg_error_mgr * err));


/* Initialization of JPEG compression objects.
 * jpeg_create_compress() and jpegxr_create_decompress() are the exported
 * names that applications should call.  These expand to calls on
 * jpeg_CreateCompress and jpegxr_CreateDecompress with additional information
 * passed for version mismatch checking.
 * NB: you must set up the error-manager BEFORE calling jpeg_create_xxx.
 */
#define jpegxr_file_create_decompress(finfo) \
    jpegxr_file_CreateDecompress((finfo), JPEGXR_LIB_VERSION, \
			  (size_t) sizeof(struct jpegxr_file_struct))
EXTERN(void) jpegxr_file_CreateDecompress JPP((j_file_ptr finfo,
					int version, size_t structsize));
					
#define jpegxr_dir_create_decompress(dinfo) \
    jpegxr_dir_CreateDecompress((dinfo), JPEGXR_LIB_VERSION, \
			  (size_t) sizeof(struct jpegxr_dir_struct))
EXTERN(void) jpegxr_dir_CreateDecompress JPP((j_dir_ptr dinfo,
					int version, size_t structsize));
          
#define jpegxr_image_create_decompress(iinfo) \
    jpegxr_image_CreateDecompress((iinfo), JPEGXR_LIB_VERSION, \
			  (size_t) sizeof(struct jpegxr_image_struct))
EXTERN(void) jpegxr_image_CreateDecompress JPP((j_image_ptr iinfo,
					int version, size_t structsize));

/* Standard data source manager: stdio streams. */
/* Caller is responsible for opening the file before and closing after. */
EXTERN(void) jpeg_stdio_src JPP((j_common_ptr cinfo, FILE * infile));


/* Read metadata from a .jxr file stream to obtain details of directories
 * and coded images contained within. */
EXTERN(int) jpegxr_file_read_metadata JPP((j_file_ptr finfo));
/* Read start of .jxr file stream */
EXTERN(int) jpegxr_file_read_header JPP((j_file_ptr finfo));

/* Read metadata from a directory stream to obtain details of coded
 * image codestreams contained within. */
EXTERN(int) jpegxr_dir_read_metadata JPP((j_dir_ptr dinfo));
/* Read start of a JPEG-XR directory stream. */
EXTERN(int) jpegxr_dir_read_header JPP((j_dir_ptr dinfo));

/* Read the metadata from a coded image codestream to obtain decompression
 * parameters and details of macroblock and tile layers.*/
EXTERN(int) jpegxr_image_read_metadata JPP((j_image_ptr iinfo));


/* Destroy JPEG-XR file object. */
EXTERN(void) jpegxr_file_destroy JPP((j_file_ptr cinfo));
/* Generic version */
EXTERN(void) jpegxr_destroy JPP((j_common_ptr cinfo));

          
/* Return value is one of: */
#define JPEG_SUSPENDED		0 /* Suspended due to lack of input data */
#define JPEG_HEADER_OK		1 /* Found valid image datastream */
#define JPEG_HEADER_TABLES_ONLY	2 /* Found valid table-specs-only datastream */
/* If you pass require_image = TRUE (normal case), you need not check for
 * a TABLES_ONLY return code; an abbreviated file will cause an error exit.
 * JPEG_SUSPENDED is only possible if you use a data source module that can
 * give a suspension return (the stdio source module doesn't).
 */
 
/* Return value is one of: */
/* #define JPEG_SUSPENDED	0    Suspended due to lack of input data */
#define JPEG_REACHED_SOS	1 /* Reached start of new scan */
#define JPEG_REACHED_EOI	2 /* Reached end of image */
#define JPEG_ROW_COMPLETED	3 /* Completed one iMCU row */
#define JPEG_SCAN_COMPLETED	4 /* Completed last iMCU row of a scan */

              
/* These marker codes are exported since applications and data source modules
 * are likely to want to use them.
 */
#define JPEG_RST0	0xD0	/* RST0 marker code */
#define JPEG_EOI	0xD9	/* EOI marker code */
#define JPEG_APP0	0xE0	/* APP0 marker code */
#define JPEG_COM	0xFE	/* COM marker code */

/*
 * The JPEG library modules define JPEG_INTERNALS before including this file.
 * The internal structure declarations are read only when that is true.
 * Applications using the library should not include jpegint.h, but may wish
 * to include jerror.h.
 */

#ifdef JPEG_INTERNALS
#include "jpegint.h"		/* fetch private declarations */
#include "jerror.h"		/* fetch error codes too */
#endif

#endif /* JPEGLIBXR_H */
