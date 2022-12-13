#ifndef _block_add_h
#define _block_add_h
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

/*******************************************************************************
A block addition actor.
*******************************************************************************/
#include "lide_c_actor.h"
#include "lide_c_fifo.h"

/* Actor modes */
#define LIDE_C_BLOCK_ADD_MODE_ERROR          0
#define LIDE_C_BLOCK_ADD_MODE_READ_BLOCK1    1
#define LIDE_C_BLOCK_ADD_MODE_READ_BLOCK2    2 
#define LIDE_C_BLOCK_ADD_MODE_SUM            3 

/*******************************************************************************
TYPE DEFINITIONS
*******************************************************************************/

/* Structure and pointer types associated with block add objects. */
struct _lide_c_block_add_context_struct;
typedef struct _lide_c_block_add_context_struct
        lide_c_block_add_context_type;

/*******************************************************************************
INTERFACE FUNCTIONS
*******************************************************************************/

lide_c_block_add_context_type *lide_c_block_add_new(lide_c_fifo_pointer in1,
        lide_c_fifo_pointer in2,  lide_c_fifo_pointer out, int block_length);

bool lide_c_block_add_enable(lide_c_block_add_context_type *context);

int lide_c_block_add_get_block_length(lide_c_block_add_context_type 
        *context); 

void lide_c_block_add_set_block_length(lide_c_block_add_context_type 
        *context, int block_length); 

void lide_c_block_add_invoke(lide_c_block_add_context_type *context); 

void lide_c_block_add_terminate(lide_c_block_add_context_type *context);

#endif
