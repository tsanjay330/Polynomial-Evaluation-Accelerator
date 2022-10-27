/*******************************************************************************
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
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "lide_c_bmp.h"
#include "lide_c_bmp_file_write.h"
#include "lide_c_util.h"

struct _lide_c_bmp_file_write_context_type_struct {
#include "lide_c_actor_context_type_common.h"
    int active;
    FILE *file;  

    /* Actor interface ports. */
    lide_c_fifo_pointer tile_in;
    lide_c_fifo_pointer bmpinfo_in;
    lide_c_fifo_pointer newrow_in;

    /*local state*/
    bmp_file_info bmpinfo; /*info token about loaded bmp*/
 
    int tileX;     /*length of the tile */
    int tileY;     /*height of the tile */
    int halo;      /*halo padding around tile dimensions*/

    int tileIndexX;
    int tileIndexY;
 
    int pullBmpInfo;

    unsigned char * data[100];
};

lide_c_bmp_file_write_context_type *lide_c_bmp_file_write_new(FILE *file, 
        int tileX, int tileY, lide_c_fifo_pointer tile_in, 
        lide_c_fifo_pointer newrow_in, lide_c_fifo_pointer bmpinfo_in) {

    lide_c_bmp_file_write_context_type *context = NULL;
    context = lide_c_util_malloc(sizeof(lide_c_bmp_file_write_context_type));

    context->enable = (lide_c_actor_enable_function_type)
            lide_c_bmp_file_write_enable;
    context->invoke = (lide_c_actor_invoke_function_type)
            lide_c_bmp_file_write_invoke;
    context->mode = LIDE_C_BMP_FILE_WRITE_MODE_INFO;

    /*length of the tile */
    context->tileX = tileX;

    /*height of the tile */
    context->tileY = tileY;

    /* tile output - tokens are pointers to image data*/
    context->tile_in = tile_in; 

    /*link for sending the bmp header info to the writer */
    context->bmpinfo_in = bmpinfo_in;

    /* True token indicates tile is part of a new row */
    context->newrow_in = newrow_in;

    context->tileIndexX = 0;
    context->tileIndexY = 0;

    context->file = file;

    return context;
}

bool lide_c_bmp_file_write_enable(
        lide_c_bmp_file_write_context_type *context) {
    bool result = false;

    switch (context->mode) {
        case LIDE_C_BMP_FILE_WRITE_MODE_INFO:
            result =  (lide_c_fifo_population(context->bmpinfo_in) >= 1);
            break;
        case LIDE_C_BMP_FILE_WRITE_MODE_PROCESS:
            result = (lide_c_fifo_population(context->tile_in) >= 1) &&
                    (lide_c_fifo_population(context->newrow_in) >= 1);
            break;
        default:
            result = false;
            break;
    }
    return result;
}

void lide_c_bmp_file_write_invoke(lide_c_bmp_file_write_context_type *context) {
    int tileX = context->tileX;
    int tileY = context->tileY;
    int tileIndexX = context->tileIndexX;
    float * newtile;
    int newrow;
    int x, y;

    switch (context->mode) {
        case LIDE_C_BMP_FILE_WRITE_MODE_INFO:
            /*pull bmpinfo just once, so that the bmp header is written out.*/
            lide_c_fifo_read(context->bmpinfo_in, &(context->bmpinfo)); 
            fwrite(&context->bmpinfo.bmptype, sizeof(short), 1, context->file);
            fwrite(&context->bmpinfo.bmpheader, sizeof(bmp_file_header), 1,
                    context->file);
            fwrite(context->bmpinfo.pallet,
                    sizeof(int),context->bmpinfo.bmpheader.header.num_colors,
                    context->file);
            context->mode = LIDE_C_BMP_FILE_WRITE_MODE_PROCESS;
            break;
        case LIDE_C_BMP_FILE_WRITE_MODE_PROCESS:
            lide_c_fifo_read(context->tile_in, &newtile);

            /*form a new tile*/
            context->data[tileIndexX] = malloc(sizeof(byte)*(tileX)*(tileY));

            /*assume the last index points to the right part of memory*/
            for (y = 0; y < tileY; y++) {
                for (x = 0; x < tileX; x++) {
                    context->data[tileIndexX][(tileX)*y+x] =
                            (byte)newtile[(tileX)*y+x];
                }
            }

            lide_c_fifo_read(context->newrow_in, &newrow); 
        
            if (newrow) {
                /*save away existing row to file*/ 
                for (y = 0; y < tileY; y++) {
                    for (x = 0;x < tileIndexX + 1; x++) {
                      fwrite(&(context->data[x][y*tileX]), sizeof(byte), tileX,
                            context->file);
                    }
                }
                context->tileIndexX = 0;
                context->tileIndexY++;
            } else {
                /*otherwise keep collecting tiles, because we are note done with
                 * the row.
                 */
                context->tileIndexX++;
            }
            context->mode = LIDE_C_BMP_FILE_WRITE_MODE_PROCESS;
            break;
    }   
}

void lide_c_bmp_file_write_terminate(
        lide_c_bmp_file_write_context_type *context) {

    fclose(context->file);
    free(context);
}
