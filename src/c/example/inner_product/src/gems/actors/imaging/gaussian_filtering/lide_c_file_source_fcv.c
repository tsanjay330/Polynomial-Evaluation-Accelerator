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

#include "lide_c_actor.h"
#include "lide_c_file_source_fcv.h"
#include "lide_c_util.h"

struct _lide_c_file_source_fcv_context_type_struct {
#include "lide_c_actor_context_type_common.h"
    FILE *file; 
    lide_c_fifo_pointer out;
};

lide_c_file_source_fcv_context_type *lide_c_file_source_fcv_new(FILE *file,
        lide_c_fifo_pointer out) {
    lide_c_file_source_fcv_context_type *context = NULL;

    context = lide_c_util_malloc(sizeof(lide_c_file_source_fcv_context_type));
    context->enable = 
            (lide_c_actor_enable_function_type)lide_c_file_source_fcv_enable;
    context->invoke = 
            (lide_c_actor_invoke_function_type)lide_c_file_source_fcv_invoke;
    context->file = file;
    context->mode = LIDE_C_FILE_SOURCE_FCV_MODE_PROCESS;
    context->out = out;
    return context;
}

bool lide_c_file_source_fcv_enable(
        lide_c_file_source_fcv_context_type *context) {
    bool result = false;

    switch (context->mode) {
        case LIDE_C_FILE_SOURCE_FCV_MODE_PROCESS:
            result = lide_c_fifo_population(context->out) <
                    lide_c_fifo_capacity(context->out);
            break;
        case LIDE_C_FILE_SOURCE_FCV_MODE_INACTIVE:
            result = false;
            break;
        default:
            result = false;
            break;
        }
    return result;
}

void lide_c_file_source_fcv_invoke(
        lide_c_file_source_fcv_context_type *context) {
    int sizeX = 0;
    int sizeY = 0;
    float value = 0.0;
    float *fcv;
    int x;

    switch (context->mode) {
        case LIDE_C_FILE_SOURCE_FCV_MODE_PROCESS:
            if (fscanf(context->file, "%d", &sizeY) != 1) { 
                /* End of input */
                context->mode = LIDE_C_FILE_SOURCE_FCV_MODE_INACTIVE;
                return;
            }

            if (fscanf(context->file, "%d", &sizeX) != 1) { 
                /* End of input */
                context->mode = LIDE_C_FILE_SOURCE_FCV_MODE_INACTIVE;
                return;
            }

            fcv = lide_c_util_malloc(sizeof(float) * (sizeY * sizeX + 2));
            fcv[0] = sizeY;
            fcv[1] = sizeX;

            for (x = 0; x < sizeY * sizeX; x++) {
                if (fscanf(context->file, "%f", &value) != 1) { 
                    /* End of input */
                    context->mode = LIDE_C_FILE_SOURCE_FCV_MODE_INACTIVE;
                    return;
                }
                fcv[x + 2] = value;
            }
           
            lide_c_fifo_write(context->out, &fcv);

            context->mode = LIDE_C_FILE_SOURCE_FCV_MODE_PROCESS;
            break;
        }
}

void lide_c_file_source_fcv_terminate(
        lide_c_file_source_fcv_context_type *context) {
    fclose(context->file);
    free(context);
}

