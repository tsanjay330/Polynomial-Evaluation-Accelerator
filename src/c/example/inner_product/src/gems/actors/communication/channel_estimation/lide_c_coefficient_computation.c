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
#include "lide_c_coefficient_computation.h"

/*******************************************************************************
INNER PRODUCT STRUCTURE DEFINITION
*******************************************************************************/

struct _lide_c_coefficient_computation_context_struct {
#include "lide_c_actor_context_type_common.h"

    /* local variables */
    int a_row_count;
    int b_row_count;
    int a_column_count;
    int b_column_count;
    int a_entry_count;
    int b_entry_count;
    double *a_entries;

    /* input ports */
    lide_c_fifo_pointer ffp_input_A;
    lide_c_fifo_pointer ffp_input_B;

    /* output ports */
    lide_c_fifo_pointer ffp_output_C;
};

/*******************************************************************************
IMPLEMENTATIONS OF INTERFACE FUNCTIONS.
*******************************************************************************/

lide_c_coefficient_computation_context_type *lide_c_coefficient_computation_new(
        lide_c_fifo_pointer ffp_input_A, lide_c_fifo_pointer ffp_input_B, 
        lide_c_fifo_pointer ffp_output_C, int m, int n, int p) {

    lide_c_coefficient_computation_context_type *context = NULL;

    context = lide_c_util_malloc(
            sizeof(lide_c_coefficient_computation_context_type));
    context->mode = LIDE_C_COEFFICIENT_COMPUTATION_MODE_LOAD;
    context->enable =
            (lide_c_actor_enable_function_type)
            lide_c_coefficient_computation_enable;
    context->invoke =
            (lide_c_actor_invoke_function_type)
            lide_c_coefficient_computation_invoke;

    context->a_row_count = m;
    context->b_row_count = n;
    context->a_column_count = n;
    context->b_column_count = p;
    context->a_entry_count = m * n;    
    context->b_entry_count = n * p;
    context->a_entries = lide_c_util_malloc(
                sizeof(double) * (context->a_entry_count));

    context->ffp_input_A = ffp_input_A;
    context->ffp_input_B = ffp_input_B;

    context->ffp_output_C = ffp_output_C;

    return context;
}

bool lide_c_coefficient_computation_enable(
        lide_c_coefficient_computation_context_type *context) {
    bool result = false;

    switch (context->mode) {
    case LIDE_C_COEFFICIENT_COMPUTATION_MODE_LOAD:
        result = (lide_c_fifo_population(context->ffp_input_A) 
                < lide_c_fifo_capacity(context->ffp_input_A));
        result = result && (lide_c_fifo_population(context->ffp_input_A) 
                >= (context->a_entry_count));        
        break;
    case LIDE_C_COEFFICIENT_COMPUTATION_MODE_PROCESS:
        result = (lide_c_fifo_population(context->ffp_input_B) 
                < lide_c_fifo_capacity(context->ffp_input_B));
        result = result && (lide_c_fifo_population(context->ffp_input_B) 
                >= (context->b_entry_count));
        break;
    default:
        result = false;
        break;
    }
    return result;
}

void lide_c_coefficient_computation_invoke(
        lide_c_coefficient_computation_context_type *context) {
    int i = 0;
    int row = 0;
    int col = 0;
    double entry = 0;
    double *b_entries = NULL;

    switch (context->mode) {
    case LIDE_C_COEFFICIENT_COMPUTATION_MODE_LOAD:
        /* read entries of A */
        for (i = 0; i < (context->a_entry_count); i++) {
            lide_c_fifo_read(context->ffp_input_A, (context->a_entries + i));
        }
        context->mode = LIDE_C_COEFFICIENT_COMPUTATION_MODE_PROCESS;
        break;
    case LIDE_C_COEFFICIENT_COMPUTATION_MODE_PROCESS:       
        b_entries = lide_c_util_malloc(
                sizeof(double) * (context->b_entry_count));
        
        /* read entries of B */
        for (i = 0; i < (context->b_entry_count); i++) {
            lide_c_fifo_read(context->ffp_input_B, (b_entries + i));
        }

        /* calculate and write entries of C = A*B  */
        for (row = 0; row < (context->a_row_count); row++) {
            for (col = 0; col < (context->b_column_count); col++) {
                /* calculate the entry (row, col)  of A*B */
                entry = 0;
                for (i = 0; i < (context->a_column_count); i++){
                    entry += (*(context->a_entries + i + row * 
                            (context->a_column_count))) * 
                            (*(b_entries + col + i * 
                            (context->b_column_count)));
                }
                /* write the entry (row, col) to fifo */
                lide_c_fifo_write(context->ffp_output_C, &entry);
            }            
        }
        free(b_entries);

        context->mode = LIDE_C_COEFFICIENT_COMPUTATION_MODE_PROCESS;
        break;
    default:
        context->mode = LIDE_C_COEFFICIENT_COMPUTATION_MODE_ERROR;
        break;
    }
    return;
}

void lide_c_coefficient_computation_terminate(
        lide_c_coefficient_computation_context_type *context) {
    free(context);
}
