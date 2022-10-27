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

#include "lide_c_fifo.h"
#include "lide_c_inner_product.h"
#include "lide_c_util.h"

/*******************************************************************************
INNER PRODUCT STRUCTURE DEFINITION
*******************************************************************************/

struct _lide_c_inner_product_context_struct { 
#include "lide_c_actor_context_type_common.h"

    /* Vector length variable. */
    int length;  

    /* Input ports. */
    lide_c_fifo_pointer m;
    lide_c_fifo_pointer x;
    lide_c_fifo_pointer y;

    /* Output port. */
    lide_c_fifo_pointer out;
};

/*******************************************************************************
IMPLEMENTATIONS OF INTERFACE FUNCTIONS.
*******************************************************************************/

lide_c_inner_product_context_type *lide_c_inner_product_new(
        lide_c_fifo_pointer m, lide_c_fifo_pointer x, lide_c_fifo_pointer y,
        lide_c_fifo_pointer out) { 

    lide_c_inner_product_context_type *context = NULL;

    context = lide_c_util_malloc(sizeof(lide_c_inner_product_context_type));
    context->mode = LIDE_C_INNER_PRODUCT_MODE_STORE_LENGTH;
    context->enable =
            (lide_c_actor_enable_function_type)lide_c_inner_product_enable;
    context->invoke = 
            (lide_c_actor_invoke_function_type)lide_c_inner_product_invoke;
    context->length = 0;
    context->m = m;
    context->x = x;
    context->y = y;
    context->out = out;
    return context;
}

bool lide_c_inner_product_enable(
        lide_c_inner_product_context_type *context) {
    bool result = false;

    switch (context->mode) {
    case LIDE_C_INNER_PRODUCT_MODE_STORE_LENGTH:
        result = lide_c_fifo_population(context->m) >= 1;
        break;
    case LIDE_C_INNER_PRODUCT_MODE_PROCESS:
        result = (lide_c_fifo_population(context->x) >= context->length) &&
                (lide_c_fifo_population(context->y) >= context->length) &&
                ((lide_c_fifo_population(context->out) <
                lide_c_fifo_capacity(context->out)));
        break;
    default:
            result = false;
            break;
    }
    return result;
}

void lide_c_inner_product_invoke(lide_c_inner_product_context_type *context) {
    int i = 0;
    int sum = 0;
    int x_value = 0;
    int y_value = 0;

    switch (context->mode) {
    case LIDE_C_INNER_PRODUCT_MODE_STORE_LENGTH:
        lide_c_fifo_read(context->m, &(context->length));
        if (context->length <= 0) {
            context->mode = LIDE_C_INNER_PRODUCT_MODE_STORE_LENGTH;
            return;
        }
        context->mode = LIDE_C_INNER_PRODUCT_MODE_PROCESS;
        break;

    case LIDE_C_INNER_PRODUCT_MODE_PROCESS:
        for (i = 0; i < context->length; i++) {
            lide_c_fifo_read(context->x, &(x_value));
            lide_c_fifo_read(context->y, &(y_value));
            sum += (x_value * y_value);
        }
        lide_c_fifo_write(context->out, &sum);
        context->mode = LIDE_C_INNER_PRODUCT_MODE_STORE_LENGTH;
        break; 
    default:
        context->mode = LIDE_C_INNER_PRODUCT_MODE_STORE_LENGTH;
        break;
    } 
    return;
}

void lide_c_inner_product_terminate(
        lide_c_inner_product_context_type *context) {
    free(context);
}
