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
#include "lide_c_matrix_inversion.h"

/*******************************************************************************
MATRIX INVERSION STRUCTURE DEFINITION
*******************************************************************************/
struct _lide_c_matrix_inversion_context_struct {
#include "lide_c_actor_context_type_common.h"

    /* local variables */
    int matrix_dimension;

    /* input ports */
    lide_c_fifo_pointer ffp_input_A;

    /* output ports */
    lide_c_fifo_pointer ffp_output_B;
};

/*******************************************************************************
IMPLEMENTATIONS OF INTERFACE FUNCTIONS.
*******************************************************************************/

lide_c_matrix_inversion_context_type *lide_c_matrix_inversion_new(
        lide_c_fifo_pointer ffp_input_A, int matrix_dimension, 
        lide_c_fifo_pointer ffp_output_B) {

    lide_c_matrix_inversion_context_type *context = NULL;

    context = lide_c_util_malloc(sizeof(lide_c_matrix_inversion_context_type));
    context->mode = LIDE_C_MATRIX_INVERSION_MODE_PROCESS;
    context->enable =
            (lide_c_actor_enable_function_type)lide_c_matrix_inversion_enable;
    context->invoke =
            (lide_c_actor_invoke_function_type)lide_c_matrix_inversion_invoke;

	context->matrix_dimension = matrix_dimension;

    context->ffp_input_A = ffp_input_A;

    context->ffp_output_B = ffp_output_B;

    return context;
}

bool lide_c_matrix_inversion_enable(lide_c_matrix_inversion_context_type *
        context) {
    bool result = false;

    switch (context->mode) {
    case LIDE_C_MATRIX_INVERSION_MODE_PROCESS:
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

void lide_c_matrix_inversion_invoke(lide_c_matrix_inversion_context_type *
        context) {
	int length = context->matrix_dimension;
    int size = length * length;
    double *A = lide_c_util_malloc(sizeof(double) * size);
    double *L = lide_c_util_malloc(sizeof(double) * size);
    double *U = lide_c_util_malloc(sizeof(double) * size);
    double *A_inverse = lide_c_util_malloc(sizeof(double) * size);
    double *L_inverse = lide_c_util_malloc(sizeof(double) * size);
    double *U_inverse = lide_c_util_malloc(sizeof(double) * size);
	int row = 0;
	int col = 0;
	int k = 0;

	/* initialization of matrices */
	for (row =0; row < length; row++){
			for (col =0; col < length; col++){
                value(A, length, row, col) = 0;
                value(L, length, row, col) = 0;
                value(U, length, row, col) = 0;
                value(A_inverse, length, row, col) = 0;
                value(L_inverse, length, row, col) = 0;
                value(U_inverse, length, row, col) = 0;
		}
	}

    switch (context->mode) {
    case LIDE_C_MATRIX_INVERSION_MODE_PROCESS:
		/* read input matrix from fifo */
		for (row =0; row < length; row++){
			for (col =0; col < length; col++){
				lide_c_fifo_read(context->ffp_input_A, A + col + row * length);
			}
		}

		/* perform lu decomposition, i.e. A = L*U */
		for (row = 0; row < length; row++) {
            value(L, length, row, row) = 1;
		}
		for (col = 0; col < length; col++) {
			for (row = 0; row <= col; row++) {
                value(U, length, row, col) = value(A, length, row, col);
				for (k = 0; k < row; k++) {
                    value(U, length, row, col) -= 
                            value(L, length, row, k) * value(U, length, k, col);
				}
			}
			for (row = col + 1; row < length; row++) {
                value(L, length, row, col) = value(A, length, row, col);
				for (k = 0; k < col; k++) {
                    value(L, length, row, col) -=
                            value(L, length, row, k) * value(U, length, k, col);
				}
                value(L, length, row, col) /= value(U, length, col, col);
			}       
		}

		/* compute inverse of L and U */		
		 for (row = 0; row < length; row++) {
             value(L_inverse, length, row, row) = 
                     1 / value(L, length, row, row);
             value(U_inverse, length, row, row) =
                     1 / value(U, length, row, row);
		 }
	     
		 for (col = 0; col < length; col++) {
			 for (row = col+1; row < length; row++) {
                 value(L_inverse, length, row, col) = 0;
				 for (k = 0; k < row; k++) {
                     value(L_inverse, length, row, col) -=
                            value(L, length, row, k) * 
                            value(L_inverse, length, k, col);
				 }
                 value(L_inverse, length, row, col) *= 
                         value(L_inverse, length, row, row);
			 }         
		  }
	     
		  for (col = 0; col< length; col++) {
			 for (row = col-1; row >= 0; row--) {
                 value(U_inverse, length, row, col) = 0;
				 for (k = col; k > row; k--) {
                     value(U_inverse, length, row, col) -=
                            value(U, length, row, k) *
                            value(U_inverse, length, k, col);
				 }
                 value(U_inverse, length, row, col) *=
                         value(U_inverse, length, row, row);
			 }         
		  }

		  /* compute inverse of A, i.e. B = inv(U)*inv(L) */
		  for (row = 0; row< length; row++) {
			 for (col = 0; col< length; col++) {
				 for (k = 0; k < length; k++) {
                     value(A_inverse, length, row, col) +=
                            value(U_inverse, length, row, k) *
                            value(L_inverse, length, k, col);
				 }
				 /* write the entry to fifo */
				 lide_c_fifo_write(context->ffp_output_B, 
                         A_inverse + row * length + col);
			 }         
		  }        
        free(A);
        free(L);
        free(U);
        free(A_inverse);
        free(L_inverse);
        free(U_inverse);

        context->mode = LIDE_C_MATRIX_INVERSION_MODE_PROCESS;
        break;
    default:
        context->mode = LIDE_C_MATRIX_INVERSION_MODE_ERROR;
        break;
    }
    return;
}

void lide_c_matrix_inversion_terminate(lide_c_matrix_inversion_context_type *
        context) {
    free(context);
}
