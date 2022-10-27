#ifndef _lide_c_response_computation_h
#define _lide_c_response_computation_h

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
This actor reads the input vector A and parameter input B, and computes the 
inner product c = <A,B> at the PROCESS mode.
*******************************************************************************/

/* Actor modes */
#define LIDE_C_RESPONSE_COMPUTATION_MODE_ERROR          0
#define LIDE_C_RESPONSE_COMPUTATION_MODE_PROCESS        1

/*******************************************************************************
TYPE DEFINITIONS
*******************************************************************************/

/* Structure and pointer types associated with actor objects. */
struct _lide_c_response_computation_context_struct;
typedef struct _lide_c_response_computation_context_struct
        lide_c_response_computation_context_type;

/*******************************************************************************
INTERFACE FUNCTIONS
*******************************************************************************/

/*****************************************************************************
Construct function of the lide_c_response_computation actor. Create actor 
objects with
ffp_input_A: the input vector
B: the parameter vector
length: the length of vector A and B
ffp_output_c: the inner product output
*****************************************************************************/
lide_c_response_computation_context_type *lide_c_response_computation_new(
        lide_c_fifo_pointer ffp_input_A, double *B, int length, 
        lide_c_fifo_pointer ffp_output_c);

/*****************************************************************************
Enable function of the lide_c_response_computation actor.
*****************************************************************************/
bool lide_c_response_computation_enable(
        lide_c_response_computation_context_type *context);

/*****************************************************************************
Invoke function of the lide_c_response_computation actor.
*****************************************************************************/
void lide_c_response_computation_invoke(
        lide_c_response_computation_context_type *context);

/*****************************************************************************
Terminate function of the lide_c_response_computation actor.
*****************************************************************************/
void lide_c_response_computation_terminate(
        lide_c_response_computation_context_type *context);

#endif
