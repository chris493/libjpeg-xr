/*
 * jxrfile.c
 *
 * Copyright (C) 2011, Chris Harding.
 * Copyright (C) 1994-1998, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 * 
 * This file contains algorithms used to parse the image and image plane
 * layers of a JPEG-XR coded image.
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


/* Determine number of bands in a plane. The number of bands in the alpha
 * plane must be equal to or greater than then number in the primary
 * plane.
 */
LOCAL(unsigned int)
determine_num_bands(j_image_ptr iinfo, jxr_image_plane_header * plane_hdr) {
  
  if (plane_hdr->bands_present == JBANDS_ALL)
    return 4;
  else if (plane_hdr->bands_present == JBANDS_NOFLEXBITS)
    return 3;
  else if (plane_hdr->bands_present == JBANDS_NOHIGHPASS)
    return 2;
  else if (plane_hdr->bands_present == JBANDS_DCONLY)
    return 1;
  else {
    WARNMSS(iinfo,JXRWRN_RESERVED_VALUE,"BANDS_PRESENT");
    return 4;
  }

}


/* Determine number of colour components, based on INTERNAL_CLR_FMT,
 * NUM_COMPONENTs_MINUS1 and NUM_COMPONENTS_EXTENDED_MINUS16, which must
 * all have been set (if they exist in the codestream) before calling.
 */
LOCAL(unsigned int)
determine_num_components(j_image_ptr iinfo, jxr_image_plane_header * hdr) {
  
  unsigned int num_components;
  
  if (hdr->internal_clr_fmt == JINTCOL_NCOMPONENT) {
    if (hdr->num_components_minus1 == 0xf)
      num_components = hdr->num_components_extended_minus16 + 16;
    else
      num_components = hdr->num_components_minus1 + 1;
  } else if (hdr->internal_clr_fmt == JINTCOL_YONLY) {
    num_components = 1;
  } else if ( hdr->internal_clr_fmt == JINTCOL_YUV420 ||
              hdr->internal_clr_fmt == JINTCOL_YUV422 ||
              hdr->internal_clr_fmt == JINTCOL_YUV444  ) {
    num_components = 3;
  } else if (hdr->internal_clr_fmt == JINTCOL_YUVK) {
    num_components = 4;
  } else {
    TRACEMSS(iinfo,0,JXRTRC_RESERVED_VALUE,"INTERNAL_COLOUR_FORMAT");
  }
  
  TRACEMS1(iinfo, 3, JXRTRC_NUM_COMP, num_components);
  
  return num_components;
}


/*
 * Read prfile level info.
 *  
 */          
LOCAL(int)
read_profile_level_info(j_image_ptr iinfo, UINT16 * i_bytes) {
  
  UINT8 c;
  UINT16 c2;
  
  UINT16 num_bytes = 0;
  
  /* Vars */
  UINT8 profile_idc ;
  UINT8 level_idc   ;
  UINT16 reserved_l ; 
  UINT8 last_flag   ;
  
  /* Read the profile information */
  INPUT_VARS(iinfo);
  for (int i_last=0; i_last == 0; i_last = last_flag) {
    
    TRACEMS1( iinfo, 2, JXRTRC_PROFILE_INFO, idx );
    
    INPUT_BYTE((j_common_ptr)iinfo,c,return FALSE);
    profile_idc = c;
    INPUT_BYTE((j_common_ptr)iinfo,c,return FALSE);
    level_idc   = c;
    INPUT_2BYTES((j_common_ptr)iinfo,c2,return FALSE);
    // TODO - fix possible endianness issues
    reserved_l   = (c2 >> 1);
    last_flag    = (c2 & 0x0001);
    num_bytes += 4;
  }
  INPUT_SYNC(iinfo);
  
  /* Check that we support this image. */
  if (profile_idc > JXR_PROFILE_IDC_MAX_SUPPORTED) {
    WARNMS2(iinfo,JXRWRN_PROFILE_UNSUPPORTED, profile_idc, JXR_PROFILE_IDC_MAX_SUPPORTED);
  } else {
    TRACEMS1(iinfo,3,JXRTRC_PROFILE_SUPPORTED, profile_idc);
  }
  
  TRACEMS1(iinfo,3,JXRTRC_IBYTES, num_bytes);
  *i_bytes = num_bytes;
  
  /* TODO return correct code */
  return JPEG_REACHED_SOS;
  
}


/*
 * Read a two, four or eight byte index element. Currently 8-bytes are
 * not supported.
 *  
 */          
LOCAL(int)
vlw_esc (j_image_ptr iinfo, UINT16 * i_value, unsigned int count)
{
  UINT8 first_byte,second_byte;
  UINT32 four_bytes;
  // Currently 8-bytes is unsupported
  
  INPUT_VARS(iinfo);
  
  for (unsigned int i=0; i<count;i++) {
    
    INPUT_BYTE((j_common_ptr)iinfo,first_byte,return FALSE);
    if (first_byte < 0xFB) {
      INPUT_BYTE((j_common_ptr)iinfo,second_byte,return FALSE);
      i_value[i] = first_byte * 256 + second_byte;
    } else if (first_byte == 0xFB) {
      INPUT_4BYTES((j_common_ptr)iinfo,four_bytes,return FALSE);
      i_value[i] = four_bytes;
    } else if (first_byte == 0xFC) {
      ERREXIT(iinfo,JXRERR_64_BITS);
    } else {/* first_byte is 0xFD, or 0xFE, or 0xFF */
      i_value[i] = 0; /* Escape Mode */
    }
    TRACEMS1(iinfo,4,JXRTRC_VLW_ESC, i_value[i]);
    
  }
  
  INPUT_SYNC(iinfo);
  
  /* TODO return correct code */
  return JPEG_REACHED_SOS;
}


/*
 * Read additional bytes before the coded tiles.
 *  
 */          
LOCAL(int)
read_additional (j_image_ptr iinfo)
{
  UINT8 c;
  UINT16 i_bytes;
  
  INPUT_VARS(iinfo);
  
  TRACEMS1(iinfo,2,JXRTRC_BEGIN_ABYTES ,idx);

  // Read subsequent bytes value
  vlw_esc(iinfo, &iinfo->vars.subsequent_bytes, 1);
  TRACEMS1(iinfo,3,JXRTRC_SUBBYTES, iinfo->vars.subsequent_bytes );
  
  if (iinfo->vars.subsequent_bytes > 0) {
    
    /* Read profile information and check supported */
    read_profile_level_info(iinfo,&i_bytes);
    
    // Calculate how many bytes before the coded tiles 
    UINT16 value_additional_bytes = 
      iinfo->vars.subsequent_bytes - i_bytes;
    
    TRACEMS1(iinfo,2,JXRTRC_SKIP_TO_TILES, value_additional_bytes);

    // Skip forward by number of additional bytes
    (*iinfo->src->skip_input_data) ((j_common_ptr) iinfo, (long) value_additional_bytes);
     
  }
    
  /* TODO return correct code */
  return JPEG_REACHED_SOS;
}


/*
 * Read a tile index table from the coded image.
 *  
 */          
LOCAL(int)
read_index_table (j_image_ptr iinfo)
{
  UINT16 c2;
  UINT16 num_index_table_entries;
  INPUT_VARS(iinfo);
  
  /* Alias to header */
  jxr_image_header * hdr = &iinfo->hdr;  
  
  TRACEMS1(iinfo,2,JXRTRC_INDEX_TABLE_BEGIN,idx);
  
  /* Calculate number of indextable entries */  
  if (!hdr->frequency_mode_codestream_flag) {
    num_index_table_entries =
      (hdr->num_hor_tiles_minus1 + 1) *
      (hdr->num_ver_tiles_minus1 + 1);
  } else {
    num_index_table_entries =
      (hdr->num_hor_tiles_minus1 + 1) *
      (hdr->num_ver_tiles_minus1 + 1) *
      (iinfo->vars.num_bands_of_primary);
  }
  TRACEMS1( iinfo, 3, JXRTRC_IDX_TABLE_ENTRIES, num_index_table_entries);
  
  /* 16-bit start code. Values other that JXR_INDEX_TABLE_STARTCODE are
   * reserved */
  INPUT_2BYTES((j_common_ptr)iinfo,c2,return FALSE);
  if (c2 != JXR_INDEX_TABLE_STARTCODE)
    TRACEMSS(iinfo,0,JXRTRC_RESERVED_VALUE,"INDEX_TABLE_STARTCODE");
  
  /* Allocate index offset array */
  UINT16 * index_offset_tile = (*iinfo->mem->alloc_small) (
        (j_common_ptr) iinfo,
        JPOOL_IMAGE,
        num_index_table_entries * SIZEOF(UINT16)
  );
  iinfo->vars.index_offset_tile = index_offset_tile;
  /* Read the index table offset entries */
  INPUT_SYNC(iinfo);
  vlw_esc(iinfo, index_offset_tile, num_index_table_entries);

  /* TODO return correct code */
  return JPEG_REACHED_SOS;
}


LOCAL(void)
calculate_component_array_sizes (j_image_ptr iinfo, jxr_image_header * hdr, jxr_image_plane_header * plane_hdr)
{
  /* Allocate extended dimensions arrays */
  UINT32 * extended_width = (*iinfo->mem->alloc_small) (
        (j_common_ptr) iinfo,
        JPOOL_IMAGE,
        (iinfo->image_plane_vars.num_components) * SIZEOF(UINT32)
  );
  UINT32 * extended_height = (*iinfo->mem->alloc_small) (
        (j_common_ptr) iinfo,
        JPOOL_IMAGE,
        (iinfo->image_plane_vars.num_components) * SIZEOF(UINT32)
  );
  iinfo->vars.extended_width = extended_width;
  iinfo->vars.extended_height = extended_height;
  
  /* Calculate extended dimensions for luma component */
  extended_width[0]  = hdr->width_minus1 + 1 +
                                    hdr->top_margin +
                                    hdr->bottom_margin;
  extended_height[0] = hdr->height_minus1 + 1 +
                                    hdr->left_margin +
                                    hdr->right_margin;
  TRACEMS2(iinfo, 3, JXRTRC_LUMA_DIMENSIONS,
                      extended_width[0],
                      extended_height[0]);
            
                                    
  /* Calculate extended dimensions for chroma components */
  if (plane_hdr->internal_clr_fmt == JINTCOL_YUV420) {
    extended_height[1] = extended_height[0] / 2;
    extended_width[1] = extended_width[0] / 2;
  } else if (plane_hdr->internal_clr_fmt == JINTCOL_YUV422) {
    extended_height[1] = extended_height[0];
    extended_width[1] = extended_width[0] / 2;
  } else {
    extended_height[1] = extended_height[0];
    extended_width[1] = extended_width[0];
  }
  TRACEMS2(iinfo, 3, JXRTRC_CHROMA_DIMENSIONS,
                      extended_width[1],
                      extended_height[1]);
  
  /* Specification says all other dimensions are the same,
   * slightly pointless but for clarity we duplicate them */
  for (unsigned int i=1; i < iinfo->image_plane_vars.num_components; i++) {
      extended_height[i] = extended_height[1];
      extended_width[i] = extended_width[1];
  }
  
  /* Also store sizes in macroblocks, one MB = 16x16 samples */
  iinfo->vars.mb_height = extended_height[0] / 16;
  iinfo->vars.mb_width = extended_width[0] / 16;
  
}



/*
 * Read a single image plane header of a coded image from the source data. The
 * source data should be at the start of the plane header.
 *  
 */          
LOCAL(int)
read_plane_header (j_image_ptr iinfo, boolean alpha)
{
  UINT8 b; // for <=8 bits
  UINT8 c, cc;
  UINT16 c2;
  UINT32 c4;
  
  /* Aliases to headers and vars */
  jxr_image_header * hdr = &iinfo->hdr;
  jxr_image_plane_header * plane_hdr;
  jxr_image_plane_vars * plane_vars;
  /* set alpha flag while where at it */
  if (alpha) {
    plane_hdr = &iinfo->alpha_plane_hdr;
    plane_vars = &iinfo->alpha_plane_vars;
    plane_vars->is_curr_plane_alpha_flag = TRUE;
  } else {
    plane_hdr = &iinfo->image_plane_hdr;
    plane_vars = &iinfo->image_plane_vars;
    plane_vars->is_curr_plane_alpha_flag = FALSE;
  }
  
  /* Definitions to output enum strings */
  DEFINE_ENUM( JXR_BANDS_PRESENT, JXR_BANDS_PRESENT_DEF );
  DEFINE_ENUM( JXR_INTERNAL_CLR_FMT, JXR_INTERNAL_CLR_FMT_DEF );
    
  INPUT_VARS(iinfo);

  TRACEMS2(iinfo,2,JXRTRC_IMAGE_PLANE_BEGIN,idx,alpha);

  /* Read in first byte */
  INPUT_BYTE(((j_common_ptr)iinfo),c,return FALSE);
  // internal colour format (3 bits)
  plane_hdr->internal_clr_fmt = GETBITS(c,0,3);
  TRACEMSS(iinfo,3,JXRTRC_INTERNAL_CLR_FORMAT,
    GetString_JXR_INTERNAL_CLR_FMT( plane_hdr->internal_clr_fmt) );
  // scaled flag
  plane_hdr->scaled_flag = GETBITS(c,3,1);
  TRACEMS1(iinfo,3,JXRTRC_SCALED_FLAG,plane_hdr->scaled_flag);
  // bands present (4 bits)
  plane_hdr->bands_present = GETBITS(c,4,4);
  TRACEMSS(iinfo,3,JXRTRC_BANDS_PRESENT, GetString_JXR_BANDS_PRESENT(plane_hdr->bands_present));
  // calculate number of bands
  plane_vars->num_bands = determine_num_bands(iinfo,plane_hdr);
  TRACEMS1(iinfo, 3, JXRTRC_FREQUENCY_BANDS, plane_vars->num_bands);
  if (!alpha) {
    iinfo->vars.num_bands_of_primary = plane_vars->num_bands;
  } else if (plane_vars->num_bands > iinfo->vars.num_bands_of_primary) {
    WARNMS(iinfo,JXRWRN_NUM_BANDS);
  }
  
  /* Internal colour format specific parameters */
  // Defaults
  plane_hdr->chroma_centering_x = 0;
  plane_hdr->chroma_centering_y = 0;
  if (plane_hdr->internal_clr_fmt ==  JINTCOL_YUV444 ||
      plane_hdr->internal_clr_fmt ==  JINTCOL_YUV420 ||
      plane_hdr->internal_clr_fmt ==  JINTCOL_YUV422) {
    
    /* We only require one byte of colour format specific stuff in this
     * case */
    INPUT_BYTE(((j_common_ptr)iinfo),c,return FALSE);
    /* First four bits */
    if (plane_hdr->internal_clr_fmt == JINTCOL_YUV420 ||
        plane_hdr->internal_clr_fmt == JINTCOL_YUV422) {
      plane_hdr->reserved_e_bit = GETBITS(c,0,1);
      if (plane_hdr->reserved_e_bit != 0) TRACEMSS(iinfo,0,JXRTRC_FUTURE_SPEC,"RESERVED_E_BIT");
      plane_hdr->chroma_centering_x = GETBITS(c,1,3);
      TRACEMS1(iinfo,3,JXTRC_CHROMA_CENTRE_X,plane_hdr->chroma_centering_x);
      // check for reserved values
      if (plane_hdr->chroma_centering_x == 5 || plane_hdr->chroma_centering_x == 6) {
        plane_hdr->chroma_centering_x = 7;
        TRACEMSS(iinfo,0,JXRTRC_RESERVED_VALUE,"CHROMA_CENTERING_X");
      }
    } else if (plane_hdr->internal_clr_fmt == JINTCOL_YUV444) {
      plane_hdr->reserved_f = GETBITS(c,0,4);
      if (plane_hdr->reserved_f != 0) TRACEMSS(iinfo,0,JXRTRC_FUTURE_SPEC,"RESERVED_F");
    }
    
    /* Last four bits */
    if (plane_hdr->internal_clr_fmt == JINTCOL_YUV420) {
      plane_hdr->reserved_g_bit = GETBITS(c,4,1);
      if (plane_hdr->reserved_g_bit != 0) TRACEMSS(iinfo,0,JXRTRC_FUTURE_SPEC,"RESERVED_G_BIT");
      plane_hdr->chroma_centering_y = GETBITS(c,5,3);
      TRACEMS1(iinfo,3,JXTRC_CHROMA_CENTRE_Y,plane_hdr->chroma_centering_y);
      // check for reserved values
      if (plane_hdr->chroma_centering_y == 5 || plane_hdr->chroma_centering_y == 6) {
        plane_hdr->chroma_centering_y = 7;
        TRACEMSS(iinfo,0,JXRTRC_RESERVED_VALUE,"CHROMA_CENTERING_Y");
      }
    } else {
      plane_hdr->reserved_h = GETBITS(c,4,4);
      if (plane_hdr->reserved_h != 0) TRACEMSS(iinfo,0,JXRTRC_FUTURE_SPEC,"RESERVED_H");
    }
    
  } else if (plane_hdr->internal_clr_fmt == JINTCOL_NCOMPONENT) {
    // TODO - test and output etc etc
    /* Here we require one OR two bytes of colour format specific stuff */
    INPUT_BYTE(((j_common_ptr)iinfo), c, return FALSE);
    /* First four bits */
    plane_hdr->num_components_minus1 = GETBITS(c,0,4);
    if (plane_hdr->num_components_minus1 == 0xf) {
      /* Last 12 bits... */
      INPUT_BYTE(((j_common_ptr)iinfo), cc, return FALSE);
      plane_hdr->num_components_extended_minus16 =
          (GETBITS(c,4,4) << 4) + GETBITS(cc,0,8);
    } else {
      /* ...or alternatively - last 4 bits */
      plane_hdr->reserved_h = GETBITS(c,4,4);
    }
        
  }
  
  /* We can now determine the number of components */
  plane_vars->num_components = determine_num_components(iinfo, plane_hdr);
  
  /* For the primary image plane, we can now calculate the extended
   * dimensions */
  if (!alpha) calculate_component_array_sizes (iinfo, hdr, plane_hdr);
  
  /* Output bit depth specific parameters */
  if (hdr->output_bitdepth == JOUTDEP_BD16  ||
      hdr->output_bitdepth == JOUTDEP_BD16S ||
      hdr->output_bitdepth == JOUTDEP_BD32S) {
        INPUT_BYTE(((j_common_ptr)iinfo), c,return FALSE);
        plane_hdr->shift_bits = c;
        printf("Output formatting: shift bits set to 0x%x\n", plane_hdr->shift_bits);
  } else if (
      hdr->output_bitdepth == JOUTDEP_BD32F) {
        INPUT_BYTE(((j_common_ptr)iinfo), c,return FALSE);
        INPUT_BYTE(((j_common_ptr)iinfo),cc,return FALSE);
        plane_hdr->len_mantissa = c;
        plane_hdr->exp_bias = cc;
        printf("Output formatting: mantissa length set to %u and exponent bias %u\n",
                    plane_hdr->len_mantissa,
                    plane_hdr->exp_bias );
  } else if (
      hdr->output_bitdepth == JOUTDEP_RESERVED1 ||
      hdr->output_bitdepth == JOUTDEP_RESERVED2) {
        TRACEMSS(iinfo,0,JXRTRC_RESERVED_VALUE,"OUTPUT_BITDEPTH");
  }


  /* QP sets (unfortunately these aren't nicely byte aligned) */
  INPUT_BITS(((j_common_ptr)iinfo),b,1,return FALSE);
  // dc_image_plane_uniform_flag
  plane_hdr->dc_image_plane_uniform_flag = b;
  TRACEMS1(iinfo,3,JXTRC_DC_UNIFORM,plane_hdr->dc_image_plane_uniform_flag);
  
  // read in DC QPs if they are the same for all tiles
  if (plane_hdr->dc_image_plane_uniform_flag) {
    INPUT_SYNC(iinfo);
    parse_dc_qp(iinfo, plane_vars->num_components, &plane_hdr->dc_qp);
    INPUT_RELOAD(iinfo);
  }
  // read in low and high pass QPs if they are the same for all tiles
  if (plane_hdr->bands_present != JBANDS_DCONLY) {
    INPUT_BITS(((j_common_ptr)iinfo),b,1,return FALSE);
    plane_hdr->reserved_i_bit = b;
    INPUT_BITS(((j_common_ptr)iinfo),b,1,return FALSE);
    plane_hdr->lp_image_plane_uniform_flag = b;
    TRACEMS1(iinfo,3,JXTRC_LP_UNIFORM,plane_hdr->lp_image_plane_uniform_flag);
    
    // low pass
    if (plane_hdr->lp_image_plane_uniform_flag) {
      // first set number of LP QPs
      plane_vars->num_lp_qps = 1;
      INPUT_SYNC(iinfo);
      parse_lp_qp(iinfo,plane_vars->num_components,1,&plane_hdr->lp_qp);
      INPUT_RELOAD(iinfo);
    }
    // high pass
    if (plane_hdr->bands_present != JBANDS_NOHIGHPASS) {
      INPUT_BITS(((j_common_ptr)iinfo),b,1,return FALSE);
      plane_hdr->reserved_j_bit = b;
      INPUT_BITS(((j_common_ptr)iinfo),b,1,return FALSE);
      plane_hdr->hp_image_plane_uniform_flag = b;
      TRACEMS1(iinfo,3,JXTRC_HP_UNIFORM,plane_hdr->hp_image_plane_uniform_flag);
      
      if (plane_hdr->hp_image_plane_uniform_flag) {
        // first set number of HP QPs
        plane_vars->num_hp_qps = 1;
        INPUT_SYNC(iinfo);
        parse_hp_qp(iinfo,plane_vars->num_components,1,&plane_hdr->hp_qp);
        INPUT_RELOAD(iinfo);
      }
    }
  }
  // Remainder of byte is just byte alignment padding
  INPUT_ALIGN((j_common_ptr)iinfo, return FALSE);

  
  INPUT_SYNC(iinfo);
  
  
  /* TODO return correct code */
  return JPEG_REACHED_SOS;
}

/*
 * Determine the tile boundaries and number of MB in each tile.
 *  
 */          
LOCAL(int)
determine_tile_boundaries (j_image_ptr iinfo)
{
  
  jxr_image_header * hdr = &iinfo->hdr;
  
  // First allocate storage
   iinfo->vars.left_mb_index_of_tile = (*iinfo->mem->alloc_small) (
            (j_common_ptr) iinfo,
            JPOOL_IMAGE,
            (hdr->num_ver_tiles_minus1+1) * SIZEOF(UINT16)
        );
   iinfo->vars.top_mb_index_of_tile = (*iinfo->mem->alloc_small) (
            (j_common_ptr) iinfo,
            JPOOL_IMAGE,
            (hdr->num_hor_tiles_minus1+1) * SIZEOF(UINT16)
        );
  // TODO - are we OK allocating this much storage this way?
   iinfo->vars.num_mb_in_tile = (*iinfo->mem->alloc_small) (
            (j_common_ptr) iinfo,
            JPOOL_IMAGE,
            (hdr->num_ver_tiles_minus1+1) *
            (hdr->num_hor_tiles_minus1+1) *
            SIZEOF(UINT16)
        );
  
  // Left MB index boundaries
  iinfo->vars.left_mb_index_of_tile[0] = 0;
  for (unsigned int n = 0; n < hdr->num_ver_tiles_minus1; n++) {
    iinfo->vars.left_mb_index_of_tile[n+1] = iinfo->vars.left_mb_index_of_tile[n] + hdr->tile_width_in_mb[n];
  }
  iinfo->vars.left_mb_index_of_tile[hdr->num_ver_tiles_minus1 + 1] = iinfo->vars.mb_width;
  // Top MB index boundaries
  iinfo->vars.top_mb_index_of_tile[0] = 0;
  for (unsigned int n = 0; n < hdr->num_hor_tiles_minus1; n++) {
    iinfo->vars.top_mb_index_of_tile[n+1] = iinfo->vars.top_mb_index_of_tile[n] + hdr->tile_height_in_mb[n];
  }
  iinfo->vars.top_mb_index_of_tile[hdr->num_hor_tiles_minus1 + 1] = iinfo->vars.mb_height;
  // Total number of MB in each tile
  unsigned int n=0;
  for (unsigned int i = 0; i < hdr->num_hor_tiles_minus1 + 1; i++) {
    for (unsigned int j = 0; j < hdr->num_ver_tiles_minus1 + 1; j++) {
      iinfo->vars.num_mb_in_tile[n] =
        (hdr->tile_height_in_mb[i]) * (hdr->tile_width_in_mb[j]);
      n++;
    }
  }
  
  /* TODO return correct code */
  return JPEG_REACHED_SOS;
  
}



/*
 * Read only the image header of a coded image from the source data. The
 * source data should be at the start of the image codestream.
 *  
 */          
LOCAL(int)
read_image_header (j_image_ptr iinfo)
{
  UINT8 c, cc, ccc;
  UINT16 c2;
  UINT32 c4, cc4;
  
  /* Alias to image header */
  jxr_image_header * hdr = &iinfo->hdr;
  
  INPUT_VARS(iinfo);
  
  /* Store start offset of coded image */
  iinfo->offset = idx;
  TRACEMS1(iinfo,2,JXRTRC_IMAGE_HEAD_BEGIN,idx);
  
  /* Read in GDI signature (8 bytes) */
  INPUT_4BYTES(((j_common_ptr)iinfo), c4, return FALSE);
  INPUT_4BYTES(((j_common_ptr)iinfo), cc4, return FALSE);
  if (c4 != JXR_GDI_SIG_HI || cc4 != JXR_GDI_SIG_LO )
    ERREXIT4(iinfo, JXRERR_WRONG_GDI_SIG, c4,cc4, JXR_GDI_SIG_HI, JXR_GDI_SIG_LO );
  TRACEMS2(iinfo, 3, JXRTRC_GDI_SIG, c4, cc4);
  
  /* Read in first byte */
  INPUT_BYTE(((j_common_ptr)iinfo),c,return FALSE);
  // reserved B (4 bits)
  hdr->reserved_b = GETBITS(c,0,4);
  if (hdr->reserved_b != 1) WARNMS(iinfo,JXRWRN_IMAGE_INCOMPATIBLE);
  // hard tiling flag
  hdr->hard_tiling_flag = GETBITS(c,4,1);
  TRACEMS1(iinfo,3,JXRTRC_HARD_TILING,hdr->hard_tiling_flag);
  // reserved C (3 bits)
  hdr->reserved_c = GETBITS(c,5,3);
  if (hdr->reserved_c != 1) TRACEMSS(iinfo,0,JXRTRC_FUTURE_SPEC,"RESERVED_C");
  
  /* Read in second byte */
  INPUT_BYTE(((j_common_ptr)iinfo),c,return FALSE);
  // tiling_flag
  hdr->tiling_flag = GETBITS(c,0,1);
  TRACEMS1(iinfo,3,JXRTRC_TILING,hdr->tiling_flag);
  // frequency_mode_codestream_flag
  hdr->frequency_mode_codestream_flag= GETBITS(c,1,1);
  TRACEMS1(iinfo,3,JXRTRC_FREQ_MODE,hdr->frequency_mode_codestream_flag);
  // spatial_xfrm_subordinate (3 bits)
  hdr->spatial_xfrm_subordinate.rcw = GETBITS(c,2,1);
  hdr->spatial_xfrm_subordinate.flip_h = GETBITS(c,3,1);
  hdr->spatial_xfrm_subordinate.flip_v = GETBITS(c,4,1);
  hdr->spatial_xfrm_subordinate.fill = GETBITS(c,2,3);
  TRACEMS4(iinfo,3,JXRTRC_XFRM_SUB,
    hdr->spatial_xfrm_subordinate.rcw,
    hdr->spatial_xfrm_subordinate.flip_h,
    hdr->spatial_xfrm_subordinate.flip_v,
    hdr->spatial_xfrm_subordinate.fill);
  // index_table_present_flag
  hdr->index_table_present_flag = GETBITS(c,5,1);
  TRACEMS1(iinfo,3,JXRTRC_IDX_TABLE_PRESENT,GETBITS(c,5,1));
  // overlap_mode (2 bits)
  hdr->overlap_mode = GETBITS(c,6,2);
  TRACEMS1(iinfo,3,JXRTRC_OVERLAP_MODE, hdr->overlap_mode);
  if (hdr->overlap_mode == 3) TRACEMSS(iinfo,0,JXRTRC_RESERVED_VALUE,"OVERLAP_MODE");
  
  /* Read in third byte */
  INPUT_BYTE(((j_common_ptr)iinfo),c,return FALSE);
  // short_header_flag
  hdr->short_header_flag = GETBITS(c,0,1);
  TRACEMS1(iinfo,3,JXRTRC_SHORT_HEADER, hdr->short_header_flag);
  // long_word_flag
  hdr->long_word_flag = GETBITS(c,1,1);
  TRACEMS1(iinfo,3,JXRTRC_LONG_WORD, hdr->long_word_flag);
  // windowing_flag
  hdr->windowing_flag = GETBITS(c,2,1);
  TRACEMS1(iinfo,3,JXRTRC_WINDOWING, hdr->windowing_flag);
  // trim_flexbits_flag
  hdr->trim_flexbits_flag = GETBITS(c,3,1);
  TRACEMS1(iinfo,3,JXRTRC_TRIM_FLEXBITS, hdr->trim_flexbits_flag);
  // reserved_d (3 bits)
  hdr->reserved_d = GETBITS(c,4,3);
  if (hdr->reserved_d !=0) TRACEMSS(iinfo,0,JXRTRC_RESERVED_VALUE,"RESERVED_D");
  // alpha_image_plane_flag
  hdr->alpha_image_plane_flag = GETBITS(c,7,1);
  TRACEMS1(iinfo,3,JXRTRC_ALPHA_IMAGE_PLANE, hdr->alpha_image_plane_flag);

  /* Read in fourth byte */
  // for easy reading out of enum string identifiers
  DEFINE_ENUM( JXR_OUTPUT_CLR_FMT, JXR_OUTPUT_CLR_FMT_DEF );
  DEFINE_ENUM( JXR_OUTPUT_BIT_DEPTH, JXR_OUTPUT_BIT_DEPTH_DEF );
  INPUT_BYTE(((j_common_ptr)iinfo),c,return FALSE);
  hdr->output_clr_fmt = GETBITS(c,0,4);
  TRACEMSS(iinfo,3,JXRTRC_OUTPUT_CLR_FORMAT,
    GetString_JXR_OUTPUT_CLR_FMT( hdr->output_clr_fmt) );
  hdr->output_bitdepth = GETBITS(c,4,4);
  TRACEMSS(iinfo,3,JXRTRC_OUTPUT_BITDEPTH,
    GetString_JXR_OUTPUT_BIT_DEPTH( hdr->output_bitdepth) );
    
  /* Read in dimensions */
  if (hdr->short_header_flag) {
    INPUT_2BYTES(((j_common_ptr)iinfo),c4,return FALSE);
    INPUT_2BYTES(((j_common_ptr)iinfo),cc4,return FALSE);
  } else {
    INPUT_4BYTES(((j_common_ptr)iinfo),c4,return FALSE);
    INPUT_4BYTES(((j_common_ptr)iinfo),cc4,return FALSE);
  }
  hdr->width_minus1 = c4;
  hdr->height_minus1 = cc4;
  TRACEMS2(iinfo,3,JXRTRC_DIMENSIONS, hdr->width_minus1+1, hdr->height_minus1+1);
  
  /* Read tiling dimensions, if present */
  if (hdr->tiling_flag) {
    INPUT_BYTE(((j_common_ptr)iinfo),c,return FALSE);
    INPUT_BYTE(((j_common_ptr)iinfo),cc,return FALSE);
    INPUT_BYTE(((j_common_ptr)iinfo),ccc,return FALSE);
    hdr->num_hor_tiles_minus1 = ((c << 4) & 0xffffff) + GETBITS(cc,0,4) ;
    hdr->num_ver_tiles_minus1 = ccc + ((GETBITS(cc,4,4) << 8) & 0xffffff) ;
  } else {
    hdr->num_hor_tiles_minus1 = 0;
    hdr->num_ver_tiles_minus1 = 0;
  }
  /* We can now calcualte the number of tile columns/rows */
  iinfo->vars.num_tile_cols = hdr->num_hor_tiles_minus1 + 1;
  iinfo->vars.num_tile_rows = hdr->num_ver_tiles_minus1 + 1;
  TRACEMS2(iinfo,3,JXRTRC_TILE_DIMENSIONS,
    iinfo->vars.num_tile_cols,
    iinfo->vars.num_tile_rows);
  

  /* Read in tile sequences, if present */
  // allocate memory
  hdr->tile_height_in_mb = (*iinfo->mem->alloc_small) (
            (j_common_ptr) iinfo,
            JPOOL_IMAGE,
            hdr->num_ver_tiles_minus1 * SIZEOF(UINT16)
        );
  hdr->tile_width_in_mb = (*iinfo->mem->alloc_small) (
            (j_common_ptr) iinfo,
            JPOOL_IMAGE,
            hdr->num_hor_tiles_minus1 * SIZEOF(UINT16)
        );
  // read in from stream
  for (int i=0; i<hdr->num_ver_tiles_minus1; i++) {
    if (hdr->short_header_flag)  INPUT_BYTE(((j_common_ptr)iinfo),c2,return FALSE);
    else                        INPUT_2BYTES(((j_common_ptr)iinfo),c2,return FALSE);
    TRACEMS3(iinfo,4,JXRTRC_TILE_VSEQUENCE, i+1, hdr->num_ver_tiles_minus1+1, c2);
    hdr->tile_width_in_mb[i] = c2;
  }
  for (int i=0; i<hdr->num_hor_tiles_minus1; i++) {
    if (hdr->short_header_flag)  INPUT_BYTE(((j_common_ptr)iinfo),c2,return FALSE);
    else                        INPUT_2BYTES(((j_common_ptr)iinfo),c2,return FALSE);
    TRACEMS3(iinfo,4,JXRTRC_TILE_HSEQUENCE, i+1, hdr->num_hor_tiles_minus1+1, c2);
    hdr->tile_height_in_mb[i] = c2;
  }
  
  /* We can now calculate tile MB boundaries and MB size */
  determine_tile_boundaries(iinfo);

  
  /* Read window margins, if present */
  if (hdr->windowing_flag) {
    INPUT_BYTE(((j_common_ptr)iinfo),c,return FALSE);
    INPUT_BYTE(((j_common_ptr)iinfo),cc,return FALSE);
    INPUT_BYTE(((j_common_ptr)iinfo),ccc,return FALSE);
    hdr->top_margin    =  GETBITS(c,0,6);
    hdr->left_margin   = (GETBITS(c, 6,2) << 4) + GETBITS(cc, 0,4);
    hdr->right_margin  = (GETBITS(cc,4,4) << 2) + GETBITS(ccc,0,2);
    hdr->bottom_margin =  GETBITS(ccc,2,6);
    TRACEMS4(iinfo,3,JXRTRC_WINDOW_MARGIN,  hdr->top_margin,  
                                            hdr->bottom_margin, 
                                            hdr->left_margin,
                                            hdr->right_margin );
  }
  
  INPUT_SYNC(iinfo);
    
  /* TODO return correct code */
  return JPEG_REACHED_SOS;
}


/*
 * Read a JPEG-XR image to obtain headers and decompression
 * parameters. This reads image and image plane layer information.
 * The source data should be at the start of the image
 * codestream.
 *  
 */          
GLOBAL(int)
jpegxr_image_read_header (j_image_ptr iinfo)
{

  TRACEMS(iinfo,1,JXRTRC_IMAGE_BEGIN);
    
  /* Read the image header at the start of the codestream */
  read_image_header (iinfo);
  
  /* Read the image plane header */
  read_plane_header (iinfo, FALSE);
  
  /* Read the alpha plane header, if one is present */
  if (iinfo->hdr.alpha_image_plane_flag)
    read_plane_header (iinfo, TRUE);
  
  /* Read the tile index table, if one is present */
  if (iinfo->hdr.index_table_present_flag)
    read_index_table (iinfo);
  
  /* Read additional bytes before coded image */
  read_additional (iinfo);
  
  /* TODO return correct code */
  return JPEG_REACHED_SOS;
}
