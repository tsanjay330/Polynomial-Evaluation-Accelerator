/*******************************************************************************
@ddblock_begin copyright

Copyright (c) 1997-2017
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

#include "lide_c_file_sink_jitter.h"
#include "lide_c_util.h"

struct _lide_c_file_sink_jitter_context_struct {
#include "lide_c_actor_context_type_common.h"
    FILE *file;  

    lide_c_fifo_pointer in;
};


lide_c_file_sink_jitter_context_type *lide_c_file_sink_jitter_new(FILE *file,
        lide_c_fifo_pointer in) {

    lide_c_file_sink_jitter_context_type *context = NULL;

    context = lide_c_util_malloc(sizeof(lide_c_file_sink_jitter_context_type));
    context->invoke = 
            (lide_c_actor_invoke_function_type)lide_c_file_sink_jitter_invoke;
    context->enable = 
            (lide_c_actor_enable_function_type)lide_c_file_sink_jitter_enable;
    context->file = file;
    context->mode = LIDE_C_FILE_SINK_MODE_READ;
    context->in = in;
    return context;
}

bool lide_c_file_sink_jitter_enable(lide_c_file_sink_jitter_context_type *context) {
    bool result = false;

    switch (context->mode) {
    case LIDE_C_FILE_SINK_MODE_READ:
        result = lide_c_fifo_population(context->in) >= 1;
        break;
    default:
        result = false;
        break;
    }
    return result;
}

void lide_c_file_sink_jitter_invoke(lide_c_file_sink_jitter_context_type *context) {
    double value = 0.0;

	int i;
    switch (context->mode) {
    case LIDE_C_FILE_SINK_MODE_READ:
		for(i = 0; i<9; i++){
			lide_c_fifo_read(context->in, &value);
			if (i == 5)
				fprintf(context->file, "%.16f\n", value);
		}

		fprintf(context->file, "\n");
		context->mode = LIDE_C_FILE_SINK_MODE_READ;
        break;
    default:
        context->mode = LIDE_C_FILE_SINK_MODE_READ;
        break;
    }
	
}

void lide_c_file_sink_jitter_terminate(lide_c_file_sink_jitter_context_type *context) {
    fclose(context->file);
    free(context);
}
