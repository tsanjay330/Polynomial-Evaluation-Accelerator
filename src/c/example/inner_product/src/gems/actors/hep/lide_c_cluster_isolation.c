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

#include "lide_c_cluster_isolation.h"
#include "lide_c_util.h"

/*******************************************************************************
INNER PRODUCT STRUCTURE DEFINITION
*******************************************************************************/

struct _lide_c_cluster_isolation_context_struct {
#include "lide_c_actor_context_type_common.h"
    /* input ports */
    lide_c_fifo_pointer ffp_in_grid;

    /* output port */
    lide_c_fifo_pointer ffp_out_tau_isolation;
    lide_c_fifo_pointer ffp_out_electron_isolation;
};

/*******************************************************************************
IMPLEMENTATIONS OF INTERFACE FUNCTIONS.
*******************************************************************************/

lide_c_cluster_isolation_context_type *lide_c_cluster_isolation_new(
        lide_c_fifo_pointer ffp_in_grid, 
        lide_c_fifo_pointer ffp_out_tau_isolation, 
        lide_c_fifo_pointer ffp_out_electron_isolation) { 

    lide_c_cluster_isolation_context_type *context = NULL;

    context = lide_c_util_malloc(sizeof(lide_c_cluster_isolation_context_type));
    context->mode = LIDE_C_CLUSTER_ISOLATION_MODE_PROCESS;
    context->enable =
            (lide_c_actor_enable_function_type)lide_c_cluster_isolation_enable;
    context->invoke = 
            (lide_c_actor_invoke_function_type)lide_c_cluster_isolation_invoke;
    context->ffp_in_grid = ffp_in_grid;
    context->ffp_out_tau_isolation = ffp_out_tau_isolation;    
    context->ffp_out_electron_isolation = ffp_out_electron_isolation;
    return context;
}

bool lide_c_cluster_isolation_enable(
        lide_c_cluster_isolation_context_type *context) {
    bool result = false;
    switch (context->mode) {    
    case LIDE_C_CLUSTER_ISOLATION_MODE_PROCESS:
        result = (lide_c_fifo_population(context->ffp_in_grid) 
               < lide_c_fifo_capacity(context->ffp_in_grid));
        result = result && 
               (lide_c_fifo_population(context->ffp_in_grid)  >= GRID_SIZE);      
        break;    
    default:
            result = false;
            break;
    }
    return result;
}

void lide_c_cluster_isolation_invoke(
        lide_c_cluster_isolation_context_type *context) {
    int row = 0;
    int col = 0;
    int i = 0;
    int j = 0;
    int grid[GRID_LENGTH][GRID_HEIGHT];
    int E = 0;
    int electron_threshold = 2;
    int tau_threshold = 4;
    int electron_count = 0;
    int tau_count = 0;
    int eCoeffA = 2;
    int eCoeffB = 5;
    int tCoeffA = 1;
    int tCoeffB = 20;
    int eClusters = 0;
    int tClusters = 0;
    bool eDecision = false;
    bool tDecision = false;
    int compressed_energy = 0;
    
    switch (context->mode) {    
    case LIDE_C_CLUSTER_ISOLATION_MODE_PROCESS:
         /* read data from fifo */
        for (row = 0; row < GRID_LENGTH; row++) {           
            for (col = 0; col < GRID_HEIGHT; col++) {
                lide_c_fifo_read(context->ffp_in_grid, &grid[row][col]);
            }
        }
        
        for (i = 0; i < GRID_LENGTH; i++) {           
            for (j = 0; j < ((GRID_HEIGHT - 8) + 1); j++) {
                E = 0;
                electron_count = 0;
                tau_count = 0;
                grid[3][3] = E;

                for (row = 0; row < GRID_LENGTH; row++) {           
                    for (col = 0; col < GRID_HEIGHT; col++) {
                        if (grid[row][col] > electron_threshold){
                            electron_count += 1;
                        } else if (grid[row][col] > tau_threshold) {
                            tau_count += 1;
                        }
                    }
                }

                eClusters = electron_count;
                tClusters = tau_count;
                eDecision = false;
                tDecision = false;

                compressed_energy = E >> 4;
                eDecision = (eClusters <= COUNT_THRESHOLD(eCoeffA, eCoeffB, 
                        compressed_energy)) | (E >=160);
                tDecision = (eClusters <= COUNT_THRESHOLD(tCoeffA, tCoeffB, 
                        compressed_energy)) | (E >=160);                
            }
            
            /* write data to fifo */       
            lide_c_fifo_write(context->ffp_out_electron_isolation, &eDecision);
            lide_c_fifo_write(context->ffp_out_tau_isolation, &tDecision);
        }
        
        context->mode = LIDE_C_CLUSTER_ISOLATION_MODE_PROCESS;
        break; 
    default:
        context->mode = LIDE_C_CLUSTER_ISOLATION_MODE_ERROR;
        break;
    } 
    return;
}

void lide_c_cluster_isolation_terminate(
        lide_c_cluster_isolation_context_type *context) {
    free(context);
}
