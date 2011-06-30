/*
 * jxrparse.c
 *
 * Copyright (C) 2011, Chris Harding.
 * 
 * This file contains algorithms used in the parsing stages of JPEG XR
 * decoding.
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
	size_t bytes_in_buffer = datasrc->bytes_in_buffer

/* Unload the local copies --- do this only at a restart boundary */
#define INPUT_SYNC(cinfo)  \
	( datasrc->next_input_byte = next_input_byte,  \
	  datasrc->bytes_in_buffer = bytes_in_buffer )

/* Reload the local copies --- used only in MAKE_BYTE_AVAIL */
#define INPUT_RELOAD(cinfo)  \
	( next_input_byte = datasrc->next_input_byte,  \
	  bytes_in_buffer = datasrc->bytes_in_buffer )

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
		  V = GETJOCTET(*next_input_byte++); )

/* Read two bytes interpreted as an unsigned 16-bit integer.
 * V should be declared UINT16
 */
#define INPUT_2BYTES(cinfo,V,action)  \
	MAKESTMT( MAKE_BYTE_AVAIL(cinfo,action); \
		  bytes_in_buffer--; \
		  V = ((unsigned int) GETJOCTET(*next_input_byte++)) << 8; \
		  MAKE_BYTE_AVAIL(cinfo,action); \
		  bytes_in_buffer--; \
		  V += ((unsigned int) GETJOCTET(*next_input_byte++)) << 0; )
      
/* As above, but read two bytes interpreted as an unsigned 16-bit
 * integer in little endian format. V should be declared UINT16.
 */
#define INPUT_2BYTES_LE(cinfo,V,action)  \
	MAKESTMT( MAKE_BYTE_AVAIL(cinfo,action); \
		  bytes_in_buffer--; \
		  V = ((unsigned int) GETJOCTET(*next_input_byte++)) << 0; \
		  MAKE_BYTE_AVAIL(cinfo,action); \
		  bytes_in_buffer--; \
		  V += ((unsigned int) GETJOCTET(*next_input_byte++)) << 8; )
      
/* Read four bytes interpreted as an unsigned 32-bit integer.
 * V should be declared UINT32
 */
#define INPUT_4BYTES(cinfo,V,action)  \
	MAKESTMT( MAKE_BYTE_AVAIL(cinfo,action); \
		  bytes_in_buffer--; \
		  V = ((UINT32) GETJOCTET(*next_input_byte++)) << 24; \
		  MAKE_BYTE_AVAIL(cinfo,action); \
		  bytes_in_buffer--; \
		  V += ((UINT32) GETJOCTET(*next_input_byte++)) << 16; \
		  MAKE_BYTE_AVAIL(cinfo,action); \
		  bytes_in_buffer--; \
		  V += ((UINT32) GETJOCTET(*next_input_byte++)) << 8;  \
		  MAKE_BYTE_AVAIL(cinfo,action); \
		  bytes_in_buffer--; \
		  V += ((UINT32) GETJOCTET(*next_input_byte++)) << 0; )
      
/* As above, but read four bytes interpreted as an unsigned 32-bit
 * integer in little endian format. V should be declared UINT32
 */
#define INPUT_4BYTES_LE(cinfo,V,action)  \
	MAKESTMT( MAKE_BYTE_AVAIL(cinfo,action); \
		  bytes_in_buffer--; \
		  V = ((UINT32) GETJOCTET(*next_input_byte++))  << 0; \
		  MAKE_BYTE_AVAIL(cinfo,action); \
		  bytes_in_buffer--; \
		  V += ((UINT32) GETJOCTET(*next_input_byte++)) << 8; \
		  MAKE_BYTE_AVAIL(cinfo,action); \
		  bytes_in_buffer--; \
		  V += ((UINT32) GETJOCTET(*next_input_byte++)) << 16;  \
		  MAKE_BYTE_AVAIL(cinfo,action); \
		  bytes_in_buffer--; \
		  V += ((UINT32) GETJOCTET(*next_input_byte++)) << 24; )

/*
 * Parse image file directory entry. Eventually this
 * needs to be moved elsewhere.
 *  
 */          
LOCAL(boolean)
parse_ifd_entry (j_decompress_ptr cinfo)
{
  UINT16 c2;
  UINT32 c4;
  
  /* Get correct entry to fill in */
  /* Memory has already been allocated. */
  UINT16 idx = cinfo->file_dir->num_entries; 
  ifd_entry * ifde = &cinfo->file_dir->ifd_entry_list[idx];
  
  INPUT_VARS(cinfo);
  
  /* Parse fields */
  INPUT_2BYTES_LE(cinfo, c2, return FALSE);
  ifde->field_tag = c2;
  INPUT_2BYTES_LE(cinfo, c2, return FALSE);
  ifde->element_type = c2;
  INPUT_4BYTES_LE(cinfo, c4, return FALSE);
  ifde->num_elements = c4;
  INPUT_4BYTES_LE(cinfo, c4, return FALSE);
  ifde->values_or_offset = c4;
  
  INPUT_SYNC(cinfo);
  
  /* Increment number of entries */
  cinfo->file_dir->num_entries++;
  
  return TRUE;
}
  
/*
 * Parse image file header. We also parse the directory. Eventually this
 * needs to be moved elsewhere.
 *  
 */          
GLOBAL(int)
jpegxr_parse_file_header (j_decompress_ptr cinfo)
{
  UINT8 c;
  UINT16 c2;
  UINT32 c4;
  
  /* File structs to fill in */
  file_header file_hdr;
  image_file_directory file_dir;
  cinfo->file_hdr = &file_hdr;
  cinfo->file_dir = &file_dir;
  
  INPUT_VARS(cinfo);  
  
  /* Parse file parameters */
  INPUT_2BYTES(cinfo, c2, return FALSE);
  cinfo->file_hdr->fixed_file_header_ii_2bytes = c2;
  INPUT_BYTE(cinfo, c, return FALSE);
  cinfo->file_hdr->fixed_file_header_0xbc_byte = c;
  INPUT_BYTE(cinfo, c, return FALSE);
  cinfo->file_hdr->file_version_id = c;
  /* Offset is stored in little endian format */
  INPUT_4BYTES_LE(cinfo, c4, return FALSE);
  cinfo->file_hdr->first_ifd_offset = c4;
  
  /* Skip to the directory */
  INPUT_SYNC(cinfo);
  /* file header is fixed size of 6 bytes */
  /* TODO - a more elegant way of fast forward/rewinding? */
  (*cinfo->src->skip_input_data) (cinfo, (long) cinfo->file_hdr->first_ifd_offset-6);
  INPUT_RELOAD(cinfo);
  
  
  /* Parse directory */
  /* TODO - Currently we only support one directory, one entry */
  INPUT_2BYTES_LE(cinfo, c2, return FALSE);
  cinfo->file_dir->num_entries = 0;
  /* Create list of IFD entries*/
  ifd_entry entry_list[c2];
  cinfo->file_dir->ifd_entry_list = entry_list;
  for (int i=0; i<c2; i++) {
    if (! parse_ifd_entry(cinfo)) return FALSE;
  }
  /* Next directory */
  INPUT_4BYTES_LE(cinfo, c4, return FALSE);
  cinfo->file_dir->zero_or_next_ifd_offset = c4;
  
  INPUT_SYNC(cinfo);
   
   
   
  fprintf(stdout, "File header\n");
  fprintf(stdout, "fixed_file_header_ii_2bytes : %x\n", cinfo->file_hdr->fixed_file_header_ii_2bytes);
  fprintf(stdout, "fixed_file_header_0xbc_byte : %x\n", cinfo->file_hdr->fixed_file_header_0xbc_byte);
  fprintf(stdout, "file_version_id : %x\n", cinfo->file_hdr->file_version_id);
  fprintf(stdout, "first_ifd_offset : %x\n", cinfo->file_hdr->first_ifd_offset);
  
  fprintf(stdout, "Directory\n");
  fprintf(stdout, "num_entries: %x\n", cinfo->file_dir->num_entries);
  fprintf(stdout, "zero_or_next_ifd_offset: %x\n", cinfo->file_dir->zero_or_next_ifd_offset);
  
  fprintf(stdout, "First IFD entry\n");
  fprintf(stdout, "field_tag : %x\n",         cinfo->file_dir->ifd_entry_list[0].field_tag);
  fprintf(stdout, "element_type : %x\n",      cinfo->file_dir->ifd_entry_list[0].element_type);
  fprintf(stdout, "num_elements : %x\n",      cinfo->file_dir->ifd_entry_list[0].num_elements);
  fprintf(stdout, "values_or_offset : %x\n",  cinfo->file_dir->ifd_entry_list[0].values_or_offset);
  
  
  
  /* TODO return correct code */
  return JPEG_REACHED_SOS;
}

/*
 * Parse image/image plane headers and read the tile index table.
 *  
 */          
GLOBAL(int)
jpegxr_parse_image_tile_layer (j_decompress_ptr cinfo, boolean require_image)
{
  
  
  UINT8 t;
  
  INPUT_VARS(cinfo);
  
  fprintf(stdout, "Parsing called: ");
  for (int i=0; i<100; i++) {
    INPUT_BYTE(cinfo, t, return FALSE);
    fprintf(stdout, "%x.", t);
  }
  fprintf(stdout, "\n");
  
  INPUT_SYNC(cinfo);
  
  
  /* TODO return correct code */
  return JPEG_REACHED_SOS;
}
