#include <stdio.h>
#include <stdlib.h>

#include "lide_c_util.h"
#include "lide_c_pea.h"

/****************** STRUCTURE DEFINITION***********************************/
struct _lide_c_pea_context_struct {
#include "lide_c_actor_context_type_common.h"

    /* Persistent local variables for temporary work */
    int S[8][11];
	int N[8];
	int A;
	int b;
	int x;

    /* input ports */
    lide_c_fifo_pointer ffp_input_command;
    lide_c_fifo_pointer ffp_input_data;


    /* output ports */
    lide_c_fifo_pointer ffp_output_result;
	lide_c_fifo_pointer ffp_output_status;
};

/*******************************************************************************
IMPLEMENTATIONS OF INTERFACE FUNCTIONS.
*******************************************************************************/

lide_c_pea_context_type *lide_c_pea_new(
        lide_c_fifo_pointer ffp_input_command, lide_c_fifo_pointer ffp_input_data, lide_c_fifo_pointer ffp_output_result, lide_c_fifo_pointer ffp_output_status) {

    lide_c_pea_context_type *context = NULL;

    context = lide_c_util_malloc(sizeof(lide_c_pea_context_type));
    context->mode = LIDE_C_PEA_MODE_GET_COMMAND;
    context->enable =
            (lide_c_actor_enable_function_type)lide_c_pea_enable;
    context->invoke =
            (lide_c_actor_invoke_function_type)lide_c_pea_invoke;

    context->ffp_input_command = ffp_input_command;
    context->ffp_input_data = ffp_input_data;

    context->ffp_output_result = ffp_output_result;
	context->ffp_output_status = ffp_output_status;

    return context;
}

bool lide_c_pea_enable(lide_c_pea_context_type *context) {
    bool result = false;

    switch (context->mode) {
        case LIDE_C_PEA_MODE_GET_COMMAND:
            result = (lide_c_fifo_population(context->ffp_input_command)
                    < lide_c_fifo_capacity(context->ffp_input_command));
            result = result && (lide_c_fifo_population(context->ffp_input_command)
                    >= 16);
            break;

        case LIDE_C_PEA_MODE_STP:
            result = (lide_c_fifo_population(context->ffp_input_data)
                    < lide_c_fifo_capacity(context->ffp_input_data));
            result = result && (lide_c_fifo_population(context->ffp_input_data)
                    >= context->N[context->A]);
            break;

		case LIDE_C_PEA_MODE_EVP:
			result = (lide_c_fifo_population(context->ffp_input_data)
					< lide_c_fifo_capacity(context->ffp_input_data));
			result = result && (lide_c_fifo_population(context->ffp_input_data)
					>= 1);
			break;

		case LIDE_C_PEA_MODE_EVB:
			result = (lide_c_fifo_population(context->ffp_input_data)
					< lide_c_fifo_capacity(context->ffp_input_data));
			result = result && (lide_c_fifo_population(context->ffp_input_data)
					>= context->b);
			break;

		case LIDE_C_PEA_MODE_RST:
			result = true;
			break;

		/* TODO: fix this. Add a condition for having enough space in both buffers  */
        case LIDE_C_PEA_MODE_OUTPUT:
            result = (lide_c_fifo_population(context->ffp_output_result)
                    < lide_c_fifo_capacity(context->ffp_output_result));
            result = result && (lide_c_fifo_population(context->ffp_output_status)                      
					< lide_c_fifo_capacity(context->ffp_output_status));
            break;

        default:
            result = false;
            break;
    }
    return result;
}

void lide_c_pea_invoke(lide_c_pea_context_type *context) {

	int full_command; //16 bits
	int command;	//8 bits
	int arg1;		//3 bits
	int arg2;		//5 bits

    switch(context->mode) {
        case LIDE_C_PEA_MODE_GET_COMMAND:
			/* Read in the command */
			lide_c_fifo_read(context->ffp_input_command, &full_command); 
			command = full_command >> 8;
			arg1 = (full_command >> 5) & 0x7;
			arg2 = full_command & 0x1F;

			switch (command) {
				case 0: //STP A N
			        context->A = arg1;
					context->N[context->A] = arg2;
					context->mode = LIDE_C_PEA_MODE_STP;
					break;

				case 1: //EVP A
					context->A = arg1;
					context->mode = LIDE_C_PEA_MODE_EVP;
					break;

				case 2: //EVB A b
					context->A = arg1;
					context->b = arg2;
					context->mode = LIDE_C_PEA_MODE_EVB;
					break;

				case 3:
					context->mode = LIDE_C_PEA_MODE_RST;
					break;

				default:
					context->mode = LIDE_C_PEA_MODE_GET_COMMAND;
			}

            break;

        case LIDE_C_PEA_MODE_STP:
            lide_c_fifo_read(context->ffp_input_head, &context->head_x);
            lide_c_fifo_read(context->ffp_input_head, &context->head_y);
            context->mode = LIDE_C_HIST_GEN_MODE_PROCESS;
            break;

        case LIDE_C_PEA_MODE_OUTPUT:
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

            context->mode = LIDE_C_PEA_MODE_OUTPUT;

            break;

        default:
            context->mode = LIDE_C_PEA_MODE_GET_COMMAND;
            break;
    }
    return;
}

void lide_c_pea_terminate(lide_c_pea_context_type *context) {
    free(context);
}


