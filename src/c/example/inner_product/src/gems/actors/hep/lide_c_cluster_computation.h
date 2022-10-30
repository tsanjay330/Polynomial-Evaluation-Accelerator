#ifndef _lide_c_cluster_computation_h
#define _lide_c_cluster_computation_h

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
This actor computes the tower, egamma, and fine grain with the inputs of ecal, 
hcal, and fine grain.
*******************************************************************************/

/* Actor modes */
#define LIDE_C_CLUSTER_COMPUTATION_MODE_ERROR            0
#define LIDE_C_CLUSTER_COMPUTATION_MODE_PROCESS          1

/* Constant */
#define ELEMENT_COUNT	4

/*******************************************************************************
TYPE DEFINITIONS
*******************************************************************************/

/* Structure and pointer types associated with cluster computation objects. */
struct _lide_c_cluster_computation_context_struct;
typedef struct _lide_c_cluster_computation_context_struct
        lide_c_cluster_computation_context_type;

/*******************************************************************************
INTERFACE FUNCTIONS
*******************************************************************************/

/*****************************************************************************
Construct function of the lide_c_cluster_computation actor.
*****************************************************************************/
lide_c_cluster_computation_context_type *lide_c_cluster_computation_new(
        lide_c_fifo_pointer *ffp_in_ecal, lide_c_fifo_pointer *ffp_in_hcal, 
        lide_c_fifo_pointer *ffp_in_fg, lide_c_fifo_pointer *ffp_out_tower, 
        lide_c_fifo_pointer ffp_out_egamma, lide_c_fifo_pointer ffp_out_fg);

/*****************************************************************************
Enable function of the lide_c_cluster_computation actor.
*****************************************************************************/
bool lide_c_cluster_computation_enable(
        lide_c_cluster_computation_context_type *context);

/*****************************************************************************
Invoke function of the lide_c_cluster_computation actor.
*****************************************************************************/
void lide_c_cluster_computation_invoke(
        lide_c_cluster_computation_context_type *context);

/*****************************************************************************
Terminate function of the lide_c_cluster_computation actor.
*****************************************************************************/
void lide_c_cluster_computation_terminate(
        lide_c_cluster_computation_context_type *context);

#endif
