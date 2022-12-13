#ifndef _lide_c_pea_h
#define _lide_c_pea_h

#include "lide_c_actor.h"
#include "lide_c_fifo.h"

/*******************************************************************************
This actor reads a command from input, processes the commands and performs 
polynomial computations, and outputs results of these computations.
*******************************************************************************/

/* Actor modes */
#define LIDE_C_PEA_MODE_GET_COMMAND   0
#define LIDE_C_PEA_MODE_STP   1
#define LIDE_C_PEA_MODE_EVP   2
#define LIDE_C_PEA_MODE_EVB   3
#define LIDE_C_PEA_MODE_RST   4
#define LIDE_C_PEA_MODE_OUTPUT    5

/*******************************************************************************
TYPE DEFINITIONS
*******************************************************************************/

/* Structure and pointer types associated with PEA objects. */
struct _lide_c_pea_context_struct;
typedef struct _lide_c_pea_context_struct
        lide_c_pea_context_type;

/*******************************************************************************
INTERFACE FUNCTIONS
*******************************************************************************/

/*****************************************************************************
Construct function of the lide_c_pea actor.
*****************************************************************************/
lide_c_pea_context_type *lide_c_pea_new(
        lide_c_fifo_pointer ffp_input_command, lide_c_fifo_pointer ffp_input_data,
        lide_c_fifo_pointer ffp_output_result, lide_c_fifo_pointer ffp_output_status);

/*****************************************************************************
Enable function of the lide_c_pea actor.
*****************************************************************************/
bool lide_c_pea_enable(lide_c_pea_context_type *context);

/*****************************************************************************
Invoke function of the lide_c_pea actor.
*****************************************************************************/
void lide_c_pea_invoke(lide_c_pea_context_type *context);

/*****************************************************************************
Terminate function of the lide_c_pea actor.
*****************************************************************************/
void lide_c_pea_terminate(lide_c_pea_context_type *context);

#endif

