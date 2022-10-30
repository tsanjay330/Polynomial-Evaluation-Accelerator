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
#include "lide_c_cross_correlation_computation.h"

/*******************************************************************************
CROSS CORRELATION COMPUTATION STRUCTURE DEFINITION
*******************************************************************************/
struct _lide_c_cross_correlation_computation_context_struct {
#include "lide_c_actor_context_type_common.h"

    /* local variables */
    int subcarrier_time_index;
    int subcarrier_freq_index;
    int pilot_count;
    int *pilot_index;
    double channel_window; /* normalized channel power-delay window */
    double doppler_window;  /* normalized Doppler power-delay window */

    /* input ports */
    lide_c_fifo_pointer ffp_input_subcarrier_time_index;
    lide_c_fifo_pointer ffp_input_subcarrier_freq_index;

    /* output ports */
    lide_c_fifo_pointer ffp_output_cross_correlation;
};

/*******************************************************************************
IMPLEMENTATIONS OF INTERFACE FUNCTIONS.
*******************************************************************************/

lide_c_cross_correlation_computation_context_type
        *lide_c_cross_correlation_computation_new(
        lide_c_fifo_pointer ffp_input_subcarrier_time_index,
        lide_c_fifo_pointer ffp_input_subcarrier_freq_index,
        lide_c_fifo_pointer ffp_output_cross_correlation, int pilot_count,
        int *pilot_index, double channel_window, double doppler_window) {

    lide_c_cross_correlation_computation_context_type *context = NULL;

    context = lide_c_util_malloc(sizeof(
            lide_c_cross_correlation_computation_context_type));
    context->mode = 
            LIDE_C_CROSS_CORRELATION_COMPUTATION_MODE_STORE_SUBCARRIER_INDEX;
    context->enable =
            (lide_c_actor_enable_function_type)
            lide_c_cross_correlation_computation_enable;
    context->invoke =
            (lide_c_actor_invoke_function_type)
            lide_c_cross_correlation_computation_invoke;

	context->subcarrier_time_index = -1;
	context->subcarrier_freq_index = -1;
	context->pilot_count = pilot_count;
	context->pilot_index = pilot_index;
	context->channel_window = channel_window;
	context->doppler_window = doppler_window;

    context->ffp_input_subcarrier_time_index = ffp_input_subcarrier_time_index;
    context->ffp_input_subcarrier_freq_index = ffp_input_subcarrier_freq_index;

    context->ffp_output_cross_correlation = ffp_output_cross_correlation;

    return context;
}

bool lide_c_cross_correlation_computation_enable(
        lide_c_cross_correlation_computation_context_type *context) {
    bool result = false;

    switch (context->mode) {
    case LIDE_C_CROSS_CORRELATION_COMPUTATION_MODE_STORE_SUBCARRIER_INDEX:
        result = (lide_c_fifo_population(
                context->ffp_input_subcarrier_time_index)
                < lide_c_fifo_capacity(
                context->ffp_input_subcarrier_time_index));
        result = result && (lide_c_fifo_population(
                context->ffp_input_subcarrier_time_index) >= 1);
        result = result && (lide_c_fifo_population(
                context->ffp_input_subcarrier_freq_index)
                < lide_c_fifo_capacity(
                context->ffp_input_subcarrier_freq_index));
        result = result && (lide_c_fifo_population(
                context->ffp_input_subcarrier_freq_index) >= 1);
        break;
    case LIDE_C_CROSS_CORRELATION_COMPUTATION_MODE_PROCESS:
        result = (context->subcarrier_time_index  >= 0);
		result = result && (context->subcarrier_freq_index  >= 0);
        break;
    default:
        result = false;
        break;
    }
    return result;
}

void lide_c_cross_correlation_computation_invoke(
        lide_c_cross_correlation_computation_context_type *context) {
	int i = 0;
	double f_value = 0;
	double t_value = 0;
	double value = 0;
	double angle = 0;
	int pilot_time_index = 0;
	int pilot_freq_index = 0;

    switch (context->mode) {
    case LIDE_C_CROSS_CORRELATION_COMPUTATION_MODE_STORE_SUBCARRIER_INDEX:
        /* read subcarrier indices from fifo */
        lide_c_fifo_read(context->ffp_input_subcarrier_time_index, 
                &(context->subcarrier_time_index));
		lide_c_fifo_read(context->ffp_input_subcarrier_freq_index, 
                &(context->subcarrier_freq_index));

        context->mode = LIDE_C_CROSS_CORRELATION_COMPUTATION_MODE_PROCESS;
        break;
    case LIDE_C_CROSS_CORRELATION_COMPUTATION_MODE_PROCESS:
		for (i = 0; i < (context->pilot_count); i++){
            /* compute the cross-correlation function between the subcarrier and
               the ith pilot with r_ij = sinc(channel_window*(f_i - f_j))
               * sinc(doppler_window*(t_i - t_j))
            */
			pilot_time_index = *((context->pilot_index) + 2 * i);
			pilot_freq_index = *((context->pilot_index) + 2 * i + 1);

			angle = (context->doppler_window) * 
                    ((context->subcarrier_time_index) - pilot_time_index) * PI;
			if (angle == 0) {
				t_value = 1;
			} else {				
				t_value = sin(angle) / angle;
			}

			angle = (context->channel_window) * 
                    ((context->subcarrier_freq_index) - pilot_freq_index) * PI;
			if (angle == 0) {
				f_value = 1;
			} else {
				f_value = sin(angle) / angle;
			}
			value = t_value * f_value;
			
			 /* write the ith correlation to fifo */
			lide_c_fifo_write(context->ffp_output_cross_correlation, &value);
		}
		
		/* reset subcarrier index */
		context->subcarrier_time_index = -1;
		context->subcarrier_freq_index = -1;

        context->mode = 
               LIDE_C_CROSS_CORRELATION_COMPUTATION_MODE_STORE_SUBCARRIER_INDEX;
        break;
    default:
        context->mode = LIDE_C_CROSS_CORRELATION_COMPUTATION_MODE_ERROR;
        break;
    }
    return;
}

void lide_c_cross_correlation_computation_terminate(
        lide_c_cross_correlation_computation_context_type *context) {
    free(context);
}
