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

#include "lide_c_TRT.h"
#include "lide_c_util.h"

struct _lide_c_TRT_context_struct {
#include "lide_c_actor_context_type_common.h"
	/* Actor interface ports. */
	/*input sequence*/
	lide_c_fifo_pointer in1; /*float fifo*/
	lide_c_fifo_pointer in2; /*float pointer fifo*/
	lide_c_fifo_pointer in3; /*int pointer fifo*/
	/*Output sequence*/
	lide_c_fifo_pointer out1; /*float fifo*/
	lide_c_fifo_pointer out2; /*float pointer fifo*/
	lide_c_fifo_pointer out3; /*int pointer fifo*/
	/* Actor Parameters*/
	int Ws; /*Window Size*/
	int k,len;
	int TRlength;
	double *vol;
	int *state;
	int *TR;
	int *TRindex;
	double *TRT,*TRall;
};

lide_c_TRT_context_type *lide_c_TRT_new(int Ws, lide_c_fifo_pointer in1, lide_c_fifo_pointer in2, lide_c_fifo_pointer in3, lide_c_fifo_pointer out1,lide_c_fifo_pointer out2, lide_c_fifo_pointer out3) {
	int i;
	lide_c_TRT_context_type *context = NULL;
    context = lide_c_util_malloc(sizeof(lide_c_TRT_context_type));
    context->enable = (lide_c_actor_enable_function_type)lide_c_TRT_enable;
    context->invoke = (lide_c_actor_invoke_function_type)lide_c_TRT_invoke;
    context->in1 = in1;
	context->in2 = in2;
	context->in3 = in3;
    context->out1 = out1;
	context->out2 = out2;
	context->out3 = out3;
	context->mode = 1;
	/*Load table and Parameter*/
	context->Ws = Ws;
	context->k = 0;
	context->len = 0;
	/*Allocate memory for sorting*/
	context->TRT = (double*)malloc(sizeof(double)*Ws);
	context->TRall = (double*)malloc(sizeof(double)*15999986);
    context->TRindex = (int*)malloc(sizeof(int)*Ws);
	/* Initialize the array*/
	for (i=0; i<Ws; i++){
        context->TRT[i] = 0.0;
        context->TRindex[i] = 0.0;
    }
	for (i = 0; i<15999986; i++)
		context->TRall[i] = 0.0;
	return context;
}

bool lide_c_TRT_enable(lide_c_TRT_context_type *context) {
	bool result = false;
	//result =1;
	//return result;
	switch (context->mode) {
		case LIDE_C_TRT_MODE_PROCESS:

			result = (lide_c_fifo_population(context->in1) >= 2) &&
					(lide_c_fifo_population(context->in2) >= 1) &&
					(lide_c_fifo_population(context->in3) >= 3) &&
					(lide_c_fifo_population(context->out2) < lide_c_fifo_capacity(context->out2)) &&
					(lide_c_fifo_population(context->out3) < lide_c_fifo_capacity(context->out3)) &&
					(lide_c_fifo_population(context->out1) < lide_c_fifo_capacity(context->out1));
		break;
		default:
			result = false;
		break;
	}
	return result;
}

void lide_c_TRT_invoke(lide_c_TRT_context_type *context) {
	int tmp,i;
	double *fp;
	int	*intp;
	int *p;
	double *trt_p1;
	double *trt_p2;
	double *trt;
	double high,low,medium,dlen;
	/*Load data from three fifo*/
	lide_c_fifo_read(context->in1, &high);
	lide_c_fifo_read(context->in1, &low);
	lide_c_fifo_read(context->in1, &medium);
	medium = (double)(high+low)/2.0;
	lide_c_fifo_read(context->in2, &fp);
	context->vol = fp;
	lide_c_fifo_read(context->in3, &intp);
	context->state = intp;
	lide_c_fifo_read(context->in3, &intp);
	context->TR = intp;
	lide_c_fifo_read(context->in3, &intp);
	
	/*Compute the transition time*/
	context->TRindex = intp;
	p = intp+1;
	trt_p1 = (double*)malloc(sizeof(double)*7000);
	trt_p2 = (double*)malloc(sizeof(double)*7000);
	trt = (double*)malloc(sizeof(double)*7000);
	for (i = 0; i<context->TRindex[0]; i++){
		trt_p1[i] = fabs(context->vol[context->TRindex[i+1]]-medium);
		trt_p2[i] = fabs(context->vol[context->TRindex[i+1]+1]-context->vol[context->TRindex[i+1]]); 
	}	
	for (i = 0; i<context->TRindex[0]; i++){
		trt[i] = trt_p1[i]/trt_p2[i];
	}
	for (i = 0; i<context->TRindex[0]; i++){
		context->TRT[i] = p[i] + trt[i];
	}
	for (i = 0; i<context->TRindex[0]; i++){
		context->TRT[i] = 1+context->TRT[i];
	}
	free(trt_p1);
	free(trt_p2);
	free(trt);
	/*Updata TRall*/
	tmp = context->Ws*context->k;
	for(i = 0; i<context->TRindex[0];i++)
		context->TRall[i+context->len] = context->TRT[i]+tmp;
	context->len += context->TRindex[0];
	context->k += 1;
	dlen = (double)context->len;

	/*Store data to three fifo*/
	lide_c_fifo_write(context->out1, &high);
	lide_c_fifo_write(context->out1, &low);
	lide_c_fifo_write(context->out1, &medium);
	lide_c_fifo_write(context->out1, &dlen);
	lide_c_fifo_write(context->out2, &context->vol);
	lide_c_fifo_write(context->out2, &context->TRT);
	lide_c_fifo_write(context->out2, &context->TRall);
	lide_c_fifo_write(context->out3, &context->state);
	lide_c_fifo_write(context->out3, &context->TR);
	lide_c_fifo_write(context->out3, &context->TRindex);
	return;
	
}

void lide_c_TRT_terminate(lide_c_TRT_context_type *context){  
    free(context->TRT);  	
    free(context->TRall);  	
	free(context);
}
