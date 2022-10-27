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
#include "lide_c_jet_recon.h"

/*******************************************************************************
JET RECONSTRUCTION STRUCTURE DEFINITION
*******************************************************************************/
struct _lide_c_jet_recon_context_struct {
#include "lide_c_actor_context_type_common.h"
    /* input ports */
    lide_c_fifo_pointer ffp_input_grid;

    /* output ports */
    lide_c_fifo_pointer ffp_output_jet;
    lide_c_fifo_pointer ffp_output_es;
};

/*******************************************************************************
IMPLEMENTATIONS OF INTERFACE FUNCTIONS.
*******************************************************************************/

lide_c_jet_recon_context_type *lide_c_jet_recon_new(
        lide_c_fifo_pointer ffp_input_grid, lide_c_fifo_pointer ffp_output_jet, 
        lide_c_fifo_pointer ffp_output_es) {

    lide_c_jet_recon_context_type *context = NULL;

    context = lide_c_util_malloc(sizeof(lide_c_jet_recon_context_type));
    context->mode = LIDE_C_JET_RECON_MODE_PROCESS;
    context->enable =
            (lide_c_actor_enable_function_type)lide_c_jet_recon_enable;
    context->invoke =
            (lide_c_actor_invoke_function_type)lide_c_jet_recon_invoke;

    context->ffp_input_grid = ffp_input_grid;

    context->ffp_output_jet = ffp_output_jet;
    context->ffp_output_es = ffp_output_es;

    return context;
}

bool lide_c_jet_recon_enable(lide_c_jet_recon_context_type *context) {
    bool result = false;

    switch (context->mode) {
    case LIDE_C_JET_RECON_MODE_PROCESS:
        result = (lide_c_fifo_population(context->ffp_input_grid)
                < lide_c_fifo_capacity(context->ffp_input_grid));
        result = result && 
                (lide_c_fifo_population(context->ffp_input_grid)  >= 64);
        break;
    default:
        result = false;
        break;
    }
    return result;
}

void lide_c_jet_recon_invoke(lide_c_jet_recon_context_type *context) {
    int TotalEnergy = 0;
    int EnergyUp = 0;
    int EnergyDown = 0;
    int EnergyLeft = 0;
    int EnergyRight = 0;
    int vEnergy = 0;
    int hEnergy = 0;
    double check = 12.5;
    double vpEnergy = 0;
    double hpEnergy = 0;
    bool isJet = false;
    int gridLength = 8;
    int grid[8][8] = {{0}};
    int row = 0;
    int col = 0;

    switch (context->mode) {
    case LIDE_C_JET_RECON_MODE_PROCESS:
        for (row = 0; row < gridLength; row++) {            
            for (col = 0; col < gridLength; col++) {
                /* read data from fifo */
                lide_c_fifo_read(context->ffp_input_grid, &grid[row][col]);

                /* data processing */
                TotalEnergy = TotalEnergy + grid[row][col];

                if ((row <= (gridLength)/2) && (col < gridLength)) {
                    EnergyUp = EnergyUp + grid[row][col];
                }
                if ((row > (gridLength)/2) && (col < gridLength)) {
                    EnergyDown = EnergyDown + grid[row][col];
                }
                if ((row < gridLength) && (col <= (gridLength)/2)) {
                    EnergyLeft = EnergyLeft + grid[row][col];
                }
                if ((row < gridLength) && (col >= (gridLength)/2)) {
                    EnergyRight = EnergyRight + grid[row][col];
                }
            }
        }
        vEnergy = EnergyUp >= EnergyDown?(EnergyUp - EnergyDown):(EnergyDown - EnergyUp);
        hEnergy = EnergyRight >= EnergyLeft?(EnergyRight - EnergyLeft):(EnergyLeft - EnergyRight);
        vpEnergy = (vEnergy/TotalEnergy) * 100;
        hpEnergy = (hEnergy/TotalEnergy) * 100;
       
        if ((vpEnergy < check) && (hpEnergy < check)) {
            isJet = true;
        } else {
            TotalEnergy = 0;    
        }        

        /* write data to fifo */
        lide_c_fifo_write(context->ffp_output_jet, &isJet);
        lide_c_fifo_write(context->ffp_output_es, &TotalEnergy);

        context->mode = LIDE_C_JET_RECON_MODE_PROCESS;
        break;
    default:
        context->mode = LIDE_C_JET_RECON_MODE_ERROR;
        break;
    }
    return;
}

void lide_c_jet_recon_terminate(lide_c_jet_recon_context_type *context) {
    free(context);
}
