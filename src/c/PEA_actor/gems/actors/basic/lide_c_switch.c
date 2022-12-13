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

#include "lide_c_switch.h"
#include "lide_c_util.h"
#include "lide_c_util.h"

struct _lide_c_switch_context_struct {
#include "lide_c_actor_context_type_common.h"
    int control_value;
    int input_value;

    /* Actor interface ports. */
    lide_c_fifo_pointer in;
    lide_c_fifo_pointer control;
    lide_c_fifo_pointer out0;
    lide_c_fifo_pointer out1;
};


lide_c_switch_context_type *lide_c_switch_new(lide_c_fifo_pointer in,
        lide_c_fifo_pointer control, lide_c_fifo_pointer out0, 
        lide_c_fifo_pointer out1) {

    lide_c_switch_context_type *context = NULL;

    context = lide_c_util_malloc(sizeof(lide_c_switch_context_type));
    context->mode = LIDE_C_SWITCH_MODE_CONTROL;
    context->enable = (lide_c_actor_enable_function_type)lide_c_switch_enable;
    context->invoke = (lide_c_actor_invoke_function_type)lide_c_switch_invoke;
    context->in = in;
    context->control = control;
    context->out0 = out0;
    context->out1 = out1;
    return context;
}

bool lide_c_switch_enable(lide_c_switch_context_type *context) {
    bool result = false;

    switch (context->mode) {
    case LIDE_C_SWITCH_MODE_CONTROL:
        result = (lide_c_fifo_population(context->control) >= 1);
        break;
    case LIDE_C_SWITCH_MODE_OUT0:
        result = (lide_c_fifo_population(context->in) >= 1) &&
                ((lide_c_fifo_population(context->out0) <
                lide_c_fifo_capacity(context->out0)));
        break;
    case LIDE_C_SWITCH_MODE_OUT1:
        result = (lide_c_fifo_population(context->in) >= 1) &&
                ((lide_c_fifo_population(context->out1) <
                lide_c_fifo_capacity(context->out1)));
        break;
    default:
        result = false;
        break;
    }
    return result;
}

void lide_c_switch_invoke(lide_c_switch_context_type *context) {

    switch (context->mode) {
    case LIDE_C_SWITCH_MODE_CONTROL:
        lide_c_fifo_read(context->control, &(context->control_value));
        if (context->control_value == 0) {
            context->mode = LIDE_C_SWITCH_MODE_OUT0;
        } else {
            context->mode = LIDE_C_SWITCH_MODE_OUT1;
        }
        break;

    case LIDE_C_SWITCH_MODE_OUT0:
        lide_c_fifo_read(context->in, &(context->input_value));
        lide_c_fifo_write(context->out0, &(context->input_value));
        context->mode = LIDE_C_SWITCH_MODE_CONTROL;
        break;

    case LIDE_C_SWITCH_MODE_OUT1:
        lide_c_fifo_read(context->in, &(context->input_value));
        lide_c_fifo_write(context->out1, &(context->input_value));
        context->mode = LIDE_C_SWITCH_MODE_CONTROL;
        break;
    default:
        context->mode = LIDE_C_SWITCH_MODE_CONTROL;
        break;        
    }
}

void lide_c_switch_terminate(lide_c_switch_context_type *context) {
    free(context);
}
