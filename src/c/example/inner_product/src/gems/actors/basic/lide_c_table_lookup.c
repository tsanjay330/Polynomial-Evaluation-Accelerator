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

#include "lide_c_table_lookup.h"
#include "lide_c_util.h"

struct _lide_c_table_lookup_context_struct {
#include "lide_c_actor_context_type_common.h"
    int size;
    int *table;
    lide_c_fifo_pointer in;
    lide_c_fifo_pointer out;
};


lide_c_table_lookup_context_type *lide_c_table_lookup_new(FILE *file, int size,
        lide_c_fifo_pointer in, lide_c_fifo_pointer out) {

    lide_c_table_lookup_context_type *context = NULL;
    int data = 0;
    int i = 0;

    if (size <= 0) {
        fprintf(stderr, "lide_c_table_lookup_new error: invalid size");
        exit(1);
    }
    context = lide_c_util_malloc(sizeof(lide_c_table_lookup_context_type));
    context->table = lide_c_util_malloc(size * sizeof(int));
    context->size = size;
    context->enable =
            (lide_c_actor_enable_function_type)lide_c_table_lookup_enable;
    context->invoke =
            (lide_c_actor_invoke_function_type)lide_c_table_lookup_invoke;
 
    for (i = 0; i < size; i++) {
        if (fscanf(file, "%d", &data) != 1) {
            fprintf(stderr, "lide_c_table_lookup_new error: integer expected");
            exit(1);
        }   
        (context->table)[i] = data;
    }
 
    context->mode = LIDE_C_TABLE_LOOKUP_MODE_READ;
    context->in = in;
    context->out = out;

    return context;
}

bool lide_c_table_lookup_enable(lide_c_table_lookup_context_type *context) {
    bool result =false;

    switch (context->mode) {
    case LIDE_C_TABLE_LOOKUP_MODE_READ:
        result = (lide_c_fifo_population(context->in) >= 1) &&
                ((lide_c_fifo_population(context->out) <
                lide_c_fifo_capacity(context->out)));
        break;
    default:
        result =false;
        break;
    }
    return result;
}

void lide_c_table_lookup_invoke(lide_c_table_lookup_context_type *context) {
    int index;
    int result = 0;

    switch (context->mode) {
    case LIDE_C_TABLE_LOOKUP_MODE_READ:
        lide_c_fifo_read(context->in, &index);

        if ((index < 0) || (index >= context->size)){
            context->mode = LIDE_C_TABLE_LOOKUP_MODE_READ;
            return;
        }
        result = (context->table)[index];
        lide_c_fifo_write(context->out, &result);
        context->mode = LIDE_C_TABLE_LOOKUP_MODE_READ;
        break;
    default:
        context->mode= LIDE_C_TABLE_LOOKUP_MODE_ERROR;
        break;
   }
}

void lide_c_table_lookup_terminate(lide_c_table_lookup_context_type *context) {
    free(context->table);
    free(context);
}

