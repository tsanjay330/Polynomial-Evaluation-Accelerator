#ifndef _lide_c_pea_actor_h
#define _lide_c_pea_actor_h

#include "lide_c_actor.h"
#include "lide_c_fifo.h"

/*******************************************************************************
This actor reads a command from input, processes the commands and performs 
polynomial computations, and outputs results of these computations.
*******************************************************************************/

/* Actor modes */
#define LIDE_C_PEA_ACTOR_MODE_GET_COMMAND   0
#define LIDE_C_PEA_ACTOR_MODE_PROCESS_COMMAND   1
#define LIDE_C_PEA_ACTOR_MODE_OUTPUT    2

/*******************************************************************************
TYPE DEFINITIONS
*******************************************************************************/

/* Structure and pointer types associated with PEA actor objects. */
struct _lide_c_pea_actor_context_struct;
typedef struct _lide_c_pea_actor_context_struct
        lide_c_pea_actor_context_type;

/*******************************************************************************
INTERFACE FUNCTIONS
*******************************************************************************/

/*****************************************************************************
Construct function of the lide_c_pea_actor actor.
*****************************************************************************/
lide_c_hist_gen_context_type *lide_c_hist_gen_new(
        lide_c_fifo_pointer ffp_input_bins, lide_c_fifo_pointer ffp_input_head,
        lide_c_fifo_pointer ffp_input_img, int num_bins, int w, int h,
        int stride, int height, lide_c_fifo_pointer ffp_output_hist);

/*****************************************************************************
Enable function of the lide_c_hist_gen actor.
*****************************************************************************/
bool lide_c_hist_gen_enable(lide_c_hist_gen_context_type *context);

/*****************************************************************************
Invoke function of the lide_c_hist_gen actor.
*****************************************************************************/
void lide_c_hist_gen_invoke(lide_c_hist_gen_context_type *context);

/*****************************************************************************
Terminate function of the lide_c_hist_gen actor.
*****************************************************************************/
void lide_c_hist_gen_terminate(lide_c_hist_gen_context_type *context);

#endif

