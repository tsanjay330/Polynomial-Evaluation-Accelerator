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
#include <math.h>

#include "lide_c_util.h"
#include "lide_c_auto_correlation_computation.h"

/*******************************************************************************
STRUCTURE DEFINITION OF ACTOR CONTEXT
*******************************************************************************/

struct _lide_c_auto_correlation_computation_context_struct {
#include "lide_c_actor_context_type_common.h"

    /* local variables */
    int pilot_count;
    int *pilot_index;
    double channel_window;	/* channel power-delay window */
    double doppler_window;  /* Doppler power-delay window */

    /* input ports */

    /* output ports */
    lide_c_fifo_pointer ffp_output_auto_correlation_matrix;

};

/*******************************************************************************
IMPLEMENTATIONS OF INTERFACE FUNCTIONS.
*******************************************************************************/

lide_c_auto_correlation_computation_context_type *
        lide_c_auto_correlation_computation_new(int pilot_count, 
        int *pilot_index, double channel_window, double doppler_window, 
        lide_c_fifo_pointer ffp_output_auto_correlation_matrix) {

    lide_c_auto_correlation_computation_context_type *context = NULL;

    context = lide_c_util_malloc(sizeof(
            lide_c_auto_correlation_computation_context_type));
    context->mode = LIDE_C_AUTO_CORRELATION_COMPUTATION_MODE_PROCESS;
    context->enable =
            (lide_c_actor_enable_function_type)
            lide_c_auto_correlation_computation_enable;
    context->invoke =
            (lide_c_actor_invoke_function_type)
            lide_c_auto_correlation_computation_invoke;

    context->pilot_count = pilot_count;
    context->pilot_index = pilot_index;
    context->channel_window = channel_window;
    context->doppler_window = doppler_window;
    
    context->ffp_output_auto_correlation_matrix = 
            ffp_output_auto_correlation_matrix;

    return context;
}

bool lide_c_auto_correlation_computation_enable(
        lide_c_auto_correlation_computation_context_type *context) {
    bool result = false;

    switch (context->mode) {
    case LIDE_C_AUTO_CORRELATION_COMPUTATION_MODE_PROCESS:
        result = (lide_c_fifo_population(
                context->ffp_output_auto_correlation_matrix)
                < lide_c_fifo_capacity(
                context->ffp_output_auto_correlation_matrix));        
        break;
    case LIDE_C_AUTO_CORRELATION_COMPUTATION_MODE_SLEEP:
        result = false;
        break;
    default:
        result = false;
        break;
    }
    return result;
}

void lide_c_auto_correlation_computation_invoke(
        lide_c_auto_correlation_computation_context_type *context) {
    int i = 0;
    int j = 0;
    double f_value = 0;
    double t_value = 0;
    double value = 0;
    double angle = 0;
    int pilot_time_index = 0;
    int pilot_freq_index = 0;
    int current_time_index = 0;
    int current_freq_index = 0;

    switch (context->mode) {
    case LIDE_C_AUTO_CORRELATION_COMPUTATION_MODE_PROCESS:        
        /* data processing */
        for (i = 0; i < (context->pilot_count); i++){
            pilot_time_index = *((context->pilot_index) + 2 * i);
            pilot_freq_index = *((context->pilot_index) + 2 * i + 1);           
            for (j = 0; j < (context->pilot_count); j++){
                /* calculate the correlation between the ith and jth pilots */
                if (j == i){
                    value = 1;
                } else {
                    current_time_index = *((context->pilot_index) + 2 * j);
                    current_freq_index = *((context->pilot_index) + 2 * j + 1);

                    angle = (context->doppler_window) 
                            * (current_time_index - pilot_time_index) * PI;
                    if (angle == 0) {
                        t_value = 1;
                    } else {
                        t_value = sin(angle) / angle;
                    }
                    angle = (context->channel_window) 
                            * (current_freq_index - pilot_freq_index) * PI;
                    if (angle == 0) {
                        f_value = 1;
                    } else {
                        f_value = sin(angle) / angle;
                    }
                    value = t_value * f_value;
                }            
                
                 /* write data to fifo */
                lide_c_fifo_write(context->ffp_output_auto_correlation_matrix, 
                        &value);
            }            
        }
        
        /* Need only compute the matrix once */
        context->mode = LIDE_C_AUTO_CORRELATION_COMPUTATION_MODE_SLEEP;
        break;
    case LIDE_C_AUTO_CORRELATION_COMPUTATION_MODE_SLEEP:
        context->mode = LIDE_C_AUTO_CORRELATION_COMPUTATION_MODE_SLEEP;
        break;
    default:
        context->mode = LIDE_C_AUTO_CORRELATION_COMPUTATION_MODE_ERROR;
        break;
    }
    return;
}

void lide_c_auto_correlation_computation_terminate(
        lide_c_auto_correlation_computation_context_type *context) {
    free(context);
}
