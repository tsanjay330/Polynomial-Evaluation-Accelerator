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
#include <string.h>

#include "lide_c_t.h"
#include "lide_c_util.h"

struct _lide_c_t_context_type_struct {
#include "lide_c_actor_context_type_common.h"
    FILE *file;  

    /* Actor interface ports. */
    lide_c_fifo_pointer in;
    lide_c_fifo_pointer out1;
    lide_c_fifo_pointer out2;

    /*local state*/
    size_t size; /* size of the token to be copied*/
};

lide_c_t_context_type *lide_c_t_new(size_t size, lide_c_fifo_pointer in,
        lide_c_fifo_pointer out1, lide_c_fifo_pointer out2) {

    lide_c_t_context_type *context = NULL;
    context = lide_c_util_malloc(sizeof(lide_c_t_context_type));

    context->enable = (lide_c_actor_enable_function_type)lide_c_t_enable;
    context->invoke = (lide_c_actor_invoke_function_type)lide_c_t_invoke;
    context->mode = LIDE_C_T_MODE_NORMAL;

    /*size of the token to be copied */
    context->size = size;

    /*connections*/
    context->in = in;
    context->out1 = out1;
    context->out2 = out2;

    return context;
}

bool lide_c_t_enable(lide_c_t_context_type *context) {
    bool result = false;

    switch (context->mode) {
        case LIDE_C_T_MODE_NORMAL:
		    result = ((lide_c_fifo_population(context->in) >= 1)
					  && (lide_c_fifo_population(context->out1) <
						  lide_c_fifo_capacity(context->out1))
					  && (lide_c_fifo_population(context->out2) <
						  lide_c_fifo_capacity(context->out2)));
            break;
        default:
            result = false;
            break;
    }
    return result;
}

void lide_c_t_invoke(lide_c_t_context_type *context) {
  void *temp, *newtoken;

    switch (context->mode) {
        case LIDE_C_T_MODE_NORMAL:
  		    /*read input*/
		    lide_c_fifo_read(context->in, &temp);

            /*form a new token*/
		    newtoken = malloc(sizeof(context->size));
			memcpy(newtoken, temp, context->size);

			/*write out tokens*/
			lide_c_fifo_write(context->out1, &(temp));
            lide_c_fifo_write(context->out2, &(newtoken));

            context->mode = LIDE_C_T_MODE_NORMAL;
            break;
    }
}

void lide_c_t_terminate(lide_c_t_context_type *context) {
    free(context);
}

