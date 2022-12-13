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

#include "lide_c_block_add.h"
#include "lide_c_util.h"

struct _lide_c_block_add_context_struct {
#include "lide_c_actor_context_type_common.h"

    /* Block length (static parameter). */
    int block_length;

    /* Work space for storing operands and results */
    int *value1;
    int *value2;
    int *sum;

    /* Actor interface ports. */
    lide_c_fifo_pointer in1;
    lide_c_fifo_pointer in2;
    lide_c_fifo_pointer out;
};

lide_c_block_add_context_type *lide_c_block_add_new(lide_c_fifo_pointer in1,
        lide_c_fifo_pointer in2, lide_c_fifo_pointer out, int block_length) {

    lide_c_block_add_context_type *context = NULL;

    if (block_length <= 0) {
        fprintf(stderr, "lide_c_block_add_new: error invalid block length\n");
        exit(1);
    }

    context = lide_c_util_malloc(sizeof(lide_c_block_add_context_type));
    context->enable = 
            (lide_c_actor_enable_function_type)lide_c_block_add_enable;
    context->invoke = 
            (lide_c_actor_invoke_function_type)lide_c_block_add_invoke;
    context->block_length = block_length;
    context->value1 = lide_c_util_malloc(sizeof(int) * block_length);
    context->value2 = lide_c_util_malloc(sizeof(int) * block_length);
    context->sum = lide_c_util_malloc(sizeof(int) * block_length);
    context->mode = LIDE_C_BLOCK_ADD_MODE_READ_BLOCK1;
    context->in1 = in1;
    context->in2 = in2;
    context->out = out;
    return context;
}

bool lide_c_block_add_enable(lide_c_block_add_context_type *context) {
    bool result = false;

    switch (context->mode) {
    case LIDE_C_BLOCK_ADD_MODE_READ_BLOCK1:
        result = lide_c_fifo_population(context->in1) >= context->block_length;
        break;
    case LIDE_C_BLOCK_ADD_MODE_READ_BLOCK2:
        result = lide_c_fifo_population(context->in2) >= context->block_length;
        break;
    case LIDE_C_BLOCK_ADD_MODE_SUM:
        result = lide_c_fifo_population(context->out) <
                lide_c_fifo_capacity(context->out);
        break;
    case LIDE_C_BLOCK_ADD_MODE_ERROR:
        result = false;
        break;
    default:
        result = false;
        break;
    }
    return result;
}

void lide_c_block_add_invoke(lide_c_block_add_context_type *context) {
    int i = 0;
    switch (context->mode) {
    case LIDE_C_BLOCK_ADD_MODE_READ_BLOCK1:
        lide_c_fifo_read_block(context->in1, context->value1,
                context->block_length);
        context->mode = LIDE_C_BLOCK_ADD_MODE_READ_BLOCK2;
        break;
    case LIDE_C_BLOCK_ADD_MODE_READ_BLOCK2:
        lide_c_fifo_read_block(context->in2, context->value2,
                context->block_length);
        context->mode = LIDE_C_BLOCK_ADD_MODE_SUM;
        break;
    case LIDE_C_BLOCK_ADD_MODE_SUM:
        for (i = 0; i < context->block_length; i++) {
            context->sum[i] = (context->value1)[i] + (context->value2)[i];
        }
        lide_c_fifo_write_block(context->out, context->sum, 
                context->block_length);
        context->mode = LIDE_C_BLOCK_ADD_MODE_READ_BLOCK1;
        break;
    case LIDE_C_BLOCK_ADD_MODE_ERROR:
        /* Do no nothing and remain in this mode */
        context->mode = LIDE_C_BLOCK_ADD_MODE_ERROR;
        break;
    default:
        context->mode = LIDE_C_BLOCK_ADD_MODE_ERROR;
        break;
    }
}

int lide_c_block_add_get_block_length(lide_c_block_add_context_type 
        *context) {
    return context->block_length;
}

void lide_c_block_add_set_block_length(lide_c_block_add_context_type 
        *context, int block_length) {
    context->block_length = block_length;
}

void lide_c_block_add_terminate(lide_c_block_add_context_type *context) {
    free(context->value1);
    free(context->value2);
    free(context->sum);
    free(context);
}
