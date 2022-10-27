/* Source code and implementation for objtr_hist_gen */

#include <stdio.h>
#include <stdlib.h>

#include "lide_c_fifo.h"
#include "objtr_hist_gen.h"
#include "lide_c_util.h"



/*******************************************************************************
OBJTR HIST GEN STRUCTURE DEFINITION
*******************************************************************************/

struct _objtr_hist_gen_context_struct {
#include "lide_c_actor_context_type_common.h"

	/*Bin/image params*/
	int numbins;
	int size;
	int *s_bins;

    /* Input ports. */
	lide_c_fifo_pointer num_bins;
    lide_c_fifo_pointer bins;
    lide_c_fifo_pointer image_size;
    lide_c_fifo_pointer image;

    /* Output port. */
    lide_c_fifo_pointer out;
};

/*******************************************************************************
IMPLEMENTATIONS OF INTERFACE FUNCTIONS.
*******************************************************************************/

objtr_hist_gen_context_type *objtr_hist_gen_new(
	lide_c_fifo_pointer num_bins, lide_c_fifo_pointer bins, lide_c_fifo_pointer image_size, lide_c_fifo_pointer image, lide_c_fifo_pointer out) {

    objtr_hist_gen_context_type *context = NULL;
	

    context = lide_c_util_malloc(sizeof(objtr_hist_gen_context_type));
	context->mode = OBJTR_HIST_GEN_MODE_STORE_NUMBINS;
    context->enable =
            (lide_c_actor_enable_function_type)objtr_hist_gen_enable;
    context->invoke =
            (lide_c_actor_invoke_function_type)objtr_hist_gen_invoke;

	context->numbins = 0;
	context->size = 0;
	context->s_bins = NULL;

	context->num_bins = num_bins;
    context->bins = bins;
    context->image_size = image_size;
    context->image = image;
    context->out = out;
    return context;
}


bool objtr_hist_gen_enable(
        objtr_hist_gen_context_type *context) {
    bool result = false;

    switch (context->mode) {

    case OBJTR_HIST_GEN_MODE_STORE_NUMBINS:
        result = lide_c_fifo_population(context->num_bins) >= 1;
        break;

	case OBJTR_HIST_GEN_MODE_STORE_IMAGE_SIZE:
		result = lide_c_fifo_population(context->image_size) >= 1;
		break;

	case OBJTR_HIST_GEN_MODE_STORE_BINS:
		result = lide_c_fifo_population(context->bins) >= context->numbins;
		break;
	
    case OBJTR_HIST_GEN_MODE_PROCESS:
	
        result =(lide_c_fifo_population(context->image) >= context->size) &&
                ((lide_c_fifo_population(context->out) <
                lide_c_fifo_capacity(context->out)));
        break;
    default:
            result = false;
            break;
    }
    return result;
}

void objtr_hist_gen_invoke(objtr_hist_gen_context_type *context) {
    int i = 0;
    int j = 0;
	int k = 0;
	int value = 0;
	int *output = NULL;

    switch (context->mode) {
    case OBJTR_HIST_GEN_MODE_STORE_NUMBINS:
        lide_c_fifo_read(context->num_bins, &(context->numbins));
        if (context->numbins <= 0) {
            context->mode = OBJTR_HIST_GEN_MODE_STORE_NUMBINS;
            return;
        }
        context->mode = OBJTR_HIST_GEN_MODE_STORE_IMAGE_SIZE;
        break;

	case OBJTR_HIST_GEN_MODE_STORE_IMAGE_SIZE:
		lide_c_fifo_read(context->image_size, &(context->size));
		if (context->size <= 0){
			context->mode = OBJTR_HIST_GEN_MODE_STORE_IMAGE_SIZE;
			return;
		}
		context->mode = OBJTR_HIST_GEN_MODE_STORE_BINS;
        break;
	
	case OBJTR_HIST_GEN_MODE_STORE_BINS:
		context->s_bins = (int *)malloc(sizeof(int)*(context->numbins)*2);
		for(i = 0 ; i < (context->numbins)*2 ; i++){
			lide_c_fifo_read(context->bins,&(context->s_bins[i]));
		}
		if (context->numbins <= 0){
			context->mode = OBJTR_HIST_GEN_MODE_STORE_BINS;
			return;
		}
		context->mode = OBJTR_HIST_GEN_MODE_PROCESS;
		break;

    case OBJTR_HIST_GEN_MODE_PROCESS:
		output = (int *)calloc((context->numbins),sizeof(int));
        for (i = 0; i < context->size; i++) {
            lide_c_fifo_read(context->image, &(value));
			k = 0;
			for(j = 0; j < (context->numbins) ;j++) {
				if ((value >= context->s_bins[k]) && (value <= context->s_bins[k+1])) {
                output[j] = output[j] + 1;
				j = context->numbins;
				}            
			k = k+2;
        	}
		}

		for(i = 0; i < context->numbins; i++){
			lide_c_fifo_write(context->out, &output[i]);
		}
	
       	context->mode = OBJTR_HIST_GEN_MODE_STORE_NUMBINS;
        break;
    default:
        context->mode = OBJTR_HIST_GEN_MODE_STORE_NUMBINS;
        break;
    }
    return;
}

void objtr_hist_gen_terminate(
        objtr_hist_gen_context_type *context) {
    free(context);
}
