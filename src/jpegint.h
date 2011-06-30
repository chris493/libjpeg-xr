/*
 * jpegint.h
 *
 * Copyright (C) 1991-1997, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file provides common declarations for the various JPEG modules.
 * These declarations are considered internal to the JPEG library; most
 * applications using the library shouldn't need to include this file.
 */


/* Declarations for both compression & decompression */

typedef enum {			/* Operating modes for buffer controllers */
	JBUF_PASS_THRU,		/* Plain stripwise operation */
	/* Remaining modes require a full-image buffer to have been created */
	JBUF_SAVE_SOURCE,	/* Run source subobject only, save output */
	JBUF_CRANK_DEST,	/* Run dest subobject only, using saved data */
	JBUF_SAVE_AND_PASS	/* Run both subobjects, save output */
} J_BUF_MODE;

/* Values of global_state field (jdapi.c has some dependencies on ordering!) */
#define CSTATE_START	100	/* after create_compress */
#define CSTATE_SCANNING	101	/* start_compress done, write_scanlines OK */
#define CSTATE_RAW_OK	102	/* start_compress done, write_raw_data OK */
#define CSTATE_WRCOEFS	103	/* jpeg_write_coefficients done */
#define DSTATE_START	200	/* after create_decompress */
#define DSTATE_INHEADER	201	/* reading header markers, no SOS yet */
#define DSTATE_READY	202	/* found SOS, ready for start_decompress */
#define DSTATE_PRELOAD	203	/* reading multiscan file in start_decompress*/
#define DSTATE_PRESCAN	204	/* performing dummy pass for 2-pass quant */
#define DSTATE_SCANNING	205	/* start_decompress done, read_scanlines OK */
#define DSTATE_RAW_OK	206	/* start_decompress done, read_raw_data OK */
#define DSTATE_BUFIMAGE	207	/* expecting jpeg_start_output */
#define DSTATE_BUFPOST	208	/* looking for SOS/EOI in jpeg_finish_output */
#define DSTATE_RDCOEFS	209	/* reading file in jpeg_read_coefficients */
#define DSTATE_STOPPING	210	/* looking for EOI in jpeg_finish_decompress */


/* Miscellaneous useful macros */

#undef MAX
#define MAX(a,b)	((a) > (b) ? (a) : (b))
#undef MIN
#define MIN(a,b)	((a) < (b) ? (a) : (b))


/* We assume that right shift corresponds to signed division by 2 with
 * rounding towards minus infinity.  This is correct for typical "arithmetic
 * shift" instructions that shift in copies of the sign bit.  But some
 * C compilers implement >> with an unsigned shift.  For these machines you
 * must define RIGHT_SHIFT_IS_UNSIGNED.
 * RIGHT_SHIFT provides a proper signed right shift of an INT32 quantity.
 * It is only applied with constant shift counts.  SHIFT_TEMPS must be
 * included in the variables of any routine using RIGHT_SHIFT.
 */

#ifdef RIGHT_SHIFT_IS_UNSIGNED
#define SHIFT_TEMPS	INT32 shift_temp;
#define RIGHT_SHIFT(x,shft)  \
	((shift_temp = (x)) < 0 ? \
	 (shift_temp >> (shft)) | ((~((INT32) 0)) << (32-(shft))) : \
	 (shift_temp >> (shft)))
#else
#define SHIFT_TEMPS
#define RIGHT_SHIFT(x,shft)	((x) >> (shft))
#endif


/* Memory manager initialization */
EXTERN(void) jinit_memory_mgr JPP((j_common_ptr cinfo));

/* Utility routines in jutils.c */
EXTERN(long) jdiv_round_up JPP((long a, long b));
EXTERN(long) jround_up JPP((long a, long b));
EXTERN(void) jcopy_sample_rows JPP((JSAMPARRAY input_array, int source_row,
				    JSAMPARRAY output_array, int dest_row,
				    int num_rows, JDIMENSION num_cols));
EXTERN(void) jcopy_block_row JPP((JBLOCKROW input_row, JBLOCKROW output_row,
				  JDIMENSION num_blocks));
EXTERN(void) jzero_far JPP((void FAR * target, size_t bytestozero));
/* Constant tables in jutils.c */
#if 0				/* This table is not actually needed in v6a */
extern const int jpeg_zigzag_order[]; /* natural coef order to zigzag order */
#endif
extern const int jpeg_natural_order[]; /* zigzag coef order to natural order */

/* Suppress undefined-structure complaints if necessary. */

#ifdef INCOMPLETE_TYPES_BROKEN
#ifndef AM_MEMORY_MANAGER	/* only jmemmgr.c defines these */
struct jvirt_sarray_control { long dummy; };
struct jvirt_barray_control { long dummy; };
#endif
#endif /* INCOMPLETE_TYPES_BROKEN */
