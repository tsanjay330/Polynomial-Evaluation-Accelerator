#ifndef _lide_c_RRE_h
#define _lide_c_RRE_h
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


/*******************************************************************************
DESCRIPTION:
This is a header file of the RRE (Refined Estimation) actor.
*******************************************************************************/


#include "lide_c_actor.h"
#include "lide_c_fifoext.h"

/* Actor modes */
#define LIDE_C_RRE_MODE_PROCESS  1
#define LIDE_C_RRE_MODE_HALT 0
#define LIDE_C_RRE_INPUT_THRESHOLD 5
#define LIDE_C_STATE_HIGH	1
#define LIDE_C_STATE_LOW	0
#define LIDE_C_STATE_MID	2
#define LIDE_C_TRAN			1
#define LIDE_C_NON_TRAN		0

/*******************************************************************************
TYPE DEFINITIONS
*******************************************************************************/
/* Structure and pointer types associated with add objects. */
struct _lide_c_RRE_context_struct;
typedef struct _lide_c_RRE_context_struct lide_c_RRE_context_type;



/*******************************************************************************
Create a new actor with two input fifos and three output fifos. 
*******************************************************************************/
lide_c_RRE_context_type *lide_c_RRE_new(int Ws, lide_c_fifo_pointer in1, lide_c_fifo_pointer in2, lide_c_fifo_pointer in3, lide_c_fifo_pointer out1,lide_c_fifo_pointer out2, lide_c_fifo_pointer out3);
/*******************************************************************************
Enable function checks whether there is enough data for firing the actor
*******************************************************************************/
bool lide_c_RRE_enable(lide_c_RRE_context_type *context);
/*******************************************************************************
Invoke function will Refine the rough estimation.
*******************************************************************************/
void lide_c_RRE_invoke(lide_c_RRE_context_type *context); 
/*******************************************************************************
Terminate function will free the actor including all the memory allocated.
*******************************************************************************/
void lide_c_RRE_terminate(lide_c_RRE_context_type *context);

#endif

