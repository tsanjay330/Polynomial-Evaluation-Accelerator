#ifndef _lide_c_bmp_h
#define _lide_c_bmp_h

/******************************************************************************
@ddblock_begin copyright

Copyright (c) 1997-2018
Maryland DSPCAD Research Group, The University of Maryland at College Park 

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

IN NO EVENT SHALL THE UNIVERSITY OF MARYLAND BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF MARYLAND HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF MARYLAND SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
MARYLAND HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

@ddblock_end copyright
*****************************************************************************/
#include <stdio.h>

typedef union bmp_file_header_union {
    struct header_struct{ 
        unsigned int    size; 
        unsigned int    reserved; 
        unsigned int    bitmap_offset;
        unsigned int    header_size; 
        signed   int    width; 
        signed   int    height; 
        unsigned short  planes; 
        unsigned short  bits_per_pixel; 
        unsigned int    compression; 
        unsigned int    bitmap_size;
        signed   int    horizontal_resolution;
        signed   int    vertical_resolution;
        unsigned int    num_colors; 
        unsigned int    num_important_colors; 
    } header;
    unsigned char raw[52];
} bmp_file_header;

typedef struct bmp_file_info_struct {
    unsigned short bmptype;
    bmp_file_header bmpheader;
    int pallet[500];
} bmp_file_info;

#endif
