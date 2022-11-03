#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

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
	printf("PEA_NEW\n");
    lide_c_pea_context_type *context = NULL;

    context = lide_c_util_malloc(sizeof(lide_c_pea_context_type));
    context->mode = LIDE_C_PEA_MODE_GET_COMMAND;
    context->enable =
            (lide_c_actor_enable_function_type)lide_c_pea_enable;
    context->invoke =
            (lide_c_actor_invoke_function_type)lide_c_pea_invoke;

	for (int i = 0; i <= 7; i++) {
		context->N[i] = -1;
	}
	
	context->error = 0;

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
                    >= context->N[context->A] + 1);
			printf("result %d\n", result);
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

        case LIDE_C_PEA_MODE_OUTPUT:
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
	int command;	//8 bits
	int arg1;		//3 bits
	int arg2;		//5 bits
	int x;
	bool error = 0;

    switch(context->mode) {
        case LIDE_C_PEA_MODE_GET_COMMAND:
			/* Read in the command */
			lide_c_fifo_read(context->ffp_input_command, &full_command); 
			command = full_command >> 8;
			arg1 = (full_command >> 5) & 0x7;
			arg2 = full_command & 0x1F;
			printf("command %d : arg1 %d : arg2 %d \n",command, arg1,arg2);
			switch (command) {
				case 0: //STP A N
					context->b = 1;

					// if A < 0 or A > 7
					if (arg1 < 0 || arg1 > 7) {
						context->error += 0b0001;
						error = 1;
					}

					// if N < 0 or N > 10
					if (arg2 < 0 || arg2 > 10) {
						context->error += 0b0010;
						error = 1;
					}

					if (error) {
						context->mode = LIDE_C_PEA_MODE_OUTPUT;
						break;
					}
	
			        context->A = arg1;
					context->N[context->A] = arg2;
					context->mode = LIDE_C_PEA_MODE_STP;
					break;

				case 1: //EVP A
					context->b = 1;

					// if A < 0 or A > 7
					if (arg1 < 0 || arg1 > 7) {
						context->error += 0b0001;
						error = 1;
					} else {
						// if S_A is not set
						if (context->N[arg1] == -1) {
							context->error += 0b1000;
							error = 1;
						} 
					}
	
					if (error) {
						context->mode = LIDE_C_PEA_MODE_OUTPUT;
						break;
					}

					context->A = arg1;
					context->mode = LIDE_C_PEA_MODE_EVP;
					break;

				case 2: //EVB A b
					// if A < 0 or A > 7
					if (arg1 < 0 || arg1 > 7) {
                        context->error += 0b0001;
                        context->b = 1;     // TODO: does this make sense?
                        error = 1;
                    } else {
						// if S_A is not set
						if (context->N[arg1] == -1) {
							context->error += 0b1000;
							error = 1;
						}
					}

					// if b < 0 or b > 31
					if (arg2 < 0 || arg2 > 31) {
						context->error += 0b0100;
						context->b = 1;		// TODO: does this make sense?
						error = 1;
					}

					if (error) {
						context->mode = LIDE_C_PEA_MODE_OUTPUT;
						break;
					}

					context->A = arg1;
					context->b = arg2;
					context->mode = LIDE_C_PEA_MODE_EVB;
					break;

				case 3:
					// TODO: should we allow RST to ignore arguments? 
					// Or should we output errors if arguments are provided?
					context->b = 0;
					context->mode = LIDE_C_PEA_MODE_RST;
					break;

				default:
					context->mode = LIDE_C_PEA_MODE_GET_COMMAND;
			}

            break;

        case LIDE_C_PEA_MODE_STP:
			for (int i = 0; i <= context->N[context->A]; i++) {
            	lide_c_fifo_read(context->ffp_input_data, &x);
				context->S[context->A][i] = x;
			}
	
			for (int i = context->N[context->A] + 1; i <= 10; i++) {
				context->S[context->A][i] = 0;
			}

			context->result[0] = 1;
            context->mode = LIDE_C_PEA_MODE_OUTPUT;
            break;

		case LIDE_C_PEA_MODE_EVP:
			context->result[0] = 0;
			lide_c_fifo_read(context->ffp_input_data, &x);
			
			for (int i = 0; i <= context->N[context->A]; i++) {
				context->result[0] += context->S[context->A][i] * pow((double)x, (double)i);
			}

			context->mode = LIDE_C_PEA_MODE_OUTPUT;
			break; 

		case LIDE_C_PEA_MODE_EVB:
			for (int i = 0; i < context->b; i++) {
				context->result[i] = 0;
				lide_c_fifo_read(context->ffp_input_data, &x);
				
				for (int j = 0; j <= context->N[context->A]; j++) {
					context->result[i] += context->S[context->A][j] * pow((double)x, (double)j);
				}
			}

			context->mode = LIDE_C_PEA_MODE_OUTPUT;
			break;

		case LIDE_C_PEA_MODE_RST:
			for (int i = 0; i <= 7; i++) {
				context->N[i] = -1;
			}

			// does anything else need to be done here?

			context->mode = LIDE_C_PEA_MODE_GET_COMMAND;
			break;

        case LIDE_C_PEA_MODE_OUTPUT:
			if (context->error) {
				context->result[0] = 0;
			}

			for (int i = 0; i < context->b; i++) {
				lide_c_fifo_write(context->ffp_output_result, &context->result[i]);
				lide_c_fifo_write(context->ffp_output_status, &context->error);
			}

			context->error = 0;
	
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


