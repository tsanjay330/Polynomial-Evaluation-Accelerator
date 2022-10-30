#ifndef _lide_c_inner_product_h
#define _lide_c_inner_product_h

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
This inner product actor consumes vector length (the number of elements in each
vector) and vectors and computes the inner product. There are two modes
associated with this actor. At the STORE_LENGTH mode, this actor consumes
vector length, sets the length variable, and sets the PROCESS mode as the next
mode. At the PROCESS mode, this actor consumes vectors based on the configured
vector length, computes the inner product, and sets the STORE_LENGTH mode as
the next mode.
*******************************************************************************/

/* Actor modes */
#define LIDE_C_INNER_PRODUCT_MODE_STORE_LENGTH   1
#define LIDE_C_INNER_PRODUCT_MODE_PROCESS        2 

/*******************************************************************************
TYPE DEFINITIONS
*******************************************************************************/

/* Structure and pointer types associated with inner product objects. */
struct _lide_c_inner_product_context_struct;
typedef struct _lide_c_inner_product_context_struct
        lide_c_inner_product_context_type;

/*******************************************************************************
INTERFACE FUNCTIONS
*******************************************************************************/

/*****************************************************************************
Construct function of the lide_c_inner_product actor. Create a new
lide_c_inner_product with the specified input FIFO pointer m for the
configuration of vector length, the specified input FIFO pointer x for the
first vector, the specified input FIFO pointer y for the second vector, and the
specified output FIFO pointer.
*****************************************************************************/
lide_c_inner_product_context_type *lide_c_inner_product_new(
        lide_c_fifo_pointer m, lide_c_fifo_pointer x, lide_c_fifo_pointer y,
        lide_c_fifo_pointer out);

/*****************************************************************************
Enable function of the lide_c_inner_product actor.
*****************************************************************************/
bool lide_c_inner_product_enable(lide_c_inner_product_context_type *context);

/*****************************************************************************
Invoke function of the lide_c_inner_product actor.
*****************************************************************************/
void lide_c_inner_product_invoke(lide_c_inner_product_context_type *context);

/*****************************************************************************
Terminate function of the lide_c_inner_product actor.
*****************************************************************************/
void lide_c_inner_product_terminate(lide_c_inner_product_context_type *context);

#endif
