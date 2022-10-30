#ifndef _lide_c_cluster_weight_h
#define _lide_c_cluster_weight_h

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
This actor computes the final weight with the input of central towers.
*******************************************************************************/

/* Actor modes */
#define LIDE_C_CLUSTER_WEIGHT_MODE_ERROR        0
#define LIDE_C_CLUSTER_WEIGHT_MODE_PROCESS      1

/* Structure and pointer types associated with cluster computation objects. */
struct _lide_c_cluster_weight_context_struct;
typedef struct _lide_c_cluster_weight_context_struct
        lide_c_cluster_weight_context_type;

/*******************************************************************************
INTERFACE FUNCTIONS
*******************************************************************************/

/*****************************************************************************
Construct function of the lide_c_cluster_weight actor.
*****************************************************************************/
lide_c_cluster_weight_context_type *lide_c_cluster_weight_new(
        lide_c_fifo_pointer ffp_input_centralTower0, 
        lide_c_fifo_pointer ffp_input_centralTower1, 
        lide_c_fifo_pointer ffp_input_centralTower2, 
        lide_c_fifo_pointer ffp_input_centralTower3, 
        lide_c_fifo_pointer ffp_input_etotal, 
        lide_c_fifo_pointer ffp_output_finalWeight);

/*****************************************************************************
Enable function of the lide_c_cluster_weight actor.
*****************************************************************************/
bool lide_c_cluster_weight_enable(
        lide_c_cluster_weight_context_type *context);

/*****************************************************************************
Invoke function of the lide_c_cluster_weight actor.
*****************************************************************************/
void lide_c_cluster_weight_invoke(lide_c_cluster_weight_context_type *context);

/*****************************************************************************
Terminate function of the lide_c_cluster_weight actor.
*****************************************************************************/
void lide_c_cluster_weight_terminate(
        lide_c_cluster_weight_context_type *context);

#endif
