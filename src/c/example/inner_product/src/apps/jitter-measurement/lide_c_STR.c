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
#include <math.h>

#include "lide_c_STR.h"
#include "lide_c_util.h"

struct _lide_c_STR_context_struct {
#include "lide_c_actor_context_type_common.h"
	/* Actor interface ports. */
	/*input sequence*/
	lide_c_fifo_pointer in1; /*float fifo*/
	lide_c_fifo_pointer in2; /*float pointer fifo*/
	/*output sequence*/
	lide_c_fifo_pointer out1;/*float fifo*/
	lide_c_fifo_pointer out2;/*float pointer fifo*/
	lide_c_fifo_pointer out3;/*int pointer fifo*/
	/* Actor Parameters*/
	double *vol;
	int Ws;
	double high,low;
	int *state;
};

lide_c_STR_context_type *lide_c_STR_new(int Ws, lide_c_fifo_pointer in1, lide_c_fifo_pointer in2, lide_c_fifo_pointer out1, lide_c_fifo_pointer out2, lide_c_fifo_pointer out3) {
	int i;
	lide_c_STR_context_type *context = NULL;
	context = lide_c_util_malloc(sizeof(lide_c_STR_context_type));
    context->enable = (lide_c_actor_enable_function_type)lide_c_STR_enable;
    context->invoke = (lide_c_actor_invoke_function_type)lide_c_STR_invoke;
    context->in1 = in1;
	context->in2 = in2;
	context->out1 = out1;
	context->out2 = out2;
	context->out3 = out3;
	context->mode = 1;
	/*Load table and Parameter*/
	context->Ws = Ws;
	context->high = 0.0;
	context->low = 0.0;
	context->vol = NULL;
	/*Allocate memory for sorting*/
	context->state = (int*)malloc(sizeof(int)*Ws);
	/* Initialize the array*/
	for (i=0; i<Ws; i++)   
        context->state[i] = 2; 
	return context;
}

bool lide_c_STR_enable(lide_c_STR_context_type *context) {
	bool result = false;
	switch (context->mode) {
		case LIDE_C_STR_MODE_PROCESS:
			result = (lide_c_fifo_population(context->in1) >= LIDE_C_STR_INPUT_THRESHOLD) &&
					(lide_c_fifo_population(context->in2) >= 1) &&
					(lide_c_fifo_population(context->out1) < lide_c_fifo_capacity(context->out1)) &&
					(lide_c_fifo_population(context->out2) < lide_c_fifo_capacity(context->out2)) &&
					(lide_c_fifo_population(context->out3) < lide_c_fifo_capacity(context->out3));
		break;
		default:
			result = false;
		break;
	}
	return result;
}

void lide_c_STR_invoke(lide_c_STR_context_type *context) {
	double tmp;
	double high,low,medium;
	double *p;
	int i;
	/*Load data from input fifo*/
	lide_c_fifo_read(context->in1, &tmp);
	high = tmp;
	lide_c_fifo_read(context->in1, &tmp);
	low = tmp;
	lide_c_fifo_read(context->in1, &tmp);
	medium = tmp;
	lide_c_fifo_read(context->in2, &p);
	context->vol = p;
	/*Assign the state for every input*/
	for (i = 0; i<context->Ws; i++){
		context->state[i] = (p[i] >= medium);
	}
	/*Store results*/
	lide_c_fifo_write(context->out1, &high);
	lide_c_fifo_write(context->out1, &low);
	lide_c_fifo_write(context->out1, &medium);
	lide_c_fifo_write(context->out2, &context->vol);	
	lide_c_fifo_write(context->out3, &context->state);

	return;
}

void lide_c_STR_terminate(lide_c_STR_context_type *context){   
	free(context->state);  		
	free(context);
}
