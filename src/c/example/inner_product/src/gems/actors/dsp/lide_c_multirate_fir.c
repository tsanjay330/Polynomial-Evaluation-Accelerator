/*******************************************************************************
@ddblock_begin copyright

Copyright (c) 1997-2018
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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "lide_c_multirate_fir.h"
#include "lide_c_util.h"

struct _lide_c_multirate_fir_context_type_struct {
#include "lide_c_actor_context_type_common.h"

    /* Persistent local variables */
    int tapsLength;
    float *taps;
    float *feedback;  
    int mostRecent;
    int interpolation;
    int decimation;
    int decPhase;

    /* input ports */
    lide_c_fifo_pointer in;

    /* output port */
    lide_c_fifo_pointer out;
};

lide_c_multirate_fir_context_type *lide_c_multirate_fir_new(
        lide_c_fifo_pointer in, lide_c_fifo_pointer out, int tapslength, 
        float *taps, int interpolation, int decimation, int decPhase) {

    lide_c_multirate_fir_context_type *context = NULL;
    context = lide_c_util_malloc(sizeof(lide_c_multirate_fir_context_type));
    context->enable =
            (lide_c_actor_enable_function_type)lide_c_multirate_fir_enable;

    context->invoke =
            (lide_c_actor_invoke_function_type)lide_c_multirate_fir_invoke;

    context->mode = LIDE_C_MULTIRATE_FIR_MODE_PROCESS;
    context->tapsLength = tapslength;
    context->taps = taps;
    context->feedback = lide_c_util_malloc(sizeof(float)*context->tapsLength);
    {
      int i;
      for (i = 0; i < context->tapsLength; i++){
	        context->feedback[i] = 0;
      }
    }
    context->out = out;
    context->in = in;
    context->mostRecent = 0;
    context->interpolation = interpolation;
    context->decimation = decimation;
    context->decPhase = decPhase;
    return context;
}

bool lide_c_multirate_fir_enable(
        lide_c_multirate_fir_context_type *context) {

    bool result = false;

    switch (context->mode) {
        case LIDE_C_MULTIRATE_FIR_MODE_PROCESS:
            result = (lide_c_fifo_population(context->in) >= 1) &
                ((lide_c_fifo_population(context->out) <
                lide_c_fifo_capacity(context->out)));
            break;
        default:
            result = false;
            break;
        }
        return result;
}

void lide_c_multirate_fir_invoke(lide_c_multirate_fir_context_type *context) {
    
    int phaseLength = (int)(context->tapsLength / context->interpolation);
    int length = phaseLength + context->decimation;
    int phase = context->decimation - context->decPhase - 1;
    int i = 0;
    int j = 0;

    if ((context->tapsLength % context->interpolation) != 0) {
        phaseLength++;
    }

    switch (context->mode) {
        case LIDE_C_MULTIRATE_FIR_MODE_PROCESS:
            if (lide_c_fifo_population(context->in) < (context->decimation)) {
                context->mode = LIDE_C_MULTIRATE_FIR_MODE_PROCESS;
                return;
            }       
 
            for (j = 0; j<context->decimation; j++) {
                if (--(context->mostRecent) < 0) {
                    (context->mostRecent) = length - 1;
                }

                lide_c_fifo_read(context->in,
                    &(context->feedback[(context->mostRecent)]));
            }    

            for (j = 1; j <= context->decimation; j++) {
                while (phase < context->interpolation) {
                    float out = 0;
                    for (i = 0; i < phaseLength; i++) {
                        int tapsIdx = i * context->interpolation + phase;
                        int dataIdx = ((context->mostRecent) +
                            context->decimation - j + i) % (length);

                        if (tapsIdx < context->tapsLength) {
                            out += context->taps[tapsIdx] *
                                context->feedback[dataIdx];
                        }
                    }
                    lide_c_fifo_write(context->out, &out); 
                    phase += context->decimation;
                }
                phase -=context->interpolation;
            }
            context->mode = LIDE_C_MULTIRATE_FIR_MODE_PROCESS;
            break;
    }
}


void lide_c_multirate_fir_terminate(
                lide_c_multirate_fir_context_type *context){
    free(context->feedback);
    free(context);
}

