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

#include "lide_c_RE.h"
#include "lide_c_util.h"

struct _lide_c_RE_context_struct {
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
	int TRlength;
	double *vol;
	int *state;
	int *TR;
	int *TRindex;
	double *TRT,*TRall;
	double *d;
};

int cmp (const void * a, const void * b)
{
	double c = *(double*)a - *(double*)b;
	if (c>0)
		return 1;
	else if (c<0)
		return -1;
	else 
		return 0;

}

lide_c_RE_context_type *lide_c_RE_new(int Ws, lide_c_fifo_pointer in1, lide_c_fifo_pointer in2, lide_c_fifo_pointer in3, lide_c_fifo_pointer out1,lide_c_fifo_pointer out2, lide_c_fifo_pointer out3) {
	int i;
	lide_c_RE_context_type *context = NULL;
	context = lide_c_util_malloc(sizeof(lide_c_RE_context_type));
    context->enable = (lide_c_actor_enable_function_type)lide_c_RE_enable;
    context->invoke = (lide_c_actor_invoke_function_type)lide_c_RE_invoke;
    context->in1 = in1;
	context->in2 = in2;
	context->in3 = in3;
    context->out1 = out1;
	context->out2 = out2;
	context->out3 = out3;
	context->mode = 1;
	/*Load table and Parameter*/
	context->Ws = Ws;
	/*Allocate memory for sorting*/
	context->d = (double*)malloc(sizeof(double)*15999986);
	/* initialize the array*/
	for (i=0; i<Ws; i++)   
        context->d[i] = 0.0; 
	return context;
}

bool lide_c_RE_enable(lide_c_RE_context_type *context) {
	bool result = false;
	//result =1;
	//return result;
	switch (context->mode) {
		case LIDE_C_RE_MODE_PROCESS:
			result = (lide_c_fifo_population(context->in1) >= 3) &&
					(lide_c_fifo_population(context->in2) >= 2) &&
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

void lide_c_RE_invoke(lide_c_RE_context_type *context) {
	int tmp,i,len;
	double *fp;
	int	*intp;
	double high,low,medium,REtime,dlen;
	double *d = NULL;
	/*Load data from three fifo*/
	lide_c_fifo_read(context->in1, &high);
	lide_c_fifo_read(context->in1, &low);
	lide_c_fifo_read(context->in1, &medium);
	lide_c_fifo_read(context->in1, &dlen);
	len = (int)dlen;
	lide_c_fifo_read(context->in2, &fp);
	context->vol = fp;
	lide_c_fifo_read(context->in2, &fp);
	context->TRT = fp;
	lide_c_fifo_read(context->in2, &fp);
	context->TRall = fp;
	lide_c_fifo_read(context->in3, &intp);
	context->state = intp;
	lide_c_fifo_read(context->in3, &intp);
	context->TR = intp;
	lide_c_fifo_read(context->in3, &intp);
	context->TRindex = intp;
	d = (double*)malloc(sizeof(double)*len);
	if(d == NULL)
		printf("RE malloc ERROR ");
	
	/*Get the rough estimation*/
	for (i = 0; i<len-1; i++){
		d[i] = context->TRall[i+1] - context->TRall[i];
		context->d[i] = d[i];
    /*
		if(d[i]<0.0)
			printf("HERE:%d\t%lf\t%lf\t",i,context->TRT[i+1],context->TRT[i]);
	*/
    }
	
	qsort(d, len-1, sizeof(d[0]), cmp);

	tmp = (int)((len-1)/4);
	REtime = d[tmp];
	
	
	/*Store data to three fifo*/
	lide_c_fifo_write(context->out1, &high);
	lide_c_fifo_write(context->out1, &low);
	lide_c_fifo_write(context->out1, &medium);
	lide_c_fifo_write(context->out1, &dlen);
	lide_c_fifo_write(context->out1, &REtime);
	lide_c_fifo_write(context->out2, &context->vol);	
	lide_c_fifo_write(context->out2, &context->TRT);
	lide_c_fifo_write(context->out2, &context->TRall);
	lide_c_fifo_write(context->out2, &context->d);
	lide_c_fifo_write(context->out3, &context->state);
	lide_c_fifo_write(context->out3, &context->TR);
	lide_c_fifo_write(context->out3, &context->TRindex);
	free(d);

	return;
	
}

void lide_c_RE_terminate(lide_c_RE_context_type *context){  
    free(context->d);  	
	free(context);
}
