#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "lide_c_util.h"
#include "lide_c_pea.h"

/****************** STRUCTURE DEFINITION***********************************/
struct _lide_c_pea_context_struct {
#include "lide_c_actor_context_type_common.h"

    /* Struct variables */
    int S[8][11];
	int N[8];
	int A;
	int b;
	int x;
	unsigned int result[32];
	unsigned int error;

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
    /* Create the new PEA context */
	lide_c_pea_context_type *context = NULL;
    context = lide_c_util_malloc(sizeof(lide_c_pea_context_type));

	/* Set the initial mode and function pointers for enable and invoke */
    context->mode = LIDE_C_PEA_MODE_GET_COMMAND;
    context->enable =
            (lide_c_actor_enable_function_type)lide_c_pea_enable;
    context->invoke =
            (lide_c_actor_invoke_function_type)lide_c_pea_invoke;

	/* Initialize N, the length array, to -1 */
	for (int i = 0; i <= 7; i++) {
		context->N[i] = -1;
	}
	
	/* Initialize error to 0, indicating no error */
	context->error = 0;

	/* Initialize the FIFOs */
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
			/* Checks that command FIFO has not reached capacity and has at least 1 value */
            result = (lide_c_fifo_population(context->ffp_input_command)
                    < lide_c_fifo_capacity(context->ffp_input_command));
            result = result && (lide_c_fifo_population(context->ffp_input_command)
                    >= 1);
            break;

        case LIDE_C_PEA_MODE_STP:
			/* Checks that data FIFO has not reached capacity and has at least N[A] + 1 values (to fill N[A] + 1 coefficients for S[A]) */
            result = (lide_c_fifo_population(context->ffp_input_data)
                    < lide_c_fifo_capacity(context->ffp_input_data));
            result = result && (lide_c_fifo_population(context->ffp_input_data)
                    >= context->N[context->A] + 1);
            break;

		case LIDE_C_PEA_MODE_EVP:
			/* Checks that data FIFO has not reached capacity and has at least 1 value */
			result = (lide_c_fifo_population(context->ffp_input_data)
					< lide_c_fifo_capacity(context->ffp_input_data));
			result = result && (lide_c_fifo_population(context->ffp_input_data)
					>= 1);
			break;

		case LIDE_C_PEA_MODE_EVB:
			/* Checks that data FIFO has not reached capacity and has at least b values */
			result = (lide_c_fifo_population(context->ffp_input_data)
					< lide_c_fifo_capacity(context->ffp_input_data));
			result = result && (lide_c_fifo_population(context->ffp_input_data)
					>= context->b);
			break;

		case LIDE_C_PEA_MODE_RST:
			/* Operation always permitted */
			result = true;
			break;

        case LIDE_C_PEA_MODE_OUTPUT:
			/* Checks that result FIFO and status FIFOs have not reached capacity and there is room for b values */
            result = (lide_c_fifo_population(context->ffp_output_result)
                    < lide_c_fifo_capacity(context->ffp_output_result));
            result = result && (lide_c_fifo_population(context->ffp_output_status)                      
					< lide_c_fifo_capacity(context->ffp_output_status));
			result = result && (lide_c_fifo_population(context->ffp_output_result) + context->b
					<= lide_c_fifo_capacity(context->ffp_output_result));
			result = result && (lide_c_fifo_population(context->ffp_output_status) + context->b
					<= lide_c_fifo_capacity(context->ffp_output_status));
            break;

        default:
            result = false;
            break;
    }
    return result;
}

void lide_c_pea_invoke(lide_c_pea_context_type *context) {

	int full_command; //16 bits
	int command;	//8 bits [15 14 13 12 11 10 9 8] 7 6 5 4 3 2 1 0
	int arg1;		//3 bits 15 14 13 12 11 10 9 8 [7 6 5] 4 3 2 1 0
	int arg2;		//5 bits 15 14 13 12 11 10 9 8 7 6 5 [4 3 2 1 0]
	int x;
	bool error = 0;

    switch(context->mode) {
        case LIDE_C_PEA_MODE_GET_COMMAND:
			/* Read in the command */
			lide_c_fifo_read(context->ffp_input_command, &full_command);
			/* Breaking up the individual parts of a command string */ 
			command = full_command >> 8;
			arg1 = (full_command >> 5) & 0x7;
			arg2 = full_command & 0x1F;

			switch (command) {
				case 0: // STP A N
					context->b = 1;

					/* If N < 0 or N > 10, end and handle error */
					if (arg2 < 0 || arg2 > 10) {
						context->error += 0b01;
						error = 1;
						context->mode = LIDE_C_PEA_MODE_OUTPUT;
						break;
					}
	
					/* Set A and N, and set mode to process STP */
			        context->A = arg1;
					context->N[context->A] = arg2;
					context->mode = LIDE_C_PEA_MODE_STP;
					break;

				case 1: // EVP A
					context->b = 1;

					/* If S[A] is not set, end and handle error */
					if (context->N[arg1] == -1) {
						context->error += 0b10;
						error = 1;
						context->mode = LIDE_C_PEA_MODE_OUTPUT;
						break;
					} 

					/* Set A, and set mode to process EVP */
					context->A = arg1;
					context->mode = LIDE_C_PEA_MODE_EVP;
					break;

				case 2: //EVB A b
					/* If S[A] is not set, end and handle error */
					if (context->N[arg1] == -1) {
						context->error += 0b10;
						context->b = 1;
						error = 1;
						context->mode = LIDE_C_PEA_MODE_OUTPUT;
						break;
					}

					/* Set A and b, and set mode to process EVB */
					context->A = arg1;
					context->b = arg2;
					context->mode = LIDE_C_PEA_MODE_EVB;
					break;

				case 3:
					/* Set mode to process RST */
					context->b = 0;
					context->mode = LIDE_C_PEA_MODE_RST;
					break;

				default:
					/* If an invalid command is provided, process a new one */
					context->mode = LIDE_C_PEA_MODE_GET_COMMAND;
			}

            break;

        case LIDE_C_PEA_MODE_STP:
			/* Set N[A] number of coefficients of S[A] by reading from data FIFO */
			for (int i = 0; i <= context->N[context->A]; i++) {
            	lide_c_fifo_read(context->ffp_input_data, &x);
				context->S[context->A][i] = x;
			}
	
			/* Set the unused coefficients of S[A] to 0 */
			for (int i = context->N[context->A] + 1; i <= 10; i++) {
				context->S[context->A][i] = 0;
			}

			/* Indicate success by setting result and set the mode to OUTPUT */
			context->result[0] = 1;
            context->mode = LIDE_C_PEA_MODE_OUTPUT;
            break;

		case LIDE_C_PEA_MODE_EVP:
			/* Initialize result, which holds a sum, to 0 */
			context->result[0] = 0;
			/* Read in x, which will be used for computations */
			lide_c_fifo_read(context->ffp_input_data, &x);
			
			/* Perform the computation using S[A] */
			for (int i = 0; i <= context->N[context->A]; i++) {
				context->result[0] += context->S[context->A][i] * pow((double)x, (double)i);
			}

			/* Set the next mode to OUTPUT */
			context->mode = LIDE_C_PEA_MODE_OUTPUT;
			break; 

		case LIDE_C_PEA_MODE_EVB:
			/* Do this b times, resulting in b sums in result array */
			for (int i = 0; i < context->b; i++) {
				context->result[i] = 0;
				lide_c_fifo_read(context->ffp_input_data, &x);
				
				for (int j = 0; j <= context->N[context->A]; j++) {
					context->result[i] += context->S[context->A][j] * pow((double)x, (double)j);
				}
			}

			/* Set the next mode to OUTPUT */
			context->mode = LIDE_C_PEA_MODE_OUTPUT;
			break;

		case LIDE_C_PEA_MODE_RST:
			/* Set every N[A] to -1 to re-initialize */
			for (int i = 0; i <= 7; i++) {
				context->N[i] = -1;
			}

			/* Set the next mode to OUTPUT */
			context->mode = LIDE_C_PEA_MODE_GET_COMMAND;
			break;

        case LIDE_C_PEA_MODE_OUTPUT:
			/* If there was an error, set result to 0 */
			if (context->error) {
				context->result[0] = 0;
			}

			/* Write b values to result FIFO and status FIFO. b varies depending on the instruction being outputted */
			for (int i = 0; i < context->b; i++) {
				lide_c_fifo_write(context->ffp_output_result, &context->result[i]);
				lide_c_fifo_write(context->ffp_output_status, &context->error);
			}

			/* Reset the error for future use */
			context->error = 0;
	
			/* Set the mode to GET_COMMAND to read in the next command */
			context->mode = LIDE_C_PEA_MODE_GET_COMMAND;
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


