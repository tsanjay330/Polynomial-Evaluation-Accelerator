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
#include "lide_c_response_computation.h"

/*******************************************************************************
RESPONSE COMPUTATION STRUCTURE DEFINITION
*******************************************************************************/
struct _lide_c_response_computation_context_struct {
#include "lide_c_actor_context_type_common.h"

    /* Persistent local variables for temporary work */
    int length;
    double *B;

    /* input ports */
    lide_c_fifo_pointer ffp_input_A;

    /* output ports */
    lide_c_fifo_pointer ffp_output_c;
};

/*******************************************************************************
IMPLEMENTATIONS OF INTERFACE FUNCTIONS.
*******************************************************************************/

lide_c_response_computation_context_type *lide_c_response_computation_new(
        lide_c_fifo_pointer ffp_input_A, double *B, int length, 
        lide_c_fifo_pointer ffp_output_c) {

    lide_c_response_computation_context_type *context = NULL;

    context = lide_c_util_malloc(sizeof(
            lide_c_response_computation_context_type));
    context->mode = LIDE_C_RESPONSE_COMPUTATION_MODE_PROCESS;
    context->enable =
            (lide_c_actor_enable_function_type)
            lide_c_response_computation_enable;
    context->invoke =
            (lide_c_actor_invoke_function_type)
            lide_c_response_computation_invoke;

	context->length = length;
    context->B = B;

    context->ffp_input_A = ffp_input_A;

    context->ffp_output_c = ffp_output_c;

    return context;
}

bool lide_c_response_computation_enable(
        lide_c_response_computation_context_type *context) {
    bool result = false;

    switch (context->mode) {
    case LIDE_C_RESPONSE_COMPUTATION_MODE_PROCESS:
        result = (lide_c_fifo_population(context->ffp_input_A)
                < lide_c_fifo_capacity(context->ffp_input_A));
        result = result && (lide_c_fifo_population(context->ffp_input_A)  
                >= (context->length));
        break;
    default:
        result = false;
        break;
    }
    return result;
}

void lide_c_response_computation_invoke(
        lide_c_response_computation_context_type *context) {
	int i = 0;
	double a_value = 0;
	double b_value = 0;
	double value = 0;

    switch (context->mode) {
    case LIDE_C_RESPONSE_COMPUTATION_MODE_PROCESS:
		for (i = 0; i < context->length; i++){
			/* read data from fifo */
			lide_c_fifo_read(context->ffp_input_A, &a_value);
			b_value = *((context->B) + i);

			/* data processing */
			value += a_value * b_value;
		}        

        /* write data to fifo */
        lide_c_fifo_write(context->ffp_output_c, &value);

        context->mode = LIDE_C_RESPONSE_COMPUTATION_MODE_PROCESS;
        break;
    default:
        context->mode = LIDE_C_RESPONSE_COMPUTATION_MODE_ERROR;
        break;
    }
    return;
}

void lide_c_response_computation_terminate(
        lide_c_response_computation_context_type *context) {
    free(context);
}
