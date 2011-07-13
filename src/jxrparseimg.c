/*
 * jxrfile.c
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

/* Data read macros */
#ifndef JXR_DATARD_INCLUDED
#include "jxrdatard.h"
#endif

/*
 * Parse the DC component quantisation paramter set.
 *  
 */          
LOCAL(boolean)
parse_dc_qp (j_image_ptr iinfo)
{
  TRACEMS(iinfo,2,JXRTRC_PARSE_DC_QP);
  
  int num_components = 0;
  
  /* Parse or infer component mode */
  if (num_components != 1) {
    INPUT_BITS(((j_common_ptr)iinfo),b,2,return FALSE);
    iinfo->img_plane_hdr->component_mode = b;
  } else {
    iinfo->img_plane_hdr->component_mode = JCOMPMODE_UNIFORM;
  }
  
  /* Parse any relevant quantisation parameters */
  if (iinfo->img_plane_hdr->component_mode == JCOMPMODE_UNIFORM) {
    INPUT_BITS(((j_common_ptr)iinfo),b,8,return FALSE);
    iinfo->img_plane_hdr->dc_quant = b;
  } else if (iinfo->img_plane_hdr->component_mode == JCOMPMODE_SEPARATE) {
    INPUT_BITS(((j_common_ptr)iinfo),b,8,return FALSE);
    iinfo->img_plane_hdr->dc_quant_luma = b;
    INPUT_BITS(((j_common_ptr)iinfo),b,8,return FALSE);
    iinfo->img_plane_hdr->dc_quant_chroma = b;
  } else if (iinfo->img_plane_hdr->component_mode == JCOMPMODE_INDEPENDENT) {
    for (int i = 0; i<num_components; i++) {
      INPUT_BITS(((j_common_ptr)iinfo),b,8,return FALSE);
      iinfo->img_plane_hdr->dc_quant_ch[i] = b;
    }
  }



  return TRUE;
}
/*
 * Parse the low pass quantisation paramter set.
 *  
 */          
LOCAL(boolean)
parse_lp_qp (j_image_ptr iinfo, unsigned int num_qps)
{
  TRACEMS(iinfo,2,JXRTRC_PARSE_LP_QP);
  
  return TRUE;
}
/*
 * Parse the high pass quantisation paramter set.
 *  
 */          
LOCAL(boolean)
parse_hp_qp (j_image_ptr iinfo, unsigned int num_qps)
{
  TRACEMS(iinfo,2,JXRTRC_PARSE_HP_QP);
  
  return TRUE;
}


/*
 * Read a JPEG-XR image to obtain headers and decompression
 * parameters. This reads image, tile and macroblock layer 
 * information. The source data should be at the start of the image
 * codestream.
 *  
 */          
GLOBAL(int)
jpegxr_image_read_metadata (j_image_ptr iinfo)
{

  TRACEMS(iinfo,1,JXRTRC_IMAGE_BEGIN);
    
  /* Read the image header at the start of the codestream */
  jpegxr_image_read_header (iinfo);
  
  /* Read the image plane header */
  jpegxr_image_read_plane_header (iinfo, FALSE);
  
  /* Read the alpha plane header, if one is present */
  
  /* Read the tile index table, if one is present */
  
  /* Read the tile index table, if one is present */
  
  /* TODO return correct code */
  return JPEG_REACHED_SOS;
}


/*
 * Read only the image header of a coded image from the source data. The
 * source data should be at the start of the image codestream.
 *  
 */          
GLOBAL(int)
jpegxr_image_read_header (j_image_ptr iinfo)
{
  UINT8 c, cc, ccc;
  UINT16 c2;
  UINT32 c4, cc4;
  
  TRACEMS(iinfo,2,JXRTRC_IMAGE_HEAD_BEGIN);
  
  /* Header to put things into */
  jxr_image_header hdr;
  iinfo->hdr = &hdr;
    
  INPUT_VARS(iinfo);
  
  /* Read in GDI signature (8 bytes) */
  INPUT_4BYTES(((j_common_ptr)iinfo), c4, return FALSE);
  INPUT_4BYTES(((j_common_ptr)iinfo), cc4, return FALSE);
  if (c4 != JXR_GDI_SIG_HI || cc4 != JXR_GDI_SIG_LO )
    ERREXIT4(iinfo, JXRERR_WRONG_GDI_SIG, c4,cc4, JXR_GDI_SIG_HI, JXR_GDI_SIG_LO );
  TRACEMS2(iinfo, 3, JXRTRC_GDI_SIG, c4, cc4);
  
  /* Read in first byte */
  INPUT_BYTE(((j_common_ptr)iinfo),c,return FALSE);
  // reserved B (4 bits)
  hdr.reserved_b = GETBITS(c,0,4);
  if (hdr.reserved_b != 1) WARNMS(iinfo,JXRWRN_IMAGE_INCOMPATIBLE);
  // hard tiling flag
  hdr.hard_tiling_flag = GETBITS(c,4,1);
  TRACEMS1(iinfo,3,JXRTRC_HARD_TILING,hdr.hard_tiling_flag);
  // reserved C (3 bits)
  hdr.reserved_c = GETBITS(c,5,3);
  if (hdr.reserved_c != 1) TRACEMS(iinfo,0,JXRTRC_FUTURE_SPEC);
  
  /* Read in second byte */
  INPUT_BYTE(((j_common_ptr)iinfo),c,return FALSE);
  // tiling_flag
  hdr.tiling_flag = GETBITS(c,0,1);
  TRACEMS1(iinfo,3,JXRTRC_TILING,hdr.tiling_flag);
  // frequency_mode_codestream_flag
  hdr.frequency_mode_codestream_flag= GETBITS(c,1,1);
  TRACEMS1(iinfo,3,JXRTRC_FREQ_MODE,hdr.frequency_mode_codestream_flag);
  // spatial_xfrm_subordinate (3 bits)
  hdr.spatial_xfrm_subordinate.rcw = GETBITS(c,2,1);
  hdr.spatial_xfrm_subordinate.flip_h = GETBITS(c,3,1);
  hdr.spatial_xfrm_subordinate.flip_v = GETBITS(c,4,1);
  hdr.spatial_xfrm_subordinate.fill = GETBITS(c,2,3);
  TRACEMS4(iinfo,3,JXRTRC_XFRM_SUB,
    hdr.spatial_xfrm_subordinate.rcw,
    hdr.spatial_xfrm_subordinate.flip_h,
    hdr.spatial_xfrm_subordinate.flip_v,
    hdr.spatial_xfrm_subordinate.fill);
  // index_table_present_flag
  hdr.index_table_present_flag = GETBITS(c,5,1);
  TRACEMS1(iinfo,3,JXRTRC_IDX_TABLE,GETBITS(c,5,1));
  // overlap_mode (2 bits)
  hdr.overlap_mode = GETBITS(c,6,2);
  TRACEMS1(iinfo,3,JXRTRC_OVERLAP_MODE, hdr.overlap_mode);
  if (hdr.overlap_mode == 3) TRACEMS(iinfo,0,JXRTRC_RESERVED_VALUE);
  
  /* Read in third byte */
  INPUT_BYTE(((j_common_ptr)iinfo),c,return FALSE);
  // short_header_flag
  hdr.short_header_flag = GETBITS(c,0,1);
  TRACEMS1(iinfo,3,JXRTRC_SHORT_HEADER, hdr.short_header_flag);
  // long_word_flag
  hdr.long_word_flag = GETBITS(c,1,1);
  TRACEMS1(iinfo,3,JXRTRC_LONG_WORD, hdr.long_word_flag);
  // windowing_flag
  hdr.windowing_flag = GETBITS(c,2,1);
  TRACEMS1(iinfo,3,JXRTRC_WINDOWING, hdr.windowing_flag);
  // trim_flexbits_flag
  hdr.trim_flexbits_flag = GETBITS(c,3,1);
  TRACEMS1(iinfo,3,JXRTRC_TRIM_FLEXBITS, hdr.trim_flexbits_flag);
  // reserved_d (3 bits)
  hdr.reserved_d = GETBITS(c,4,3);
  if (hdr.reserved_d !=0) TRACEMS(iinfo,0,JXRTRC_RESERVED_VALUE);
  // alpha_image_plane_flag
  hdr.alpha_image_plane_flag = GETBITS(c,7,1);
  TRACEMS1(iinfo,3,JXRTRC_ALPHA_IMAGE_PLANE, hdr.alpha_image_plane_flag);

  /* Read in fourth byte */
  // for easy reading out of enum string identifiers
  DEFINE_ENUM( JXR_OUTPUT_CLR_FMT, JXR_OUTPUT_CLR_FMT_DEF );
  DEFINE_ENUM( JXR_OUTPUT_BIT_DEPTH, JXR_OUTPUT_BIT_DEPTH_DEF );
  INPUT_BYTE(((j_common_ptr)iinfo),c,return FALSE);
  hdr.output_clr_fmt = GETBITS(c,0,4);
  TRACEMSS(iinfo,3,JXRTRC_OUTPUT_CLR_FORMAT,
    GetString_JXR_OUTPUT_CLR_FMT( hdr.output_clr_fmt) );
  hdr.output_bitdepth = GETBITS(c,4,4);
  TRACEMSS(iinfo,3,JXRTRC_OUTPUT_BITDEPTH,
    GetString_JXR_OUTPUT_BIT_DEPTH( hdr.output_bitdepth) );
    
  /* Read in dimensions */
  if (hdr.short_header_flag) {
    INPUT_2BYTES(((j_common_ptr)iinfo),c4,return FALSE);
    INPUT_2BYTES(((j_common_ptr)iinfo),cc4,return FALSE);
  } else {
    INPUT_4BYTES(((j_common_ptr)iinfo),c4,return FALSE);
    INPUT_4BYTES(((j_common_ptr)iinfo),cc4,return FALSE);
  }
  hdr.width_minus1 = c4;
  hdr.height_minus1 = cc4;
  TRACEMS2(iinfo,3,JXRTRC_DIMENSIONS, hdr.width_minus1+1, hdr.height_minus1+1);
  
  /* Read tiling dimensions, if present */
  if (hdr.tiling_flag) {
    INPUT_BYTE(((j_common_ptr)iinfo),c,return FALSE);
    INPUT_BYTE(((j_common_ptr)iinfo),cc,return FALSE);
    INPUT_BYTE(((j_common_ptr)iinfo),ccc,return FALSE);
    hdr.num_hor_tiles_minus1 = ((c << 4) & 0xffffff) + GETBITS(cc,0,4) ;
    hdr.num_ver_tiles_minus1 = ccc + ((GETBITS(cc,4,4) << 8) & 0xffffff) ;
  } else {
    hdr.num_hor_tiles_minus1 = 0;
    hdr.num_ver_tiles_minus1 = 0;
  }
  TRACEMS2(iinfo,3,JXRTRC_TILE_DIMENSIONS, hdr.num_hor_tiles_minus1+1, hdr.num_ver_tiles_minus1+1);
  
  /* Read in tile sequences, if present */
  if (hdr.tiling_flag) {
    // allocate memory
    UINT16 * tile_height_in_mb = (*iinfo->mem->alloc_small) (
              (j_common_ptr) iinfo,
              JPOOL_IMAGE,
              hdr.num_ver_tiles_minus1 * SIZEOF(UINT16)
          );
    UINT16 * tile_width_in_mb = (*iinfo->mem->alloc_small) (
              (j_common_ptr) iinfo,
              JPOOL_IMAGE,
              hdr.num_hor_tiles_minus1 * SIZEOF(UINT16)
          );
    // read in from stream
    for (int i=0; i<hdr.num_ver_tiles_minus1; i++) {
      if (hdr.short_header_flag)  INPUT_BYTE(((j_common_ptr)iinfo),c2,return FALSE);
      else                        INPUT_2BYTES(((j_common_ptr)iinfo),c2,return FALSE);
      TRACEMS3(iinfo,4,JXRTRC_TILE_VSEQUENCE, i+1, hdr.num_ver_tiles_minus1+1, c2);
      tile_width_in_mb[i] = c2;
    }
    for (int i=0; i<hdr.num_hor_tiles_minus1; i++) {
      if (hdr.short_header_flag)  INPUT_BYTE(((j_common_ptr)iinfo),c2,return FALSE);
      else                        INPUT_2BYTES(((j_common_ptr)iinfo),c2,return FALSE);
      TRACEMS3(iinfo,4,JXRTRC_TILE_HSEQUENCE, i+1, hdr.num_hor_tiles_minus1+1, c2);
      tile_height_in_mb[i] = c2;
    }
    // save to object
    hdr.tile_height_in_mb = tile_height_in_mb;
    hdr.tile_width_in_mb = tile_width_in_mb;
  }
  
  /* Read window margins, if present */
  if (hdr.windowing_flag) {
    INPUT_BYTE(((j_common_ptr)iinfo),c,return FALSE);
    INPUT_BYTE(((j_common_ptr)iinfo),cc,return FALSE);
    INPUT_BYTE(((j_common_ptr)iinfo),ccc,return FALSE);
    hdr.top_margin    =  GETBITS(c,0,6);
    hdr.left_margin   = (GETBITS(c, 6,2) << 4) + GETBITS(cc, 0,4);
    hdr.right_margin  = (GETBITS(cc,4,4) << 2) + GETBITS(ccc,0,2);
    hdr.bottom_margin =  GETBITS(ccc,2,6);
    TRACEMS4(iinfo,3,JXRTRC_WINDOW_MARGIN,  hdr.top_margin,  
                                            hdr.bottom_margin, 
                                            hdr.left_margin,
                                            hdr.right_margin );
  }
  
  INPUT_SYNC(iinfo);
  
  /* TODO return correct code */
  return JPEG_REACHED_SOS;
}

/*
 * Read a single image plane header of a coded image from the source data. The
 * source data should be at the start of the plane header.
 *  
 */          
GLOBAL(int)
jpegxr_image_read_plane_header (j_image_ptr iinfo, boolean alpha)
{
  UINT8 b; // for <=8 bits
  UINT8 c, cc;
  UINT16 c2;
  UINT32 c4;
  
  TRACEMS1(iinfo,2,JXRTRC_IMAGE_PLANE_BEGIN,alpha);
  
  /* Header to put things into */
  jxr_image_plane_header phdr;
  if (alpha) iinfo->alpha_plane_hdr = &phdr;
  else       iinfo->img_plane_hdr   = &phdr;
  
  /* Definitions to output enum strings */
  DEFINE_ENUM( JXR_BANDS_PRESENT, JXR_BANDS_PRESENT_DEF );
    
  INPUT_VARS(iinfo);

  /* Read in first byte */
  INPUT_BYTE(((j_common_ptr)iinfo),c,return FALSE);
  // internal colour format (3 bits)
  phdr.internal_clr_fmt = GETBITS(c,0,3);
  // scaled flag
  phdr.scaled_flag = GETBITS(c,3,1);
  TRACEMS1(iinfo,2,JXRTRC_SCALED_FLAG,phdr.scaled_flag);
  // bands present (4 bits)
  phdr.bands_present = GETBITS(c,4,4);
  TRACEMSS(iinfo,2,JXRTRC_BANDS_PRESENT, GetString_JXR_BANDS_PRESENT(phdr.bands_present));
  
  /* Internal colour format specific parameters */
  if (phdr.internal_clr_fmt ==  JINTCOL_YUV444 ||
      phdr.internal_clr_fmt ==  JINTCOL_YUV420 ||
      phdr.internal_clr_fmt ==  JINTCOL_YUV422) {
    
    /* We only require one byte of colour format specific stuff in this
     * case */
    INPUT_BYTE(((j_common_ptr)iinfo),c,return FALSE);
    /* First four bits */
    if (phdr.internal_clr_fmt == JINTCOL_YUV420 ||
        phdr.internal_clr_fmt == JINTCOL_YUV422) {
      phdr.reserved_e_bit = GETBITS(c,0,1);
      phdr.chroma_centering_x = GETBITS(c,1,3);
    } else if (phdr.internal_clr_fmt == JINTCOL_YUV444)
      phdr.reserved_f = GETBITS(c,0,4);
    /* Last four bits */
    if (phdr.internal_clr_fmt == JINTCOL_YUV420) {
      phdr.reserved_g_bit = GETBITS(c,4,1);
      phdr.chroma_centering_y = GETBITS(c,5,3);
    } else
      phdr.reserved_h = GETBITS(c,4,4);
    
  } else if (phdr.internal_clr_fmt == JINTCOL_NCOMPONENT) {
    
    /* Here we require one OR two bytes of colour format specific stuff */
    INPUT_BYTE(((j_common_ptr)iinfo), c, return FALSE);
    /* First four bits */
    phdr.num_components_minus1 = GETBITS(c,0,4);
    if (phdr.num_components_minus1 == 0xf) {
      /* Last 12 bits... */
      INPUT_BYTE(((j_common_ptr)iinfo), cc, return FALSE);
      phdr.num_components_extended_minus16 =
          (GETBITS(c,4,4) << 4) + GETBITS(cc,0,8);
    } else {
      /* ...or alternatively - last 4 bits */
      phdr.reserved_h = GETBITS(c,4,4);
    }
        
  } else if (phdr.internal_clr_fmt ==  JINTCOL_RESERVED1 ||
             phdr.internal_clr_fmt ==  JINTCOL_RESERVED2) {
    TRACEMS(iinfo,0,JXRTRC_RESERVED_VALUE);
  }
  
  /* Output bit depth specific parameters */
  INPUT_BYTE(((j_common_ptr)iinfo), c,return FALSE);
  if (iinfo->hdr->output_bitdepth == JOUTDEP_BD16  ||
      iinfo->hdr->output_bitdepth == JOUTDEP_BD16S ||
      iinfo->hdr->output_bitdepth == JOUTDEP_BD32S) {
        phdr.shift_bits = c;
  } else if (
      iinfo->hdr->output_bitdepth == JOUTDEP_BD32F) {
        INPUT_BYTE(((j_common_ptr)iinfo),cc,return FALSE);
        phdr.len_mantissa = c;
        phdr.exp_bias = cc;
  } else if (
      iinfo->hdr->output_bitdepth == JOUTDEP_RESERVED1 ||
      iinfo->hdr->output_bitdepth == JOUTDEP_RESERVED2) {
        TRACEMS(iinfo,0,JXRTRC_RESERVED_VALUE);
  }

  /* QP sets (unfortunately these aren't nicely byte aligned) */
  INPUT_BITS(((j_common_ptr)iinfo),b,1,return FALSE);
  // dc_image_plane_uniform_flag
  phdr.dc_image_plane_uniform_flag = b;
  // read in DC QPs if they are the same for all tiles
  if (phdr.dc_image_plane_uniform_flag)
    parse_dc_qp(iinfo);
  // read in low and high pass QPs if they are the same for all tiles
  if (phdr.bands_present == JBANDS_RESERVED) {
    TRACEMS(iinfo,0,JXRTRC_RESERVED_VALUE);
  } else if (phdr.bands_present != JBANDS_DCONLY) {
    INPUT_BITS(((j_common_ptr)iinfo),b,1,return FALSE);
    phdr.reserved_i_bit = b;
    INPUT_BITS(((j_common_ptr)iinfo),b,1,return FALSE);
    phdr.lp_image_plane_uniform_flag = b;
    // low pass
    if (phdr.lp_image_plane_uniform_flag) {
      parse_lp_qp(iinfo,1);
    }
    // high pass
    if (phdr.bands_present != JBANDS_NOHIGHPASS) {
      INPUT_BITS(((j_common_ptr)iinfo),b,1,return FALSE);
      phdr.reserved_j_bit = b;
      INPUT_BITS(((j_common_ptr)iinfo),b,1,return FALSE);
      phdr.hp_image_plane_uniform_flag = b;
      if (phdr.hp_image_plane_uniform_flag) {
        parse_hp_qp(iinfo,1);
      }
    }
  }
  // Remainder of byte is just byte alignment padding


  INPUT_SYNC(iinfo);
  
  /* TODO return correct code */
  return JPEG_REACHED_SOS;
}
