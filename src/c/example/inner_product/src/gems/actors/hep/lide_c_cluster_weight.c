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

#include "lide_c_util.h"
#include "lide_c_cluster_weight.h"

/*******************************************************************************
CLUSTER WEIGHT STRUCTURE DEFINITION
*******************************************************************************/
struct _lide_c_cluster_weight_context_struct {
#include "lide_c_actor_context_type_common.h"

    /* Persistent local variables for temporary work */

    /* input ports */
    lide_c_fifo_pointer ffp_input_centralTower0;
    lide_c_fifo_pointer ffp_input_centralTower1;
    lide_c_fifo_pointer ffp_input_centralTower2;
    lide_c_fifo_pointer ffp_input_centralTower3;
    lide_c_fifo_pointer ffp_input_etotal;

    /* output ports */
    lide_c_fifo_pointer ffp_output_finalWeight;
};

/*******************************************************************************
IMPLEMENTATIONS OF INTERFACE FUNCTIONS.
*******************************************************************************/

lide_c_cluster_weight_context_type *lide_c_cluster_weight_new(
        lide_c_fifo_pointer ffp_input_centralTower0, 
        lide_c_fifo_pointer ffp_input_centralTower1, 
        lide_c_fifo_pointer ffp_input_centralTower2, 
        lide_c_fifo_pointer ffp_input_centralTower3, 
        lide_c_fifo_pointer ffp_input_etotal, 
        lide_c_fifo_pointer ffp_output_finalWeight) {

    lide_c_cluster_weight_context_type *context = NULL;

    context = lide_c_util_malloc(sizeof(lide_c_cluster_weight_context_type));
    context->mode = LIDE_C_CLUSTER_WEIGHT_MODE_PROCESS;
    context->enable =
            (lide_c_actor_enable_function_type)lide_c_cluster_weight_enable;
    context->invoke =
            (lide_c_actor_invoke_function_type)lide_c_cluster_weight_invoke;

    context->ffp_input_centralTower0 = ffp_input_centralTower0;
    context->ffp_input_centralTower1 = ffp_input_centralTower1;
    context->ffp_input_centralTower2 = ffp_input_centralTower2;
    context->ffp_input_centralTower3 = ffp_input_centralTower3;
    context->ffp_input_etotal = ffp_input_etotal;

    context->ffp_output_finalWeight = ffp_output_finalWeight;

    return context;
}

bool lide_c_cluster_weight_enable(
        lide_c_cluster_weight_context_type *context) {
    bool result = false;

    switch (context->mode) {
    case LIDE_C_CLUSTER_WEIGHT_MODE_PROCESS:
        result = (lide_c_fifo_population(context->ffp_input_centralTower0)
                < lide_c_fifo_capacity(context->ffp_input_centralTower0));
        result = result && 
                (lide_c_fifo_population(context->ffp_input_centralTower1)
                < lide_c_fifo_capacity(context->ffp_input_centralTower1));
        result = result && 
                (lide_c_fifo_population(context->ffp_input_centralTower2)
                < lide_c_fifo_capacity(context->ffp_input_centralTower2));
        result = result && 
                (lide_c_fifo_population(context->ffp_input_centralTower3)
                < lide_c_fifo_capacity(context->ffp_input_centralTower3));
        result = result && 
                (lide_c_fifo_population(context->ffp_input_etotal)
                < lide_c_fifo_capacity(context->ffp_input_etotal));
        result = result && 
                (lide_c_fifo_population(context->ffp_input_centralTower0)  
                >= 1);
        result = result && 
                (lide_c_fifo_population(context->ffp_input_centralTower1)  
                >= 1);
        result = result && 
                (lide_c_fifo_population(context->ffp_input_centralTower2)  
                >= 1);
        result = result && 
                (lide_c_fifo_population(context->ffp_input_centralTower3)  
                >= 1);
        result = result && 
                (lide_c_fifo_population(context->ffp_input_etotal)  
                >= 1);
        break;
    default:
        result = false;
        break;
    }
    return result;
}

void lide_c_cluster_weight_invoke(lide_c_cluster_weight_context_type *context) {
    int centralTower0 = 0;
    int centralTower1 = 0;
    int centralTower2 = 0;
    int centralTower3 = 0;
    int etotal = 0;
    int etaWeight = 0;
    int phiWeight = 0;
    int finalWeight = 0;
    int deltaEta = 0;
    int deltaPhi = 0;
    int threshold = 0;

    switch (context->mode) {
    case LIDE_C_CLUSTER_WEIGHT_MODE_PROCESS:
        /* read data from fifo */
        lide_c_fifo_read(context->ffp_input_centralTower0, &centralTower0);
        lide_c_fifo_read(context->ffp_input_centralTower1, &centralTower1);
        lide_c_fifo_read(context->ffp_input_centralTower2, &centralTower2);
        lide_c_fifo_read(context->ffp_input_centralTower3, &centralTower3);
        lide_c_fifo_read(context->ffp_input_etotal, &etotal);

        /* data processing */
        if (etotal <= threshold) {
			centralTower0 = 0;
			centralTower1 = 0;
			centralTower2 = 0;
			centralTower3 = 0;
			etotal = 0;
			etaWeight = 0;
			phiWeight = 0;
		} else {
			deltaEta = centralTower1 + centralTower3 
                    - centralTower0 - centralTower2;
			deltaPhi = centralTower2 + centralTower3 
                    - centralTower0 - centralTower1;
        	
			if (deltaEta + etotal < 0.5 * etotal) {
				etaWeight = 0;
			} else if (deltaEta + etotal < etotal) {
				etaWeight = 1;
			} else if (deltaEta + etotal < 1.5 * etotal) {
				etaWeight = 2;
			} else if (deltaEta + etotal < 2 * etotal) {
				etaWeight = 3;
			}				

			if (deltaPhi + etotal < 0.5 * etotal) {
				phiWeight = 0;
			} else if (deltaPhi + etotal < etotal) {
				phiWeight = 1;
			} else if (deltaPhi + etotal < 1.5 * etotal) {
				phiWeight = 2;
			} else if (deltaPhi + etotal < 2 * etotal) {
				phiWeight = 3;
			}	

			finalWeight = (phiWeight << 2) | etaWeight;
		}

        /* write data to fifo */
        lide_c_fifo_write(context->ffp_output_finalWeight, &finalWeight);

        context->mode = LIDE_C_CLUSTER_WEIGHT_MODE_PROCESS;
        break;
    default:
        context->mode = LIDE_C_CLUSTER_WEIGHT_MODE_ERROR;
        break;
    }
    return;
}

void lide_c_cluster_weight_terminate(
        lide_c_cluster_weight_context_type *context) {
    free(context);
}
