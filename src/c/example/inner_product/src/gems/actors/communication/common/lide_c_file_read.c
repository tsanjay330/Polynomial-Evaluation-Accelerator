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

#include "lide_c_file_read.h"
#include "lide_c_util.h"

struct _lide_c_file_read_context_type_struct {
#include "lide_c_actor_context_type_common.h"
    FILE *file; 
    lide_c_fifo_pointer out;
    int data_type;        /* 0 for integer; 1 for float; 2 for double */
    int read_rate;
};

lide_c_file_read_context_type *lide_c_file_read_new(FILE *file,
        lide_c_fifo_pointer out, int data_type, int read_rate) {

    lide_c_file_read_context_type *context = NULL;

    context = lide_c_util_malloc(sizeof(lide_c_file_read_context_type));
    context->enable =
            (lide_c_actor_enable_function_type)lide_c_file_read_enable;
    context->invoke =
            (lide_c_actor_invoke_function_type)lide_c_file_read_invoke;
    context->data_type = data_type;
    context->read_rate = read_rate;
    context->file = file;
    context->mode = LIDE_C_FILE_READ_MODE_WRITE;
    context->out = out;
    return context;
}

bool lide_c_file_read_enable(lide_c_file_read_context_type *context) {
    bool result = false;

    switch (context->mode) {
    case LIDE_C_FILE_READ_MODE_WRITE:
        result = (lide_c_fifo_population(context->out) < 
                lide_c_fifo_capacity(context->out));
        break;
    case LIDE_C_FILE_READ_MODE_INACTIVE:
        result = false;
        break;
    default:
        result = false;
        break;
    }
    return result;
}

void lide_c_file_read_invoke(lide_c_file_read_context_type *context) {
    int int_value = 0;
    float float_value = 0;
    double double_value = 0;
    int a = 0;
    int i = 0;
    switch (context->mode) {
    case LIDE_C_FILE_READ_MODE_WRITE:
        for (i = 0; i < context->read_rate; i++) {
             switch (context->data_type) {
             case 0:
                a = fscanf(context->file, "%d", &int_value);
                break;
             case 1:
                a = fscanf(context->file, "%f", &float_value);
                break;
             case 2:
                a = fscanf(context->file, "%lf", &double_value);
                break;
             default:
                a = fscanf(context->file, "%d", &int_value);
                break;
             }
             if (a != 1) { 
                /* End of input */
                context->mode = LIDE_C_FILE_READ_MODE_INACTIVE;
                return;
             }
             switch (context->data_type) {
             case 0:
                lide_c_fifo_write(context->out, &int_value);
                break;
             case 1:
                lide_c_fifo_write(context->out, &float_value);
                break;
             case 2:
                lide_c_fifo_write(context->out, &double_value);
                break;
             default:
                lide_c_fifo_write(context->out, &int_value);
                break;
             }
        }       
        
        context->mode = LIDE_C_FILE_READ_MODE_WRITE;
        break;
    case LIDE_C_FILE_READ_MODE_INACTIVE:
        break;
    default:
        context->mode = LIDE_C_FILE_READ_MODE_ERROR;
        break;
    }
}

void lide_c_file_read_terminate(lide_c_file_read_context_type *context) {
    fclose(context->file);
    free(context);
}

