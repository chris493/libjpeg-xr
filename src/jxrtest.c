/*
 * jxrtest.c
 *
 * Copyright (C) 2011, Chris Harding.
 * 
 * This file contains functions used for testing functionality and
 * conformance to the JPEG-XR specification.
 * 
 */

  fprintf(stdout, "File header\n");
  fprintf(stdout, "fixed_file_header_ii_2bytes : %x\n", finfo->fixed_file_header_ii_2bytes);
  fprintf(stdout, "fixed_file_header_0xbc_byte : %x\n", finfo->fixed_file_header_0xbc_byte);
  fprintf(stdout, "file_version_id : %x\n", finfo->file_version_id);
  fprintf(stdout, "first_ifd_offset : %x\n\n", finfo->first_ifd_offset);
  
  fprintf(stdout, "Directory\n");
  fprintf(stdout, "num_entries: %x\n", finfo->dirs[0]->num_entries );
  fprintf(stdout, "zero_or_next_ifd_offset: %x\n\n", finfo->dirs[0]->zero_or_next_ifd_offset);

  for (int i=0; i< finfo->dirs[0]->num_entries; i++ ) {
    fprintf(stdout, "IFD entry %d\n", i);
    fprintf(stdout, "field_tag : %x\n",         finfo->dirs[0]->ifd_entry_list[i]->field_tag);
    fprintf(stdout, "element_type : %x\n",      finfo->dirs[0]->ifd_entry_list[i]->element_type);
    fprintf(stdout, "num_elements : %x\n",      finfo->dirs[0]->ifd_entry_list[i]->num_elements);
    fprintf(stdout, "values_or_offset : %x\n\n",  finfo->dirs[0]->ifd_entry_list[i]->values_or_offset);
  }

  fprintf(stdout, "Supported IFD entry values\n");
  for (int i=0; i<16; i++) 
    fprintf(stdout, "pixel format: %x\n", finfo->dirs[0]->pixel_format[i] );
  fprintf(stdout, "image_width: %x\n", finfo->dirs[0]->image_width );
  fprintf(stdout, "image_height: %x\n", finfo->dirs[0]->image_height );
  fprintf(stdout, "image_offset: %x\n", finfo->dirs[0]->image_offset );
  fprintf(stdout, "image_byte_count: %x\n", finfo->dirs[0]->image_byte_count );
