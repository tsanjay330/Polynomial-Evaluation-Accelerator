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
#include "lide_c_file_source.h"
#include "lide_c_file_sink.h"
#include "lide_c_pea.h"
#include "lide_c_util.h"

#define BUFFER_CAPACITY 1024

/* An enumeration of the actors in this application. */
#define ACTOR_SOURCE_COMMAND 0
#define ACTOR_SOURCE_DATA 1
#define ACTOR_PEA 2
#define ACTOR_SINK_RESULT 3
#define ACTOR_SINK_STATUS 4

/* The total number of actors in the application. */
#define ACTOR_COUNT 5

/* 
    Usage: lide_c_pea_driver.exe command.txt data.txt result.txt status.txt
*/
int main(int argc, char **argv) {
    char *command_file = NULL;
    char *data_file = NULL; 
    char *result_file = NULL;
    char *status_file = NULL;
    lide_c_actor_context_type *actors[ACTOR_COUNT];
    
    /* Connectivity: fifo1: (source, hist_gen), fifo2: (hist_gen, sink) */
    lide_c_fifo_pointer fifo1 = NULL, fifo2 = NULL, fifo3 = NULL, fifo4 = NULL;

    int token_size = 0;
    int i = 0;
    int arg_count = 5;
    
    /* actor descriptors (for diagnostic output) */
    char *descriptors[ACTOR_COUNT] = {"source_command", "source_data", 
                                          "pea", "sink_result", "sink_status"};

    /* Check program usage. */
    if (argc != arg_count) {
        fprintf(stderr, "lide_c_pea_driver.exe error: arg count");
        exit(1);
    }   

    /* Open the input and output file(s). */
    i = 1;
    command_file = argv[i++]; 
    data_file = argv[i++];
    result_file = argv[i++];
    status_file = argv[i++];  

    /* Create the buffers. */
    token_size = sizeof(int);
    fifo1 = lide_c_fifo_new(BUFFER_CAPACITY, token_size);
    fifo2 = lide_c_fifo_new(BUFFER_CAPACITY, token_size);
    fifo3 = lide_c_fifo_new(BUFFER_CAPACITY, token_size);
    fifo4 = lide_c_fifo_new(BUFFER_CAPACITY, token_size);

    /* Create and connect the actors. */
    i = 0;
    actors[ACTOR_SOURCE_COMMAND] = (lide_c_actor_context_type
            *)(lide_c_file_source_new(command_file, fifo1));

    actors[ACTOR_SOURCE_DATA] = (lide_c_actor_context_type
			*)(lide_c_file_source_new(data_file, fifo2));

    actors[ACTOR_PEA] = (lide_c_actor_context_type
            *)(lide_c_pea_new(fifo1, fifo2, fifo3, fifo4));

    actors[ACTOR_SINK_RESULT] = (lide_c_actor_context_type
			*)(lide_c_file_sink_new(result_file, fifo3));

	actors[ACTOR_SINK_STATUS] = (lide_c_actor_context_type
			*)(lide_c_file_sink_new(status_file, fifo4));

    /* Execute the graph. */
    lide_c_util_simple_scheduler(actors, ACTOR_COUNT, descriptors);

    /* Normal termination. */
    return 0;
}
