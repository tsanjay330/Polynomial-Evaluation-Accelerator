#include <stdio.h>
#include <stdlib.h>

#include "lide_c_fifo.h"
#include "lide_c_file_source.h"
#include "lide_c_file_sink.h"
#include "objtr_hist_gen.h"
#include "lide_c_util.h"

#define BUFFER_CAPACITY 1024

/* An enumeration of the actors in this application. */
#define ACTOR_num_binsSOURCE 0
#define ACTOR_binsSOURCE 1
#define ACTOR_image_sizeSOURCE 2
#define ACTOR_imageSOURCE 3
#define ACTOR_HISTOGRAM 4
#define ACTOR_SINK 5

/* The total number of actors in the application. */
#define ACTOR_COUNT 6

/* 
    Usage: lide_c_inner_product_driver.exe m_file x_file y_file out_file
*/
int main(int argc, char **argv) {
    char *num_bins_file = NULL;
    char *bins_file = NULL;
    char *image_size_file = NULL;
	char *image_file = NULL;
    char *out_file = NULL;
    lide_c_actor_context_type *actors[ACTOR_COUNT];

    /* Connectivity: fifo1: (m, product), fifo2: (x, product); 
       fifo3: (y, product); fifo4: (product, out) 
    */
    lide_c_fifo_pointer fifo1 = NULL, fifo2 = NULL, fifo3 = NULL, fifo4 = NULL, fifo5 = NULL;

    int token_size = 0;
    int i = 0;
    int arg_count = 6;

    /* actor descriptors (for diagnostic output) */
    char *descriptors[ACTOR_COUNT] = {"num_binssource", "binssource", 
	"image_sizesource", "imagesource", "histogram", "sink"};

    /* Check program usage. */
    if (argc != arg_count) {
        fprintf(stderr, "objtr_hist_gen_driver.exe error: arg count");
        exit(1);
    }
	
    /* Open the input and output file(s). */
    i = 1;
    num_bins_file = argv[i++];
    bins_file = argv[i++];
    image_size_file = argv[i++];
	image_file = argv[i++];
    out_file = argv[i++];
	
    /* Create the buffers. */
    token_size = sizeof(int);
    fifo1 = lide_c_fifo_new(BUFFER_CAPACITY, token_size);
    fifo2 = lide_c_fifo_new(BUFFER_CAPACITY, token_size);
    fifo3 = lide_c_fifo_new(BUFFER_CAPACITY, token_size);
    fifo4 = lide_c_fifo_new(BUFFER_CAPACITY, token_size);
    fifo5 = lide_c_fifo_new(BUFFER_CAPACITY, token_size);
	
    /* Create and connect the actors. */
    i = 0;
    actors[ACTOR_num_binsSOURCE] = (lide_c_actor_context_type
            *)(lide_c_file_source_new(num_bins_file, fifo1));

    actors[ACTOR_binsSOURCE] = (lide_c_actor_context_type
            *)(lide_c_file_source_new(bins_file, fifo2));

    actors[ACTOR_image_sizeSOURCE] = (lide_c_actor_context_type
            *)(lide_c_file_source_new(image_size_file, fifo3));
	
	actors[ACTOR_imageSOURCE] = (lide_c_actor_context_type 
			*)(lide_c_file_source_new(image_file,fifo4));

    actors[ACTOR_HISTOGRAM] = (lide_c_actor_context_type
            *)(objtr_hist_gen_new(fifo1, fifo2, fifo3, fifo4,fifo5));

    actors[ACTOR_SINK] = (lide_c_actor_context_type *)
            (lide_c_file_sink_new(out_file, fifo5));

    /* Execute the graph. */
    lide_c_util_simple_scheduler(actors, ACTOR_COUNT, descriptors);

    /* Normal termination. */
    return 0;
}

