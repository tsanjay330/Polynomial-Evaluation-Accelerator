#include <stdio.h>
#include <stdlib.h>

#include "lide_c_util.h"
#include "lide_c_hist_gen.h"

/*******************************************************************************HIST GEN STRUCTURE DEFINITION
*******************************************************************************/
struct _lide_c_hist_gen_context_struct {
#include "lide_c_actor_context_type_common.h"

    /* Persistent local variables for temporary work */
    int num_bins;
    int w;
    int h;
    int stride;
    int height;
    int *bins;
    int head_x;
    int head_y;

    /* input ports */
    lide_c_fifo_pointer ffp_input_bins;
    lide_c_fifo_pointer ffp_input_head;
    lide_c_fifo_pointer ffp_input_img;

    /* output ports */
    lide_c_fifo_pointer ffp_output_hist;
};

/*******************************************************************************
IMPLEMENTATIONS OF INTERFACE FUNCTIONS.
*******************************************************************************/

lide_c_hist_gen_context_type *lide_c_hist_gen_new(
        lide_c_fifo_pointer ffp_input_bins, lide_c_fifo_pointer ffp_input_head, lide_c_fifo_pointer ffp_input_img, int num_bins, int w, int h, int stride, int height, lide_c_fifo_pointer ffp_output_hist) {

    lide_c_hist_gen_context_type *context = NULL;

    context = lide_c_util_malloc(sizeof(lide_c_hist_gen_context_type));
    context->mode = LIDE_C_HIST_GEN_MODE_SET_BINS;
    context->enable =
            (lide_c_actor_enable_function_type)lide_c_hist_gen_enable;
    context->invoke =
            (lide_c_actor_invoke_function_type)lide_c_hist_gen_invoke;

        context->num_bins = num_bins;
        context->w = w;
        context->h = h;
        context->stride = stride;
        context->height = height;

    context->ffp_input_bins = ffp_input_bins;
    context->ffp_input_head = ffp_input_head;
    context->ffp_input_img = ffp_input_img;

    context->ffp_output_hist = ffp_output_hist;

    return context;
}

bool lide_c_hist_gen_enable(lide_c_hist_gen_context_type *context) {
    bool result = false;

    switch (context->mode) {
        case LIDE_C_HIST_GEN_MODE_SET_BINS:
            result = (lide_c_fifo_population(context->ffp_input_bins)
					< lide_c_fifo_capacity(context->ffp_input_bins));
            result = result && (lide_c_fifo_population(context->ffp_input_bins)
					>= context->num_bins * 2);
            break;

        case LIDE_C_HIST_GEN_MODE_SET_HEAD:
            result = (lide_c_fifo_population(context->ffp_input_head)
					< lide_c_fifo_capacity(context->ffp_input_head));
            result = result && (lide_c_fifo_population(context->ffp_input_head)
					>= 2);
            break;

        case LIDE_C_HIST_GEN_MODE_PROCESS:
            result = (lide_c_fifo_population(context->ffp_input_img)
                    < lide_c_fifo_capacity(context->ffp_input_img));
            result = result && (lide_c_fifo_population(context->ffp_input_img)  					>= (context->stride) * (context->height));
            break;
    
        default:
            result = false;
            break;
    }
    return result;
}

void lide_c_hist_gen_invoke(lide_c_hist_gen_context_type *context) {

    int *bins;
    int head_x, head_y;
    int curr;
    int *hist;

    switch(context->mode) {
        case LIDE_C_HIST_GEN_MODE_SET_BINS:
            bins = (int *) malloc(context->num_bins * 2 * sizeof(int));

            for (int i = 0; i < context->num_bins * 2; i++) {
                lide_c_fifo_read(context->ffp_input_bins, &bins[i]);
            }

            context->bins = bins;
            context->mode = LIDE_C_HIST_GEN_MODE_SET_HEAD;
            break;

        case LIDE_C_HIST_GEN_MODE_SET_HEAD:
            lide_c_fifo_read(context->ffp_input_head, &context->head_x);
            lide_c_fifo_read(context->ffp_input_head, &context->head_y);
            context->mode = LIDE_C_HIST_GEN_MODE_PROCESS;
            break;

        case LIDE_C_HIST_GEN_MODE_PROCESS:
            head_x = context->head_x;
            head_y = context->head_y;

            hist = (int *) calloc(context->num_bins, sizeof(int));
            for (int row = 0; row < context->height; row++) {
                for (int col = 0; col < context->stride; col++) {
                    lide_c_fifo_read(context->ffp_input_img, &curr);

                    if (col < head_x || col >= head_x + context->w
                            || row < head_y || row >= head_y + context->h) {
                        continue;
                    }

                    for (int i = 0; i < context->num_bins; i++) {
                        if (curr >= context->bins[2 * i] 
                                && curr <= context->bins[2 * i + 1]) {
                            hist[i]++;
                            break;
                        }
                    }
                }
            }
           
            for (int i = 0; i < context->num_bins; i++) {
                lide_c_fifo_write(context->ffp_output_hist, &hist[i]);
            }

            free(hist);

            context->mode = LIDE_C_HIST_GEN_MODE_PROCESS;

            break;
        
        default:
            context->mode = LIDE_C_HIST_GEN_MODE_SET_BINS;
            break;
    }
    return;
}

void lide_c_hist_gen_terminate(lide_c_hist_gen_context_type *context) {
    free(context);
}
