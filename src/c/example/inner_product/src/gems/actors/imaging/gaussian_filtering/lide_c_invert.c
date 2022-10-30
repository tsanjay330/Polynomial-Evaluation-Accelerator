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
#include "lide_c_invert.h"
#include "lide_c_util.h"

struct _lide_c_invert_context_type_struct {
#include "lide_c_actor_context_type_common.h"

    /* Actor interface ports. */
    lide_c_fifo_pointer tile_out;
    lide_c_fifo_pointer tile_in;

    /*local state*/
    /*info token about loaded bmp*/
    bmp_file_info bmpinfo; 
 
    /* length of the tile */ 
    int tileX;     

    /*height of the tile */
    int tileY;    
};

lide_c_invert_context_type *lide_c_invert_new(int tileX, int tileY,
        lide_c_fifo_pointer tile_in,  lide_c_fifo_pointer tile_out)  {
  
    lide_c_invert_context_type *context = NULL; 
    context = lide_c_util_malloc(sizeof(lide_c_invert_context_type));
    context->enable= (lide_c_actor_enable_function_type)lide_c_invert_enable;
    context->invoke = (lide_c_actor_invoke_function_type)lide_c_invert_invoke;
    context->mode = LIDE_C_INVERT_MODE_PROCESS;

    /* length of the tile */
    context->tileX = tileX;

    /* height of the tile */
    context->tileY = tileY;

    /* tile input - tokens are pointers to image data */
    context->tile_in = tile_in;

    /* tile output - tokens are pointers to image data */
    context->tile_out = tile_out;

    return context;
}

bool lide_c_invert_enable(lide_c_invert_context_type *context) {
    bool result = false;

    switch (context->mode) {
        case LIDE_C_INVERT_MODE_PROCESS:
            result = (lide_c_fifo_population(context->tile_in) >= 1) &&
                    (lide_c_fifo_population(context->tile_out) <
                    lide_c_fifo_capacity(context->tile_out));
            break;
        default:
            result = false;
            break;
        }
    return result;
}

void lide_c_invert_invoke(lide_c_invert_context_type *context) {
    int tileX = context->tileX;
    int tileY = context->tileY;
    float *newtile, *tile;
    int x, y;

    switch (context->mode) {
        case LIDE_C_INVERT_MODE_PROCESS:
            lide_c_fifo_read(context->tile_in, &tile);

            /*form a new tile*/
            newtile = lide_c_util_malloc(sizeof(float) * (tileX) * (tileY));

            /*loop through the indices of the filter */
            for (x = 0; x < tileX; x++) {
                for (y = 0; y < tileY; y++) {
                    /*255 is the max value of the 8b format*/
                    newtile[(tileX) * y + x] = 255 - tile[(tileX) * y + x];
                }
            }

            /*the tile is now populated*/
            lide_c_fifo_write(context->tile_out, &newtile); 

            context->mode = LIDE_C_INVERT_MODE_PROCESS;
            break;
    }
}

void lide_c_invert_terminate(lide_c_invert_context_type *context) {
    free(context);
}
