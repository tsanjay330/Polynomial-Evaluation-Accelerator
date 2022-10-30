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

#include "lide_c_cluster_computation.h"
#include "lide_c_util.h"

/*******************************************************************************
CLUSTER COMPUTATION STRUCTURE DEFINITION
*******************************************************************************/
struct _lide_c_cluster_computation_context_struct {
#include "lide_c_actor_context_type_common.h"
    /* Persistent "local variables" for temporary work */
    float EGamma_Thr;

    /* input ports */
    lide_c_fifo_pointer *ffp_in_ecal;
    lide_c_fifo_pointer *ffp_in_hcal;
    lide_c_fifo_pointer *ffp_in_fg;

    /* output port */
    lide_c_fifo_pointer *ffp_out_tower;
    lide_c_fifo_pointer ffp_out_egamma;
    lide_c_fifo_pointer ffp_out_fg;
};

/*******************************************************************************
IMPLEMENTATIONS OF INTERFACE FUNCTIONS.
*******************************************************************************/

lide_c_cluster_computation_context_type *lide_c_cluster_computation_new(
        lide_c_fifo_pointer *ffp_in_ecal, lide_c_fifo_pointer *ffp_in_hcal, 
        lide_c_fifo_pointer *ffp_in_fg, lide_c_fifo_pointer *ffp_out_tower, 
        lide_c_fifo_pointer ffp_out_egamma, lide_c_fifo_pointer ffp_out_fg) { 

    lide_c_cluster_computation_context_type *context = NULL;

    context = lide_c_util_malloc(sizeof(
            lide_c_cluster_computation_context_type));
    context->mode = LIDE_C_CLUSTER_COMPUTATION_MODE_PROCESS;
    context->enable = (lide_c_actor_enable_function_type)
            lide_c_cluster_computation_enable;
    context->invoke = (lide_c_actor_invoke_function_type)
            lide_c_cluster_computation_invoke;
        
	context->EGamma_Thr = 0.9;
	context->ffp_in_ecal = ffp_in_ecal;
    context->ffp_in_hcal = ffp_in_hcal;    
	context->ffp_in_fg = ffp_in_fg;
    context->ffp_out_tower = ffp_out_tower;
    context->ffp_out_egamma = ffp_out_egamma;    
	context->ffp_out_fg = ffp_out_fg;
    return context;
}

bool lide_c_cluster_computation_enable(
        lide_c_cluster_computation_context_type *context) {
    bool result = false;
	int i = 0;
    switch (context->mode) {    
    case LIDE_C_CLUSTER_COMPUTATION_MODE_PROCESS:
		result = true;
		for (i=0;i<ELEMENT_COUNT;i++){
			result = result && 
                    (lide_c_fifo_population(context->ffp_in_ecal[i]) < 
                    lide_c_fifo_capacity(context->ffp_in_ecal[i]));
			result = result && 
                    (lide_c_fifo_population(context->ffp_in_hcal[i]) < 
                    lide_c_fifo_capacity(context->ffp_in_hcal[i]));
			result = result && 
                    (lide_c_fifo_population(context->ffp_in_fg[i]) < 
                    lide_c_fifo_capacity(context->ffp_in_fg[i]));
			result = result && 
                    (lide_c_fifo_population(context->ffp_in_ecal[i])  > 0);
			result = result && 
                    (lide_c_fifo_population(context->ffp_in_hcal[i])  > 0);
			result = result && 
                    (lide_c_fifo_population(context->ffp_in_fg[i])  > 0);
		}		
        break;    
    default:
            result = false;
            break;
    }
    return result;
}

void lide_c_cluster_computation_invoke(
        lide_c_cluster_computation_context_type *context) {
    int i = 0;    
	int ecal[ELEMENT_COUNT], hcal[ELEMENT_COUNT], fg[ELEMENT_COUNT];
	int tower[ELEMENT_COUNT];
	int EAdder = 0, HAdder = 0;
	int FG = 0;
	int EGamma = 0;
	
    switch (context->mode) {    
    case LIDE_C_CLUSTER_COMPUTATION_MODE_PROCESS:
        for (i = 0; i < ELEMENT_COUNT; i++) {
			/* read data from fifo */
            lide_c_fifo_read(context->ffp_in_ecal[i], &ecal[i]);
			lide_c_fifo_read(context->ffp_in_hcal[i], &hcal[i]);
			lide_c_fifo_read(context->ffp_in_fg[i], &fg[i]);
			/* data computation */
			EAdder += ecal[i];
			HAdder += hcal[i];
			FG |= fg[i];
			tower[i] = ecal[i] + hcal[i];			
		}
		/* data computation */
		EGamma = ((double)EAdder / (double)(EAdder + HAdder) >= 
               context-> EGamma_Thr);		
		/* write data to fifo */
		for (i = 0; i < ELEMENT_COUNT; i++) {
			lide_c_fifo_write(context->ffp_out_tower[i], &tower[i]);			
		}
		lide_c_fifo_write(context->ffp_out_egamma, &EGamma);
		lide_c_fifo_write(context->ffp_out_fg, &FG);
		
        context->mode = LIDE_C_CLUSTER_COMPUTATION_MODE_PROCESS;
        break; 
    default:
        context->mode = LIDE_C_CLUSTER_COMPUTATION_MODE_ERROR;
        break;
    } 
    return;
}

void lide_c_cluster_computation_terminate(
        lide_c_cluster_computation_context_type *context) {
    free(context);
}
