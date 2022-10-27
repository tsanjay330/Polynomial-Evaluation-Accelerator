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
#include "lide_c_CRE.h"
#include "lide_c_util.h"

struct _lide_c_CRE_context_struct {
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
	int Num;
	int rpro;
	int TRlength;
	double *vol;
	int *state;
	int *TR;
	int *TRindex;
	int Pindex;
	int Pmid;
	double *TRT,*TRall;
	double *TRT_all;
	double *d;
	double *phase_all;
	double *phase;
	double *phase_correct;
	double *phase_error;
};

lide_c_CRE_context_type *lide_c_CRE_new(int Ws, int k, lide_c_fifo_pointer in1, lide_c_fifo_pointer in2, lide_c_fifo_pointer in3, lide_c_fifo_pointer out1,lide_c_fifo_pointer out2, lide_c_fifo_pointer out3) {
	int i;
	lide_c_CRE_context_type *context = NULL;
	context = lide_c_util_malloc(sizeof(lide_c_CRE_context_type));
    context->enable = (lide_c_actor_enable_function_type)lide_c_CRE_enable;
    context->invoke = (lide_c_actor_invoke_function_type)lide_c_CRE_invoke;
    context->in1 = in1;
	context->in2 = in2;
	context->in3 = in3;
    context->out1 = out1;
	context->out2 = out2;
	context->out3 = out3;
	context->mode = 1;
	/*Load table and Parameter*/
	context->Ws = Ws;
	context->k = k;
	context->Num = 500;
	context->rpro = 200;
	context->Pmid = context->Num/2;
	context->Pindex = 0;
	context->phase = (double *)malloc(sizeof(double)*Ws);
	context->phase_correct = (double *)malloc(sizeof(double)*context->Num);
	context->phase_error = (double *)malloc(sizeof(double)*context->Num);
	context->phase_all = (double *)malloc(sizeof(double)*context->Num);
	context->TRT_all = (double *)malloc(sizeof(double)*context->Num);
	/* Initialize array*/
	for (i=0; i<Ws; i++){
        context->phase[i] = 0.0; 
		
	}
	for (i=0; i<context->Num; i++){
		context->phase_all[i] = 0.0; 
		context->TRT_all[i] = 0.0; 
		context->phase_correct[i] = 0.0; 
		context->phase_error[i] = 0.0; 
	}
	return context;
}

bool lide_c_CRE_enable(lide_c_CRE_context_type *context) {
	bool result = false;
	switch (context->mode) {
		case LIDE_C_CRE_MODE_PROCESS:
			result = (lide_c_fifo_population(context->in1) >= 5) &&
					(lide_c_fifo_population(context->in2) >= 3) &&
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

int lide_c_CRE_invoke(lide_c_CRE_context_type *context) {

	int i,N,ptmp = 0;
	int index = 0;
	int *rp;
	int *p;
	double *fp;
	int	*intp;
	double *phase_copy;
	double *TRT_copy;
	double high,low,medium,REtime,RREtime,dlen;
	double a,b,xy,x,y,x2,xavg,yavg,sum,CREtime;
	double fN;
	const int pindex_copy = context->Pindex;
	
	rp = (int *)malloc(sizeof(int)*context->Num);
	p = (int *)malloc(sizeof(int)*context->Num);
	if (rp == NULL || p == NULL){
		fprintf(stderr, "allocate in CRE failed\n");
		exit(-1);
	}
	/*Load data from three fifo*/
	lide_c_fifo_read(context->in1, &high);
	lide_c_fifo_read(context->in1, &low);
	lide_c_fifo_read(context->in1, &medium);
	lide_c_fifo_read(context->in1, &dlen);
	lide_c_fifo_read(context->in1, &REtime);
	lide_c_fifo_read(context->in1, &RREtime);
	lide_c_fifo_read(context->in2, &fp);
	context->vol = fp;
	lide_c_fifo_read(context->in2, &fp);
	context->TRT = fp;
	lide_c_fifo_read(context->in2, &fp);
	context->TRall = fp;
	lide_c_fifo_read(context->in2, &fp);
	context->d = fp;
	lide_c_fifo_read(context->in3, &intp);
	context->state = intp;
	lide_c_fifo_read(context->in3, &intp);
	context->TR = intp;
	lide_c_fifo_read(context->in3, &intp);
	context->TRindex = intp;
	
	/*Revise TRT*/
	ptmp = context->Ws*context->k;
	for(i = 0; i<context->TRindex[0]; i++){
		context->TRT[i] = context->TRT[i] + ptmp;
	}
	/* Compute the phase at current transition*/

	for(i = 0; i<context->TRindex[0]; i++){
		context->phase[i] = context->TRT[i]-((int)(context->TRT[i]/RREtime+0.5))*RREtime;
	}
	
	/*Update context->phase_all and context->TRT_all*/
	if (context->Pindex + context->TRindex[0] <= context->Num){
		phase_copy = context->phase_all + pindex_copy;
		TRT_copy = context->TRT_all + pindex_copy;
		for(i = 0; i< context->TRindex[0]; i++){
			phase_copy[i] = context->phase[i];
			TRT_copy[i] = context->TRT[i];
		}

		context->Pindex = context->Pindex+context->TRindex[0];	
	}else if (context->Pindex < context->Num){
		for(i = context->Pindex; i< context->Num; i++){
			context->phase_all[i] = context->phase[i-context->Pindex];
			context->TRT_all[i] = context->TRT[i-context->Pindex];
		}
		index = context->Pindex + context->TRindex[0]-context->Num;
		context->Pindex = context->Num;
		
		
	}else{
		index = context->TRindex[0];
	}
	if (index > 0){
		if (context->TRindex[0] >= context->Pmid){
			ptmp = context->TRindex[0]-context->Pmid;
			for(i = 0; i < context->Pmid;i++){
				context->phase_all[i+context->Pmid] = context->phase[i+ptmp];
				context->TRT_all[i+context->Pmid] = context->TRT[i+ptmp];
			}
			index = 1;
		}else{
			ptmp = context->Pmid - (context->TRindex[0]);
			/*Move Array*/
			for(i = 0; i<ptmp; i++){
				context->phase_all[i+context->Pmid] = context->phase[i+context->Pmid+context->TRindex[0]];
				context->TRT_all[i+context->Pmid] = context->TRT[i+context->Pmid+context->TRindex[0]];
			}
			/*Update new data*/
			for(i = 0; i < context->TRindex[0]; i++){
				context->phase_all[i+ptmp] = context->phase[i];
				context->TRT_all[i+ptmp] = context->TRT[i];
			}
			index = 2;
		}
	}

	N = context->Pindex;
	/*X and Y average; x sum and y sum*/
	/*
		X: Transition Time
		Y: Phase at transition time
	*/
	sum = 0.0;
	for (i = 0; i<N; i++){
		sum += context->phase_all[i];
	}
	y = sum;
	yavg = sum/N;

	sum = 0.0;

	for (i = 0; i<N; i++){
		sum += context->TRT_all[i];
	}
	x = sum;
	xavg = sum/N;
	
	/*X square*/
	sum = 0.0;
	for (i = 0; i<N; i++){
		sum += context->TRT_all[i] * context->TRT_all[i];
	}
	x2 = sum;
	/*xy*/
	sum = 0.0;
	for(i = 0; i<N; i++){
		sum += context->TRT_all[i] * context->phase_all[i];
	}
	xy = sum;
	/*Compute a and b*/
	b = (N*xy-x*y)/(N*x2-x*x);
	a = yavg-xavg*b;

	/*Correct phase*/
	for(i = 0; i<N; i++){
		context->phase_correct[i] = a + b*context->TRT_all[i];
	}
	for(i = 0; i<N; i++){
		context->phase_error[i] = context->phase_correct[i] - context->phase[i];		
	}
	CREtime = RREtime+RREtime*b;

	fN = (double)N;
	context->k = context->k + 1;
	lide_c_fifo_write(context->out1, &high);
	lide_c_fifo_write(context->out1, &low);
	lide_c_fifo_write(context->out1, &medium);
	lide_c_fifo_write(context->out1, &dlen);
	lide_c_fifo_write(context->out1, &REtime);
	lide_c_fifo_write(context->out1, &RREtime);
	lide_c_fifo_write(context->out1, &CREtime);
	lide_c_fifo_write(context->out1, &a);
	lide_c_fifo_write(context->out1, &b);
	lide_c_fifo_write(context->out1, &fN);
	lide_c_fifo_write(context->out1, &context->k);
	lide_c_fifo_write(context->out2, &context->vol);	
	lide_c_fifo_write(context->out2, &context->TRT);
	lide_c_fifo_write(context->out2, &context->TRall);
	lide_c_fifo_write(context->out2, &context->d);
	lide_c_fifo_write(context->out2, &context->phase_all);
	lide_c_fifo_write(context->out2, &context->TRT_all);
	lide_c_fifo_write(context->out2, &context->phase);
	lide_c_fifo_write(context->out2, &context->phase_correct);
	lide_c_fifo_write(context->out2, &context->phase_error);
	lide_c_fifo_write(context->out3, &context->state);
	lide_c_fifo_write(context->out3, &context->TR);
	lide_c_fifo_write(context->out3, &context->TRindex);
	free(rp);
	free(p);
	return 1;
	
}

void lide_c_CRE_terminate(lide_c_CRE_context_type *context){  	
	free(context->phase);
	free(context->phase_correct);
	free(context->phase_error);
	free(context->phase_all);
	free(context->TRT_all);
	free(context);
}
