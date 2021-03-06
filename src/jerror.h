/*
 * jerror.h
 *
 * Copyright (C) 1994-1997, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file defines the error and message codes for the JPEG library.
 * Edit this file to add new codes, or to translate the message strings to
 * some other language.
 * A set of error-reporting macros are defined too.  Some applications using
 * the JPEG library may wish to include this file to get the error codes
 * and/or the macros.
 */

/*
 * To define the enum list of message codes, include this file without
 * defining macro JMESSAGE.  To create a message string table, include it
 * again with a suitable JMESSAGE definition (see jerror.c for an example).
 */
#ifndef JMESSAGE
#ifndef JERROR_H
/* First time through, define the enum list */
#define JMAKE_ENUM_LIST
#else
/* Repeated inclusions of this file are no-ops unless JMESSAGE is defined */
#define JMESSAGE(code,string)
#endif /* JERROR_H */
#endif /* JMESSAGE */

#ifdef JMAKE_ENUM_LIST

typedef enum {

#define JMESSAGE(code,string)	code ,

#endif /* JMAKE_ENUM_LIST */

JMESSAGE(JMSG_NOMESSAGE, "Bogus message code %d") /* Must be first entry! */

/* For maintenance convenience, list is alphabetical by message code name */
JMESSAGE(JERR_ARITH_NOTIMPL,
	 "Sorry, there are legal restrictions on arithmetic coding")
JMESSAGE(JERR_BAD_ALIGN_TYPE, "ALIGN_TYPE is wrong, please fix")
JMESSAGE(JERR_BAD_ALLOC_CHUNK, "MAX_ALLOC_CHUNK is wrong, please fix")
JMESSAGE(JERR_BAD_BUFFER_MODE, "Bogus buffer control mode")
JMESSAGE(JERR_BAD_COMPONENT_ID, "Invalid component ID %d in SOS")
JMESSAGE(JERR_BAD_DCT_COEF, "DCT coefficient out of range")
JMESSAGE(JERR_BAD_DCTSIZE, "IDCT output block size %d not supported")
JMESSAGE(JERR_BAD_HUFF_TABLE, "Bogus Huffman table definition")
JMESSAGE(JERR_BAD_IN_COLORSPACE, "Bogus input colorspace")
JMESSAGE(JERR_BAD_J_COLORSPACE, "Bogus JPEG colorspace")
JMESSAGE(JERR_BAD_LENGTH, "Bogus marker length")
JMESSAGE(JERR_BAD_LIB_VERSION,
	 "Wrong JPEG library version: library is %d, caller expects %d")
JMESSAGE(JERR_BAD_MCU_SIZE, "Sampling factors too large for interleaved scan")
JMESSAGE(JERR_BAD_POOL_ID, "Invalid memory pool code %d")
JMESSAGE(JERR_BAD_PRECISION, "Unsupported JPEG data precision %d")
JMESSAGE(JERR_BAD_PROGRESSION,
	 "Invalid progressive parameters Ss=%d Se=%d Ah=%d Al=%d")
JMESSAGE(JERR_BAD_PROG_SCRIPT,
	 "Invalid progressive parameters at scan script entry %d")
JMESSAGE(JERR_BAD_SAMPLING, "Bogus sampling factors")
JMESSAGE(JERR_BAD_SCAN_SCRIPT, "Invalid scan script at entry %d")
JMESSAGE(JERR_BAD_STATE, "Improper call to JPEG library in state %d")
JMESSAGE(JERR_BAD_STRUCT_SIZE,
	 "JPEG parameter struct mismatch: library thinks size is %u, caller expects %u")
JMESSAGE(JERR_BAD_VIRTUAL_ACCESS, "Bogus virtual array access")
JMESSAGE(JERR_BUFFER_SIZE, "Buffer passed to JPEG library is too small")
JMESSAGE(JERR_CANT_SUSPEND, "Suspension not allowed here")
JMESSAGE(JERR_CCIR601_NOTIMPL, "CCIR601 sampling not implemented yet")
JMESSAGE(JERR_COMPONENT_COUNT, "Too many color components: %d, max %d")
JMESSAGE(JERR_CONVERSION_NOTIMPL, "Unsupported color conversion request")
JMESSAGE(JERR_DAC_INDEX, "Bogus DAC index %d")
JMESSAGE(JERR_DAC_VALUE, "Bogus DAC value 0x%x")
JMESSAGE(JERR_DHT_INDEX, "Bogus DHT index %d")
JMESSAGE(JERR_DQT_INDEX, "Bogus DQT index %d")
JMESSAGE(JERR_EMPTY_IMAGE, "Empty JPEG image (DNL not supported)")
JMESSAGE(JERR_EMS_READ, "Read from EMS failed")
JMESSAGE(JERR_EMS_WRITE, "Write to EMS failed")
JMESSAGE(JERR_EOI_EXPECTED, "Didn't expect more than one scan")
JMESSAGE(JERR_FILE_READ, "Input file read error")
JMESSAGE(JERR_FILE_WRITE, "Output file write error --- out of disk space?")
JMESSAGE(JERR_FRACT_SAMPLE_NOTIMPL, "Fractional sampling not implemented yet")
JMESSAGE(JERR_HUFF_CLEN_OVERFLOW, "Huffman code size table overflow")
JMESSAGE(JERR_HUFF_MISSING_CODE, "Missing Huffman code table entry")
JMESSAGE(JERR_IMAGE_TOO_BIG, "Maximum supported image dimension is %u pixels")
JMESSAGE(JERR_INPUT_EMPTY, "Empty input file")
JMESSAGE(JERR_INPUT_EOF, "Premature end of input file")
JMESSAGE(JERR_MISMATCHED_QUANT_TABLE,
	 "Cannot transcode due to multiple use of quantization table %d")
JMESSAGE(JERR_MISSING_DATA, "Scan script does not transmit all data")
JMESSAGE(JERR_MODE_CHANGE, "Invalid color quantization mode change")
JMESSAGE(JERR_NOTIMPL, "Not implemented yet")
JMESSAGE(JERR_NOT_COMPILED, "Requested feature was omitted at compile time")
JMESSAGE(JERR_NO_BACKING_STORE, "Backing store not supported")
JMESSAGE(JERR_NO_HUFF_TABLE, "Huffman table 0x%02x was not defined")
JMESSAGE(JERR_NO_IMAGE, "JPEG datastream contains no image")
JMESSAGE(JERR_NO_QUANT_TABLE, "Quantization table 0x%02x was not defined")
JMESSAGE(JERR_NO_SOI, "Not a JPEG file: starts with 0x%02x 0x%02x")
JMESSAGE(JERR_OUT_OF_MEMORY, "Insufficient memory (case %d)")
JMESSAGE(JERR_QUANT_COMPONENTS,
	 "Cannot quantize more than %d color components")
JMESSAGE(JERR_QUANT_FEW_COLORS, "Cannot quantize to fewer than %d colors")
JMESSAGE(JERR_QUANT_MANY_COLORS, "Cannot quantize to more than %d colors")
JMESSAGE(JERR_SOF_DUPLICATE, "Invalid JPEG file structure: two SOF markers")
JMESSAGE(JERR_SOF_NO_SOS, "Invalid JPEG file structure: missing SOS marker")
JMESSAGE(JERR_SOF_UNSUPPORTED, "Unsupported JPEG process: SOF type 0x%02x")
JMESSAGE(JERR_SOI_DUPLICATE, "Invalid JPEG file structure: two SOI markers")
JMESSAGE(JERR_SOS_NO_SOF, "Invalid JPEG file structure: SOS before SOF")
JMESSAGE(JERR_TFILE_CREATE, "Failed to create temporary file %s")
JMESSAGE(JERR_TFILE_READ, "Read failed on temporary file")
JMESSAGE(JERR_TFILE_SEEK, "Seek failed on temporary file")
JMESSAGE(JERR_TFILE_WRITE,
	 "Write failed on temporary file --- out of disk space?")
JMESSAGE(JERR_TOO_LITTLE_DATA, "Application transferred too few scanlines")
JMESSAGE(JERR_UNKNOWN_MARKER, "Unsupported marker type 0x%02x")
JMESSAGE(JERR_VIRTUAL_BUG, "Virtual array controller messed up")
JMESSAGE(JERR_WIDTH_OVERFLOW, "Image too wide for this implementation")
JMESSAGE(JERR_XMS_READ, "Read from XMS failed")
JMESSAGE(JERR_XMS_WRITE, "Write to XMS failed")
JMESSAGE(JMSG_COPYRIGHT, JCOPYRIGHT)
JMESSAGE(JMSG_VERSION, JVERSION)
JMESSAGE(JTRC_16BIT_TABLES,
	 "Caution: quantization tables are too coarse for baseline JPEG")
JMESSAGE(JTRC_ADOBE,
	 "Adobe APP14 marker: version %d, flags 0x%04x 0x%04x, transform %d")
JMESSAGE(JTRC_APP0, "Unknown APP0 marker (not JFIF), length %u")
JMESSAGE(JTRC_APP14, "Unknown APP14 marker (not Adobe), length %u")
JMESSAGE(JTRC_DAC, "Define Arithmetic Table 0x%02x: 0x%02x")
JMESSAGE(JTRC_DHT, "Define Huffman Table 0x%02x")
JMESSAGE(JTRC_DQT, "Define Quantization Table %d  precision %d")
JMESSAGE(JTRC_DRI, "Define Restart Interval %u")
JMESSAGE(JTRC_EMS_CLOSE, "Freed EMS handle %u")
JMESSAGE(JTRC_EMS_OPEN, "Obtained EMS handle %u")
JMESSAGE(JTRC_EOI, "End Of Image")
JMESSAGE(JTRC_HUFFBITS, "        %3d %3d %3d %3d %3d %3d %3d %3d")
JMESSAGE(JTRC_JFIF, "JFIF APP0 marker: version %d.%02d, density %dx%d  %d")
JMESSAGE(JTRC_JFIF_BADTHUMBNAILSIZE,
	 "Warning: thumbnail image size does not match data length %u")
JMESSAGE(JTRC_JFIF_EXTENSION,
	 "JFIF extension marker: type 0x%02x, length %u")
JMESSAGE(JTRC_JFIF_THUMBNAIL, "    with %d x %d thumbnail image")
JMESSAGE(JTRC_MISC_MARKER, "Miscellaneous marker 0x%02x, length %u")
JMESSAGE(JTRC_PARMLESS_MARKER, "Unexpected marker 0x%02x")
JMESSAGE(JTRC_QUANTVALS, "        %4u %4u %4u %4u %4u %4u %4u %4u")
JMESSAGE(JTRC_QUANT_3_NCOLORS, "Quantizing to %d = %d*%d*%d colors")
JMESSAGE(JTRC_QUANT_NCOLORS, "Quantizing to %d colors")
JMESSAGE(JTRC_QUANT_SELECTED, "Selected %d colors for quantization")
JMESSAGE(JTRC_RECOVERY_ACTION, "At marker 0x%02x, recovery action %d")
JMESSAGE(JTRC_RST, "RST%d")
JMESSAGE(JTRC_SMOOTH_NOTIMPL,
	 "Smoothing not supported with nonstandard sampling ratios")
JMESSAGE(JTRC_SOF, "Start Of Frame 0x%02x: width=%u, height=%u, components=%d")
JMESSAGE(JTRC_SOF_COMPONENT, "    Component %d: %dhx%dv q=%d")
JMESSAGE(JTRC_SOI, "Start of Image")
JMESSAGE(JTRC_SOS, "Start Of Scan: %d components")
JMESSAGE(JTRC_SOS_COMPONENT, "    Component %d: dc=%d ac=%d")
JMESSAGE(JTRC_SOS_PARAMS, "  Ss=%d, Se=%d, Ah=%d, Al=%d")
JMESSAGE(JTRC_TFILE_CLOSE, "Closed temporary file %s")
JMESSAGE(JTRC_TFILE_OPEN, "Opened temporary file %s")
JMESSAGE(JTRC_THUMB_JPEG,
	 "JFIF extension marker: JPEG-compressed thumbnail image, length %u")
JMESSAGE(JTRC_THUMB_PALETTE,
	 "JFIF extension marker: palette thumbnail image, length %u")
JMESSAGE(JTRC_THUMB_RGB,
	 "JFIF extension marker: RGB thumbnail image, length %u")
JMESSAGE(JTRC_UNKNOWN_IDS,
	 "Unrecognized component IDs %d %d %d, assuming YCbCr")
JMESSAGE(JTRC_XMS_CLOSE, "Freed XMS handle %u")
JMESSAGE(JTRC_XMS_OPEN, "Obtained XMS handle %u")
JMESSAGE(JWRN_ADOBE_XFORM, "Unknown Adobe color transform code %d")
JMESSAGE(JWRN_BOGUS_PROGRESSION,
	 "Inconsistent progression sequence for component %d coefficient %d")
JMESSAGE(JWRN_EXTRANEOUS_DATA,
	 "Corrupt JPEG data: %u extraneous bytes before marker 0x%02x")
JMESSAGE(JWRN_HIT_MARKER, "Corrupt JPEG data: premature end of data segment")
JMESSAGE(JWRN_HUFF_BAD_CODE, "Corrupt JPEG data: bad Huffman code")
JMESSAGE(JWRN_JFIF_MAJOR, "Warning: unknown JFIF revision number %d.%02d")
JMESSAGE(JWRN_JPEG_EOF, "Premature end of JPEG file")
JMESSAGE(JWRN_MUST_RESYNC,
	 "Corrupt JPEG data: found marker 0x%02x instead of RST%d")
JMESSAGE(JWRN_NOT_SEQUENTIAL, "Invalid SOS parameters for sequential JPEG")
JMESSAGE(JWRN_TOO_MUCH_DATA, "Application transferred too many scanlines")



/* The following are JPEG-XR specific codes */

/* Errors */
JMESSAGE(JXRERR_FILE_HEADER,
  "Fixed file header bytes are Ox%x and 0x%x: expecting 0x%x and 0x%x")
JMESSAGE(JXRERR_TOO_FEW_IFD_ENTRIES,
  "Invalid directory: only %u IFD entries, %u or more are required")
JMESSAGE(JXRERR_WRONG_GDI_SIG,
  "Wrong GDI signature: got 0x%x%x, expecting 0x%x%x")
JMESSAGE(JXRERR_64_BITS,
  "Images with 64-bit syntax elements are currently unsupprted")
JMESSAGE(JXRERR_BAD_SEEK,
  "Fatal error: tried to seek back to data no longer in buffer")
  
/* Warnings */
JMESSAGE(JXRWRN_IMAGE_INCOMPATIBLE, "Warning: codestream is probably incompatible")
JMESSAGE(JXRWRN_RESERVED_VALUE, "Warning: found a reserved value for %s")
JMESSAGE(JXRWRN_NUM_BANDS, "Warning: alpha plane can't have more bands present than primary")
JMESSAGE(JXRWRN_PROFILE_UNSUPPORTED, "Warning: profile IDC value %u is greater than %u, the maximum supported")


/* Trace messages */
JMESSAGE(JXRTRC_FILE_BEGIN_META, "Reading file metadata")
JMESSAGE(JXRTRC_FILE_BEGIN, "Reading file header at offset 0x%x")
JMESSAGE(JXRTRC_FILE_HEADER,
  "Found correct fixed length prefix 0x%x and 0x%x")
JMESSAGE(JXRTRC_FILE_VERSION_MATCH,
  "File version ID %u matches library version")
JMESSAGE(JXRTRC_FILE_VERSION_DIFF,
  "Caution: file version ID is %u, values greater than %u reserved for future specifications")
JMESSAGE(JXRTRC_FILE_OFFSET, "Found first IFD offset at 0x%x")
JMESSAGE(JXRTRC_SEEK_DIR, "Seeking to IFD at offset 0x%x")
JMESSAGE(JXRTRC_CREATE_DIR, "Creating IFD object")
JMESSAGE(JXRTRC_DIR_BEGIN_META, "Reading directory (IFD) metadata")
JMESSAGE(JXRTRC_DIR_BEGIN, "Reading IFD header at offset 0x%x")

JMESSAGE(JXRTRC_DIR_NUM_IFD_ENTRIES, "Directory contains %u IFD entries")
JMESSAGE(JXRTRC_DIR_FIELD_TAG, "IFD entry %u has field tag %s")
JMESSAGE(JXRTRC_DIR_ELEM_TYPE, "IFD entry %u has element type 0x%x")
JMESSAGE(JXRTRC_DIR_ELEM_TYPE_RESERVED,
  "Caution: IFD entry element type of %u is reserved for future specifications ")
JMESSAGE(JXRTRC_DIR_NUM_ELEMS, "IFD entry %u has %u elements")
JMESSAGE(JXRTRC_DIR_VALUES, "IFD entry %u has values or offset 0x%x")
JMESSAGE(JXRTRC_DIR_NEXT_IFD, "Found next IFD at offset 0x%x")

JMESSAGE(JXRTRC_DIR_BEGIN_ENTRIES, "Reading IFD entry values")
JMESSAGE(JXRTRC_DIR_NO_MORE_IFD, "No more IFDs in file")
JMESSAGE(JXRTRC_DIR_UNSUPPORTED_ENTRY,
  "Caution: ignoring IFD entry with unsupported field tag %s")
JMESSAGE(JXRTRC_DIR_READ_ENTRY, "Reading IFD entry values for %s")
JMESSAGE(JXRTRC_DIR_READ_ENTRY_MULTIPLE, "Read IFD entry value %u of %u as 0x%x")
JMESSAGE(JXRTRC_DIR_IGNORE_ENTRY,
  "Caution: ignoring IFD entry with unsupported field combination")

JMESSAGE(JXRTRC_SEEK_IMAGE, "Seeking to coded image at offset 0x%x")
JMESSAGE(JXRTRC_CREATE_IMAGE, "Creating coded image object")
JMESSAGE(JXRTRC_IMAGE_BEGIN, "Reading coded image metadata")
JMESSAGE(JXRTRC_IMAGE_HEAD_BEGIN, "Reading coded image header at offset 0x%x")
JMESSAGE(JXRTRC_GDI_SIG, "Found correct GDI signature 0x%x%x")
JMESSAGE(JXRTRC_FUTURE_SPEC, "Caution: codestream is from a future specification based on value of %s")
JMESSAGE(JXRTRC_RESERVED_VALUE, "Caution: found a reserved value for %s")
JMESSAGE(JXRTRC_HARD_TILING, "Hard tiling flag set to %u")
JMESSAGE(JXRTRC_TILING, "Tiling flag set to %u")
JMESSAGE(JXRTRC_FREQ_MODE, "Frequency mode flag set to %u")
JMESSAGE(JXRTRC_XFRM_SUB,
  "Spatial transform subordinate has CW rotation %u and flip (%u, %u)")
JMESSAGE(JXRTRC_IDX_TABLE_PRESENT, "Index table present flag set to %u")
JMESSAGE(JXRTRC_OVERLAP_MODE, "Overlap mode is 0x%x")
JMESSAGE(JXRTRC_SHORT_HEADER,      "Short header flag set to %u")
JMESSAGE(JXRTRC_LONG_WORD,         "Long word flag set to %u")
JMESSAGE(JXRTRC_WINDOWING,         "Windowing flag set to %u")
JMESSAGE(JXRTRC_TRIM_FLEXBITS,     "Trim flexbits flag set to %u")
JMESSAGE(JXRTRC_ALPHA_IMAGE_PLANE, "Alpha image plane flag set to %u")
JMESSAGE(JXRTRC_OUTPUT_CLR_FORMAT, "Output colour format is %s")
JMESSAGE(JXRTRC_OUTPUT_BITDEPTH, "Output bit depth is %s")
JMESSAGE(JXRTRC_DIMENSIONS, "Image pixel dimensions are %u x %u")
JMESSAGE(JXRTRC_TILE_DIMENSIONS, "Image tile dimensions are %u x %u")
JMESSAGE(JXRTRC_TILE_HSEQUENCE, "Horizontal tile %u of %u is %u macroblocks wide")
JMESSAGE(JXRTRC_TILE_VSEQUENCE, "Vertical tile %u of %u is %u macroblocks tall")
JMESSAGE(JXRTRC_WINDOW_MARGIN, "Window margins: top=%u; bottom=%u; left=%u; right=%u")

JMESSAGE(JXRTRC_IMAGE_PLANE_BEGIN, "Reading image plane header at offset 0x%x, current plane alpha flag is %u")
JMESSAGE(JXRTRC_INTERNAL_CLR_FORMAT, "Internal colour format is %s")
JMESSAGE(JXRTRC_SCALED_FLAG, "Scaled flag set to %u")
JMESSAGE(JXRTRC_BANDS_PRESENT, "Codestream contains frequency components %s")
JMESSAGE(JXTRC_CHROMA_CENTRE_X, "Chroma centring X is %u")
JMESSAGE(JXTRC_CHROMA_CENTRE_Y, "Chroma centring Y is %u")
JMESSAGE(JXTRC_DC_UNIFORM, "DC image plane uniform flag set to %u")
JMESSAGE(JXRTRC_PARSE_DC_QP, "Reading DC quantisation parameter set at byte offset 0x%x, bit %u")
JMESSAGE(JXTRC_LP_UNIFORM, "Low pass image plane uniform flag set to %u")
JMESSAGE(JXRTRC_PARSE_LP_QP, "Reading low pass quantisation parameter set at byte offset 0x%x, bit %u")
JMESSAGE(JXTRC_HP_UNIFORM, "High pass image plane uniform flag set to %u")
JMESSAGE(JXRTRC_PARSE_HP_QP, "Reading high pass quantisation parameter set at byte offset 0x%x, bit %u")

JMESSAGE(JXRTRC_INDEX_TABLE_BEGIN, "Reading tile index table at offset 0x%x")

JMESSAGE(JXRTRC_NUM_COMP, "Image contains %u components")
JMESSAGE(JXRTRC_PROFILE_INFO, "Reading profile info at byte offset 0x%x")
JMESSAGE(JXRTRC_PROFILE_SUPPORTED, "Profile IDC value %u is decoder compatible")
JMESSAGE(JXRTRC_IBYTES, "Found %u i_bytes")
JMESSAGE(JXRTRC_VLW_ESC, "VLW ESC found i_value 0x%x")
JMESSAGE(JXRTRC_BEGIN_ABYTES, "Reading additional bytes at offset 0x%x")
JMESSAGE(JXRTRC_SUBBYTES, "Found %u subsequent bytes")
JMESSAGE(JXRTRC_SKIP_TO_TILES, "Skipping %u bytes (subsequent bytes - i_bytes) to coded tiles")
JMESSAGE(JXRTRC_IDX_TABLE_ENTRIES, "Foud %u index table entries")
JMESSAGE(JXRTRC_LUMA_DIMENSIONS, "Luma dimensions set to %u x %u")
JMESSAGE(JXRTRC_CHROMA_DIMENSIONS, "Chroma dimensions set to %u x %u")
JMESSAGE(JXRTRC_UNIFORM_QP, "UNIFORM component mode: quantisation parameter is %u")
JMESSAGE(JXRTRC_SEPERATE_QP, "SEPERATE component mode: luma parameter %u, chroma parameter %u")
JMESSAGE(JXRTRC_INDEPENDENT_QP, "INDEPENDENT component mode: quantisation parameter %u of %u is %u")
JMESSAGE(JXRTRC_FREQUENCY_BANDS, "%u frequency bands present in plane")
 
 

#ifdef JMAKE_ENUM_LIST

  JMSG_LASTMSGCODE
} J_MESSAGE_CODE;

#undef JMAKE_ENUM_LIST
#endif /* JMAKE_ENUM_LIST */

/* Zap JMESSAGE macro so that future re-inclusions do nothing by default */
#undef JMESSAGE


#ifndef JERROR_H
#define JERROR_H

/* Macros to simplify using the error and trace message stuff */
/* The first parameter is either type of cinfo pointer */

/* Fatal errors (print message and exit) */
#define ERREXIT(cinfo,code)  \
  ((cinfo)->err->msg_code = (code), \
   (*(cinfo)->err->error_exit) ((j_common_ptr) (cinfo)))
#define ERREXIT1(cinfo,code,p1)  \
  ((cinfo)->err->msg_code = (code), \
   (cinfo)->err->msg_parm.i[0] = (p1), \
   (*(cinfo)->err->error_exit) ((j_common_ptr) (cinfo)))
#define ERREXIT2(cinfo,code,p1,p2)  \
  ((cinfo)->err->msg_code = (code), \
   (cinfo)->err->msg_parm.i[0] = (p1), \
   (cinfo)->err->msg_parm.i[1] = (p2), \
   (*(cinfo)->err->error_exit) ((j_common_ptr) (cinfo)))
#define ERREXIT3(cinfo,code,p1,p2,p3)  \
  ((cinfo)->err->msg_code = (code), \
   (cinfo)->err->msg_parm.i[0] = (p1), \
   (cinfo)->err->msg_parm.i[1] = (p2), \
   (cinfo)->err->msg_parm.i[2] = (p3), \
   (*(cinfo)->err->error_exit) ((j_common_ptr) (cinfo)))
#define ERREXIT4(cinfo,code,p1,p2,p3,p4)  \
  ((cinfo)->err->msg_code = (code), \
   (cinfo)->err->msg_parm.i[0] = (p1), \
   (cinfo)->err->msg_parm.i[1] = (p2), \
   (cinfo)->err->msg_parm.i[2] = (p3), \
   (cinfo)->err->msg_parm.i[3] = (p4), \
   (*(cinfo)->err->error_exit) ((j_common_ptr) (cinfo)))
#define ERREXITS(cinfo,code,str)  \
  ((cinfo)->err->msg_code = (code), \
   strncpy((cinfo)->err->msg_parm.s, (str), JMSG_STR_PARM_MAX), \
   (*(cinfo)->err->error_exit) ((j_common_ptr) (cinfo)))

#define MAKESTMT(stuff)		do { stuff } while (0)

/* Nonfatal errors (we can keep going, but the data is probably corrupt) */
#define WARNMS(cinfo,code)  \
  ((cinfo)->err->msg_code = (code), \
   (*(cinfo)->err->emit_message) ((j_common_ptr) (cinfo), -1))
#define WARNMS1(cinfo,code,p1)  \
  ((cinfo)->err->msg_code = (code), \
   (cinfo)->err->msg_parm.i[0] = (p1), \
   (*(cinfo)->err->emit_message) ((j_common_ptr) (cinfo), -1))
#define WARNMS2(cinfo,code,p1,p2)  \
  ((cinfo)->err->msg_code = (code), \
   (cinfo)->err->msg_parm.i[0] = (p1), \
   (cinfo)->err->msg_parm.i[1] = (p2), \
   (*(cinfo)->err->emit_message) ((j_common_ptr) (cinfo), -1))
#define WARNMSS(cinfo,code,str)  \
  ((cinfo)->err->msg_code = (code), \
   strncpy((cinfo)->err->msg_parm.s, (str), JMSG_STR_PARM_MAX), \
   (*(cinfo)->err->emit_message) ((j_common_ptr) (cinfo), -1))

/* Informational/debugging messages */
#define TRACEMS(cinfo,lvl,code)  \
  ((cinfo)->err->msg_code = (code), \
   (*(cinfo)->err->emit_message) ((j_common_ptr) (cinfo), (lvl)))
#define TRACEMS1(cinfo,lvl,code,p1)  \
  ((cinfo)->err->msg_code = (code), \
   (cinfo)->err->msg_parm.i[0] = (p1), \
   (*(cinfo)->err->emit_message) ((j_common_ptr) (cinfo), (lvl)))
#define TRACEMS2(cinfo,lvl,code,p1,p2)  \
  ((cinfo)->err->msg_code = (code), \
   (cinfo)->err->msg_parm.i[0] = (p1), \
   (cinfo)->err->msg_parm.i[1] = (p2), \
   (*(cinfo)->err->emit_message) ((j_common_ptr) (cinfo), (lvl)))
#define TRACEMS3(cinfo,lvl,code,p1,p2,p3)  \
  MAKESTMT(int * _mp = (cinfo)->err->msg_parm.i; \
	   _mp[0] = (p1); _mp[1] = (p2); _mp[2] = (p3); \
	   (cinfo)->err->msg_code = (code); \
	   (*(cinfo)->err->emit_message) ((j_common_ptr) (cinfo), (lvl)); )
#define TRACEMS4(cinfo,lvl,code,p1,p2,p3,p4)  \
  MAKESTMT(int * _mp = (cinfo)->err->msg_parm.i; \
	   _mp[0] = (p1); _mp[1] = (p2); _mp[2] = (p3); _mp[3] = (p4); \
	   (cinfo)->err->msg_code = (code); \
	   (*(cinfo)->err->emit_message) ((j_common_ptr) (cinfo), (lvl)); )
#define TRACEMS5(cinfo,lvl,code,p1,p2,p3,p4,p5)  \
  MAKESTMT(int * _mp = (cinfo)->err->msg_parm.i; \
	   _mp[0] = (p1); _mp[1] = (p2); _mp[2] = (p3); _mp[3] = (p4); \
	   _mp[4] = (p5); \
	   (cinfo)->err->msg_code = (code); \
	   (*(cinfo)->err->emit_message) ((j_common_ptr) (cinfo), (lvl)); )
#define TRACEMS8(cinfo,lvl,code,p1,p2,p3,p4,p5,p6,p7,p8)  \
  MAKESTMT(int * _mp = (cinfo)->err->msg_parm.i; \
	   _mp[0] = (p1); _mp[1] = (p2); _mp[2] = (p3); _mp[3] = (p4); \
	   _mp[4] = (p5); _mp[5] = (p6); _mp[6] = (p7); _mp[7] = (p8); \
	   (cinfo)->err->msg_code = (code); \
	   (*(cinfo)->err->emit_message) ((j_common_ptr) (cinfo), (lvl)); )
#define TRACEMSS(cinfo,lvl,code,str)  \
  ((cinfo)->err->msg_code = (code), \
   strncpy((cinfo)->err->msg_parm.s, (str), JMSG_STR_PARM_MAX), \
   (*(cinfo)->err->emit_message) ((j_common_ptr) (cinfo), (lvl)))

#endif /* JERROR_H */
