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
#include "lide_c_bmp_file_read_halo.h"
#include "lide_c_util.h"

struct _lide_c_bmp_file_read_halo_context_type_struct {
#include "lide_c_actor_context_type_common.h"
    FILE *file;  

    /* Actor interface ports. */
    lide_c_fifo_pointer tile_out;
    lide_c_fifo_pointer bmpinfo_out;
    lide_c_fifo_pointer newrow_out;
    lide_c_fifo_pointer halo_in;

    /*local state*/
    bmp_file_info bmpinfo; /*info token about loaded bmp*/
  
    int tileX;     /*length of the tile */
    int tileY;     /*height of the tile */
    int halo;      /*halo padding around tile dimensions*/

    int tileIndexX;
    int tileIndexY;

    int pushBmpInfo;

    unsigned char *data;
};

lide_c_bmp_file_read_halo_context_type *lide_c_bmp_file_read_halo_new(
        FILE *file, int tileX, int tileY, lide_c_fifo_pointer halo_in,
        lide_c_fifo_pointer tile_out, lide_c_fifo_pointer newrow_out,
        lide_c_fifo_pointer bmpinfo_out) {

    bmp_file_info bmpinfo;

    lide_c_bmp_file_read_halo_context_type *context = NULL;
    context = lide_c_util_malloc(sizeof(
            lide_c_bmp_file_read_halo_context_type));

    context->enable = 
            (lide_c_actor_enable_function_type)lide_c_bmp_file_read_halo_enable;
    context->invoke = 
            (lide_c_actor_invoke_function_type)lide_c_bmp_file_read_halo_invoke;
    context->mode = LIDE_C_BMP_FILE_READ_HALO_MODE_HALO;

    /*length of the tile */
    context->tileX = tileX;

    /*height of the tile */
    context->tileY = tileY;

    /* halo padding around tile dimensions now configurable by a dataflow
     * token
     */
    context->halo = 0;
    context->halo_in = halo_in;

    /* tile output - tokens are pointers to image data*/
    context->tile_out = tile_out;

    /* True token indicates tile is part of a new row */
    context->newrow_out = newrow_out;

    /*link for sending the bmp header info to the writer */
    context->bmpinfo_out = bmpinfo_out;

    context->tileIndexX = 0;
    context->tileIndexY = 0;

    /*read off the header information from the bmp*/
    if (!fread(&bmpinfo.bmptype, sizeof(unsigned short), 1, file)) {
        context->mode = LIDE_C_BMP_FILE_READ_HALO_MODE_INACTIVE;
    }

    if (!fread(&bmpinfo.bmpheader, sizeof(bmp_file_header), 1, file)) {
        context->mode = LIDE_C_BMP_FILE_READ_HALO_MODE_INACTIVE;
    }

    if (!fread(bmpinfo.pallet, 4, bmpinfo.bmpheader.header.num_colors, file)) {
        context->mode = LIDE_C_BMP_FILE_READ_HALO_MODE_INACTIVE;
    }

    printf("Bytes in bmp:%d\nBMP Dimensions in pixels:(%d,%d)\n",
        bmpinfo.bmpheader.header.size, bmpinfo.bmpheader.header.width,
        bmpinfo.bmpheader.header.height);

    printf("Tile dimensions(%d,%d)\nBMP Dimensions in units of tiles(%d,%d)\n",
        tileX, tileY, bmpinfo.bmpheader.header.width/tileX,
        bmpinfo.bmpheader.header.height/tileY);

    context->data =
            malloc(bmpinfo.bmpheader.header.width * 
            bmpinfo.bmpheader.header.height * sizeof(byte));

    if (!fread(context->data, sizeof(byte),
        bmpinfo.bmpheader.header.width*bmpinfo.bmpheader.header.height, file)) {
        context->mode = LIDE_C_BMP_FILE_READ_HALO_MODE_INACTIVE;
        return context;
    }

    fclose(file);

    context->bmpinfo = bmpinfo;

    return context;
}

bool lide_c_bmp_file_read_halo_enable(
        lide_c_bmp_file_read_halo_context_type *context) {

    bool result = false;

    switch (context->mode) {
        case LIDE_C_BMP_FILE_READ_HALO_MODE_HALO:
  	        result = (lide_c_fifo_population(context->halo_in) >= 1);
            break;
        case LIDE_C_BMP_FILE_READ_HALO_MODE_INFO:
            result = (lide_c_fifo_population(context->bmpinfo_out) <
                    lide_c_fifo_capacity(context->bmpinfo_out));
            break;
        case LIDE_C_BMP_FILE_READ_HALO_MODE_PROCESS:
            result = (lide_c_fifo_population(context->tile_out) <
                    lide_c_fifo_capacity(context->tile_out)) &&
                    (lide_c_fifo_population(context->newrow_out) <
                    lide_c_fifo_capacity(context->newrow_out));
            break;
        case LIDE_C_BMP_FILE_READ_HALO_MODE_INACTIVE:
            result = false;
            break;
        default:
            result = false;
            break;
    }
    return result;
}

void lide_c_bmp_file_read_halo_invoke(
        lide_c_bmp_file_read_halo_context_type *context) {
    int tileX = context->tileX;
    int tileY = context->tileY;
    int halo = context->halo;

    /*dimensions in tiles - rounded down */
    int imgDimX = context->bmpinfo.bmpheader.header.width / tileX;

    /*dimensions in tiles - rounded down */
    int imgDimY = context->bmpinfo.bmpheader.header.height / tileY; 
    
    int tileIndexX = context->tileIndexX;
    int tileIndexY = context->tileIndexY;
    float * newtile;
    int x, y;
    float *temp;

    switch (context->mode) {
        case LIDE_C_BMP_FILE_READ_HALO_MODE_HALO:
            /*read halo info for writer just once*/
            lide_c_fifo_read(context->halo_in, &temp);
	        context->halo = (((int)temp[0])-1)/2;
            context->mode = LIDE_C_BMP_FILE_READ_HALO_MODE_INFO;
            break;
        case LIDE_C_BMP_FILE_READ_HALO_MODE_INFO:
            /*push bmpinfo for writer just once*/
            lide_c_fifo_write(context->bmpinfo_out, &(context->bmpinfo));
            context->mode = LIDE_C_BMP_FILE_READ_HALO_MODE_PROCESS;
            break;
        case LIDE_C_BMP_FILE_READ_HALO_MODE_PROCESS:
            /*form a new tile*/
            newtile = malloc(sizeof(float)*(tileX+halo*2)*(tileY+halo*2));

            /*assume the last index points to the right part of memory*/
            for (y = 0; y < tileY + 2 * halo; y++) {
                for (x = 0; x < tileX + 2 * halo; x++) {
                /*if we are in the halo and on an edge, set halo to 0
                otherwise the value will be set to a value from the buffer*/
                    float val=0;
                    if (!((tileIndexX==0) && (x<halo)) && !((tileIndexY==0) &&
                            (y<halo)) && !((tileIndexX==imgDimX-1) && 
                            (x+1>halo+tileX)) && !((tileIndexY==imgDimY-1) &&
                            (y+1>halo+tileY)))

                        val =
                        context->data[(context->bmpinfo.bmpheader.header.width)*
                        (tileIndexY*tileY+y-halo)+(tileIndexX*tileX+x-halo)];
                            newtile[(tileX+2*halo)*y+x] = val; 
                } 
            }

            /*the tile is now populated, so push it out along with itss size*/
            lide_c_fifo_write(context->tile_out, &newtile); 

            /* were we at the end of a new row of tiles? */
            {
                int newrow = 0;
                if (tileIndexX == imgDimX - 1) {
                    newrow = 1;
                    lide_c_fifo_write(context->newrow_out, &newrow); 
                } else {
                    newrow = 0;
                    lide_c_fifo_write(context->newrow_out, &newrow); 
                }
            }

            context->tileIndexX++;
            if (context->tileIndexX >= imgDimX) {
                context->tileIndexX = 0;
                context->tileIndexY++;
            }

            if (context->tileIndexY >= imgDimY) {
                context->mode = LIDE_C_BMP_FILE_READ_HALO_MODE_INACTIVE;
                return;
            }

            context->mode = LIDE_C_BMP_FILE_READ_HALO_MODE_PROCESS;
            break;
    }
}

void lide_c_bmp_file_read_halo_terminate(
        lide_c_bmp_file_read_halo_context_type *context) {
    fclose(context->file);
    free(context);
}
