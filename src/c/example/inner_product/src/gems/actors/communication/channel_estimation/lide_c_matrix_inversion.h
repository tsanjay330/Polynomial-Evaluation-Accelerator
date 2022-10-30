#ifndef _lide_c_matrix_inversion_h
#define _lide_c_matrix_inversion_h

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
#include "lide_c_actor.h"
#include "lide_c_fifo.h"

/*******************************************************************************
This actor computes the matrix inversion of the input matrix A by
          A = L*U (LU decomposition)
          B = inv(A) = inv(U)*inv(L)
at the PROCESS mode.
*******************************************************************************/

/* define Macro */
#define value(pointer_of_2D_array, length, row, col) * (pointer_of_2D_array + row * length + col)

/* Actor modes */
#define LIDE_C_MATRIX_INVERSION_MODE_ERROR          0
#define LIDE_C_MATRIX_INVERSION_MODE_PROCESS        1

/*******************************************************************************
TYPE DEFINITIONS
*******************************************************************************/

/* Structure and pointer types associated with inner product objects. */
struct _lide_c_matrix_inversion_context_struct;
typedef struct _lide_c_matrix_inversion_context_struct
        lide_c_matrix_inversion_context_type;

/*******************************************************************************
INTERFACE FUNCTIONS
*******************************************************************************/

/*****************************************************************************
Construct function of the lide_c_matrix_inversion actor. Create actor objects 
with:
ffp_input_A: input square matrix with dimension matrix_dimension
ffp_output_B: output inversion matrix
*****************************************************************************/
lide_c_matrix_inversion_context_type *lide_c_matrix_inversion_new(
        lide_c_fifo_pointer ffp_input_A, int matrix_dimension, 
        lide_c_fifo_pointer ffp_output_B);

/*****************************************************************************
Enable function of the lide_c_matrix_inversion actor.
*****************************************************************************/
bool lide_c_matrix_inversion_enable(lide_c_matrix_inversion_context_type
        *context);

/*****************************************************************************
Invoke function of the lide_c_matrix_inversion actor.
*****************************************************************************/
void lide_c_matrix_inversion_invoke(lide_c_matrix_inversion_context_type *
        context);

/*****************************************************************************
Terminate function of the lide_c_matrix_inversion actor.
*****************************************************************************/
void lide_c_matrix_inversion_terminate(lide_c_matrix_inversion_context_type *
        context);

#endif
