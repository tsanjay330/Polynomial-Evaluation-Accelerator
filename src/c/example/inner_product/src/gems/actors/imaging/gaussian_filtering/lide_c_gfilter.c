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
#include "lide_c_gfilter.h"
#include "lide_c_util.h"

struct _lide_c_gfilter_context_type_struct {
#include "lide_c_actor_context_type_common.h"
    /* Actor interface ports. */
    lide_c_fifo_pointer coef_in;
    lide_c_fifo_pointer tile_in;
    lide_c_fifo_pointer tile_out;

    /*local state*/
    bmp_file_info bmpinfo; /*info token about loaded bmp*/

    float *filter; /* pointer to the filter coefficients */
    int filterX;
    int filterY;

    int tileX;     /*length of the tile */
    int tileY;     /*height of the tile */
    int halo;      /*halo padding around tile dimensions*/
};

lide_c_gfilter_context_type *lide_c_gfilter_new(int tileX, int tileY,
        lide_c_fifo_pointer coef_in, lide_c_fifo_pointer tile_in,
        lide_c_fifo_pointer tile_out ) {

    lide_c_gfilter_context_type *context = NULL;
    context = lide_c_util_malloc(sizeof(lide_c_gfilter_context_type));

    context->enable = (lide_c_actor_enable_function_type)lide_c_gfilter_enable;
    context->invoke = (lide_c_actor_invoke_function_type)lide_c_gfilter_invoke;
    context->mode = LIDE_C_GFILTER_MODE_STC;

    /* pointer to filter coefficients */
    context->filter = NULL;

    context->filterX = 0;

    context->filterY = 0;

    /*length of the tile */
    context->tileX = tileX;

    /*height of the tile */
    context->tileY = tileY;

    /*halo padding around tile dimensions - to be set by coefs*/
    context->halo = 0;

    /* coef input - tokens are pointers to coef data*/
    context->coef_in = coef_in;

    /* tile input - tokens are pointers to image data*/
    context->tile_in = tile_in;

    /* tile output - tokens are pointers to image data*/
    context->tile_out = tile_out;

  return context;
}

bool lide_c_gfilter_enable(lide_c_gfilter_context_type *context) {
    bool result = false;

    switch (context->mode) {
        case LIDE_C_GFILTER_MODE_STC:
            result = lide_c_fifo_population(context->coef_in) >= 1;
            break;
        case LIDE_C_GFILTER_MODE_PROCESS:
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

void lide_c_gfilter_invoke(lide_c_gfilter_context_type *context) {
    int tileX = context->tileX;
    int tileY = context->tileY;
    int halo = context->halo;
    float *newtile, *tile;
    float *fcv;
    int x, y;
    float sum_coefs;

    switch (context->mode) {
        case LIDE_C_GFILTER_MODE_STC:
            lide_c_fifo_read(context->coef_in, &fcv);
            /* first element of fcv stores the size of the filter. */
            context->filterY = fcv[0];
            context->filterX = fcv[1];
	        context->halo = (fcv[0]-1)/2;
	        printf("found halo of %d\n", context->halo);
            context->filter = lide_c_util_malloc(sizeof(float) * 
                    context->filterY * context->filterX);

            for (x = 0; x < context->filterY * context->filterX; x++) {
                context->filter[x] = fcv[x + 2];
            }

            sum_coefs = 0;
            for (x = 0; x < context->filterY* context->filterX; x++) {
                sum_coefs += context->filter[x];
            }

            for (x = 0; x < context->filterY * context->filterX; x++) {
                context->filter[x] /= sum_coefs;
            }
                   
            context->mode = LIDE_C_GFILTER_MODE_PROCESS;
            break;

        case LIDE_C_GFILTER_MODE_PROCESS:
            lide_c_fifo_read(context->tile_in, &tile);

            /*form a new tile*/
            newtile = malloc(sizeof(float)*(tileX)*(tileY));

            /*loop through the pixels in the tile*/
            for (x = 0; x < tileX; x++) {
                for (y =  0; y < tileY; y++) {
                    int yf, xf;
                    newtile[(tileX) * y + x] = 0;

                    /*loop through the coefs of the filter*/
                    for (yf = 0; yf < context->filterY; yf++) {
                        for (xf = 0; xf < context->filterX; xf++) {
                            newtile[(tileX) * y + x] +=
                                    context->filter[yf * context->filterX + xf]
                                    * tile[(tileX + 2 * halo) * (y + yf) + 
                                    (x + xf)];

                        } 
                    }
                }
            }

            /*the tile is now populated, so push it out along with itss size*/
            lide_c_fifo_write(context->tile_out, &newtile); 

            context->mode = LIDE_C_GFILTER_MODE_PROCESS;
            break;
    }
}

void lide_c_gfilter_terminate(lide_c_gfilter_context_type *context) {
    free(context); 
}
