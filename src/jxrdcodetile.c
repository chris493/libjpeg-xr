/*
 * jxrdcodetile.c
 *
 * Copyright (C) 2011, Chris Harding.
 * Copyright (C) 1994-1998, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 * 
 * This file contains algorithms used to parse and decode the tile layer
 * of JPEG-XR coded image. We performing parsing and decoding here at the
 * same time, though they are distinct in the specification. This is to
 * permit a single pass implementation.
 */

#define JPEG_INTERNALS
#include "jinclude.h"
#include "jpegxrlib.h"

/* Data read macros */
#ifndef JXR_DATARD_INCLUDED
#include "jxrdatard.h"
#endif

/* QP set parsing routines */
#ifndef JXR_PARSE_QP
#include "jxrparseqp.c"
#endif


/*
 * Check the start code and arbitrary byte. Common to all tiles.
 *
 */
LOCAL (void)
decode_tile_start (j_image_ptr iinfo, UINT32 * startcode)
{
  UINT8 c, cc , ccc;
  
  INPUT_VARS(iinfo);
  
  /* Check the start code */
  INPUT_BYTE((j_common_ptr) iinfo,c  ,return FALSE);
  INPUT_BYTE((j_common_ptr) iinfo,cc ,return FALSE);
  INPUT_BYTE((j_common_ptr) iinfo,ccc ,return FALSE);

  *startcode = (c  << 16) |
                     (cc  << 8) |
                     (ccc << 0);
  
  /* Read the arbitrary byte. This is ignored, in accordance with the 
   * specification*/
  INPUT_BYTE((j_common_ptr) iinfo,c  ,return FALSE);
  
}

/*
 * Read DC tile header.
 *
 */
LOCAL (void)
read_tile_header_dc (j_image_ptr iinfo, jxr_image_plane_header * plane_hdr, jxr_image_plane_vars * plane_vars )
{
  if (plane_hdr->dc_image_plane_uniform_flag == FALSE) {
    parse_dc_qp(iinfo, plane_vars->num_components, &plane_hdr->dc_qp);
  }
}

/*
 * Read DC tile header.
 *
 */
LOCAL (void)
read_tile_header_lp (j_image_ptr iinfo, jxr_image_plane_header * plane_hdr, jxr_image_plane_vars * plane_vars )
{
  /* Check that low pass QPs aren't the same for all tiles */
  if (plane_hdr->lp_image_plane_uniform_flag == FALSE) {
    
    UINT8 b;
    INPUT_VARS(iinfo);
    
    /* Check if a new QP set is present */
    INPUT_BITS((j_common_ptr) iinfo, b, 1, return FALSE);
    boolean use_dc_qp_flag = b;
    
    if (use_dc_qp_flag == FALSE) {
      
      /* Get number of low pass QPs */
      INPUT_BITS((j_common_ptr) iinfo, b, 4, return FALSE);
      plane_vars->num_lp_qps = b + 1;
      
      /* Parse QP set */
      INPUT_SYNC(iinfo);
      parse_lp_qp(  iinfo,
                    plane_vars->num_components,
                    plane_vars->num_lp_qps,
                    &plane_hdr->lp_qp );    
    }
  }

}

/*
 * Read DC tile header.
 *
 */
LOCAL (void)
read_tile_header_hp (j_image_ptr iinfo, jxr_image_plane_header * plane_hdr, jxr_image_plane_vars * plane_vars )
{
  /* Check that high pass QPs aren't the same for all tiles */
  if (plane_hdr->hp_image_plane_uniform_flag == FALSE) {
    
    UINT8 b;
    INPUT_VARS(iinfo);
    
    /* Check if a new QP set is present */
    INPUT_BITS((j_common_ptr) iinfo, b, 1, return FALSE);
    boolean use_lp_qp_flag = b;
    
    if (use_lp_qp_flag == FALSE) {
      
      /* Get number of high pass QPs */
      INPUT_BITS((j_common_ptr) iinfo, b, 4, return FALSE);
      plane_vars->num_hp_qps = b + 1;
      
      /* Parse QP set */
      INPUT_SYNC(iinfo);
      parse_hp_qp(  iinfo,
                    plane_vars->num_components,
                    plane_vars->num_hp_qps,
                    &plane_hdr->hp_qp );    
    }
    
  }
}



/*
 * Read tile headers.
 *
 */
LOCAL (void)
read_tile_headers (j_image_ptr iinfo, jxr_image_plane_header * plane_hdr, jxr_image_plane_vars * plane_vars )
{
  read_tile_header_dc (iinfo, plane_hdr, plane_vars );
  if (plane_hdr->bands_present != JBANDS_DCONLY) {
    read_tile_header_lp (iinfo, plane_hdr, plane_vars);
    if (plane_hdr->bands_present != JBANDS_NOHIGHPASS)
      read_tile_header_hp (iinfo, plane_hdr, plane_vars);
  }
}

/*
 * Read remainder of tile.
 *
 */
LOCAL (void)
read_tile (j_image_ptr iinfo, jxr_image_plane_header * plane_hdr, jxr_image_plane_vars * plane_vars )
{
  UINT8 c;
  
  INPUT_VARS(iinfo);
  
  
}

                
/*
 * Parse and decode a spatially encoded tile.
 *
 */
GLOBAL(void)
jpegxr_decode_tile_spatial (j_image_ptr iinfo)
{
  UINT8 b;
  UINT8 c;
  UINT32 startcode;
  UINT8 trim_flexbits;
  
  INPUT_VARS(iinfo);
    
  printf("Decoding spatial tile at offset 0x%x\n", idx);
  
  /* Check start code and decode arbitrary byte */
  INPUT_SYNC(iinfo);
  decode_tile_start(iinfo,&startcode);
  if (startcode == JXR_TILE_STARTCODE)
    printf("Found correct tile start code 0x%x\n", startcode);
  else {
    // TODO - can we handle this better?
    printf("Warning: skipping tile, found incorrect start code 0x%x\n", startcode);
    return;
  }
  INPUT_RELOAD(iinfo);

  /* Read trim flexbits element */
  if (iinfo->hdr.trim_flexbits_flag) {
    INPUT_BITS((j_common_ptr) iinfo, b, 4, return FALSE);
    trim_flexbits = b;
  } else trim_flexbits = 0;
  printf("Trim flexbits set to 0x%x\n",trim_flexbits);
  
  INPUT_SYNC(iinfo);
  
  /* Read tile headers for primary plane */
  read_tile_headers( iinfo,
                    &iinfo->image_plane_hdr,
                    &iinfo->image_plane_vars );
                    
  /* Read tile headers for alpha plane, if present */
  if (iinfo->hdr.alpha_image_plane_flag) {
    read_tile_headers( iinfo,
                      &iinfo->alpha_plane_hdr,
                      &iinfo->alpha_plane_vars );
  }
  
  /* Read remainder of tile for primary plane */
  read_tile( iinfo,
            &iinfo->image_plane_hdr,
            &iinfo->image_plane_vars );
                    
  /* Read remainder of tile for alpha plane, if present */
  if (iinfo->hdr.alpha_image_plane_flag) {
    read_tile ( iinfo,
                &iinfo->alpha_plane_hdr,
                &iinfo->alpha_plane_vars );
  }
  
  INPUT_RELOAD(iinfo);
  
  // Remainder of byte is just byte alignment padding
  INPUT_ALIGN((j_common_ptr)iinfo, return FALSE);

}

/*
 * Parse and decode the DC component of a coded tile.
 *
 */
GLOBAL(void)
jpegxr_decode_tile_dc (j_image_ptr iinfo)
{
  UINT8 c;
  
  UINT32 startcode;
  
  INPUT_VARS(iinfo);
    
  printf("Decoding tile DC at offset 0x%x\n", idx);
  
  /* Check start code and decode arbitrary byte */
  INPUT_SYNC(iinfo);
  decode_tile_start(iinfo,&startcode);
  if (startcode == JXR_TILE_STARTCODE)
    printf("Found correct tile start code 0x%x\n", startcode);
  else {
    // TODO - can we handle this better?
    printf("Warning: skipping tile, found incorrect start code 0x%x\n", startcode);
    return;
  }
  INPUT_RELOAD(iinfo);

}

/*
 * Parse and decode the low pass component of a coded tile.
 *
 */
GLOBAL(void)
jpegxr_decode_tile_lp (j_image_ptr iinfo)
{
  UINT8 c;
  
  UINT32 startcode;
  
  INPUT_VARS(iinfo);
    
  printf("Decoding tile LP at offset 0x%x\n", idx);
  
  /* Check start code and decode arbitrary byte */
  INPUT_SYNC(iinfo);
  decode_tile_start(iinfo,&startcode);
  if (startcode == JXR_TILE_STARTCODE)
    printf("Found correct tile start code 0x%x\n", startcode);
  else {
    // TODO - can we handle this better?
    printf("Warning: skipping tile, found incorrect start code 0x%x\n", startcode);
    return;
  }
  INPUT_RELOAD(iinfo);

}

/*
 * Parse and decode the high pass component of a coded tile.
 *
 */
GLOBAL(void)
jpegxr_decode_tile_hp (j_image_ptr iinfo)
{
  UINT8 c;
  
  UINT32 startcode;
  
  INPUT_VARS(iinfo);
    
  printf("Decoding tile HP at offset 0x%x\n", idx);
  
  /* Check start code and decode arbitrary byte */
  INPUT_SYNC(iinfo);
  decode_tile_start(iinfo,&startcode);
  if (startcode == JXR_TILE_STARTCODE)
    printf("Found correct tile start code 0x%x\n", startcode);
  else {
    // TODO - can we handle this better?
    printf("Warning: skipping tile, found incorrect start code 0x%x\n", startcode);
    return;
  }
  INPUT_RELOAD(iinfo);

}


/*
 * Parse and decode the flexbits of a coded tile.
 *
 */
GLOBAL(void)
jpegxr_decode_tile_flexbits (j_image_ptr iinfo)
{
  UINT8 b;
  UINT8 c;
  UINT32 startcode;
  UINT8 trim_flexbits;
  
  INPUT_VARS(iinfo);
    
  printf("Decoding tile flexbits at offset 0x%x\n", idx);
  
  /* Check start code and decode arbitrary byte */
  INPUT_SYNC(iinfo);
  decode_tile_start(iinfo,&startcode);
  if (startcode == JXR_TILE_STARTCODE)
    printf("Found correct tile start code 0x%x\n", startcode);
  else {
    // TODO - can we handle this better?
    printf("Warning: setting flexbits data to zero, found incorrect start code 0x%x\n", startcode);
    trim_flexbits = 15;
    // TODO need to actually set tile data to zero here
    return;
  }
  INPUT_RELOAD(iinfo);
  
  /* Read trim flexbits element */
  if (iinfo->hdr.trim_flexbits_flag) {
    INPUT_BITS((j_common_ptr) iinfo, b, 4, return FALSE);
    trim_flexbits = b;
  } else trim_flexbits = 0;
  printf("Trim flexbits set to 0x%x\n",trim_flexbits);

}
