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
#include "lide_c_inner_product_graph.h"
#include "lide_c_basic.h"
#include "lide_c_actor.h"
#include "lide_c_file_source.h"
#include "lide_c_file_sink.h"
#include "lide_c_inner_product.h"

#define NAME_LENGTH 20


struct _lide_c_inner_product_graph_context_struct {
#include "lide_c_graph_context_type_common.h"
    char *m_file; 
    char *x_file; 
    char *y_file; 
    char *out_file;
    //char *descriptors[ACTOR_COUNT];

};

lide_c_inner_product_graph_context_type *lide_c_inner_product_graph_new(
        char *m_file, char *x_file, char *y_file, char *out_file){
    int token_size, i;
    
    lide_c_inner_product_graph_context_type *context = NULL;

    context = lide_c_util_malloc(sizeof(lide_c_inner_product_graph_context_type));
    context->actor_count = ACTOR_COUNT;
    context->actors = (lide_c_actor_context_type **)lide_c_util_malloc(
                    context->actor_count * sizeof(lide_c_actor_context_type *));
    context->fifo_count = FIFO_COUNT;
    context->fifos = (lide_c_fifo_pointer *)lide_c_util_malloc(context->fifo_count * 
                        sizeof(lide_c_fifo_pointer));
    context->descriptors = (char **)lide_c_util_malloc(
                        context->actor_count * sizeof(char *));
    for(i = 0; i < context->actor_count; i++){
        context->descriptors[i] = (char *)lide_c_util_malloc(
                        context->actor_count * sizeof(char));
    }
    strcpy(context->descriptors[ACTOR_MSOURCE],"msource");
    strcpy(context->descriptors[ACTOR_XSOURCE],"xsource");
    strcpy(context->descriptors[ACTOR_YSOURCE],"ysource");
    strcpy(context->descriptors[ACTOR_PRODUCT],"inner_product");
    strcpy(context->descriptors[ACTOR_SINK],"sink");
 
    context->m_file = m_file;
    context->x_file = x_file;
    context->y_file = y_file;
    context->out_file = out_file;

    token_size = sizeof(int);
    for(i = 0; i<context->fifo_count; i++){
        context->fifos[i] = lide_c_fifo_new(BUFFER_CAPACITY, token_size);
    }
    /* Create and connect the actors. */
    i = 0;
    context->actors[ACTOR_MSOURCE] = (lide_c_actor_context_type
            *)(lide_c_file_source_new(context->m_file, 
            context->fifos[FIFO_MSRC_PROD]));

    context->actors[ACTOR_XSOURCE] = (lide_c_actor_context_type
            *)(lide_c_file_source_new(context->x_file, 
            context->fifos[FIFO_XSRC_PROD]));

    context->actors[ACTOR_YSOURCE] = (lide_c_actor_context_type
            *)(lide_c_file_source_new(context->y_file, 
            context->fifos[FIFO_YSRC_PROD]));

    context->actors[ACTOR_PRODUCT] = (lide_c_actor_context_type
            *)(lide_c_inner_product_new(context->fifos[FIFO_MSRC_PROD], 
            context->fifos[FIFO_XSRC_PROD], context->fifos[FIFO_YSRC_PROD], 
            context->fifos[FIFO_PROD_SNK]));

    context->actors[ACTOR_SINK] = (lide_c_actor_context_type *)
            (lide_c_file_sink_new(context->out_file, 
            context->fifos[FIFO_PROD_SNK]));

    
    
    return context;
}

void lide_c_inner_product_graph_terminate(
        lide_c_inner_product_graph_context_type *context){
    int i;
    /* Terminate FIFO*/
    for(i = 0; i<context->fifo_count; i++){
        lide_c_fifo_free(context->fifos[i]);
    }
    
    /* Terminate Actors*/
    lide_c_file_source_terminate((lide_c_file_source_context_type * )
                                context->actors[ACTOR_MSOURCE]);
    lide_c_file_source_terminate((lide_c_file_source_context_type * )
                                context->actors[ACTOR_XSOURCE]);
    lide_c_file_source_terminate((lide_c_file_source_context_type * )
                                context->actors[ACTOR_YSOURCE]);
    lide_c_inner_product_terminate((lide_c_inner_product_context_type *) 
                                (context->actors[ACTOR_PRODUCT]));
    lide_c_file_sink_terminate((lide_c_file_sink_context_type * )
                                context->actors[ACTOR_SINK]);
    free(context->fifos);
    free(context->actors);
    free(context->descriptors);
    //free(context);
    
    return;

}



