#ifndef _lide_c_multirate_fir_h
#define _lide_c_multirate_fir_h

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
#include "lide_c_actor.h"
#include "lide_c_fifo.h"

/*******************************************************************************
phaseLength = ceil(tapslength / interpolation) 
length = phaseLength + decimation;
mostRecent is initialized to phaseLength
feedback is the float array for feedback, and initialize to zero.
mostRecent is the feedback index for the feedback array
*******************************************************************************/

/* Actor modes */
#define LIDE_C_MULTIRATE_FIR_MODE_PROCESS 0

struct _lide_c_multirate_fir_context_type_struct;
typedef struct _lide_c_multirate_fir_context_type_struct
        lide_c_multirate_fir_context_type;

lide_c_multirate_fir_context_type *lide_c_multirate_fir_new(
        lide_c_fifo_pointer in, lide_c_fifo_pointer out, int tapslength, 
        float *taps, int interpolation, int decimation, int decPhase);

bool lide_c_multirate_fir_enable(lide_c_multirate_fir_context_type *context);

void lide_c_multirate_fir_invoke(lide_c_multirate_fir_context_type *context);

void lide_c_multirate_fir_terminate(lide_c_multirate_fir_context_type *context);
#endif
