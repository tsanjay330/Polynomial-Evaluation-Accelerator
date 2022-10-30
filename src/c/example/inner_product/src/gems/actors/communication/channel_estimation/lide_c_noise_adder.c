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
#include "lide_c_noise_adder.h"

/*******************************************************************************NOISE ADDER STRUCTURE DEFINITION
*******************************************************************************/
struct _lide_c_noise_adder_context_struct {
#include "lide_c_actor_context_type_common.h"

    /* Persistent local variables for temporary work */
    int matrix_dimension;
    double noise_variance;

    /* input ports */
    lide_c_fifo_pointer ffp_input_A;

    /* output ports */
    lide_c_fifo_pointer ffp_output_B;
};

/*******************************************************************************
IMPLEMENTATIONS OF INTERFACE FUNCTIONS.
*******************************************************************************/

lide_c_noise_adder_context_type *lide_c_noise_adder_new(
        lide_c_fifo_pointer ffp_input_A, int matrix_dimension, 
        double noise_variance, lide_c_fifo_pointer ffp_output_B) {

    lide_c_noise_adder_context_type *context = NULL;

    context = lide_c_util_malloc(sizeof(lide_c_noise_adder_context_type));
    context->mode = LIDE_C_NOISE_ADDER_MODE_PROCESS;
    context->enable =
            (lide_c_actor_enable_function_type)lide_c_noise_adder_enable;
    context->invoke =
            (lide_c_actor_invoke_function_type)lide_c_noise_adder_invoke;

	context->matrix_dimension = matrix_dimension;
	context->noise_variance = noise_variance;

    context->ffp_input_A = ffp_input_A;

    context->ffp_output_B = ffp_output_B;

    return context;
}

bool lide_c_noise_adder_enable(lide_c_noise_adder_context_type *context) {
    bool result = false;

    switch (context->mode) {
    case LIDE_C_NOISE_ADDER_MODE_PROCESS:
        result = (lide_c_fifo_population(context->ffp_input_A)
                < lide_c_fifo_capacity(context->ffp_input_A));
        result = result && (lide_c_fifo_population(context->ffp_input_A)  >= 
                (context->matrix_dimension) * (context->matrix_dimension));
        break;
    default:
        result = false;
        break;
    }
    return result;
}

void lide_c_noise_adder_invoke(lide_c_noise_adder_context_type *context) {
	int row = 0;
	int column = 0;
	double value = 0;

    switch (context->mode) {
    case LIDE_C_NOISE_ADDER_MODE_PROCESS:
		for (row = 0; row < context->matrix_dimension; row++) {
			for (column = 0; column < context->matrix_dimension; column++) {
				/* read input matrix from fifo */
				lide_c_fifo_read(context->ffp_input_A, &value);

				 /* data processing */
				value += (row == column) * (context->noise_variance);

				/* write the entry to fifo */
				lide_c_fifo_write(context->ffp_output_B, &value);
			}
		}       

        context->mode = LIDE_C_NOISE_ADDER_MODE_PROCESS;
        break;
    default:
        context->mode = LIDE_C_NOISE_ADDER_MODE_ERROR;
        break;
    }
    return;
}

void lide_c_noise_adder_terminate(lide_c_noise_adder_context_type *context) {
    free(context);
}
