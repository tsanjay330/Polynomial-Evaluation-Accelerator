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

#include "lide_c_overlap_filter.h"
#include "lide_c_util.h"

/*******************************************************************************
CLUSTER COMPUTATION STRUCTURE DEFINITION
*******************************************************************************/
struct _lide_c_overlap_filter_context_struct {
#include "lide_c_actor_context_type_common.h"
    /* input ports */
    lide_c_fifo_pointer *ffp_in_cct;

    /* output port */
    lide_c_fifo_pointer *ffp_out_central_tower;
    lide_c_fifo_pointer ffp_out_etotal;
    lide_c_fifo_pointer ffp_out_ecentral;
};

/*******************************************************************************
IMPLEMENTATIONS OF INTERFACE FUNCTIONS.
*******************************************************************************/

lide_c_overlap_filter_context_type *lide_c_overlap_filter_new(
        lide_c_fifo_pointer *ffp_in_cct, lide_c_fifo_pointer ffp_out_etotal, 
        lide_c_fifo_pointer ffp_out_ecentral,
        lide_c_fifo_pointer *ffp_out_central_tower) { 

    lide_c_overlap_filter_context_type *context = NULL;

    context = lide_c_util_malloc(sizeof(lide_c_overlap_filter_context_type));
    context->mode = LIDE_C_OVERLAP_FILTER_MODE_PROCESS;
    context->enable =
            (lide_c_actor_enable_function_type)lide_c_overlap_filter_enable;
    context->invoke = 
            (lide_c_actor_invoke_function_type)lide_c_overlap_filter_invoke;
        
    context->ffp_in_cct = ffp_in_cct;
    context->ffp_out_etotal = ffp_out_etotal;    
    context->ffp_out_ecentral = ffp_out_ecentral;
    context->ffp_out_central_tower = ffp_out_central_tower;
    return context;
}

bool lide_c_overlap_filter_enable(
        lide_c_overlap_filter_context_type *context) {
    bool result = false;
    int i = 0;
    switch (context->mode) {    
    case LIDE_C_OVERLAP_FILTER_MODE_PROCESS:
        result = true;
        for (i = 0 ; i < ELEMENT_COUNT; i++) {
            result = result && 
                    (lide_c_fifo_population(context->ffp_in_cct[i]) 
                    < lide_c_fifo_capacity(context->ffp_in_cct[i]));
            result = result && 
                    (lide_c_fifo_population(context->ffp_in_cct[i])  >= 9);
        }        
        break;    
    default:
            result = false;
            break;
    }
    return result;
}

void lide_c_overlap_filter_invoke(lide_c_overlap_filter_context_type *context) {
    int i = 0;
    int j = 0;
    int tower[ELEMENT_COUNT];
    int central_tower[ELEMENT_COUNT];
    int neighbor_tower[ELEMENT_COUNT];
    int central_etotal;
    int neighbor_etotal;
    /* prune_towers: twers to be pruned after comparing with neighboring cluster in the order E, SE, S, SSW, W, NW, N, NE
    */
    int prune_towers[] = {10, 8, 12, 4, 5, 1, 3, 2};
    int toPrune = 0;
    int central = 0;
    int output = 0;
    long etotal = 0;
    
    switch (context->mode) {    
    case LIDE_C_OVERLAP_FILTER_MODE_PROCESS:
        for (i = 0; i < 9; i++) {
            central_etotal = 0;
            neighbor_etotal = 0;
            toPrune = 0;
            central = 0;
            output = 0;
            etotal = 0;
            /* read data from fifo */
            for (j = 0; j < ELEMENT_COUNT; j++) {
                lide_c_fifo_read(context->ffp_in_cct[j], &tower[j]);
            }

            /* data computation */
            if (i == 4) {
                for (j = 0; j < ELEMENT_COUNT; j++) {
                    central_tower[j] = tower[j];
                }
            } else {
                    neighbor_tower[j] = tower[j];
            }

            for (j = 0; j < ELEMENT_COUNT; j++) {
                    central_etotal += central_tower[j];
                    neighbor_etotal += neighbor_tower[j];
            }

            if (((i / 4 == 0) && (central_etotal <= neighbor_etotal)) || 
                    ((i / 4 == 1) && (central_etotal < neighbor_etotal))) {
                toPrune |= prune_towers[i];
                for (j = 0; j < ELEMENT_COUNT; j++) {
                    central_tower[j] *= ((toPrune & (1 << j)) != (1 << j));
                }
            } 
        }
        central = (toPrune == 0);
        for (i = 0; i < ELEMENT_COUNT; i++) {
            etotal += central_tower[i];
        }
        /* write data to fifo */
        for (i = 0; i < ELEMENT_COUNT; i++) {
            lide_c_fifo_write(context->ffp_out_central_tower[i], 
                    &central_tower[i]);
        }
        lide_c_fifo_write(context->ffp_out_etotal, &etotal);
        lide_c_fifo_write(context->ffp_out_ecentral, &central);
        
        context->mode = LIDE_C_OVERLAP_FILTER_MODE_PROCESS;
        break; 
    default:
        context->mode = LIDE_C_OVERLAP_FILTER_MODE_ERROR;
        break;
    } 
    return;
}

void lide_c_overlap_filter_terminate(
        lide_c_overlap_filter_context_type *context) {
    free(context);
}
