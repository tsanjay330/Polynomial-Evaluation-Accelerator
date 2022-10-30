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

#include "lide_c_add.h"
#include "lide_c_util.h"

struct _lide_c_add_context_struct {
#include "lide_c_actor_context_type_common.h"

    /* Actor interface ports. */
    lide_c_fifo_pointer in1;
    lide_c_fifo_pointer in2;
    lide_c_fifo_pointer out;
};

lide_c_add_context_type *lide_c_add_new(lide_c_fifo_pointer in1,
        lide_c_fifo_pointer in2, lide_c_fifo_pointer out) {

    lide_c_add_context_type *context = NULL;

    context = lide_c_util_malloc(sizeof(lide_c_add_context_type));
    context->mode = LIDE_C_ADD_MODE_PROCESS;
    context->enable = (lide_c_actor_enable_function_type)lide_c_add_enable;
    context->invoke = (lide_c_actor_invoke_function_type)lide_c_add_invoke;
    context->in1 = in1;
    context->in2 = in2;
    context->out = out;
    return context;
}

bool lide_c_add_enable(lide_c_add_context_type *context) {
    bool result = false;

    switch (context->mode) {
    case LIDE_C_ADD_MODE_PROCESS:
        result = (lide_c_fifo_population(context->in1) >= 1) &&
                (lide_c_fifo_population(context->in2) >= 1) &&
                ((lide_c_fifo_population(context->out) < 
                lide_c_fifo_capacity(context->out)));
        break;
    default:
        result = false;
        break;
    }
    return result;
}

/*******************************************************************************
The invoke method assumes that a corresponding call to enable has
returned TRUE so that the required data is available.

*******************************************************************************/
void lide_c_add_invoke(lide_c_add_context_type *context) {
    int value1 = 0;
    int value2 = 0;
    int sum = 0;

    switch (context->mode) {
    case LIDE_C_ADD_MODE_PROCESS:
        lide_c_fifo_read(context->in1, &value1);
        lide_c_fifo_read(context->in2, &value2);
        sum = value1 + value2;
        lide_c_fifo_write(context->out, &sum);
        context->mode = LIDE_C_ADD_MODE_PROCESS;
        break;
    default:
        context->mode = LIDE_C_ADD_MODE_PROCESS;
        break;
    }
}

void lide_c_add_terminate(lide_c_add_context_type *context) {
    free(context);
}
