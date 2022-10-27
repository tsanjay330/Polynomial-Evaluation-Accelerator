#ifndef _lide_c_coefficient_computation_h
#define _lide_c_coefficient_computation_h

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
This actor loads the mxn input matrix A in the LOAD mode, reads the nxp input 
matrix B and outputs the mxp matrix C=A*B in the PROCESS mode.
*******************************************************************************/

/* Actor modes */	
#define LIDE_C_COEFFICIENT_COMPUTATION_MODE_ERROR          0
#define LIDE_C_COEFFICIENT_COMPUTATION_MODE_LOAD           1
#define LIDE_C_COEFFICIENT_COMPUTATION_MODE_PROCESS        2

/*******************************************************************************
TYPE DEFINITIONS
*******************************************************************************/

/* Structure and pointer types associated with the objects. */
struct _lide_c_coefficient_computation_context_struct;
typedef struct _lide_c_coefficient_computation_context_struct
        lide_c_coefficient_computation_context_type;

/*******************************************************************************
INTERFACE FUNCTIONS
*******************************************************************************/

/*****************************************************************************
Construct function of the lide_c_coefficient_computation actor. Create a new 
actor object with
ffp_input_A: input mxn matrix
ffp_input_B: parameter nxp matrix
ffp_output_C: output mxp matrix
*****************************************************************************/
lide_c_coefficient_computation_context_type *lide_c_coefficient_computation_new(
        lide_c_fifo_pointer ffp_input_A, lide_c_fifo_pointer ffp_input_B, 
        lide_c_fifo_pointer ffp_output_C, int m, int n, int p);

/*****************************************************************************
Enable function of the lide_c_coefficient_computation actor.
*****************************************************************************/
bool lide_c_coefficient_computation_enable(
        lide_c_coefficient_computation_context_type *context);

/*****************************************************************************
Invoke function of the lide_c_coefficient_computation actor.
*****************************************************************************/
void lide_c_coefficient_computation_invoke(
        lide_c_coefficient_computation_context_type *context);

/*****************************************************************************
Terminate function of the lide_c_coefficient_computation actor.
*****************************************************************************/
void lide_c_coefficient_computation_terminate(
        lide_c_coefficient_computation_context_type *context);

#endif
