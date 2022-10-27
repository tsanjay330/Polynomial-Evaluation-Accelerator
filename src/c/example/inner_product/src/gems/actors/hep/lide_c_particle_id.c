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
#include <stdio.h>
#include <stdlib.h>

#include "lide_c_util.h"
#include "lide_c_particle_id.h"

/*******************************************************************************
CLUSTER COMPUTATION STRUCTURE DEFINITION
*******************************************************************************/
struct _lide_c_particle_id_context_struct {
#include "lide_c_actor_context_type_common.h"
    /* input ports */
    lide_c_fifo_pointer ffp_input_central;
    lide_c_fifo_pointer ffp_input_fine_grain;
    lide_c_fifo_pointer ffp_input_electron;
    lide_c_fifo_pointer ffp_input_electron_iso;
    lide_c_fifo_pointer ffp_input_tau_iso;

    /* output ports */
    lide_c_fifo_pointer ffp_output_valid;
    lide_c_fifo_pointer ffp_output_type;
    lide_c_fifo_pointer ffp_output_iso;
};

/*******************************************************************************
IMPLEMENTATIONS OF INTERFACE FUNCTIONS.
*******************************************************************************/

lide_c_particle_id_context_type *lide_c_particle_id_new(
        lide_c_fifo_pointer ffp_input_central, 
        lide_c_fifo_pointer ffp_input_fine_grain, 
        lide_c_fifo_pointer ffp_input_electron, 
        lide_c_fifo_pointer ffp_input_electron_iso, 
        lide_c_fifo_pointer ffp_input_tau_iso, 
        lide_c_fifo_pointer ffp_output_valid, 
        lide_c_fifo_pointer ffp_output_type, 
        lide_c_fifo_pointer ffp_output_iso) {

    lide_c_particle_id_context_type *context = NULL;

    context = lide_c_util_malloc(sizeof(lide_c_particle_id_context_type));
    context->mode = LIDE_C_PARTICLE_ID_MODE_PROCESS;
    context->enable =
            (lide_c_actor_enable_function_type)lide_c_particle_id_enable;
    context->invoke =
            (lide_c_actor_invoke_function_type)lide_c_particle_id_invoke;

    context->ffp_input_central = ffp_input_central;
    context->ffp_input_fine_grain = ffp_input_fine_grain;
    context->ffp_input_electron = ffp_input_electron;
    context->ffp_input_electron_iso = ffp_input_electron_iso;
    context->ffp_input_tau_iso = ffp_input_tau_iso;

    context->ffp_output_valid = ffp_output_valid;
    context->ffp_output_type = ffp_output_type;
    context->ffp_output_iso = ffp_output_iso;

    return context;
}

bool lide_c_particle_id_enable(lide_c_particle_id_context_type *context) {
    bool result = false;

    switch (context->mode) {
    case LIDE_C_PARTICLE_ID_MODE_PROCESS:
        result = (lide_c_fifo_population(context->ffp_input_central)
                < lide_c_fifo_capacity(context->ffp_input_central));
        result = result && 
                (lide_c_fifo_population(context->ffp_input_fine_grain)
                < lide_c_fifo_capacity(context->ffp_input_fine_grain));
        result = result && 
                (lide_c_fifo_population(context->ffp_input_electron)
                < lide_c_fifo_capacity(context->ffp_input_electron));
        result = result && 
                (lide_c_fifo_population(context->ffp_input_electron_iso)
                < lide_c_fifo_capacity(context->ffp_input_electron_iso));
        result = result && 
                (lide_c_fifo_population(context->ffp_input_tau_iso)
                < lide_c_fifo_capacity(context->ffp_input_tau_iso));
        result = result && 
                (lide_c_fifo_population(context->ffp_input_central)  
                >= 1);
        result = result && 
                (lide_c_fifo_population(context->ffp_input_fine_grain)  
                >= 1);
        result = result && 
                (lide_c_fifo_population(context->ffp_input_electron)  
                >= 1);
        result = result && 
                (lide_c_fifo_population(context->ffp_input_electron_iso)  
                >= 1);
        result = result && 
                (lide_c_fifo_population(context->ffp_input_tau_iso)  
                >= 1);
        break;
    default:
        result = false;
        break;
    }
    return result;
}

void lide_c_particle_id_invoke(lide_c_particle_id_context_type *context) {
    int centralBit = 0;
    int fineGrain = 0;
    int electron = 0;
    int electronIso = 0;
    int tauIso = 0;
    int valid = 0;
    int type = 0;
    int iso = 0;

    switch (context->mode) {
    case LIDE_C_PARTICLE_ID_MODE_PROCESS:
        /* read data from fifo */
        lide_c_fifo_read(context->ffp_input_central, &centralBit);
        lide_c_fifo_read(context->ffp_input_fine_grain, &fineGrain);
        lide_c_fifo_read(context->ffp_input_electron, &electron);
        lide_c_fifo_read(context->ffp_input_electron_iso, &electronIso);
        lide_c_fifo_read(context->ffp_input_tau_iso, &tauIso);

        /* data processing */
        valid = centralBit;
        type = electron && (!fineGrain);
        iso = (type && electronIso) || ((!type) && tauIso);

        /* write data to fifo */
        lide_c_fifo_write(context->ffp_output_valid, &valid);
        lide_c_fifo_write(context->ffp_output_type, &type);
        lide_c_fifo_write(context->ffp_output_iso, &iso);

        context->mode = LIDE_C_PARTICLE_ID_MODE_PROCESS;
        break;
    default:
        context->mode = LIDE_C_PARTICLE_ID_MODE_ERROR;
        break;
    }
    return;
}

void lide_c_particle_id_terminate(lide_c_particle_id_context_type *context) {
    free(context);
}
