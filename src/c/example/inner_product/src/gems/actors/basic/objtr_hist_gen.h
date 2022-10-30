#ifndef _objtr_hist_gen_h
#define _objtr_hist_gen_h

/* Header for objtr_hist_gen */





#include "lide_c_actor.h"
#include "lide_c_fifo.h"

/* Actor modes */
#define OBJTR_HIST_GEN_MODE_STORE_NUMBINS     1
#define OBJTR_HIST_GEN_MODE_STORE_IMAGE_SIZE  2
#define OBJTR_HIST_GEN_MODE_STORE_BINS        3
#define OBJTR_HIST_GEN_MODE_PROCESS           4 


/*******************************************************************************
TYPE DEFINITIONS
*******************************************************************************/

/* Structure and pointer types associated with hist_gen objects. */
struct _objtr_hist_gen_context_struct;
typedef struct _objtr_hist_gen_context_struct
        objtr_hist_gen_context_type;



/*******************************************************************************
INTERFACE FUNCTIONS
*******************************************************************************/

/*****************************************************************************
Construct function of the objtr_hist_gen actor
*****************************************************************************/
objtr_hist_gen_context_type *objtr_hist_gen_new(
       lide_c_fifo_pointer num_bins, lide_c_fifo_pointer bins, lide_c_fifo_pointer image_size, lide_c_fifo_pointer image, lide_c_fifo_pointer out);

/*****************************************************************************
Enable function of the objtr hist gen actor.
*****************************************************************************/
bool objtr_hist_gen_enable(objtr_hist_gen_context_type *context);

/*****************************************************************************
Invoke function of the objtr_hist_gen actor.
*****************************************************************************/
void objtr_hist_gen_invoke(objtr_hist_gen_context_type *context);

/*****************************************************************************
Terminate function of the objtr_hist_gen actor.
*****************************************************************************/
void objtr_hist_gen_terminate(objtr_hist_gen_context_type *context);

#endif




