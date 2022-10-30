#ifndef _lide_c_file_read_h
#define _lide_c_file_read_h

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

/* Actor modes */
#define LIDE_C_FILE_READ_MODE_ERROR        0
#define LIDE_C_FILE_READ_MODE_WRITE        1
#define LIDE_C_FILE_READ_MODE_INACTIVE     2 

struct _lide_c_file_read_context_type_struct;
typedef struct _lide_c_file_read_context_type_struct
        lide_c_file_read_context_type;

/* ----------------------------  Public functions --------------------- */

/*****************************************************************************
Construct function of the lide_c_file_read actor. Create a new
lide_c_file_read with the specified file pointer, and the specified output
FIFO pointer. Data format in the input file should be all integers.
*****************************************************************************/
lide_c_file_read_context_type *lide_c_file_read_new(FILE *file,
        lide_c_fifo_pointer out, int data_type, int read_rate);

/*****************************************************************************
Enable function of the lide_c_file_read actor.
*****************************************************************************/
bool lide_c_file_read_enable(lide_c_file_read_context_type *context);

/*****************************************************************************
Invoke function of the lide_c_file_read actor.
*****************************************************************************/
void lide_c_file_read_invoke(lide_c_file_read_context_type *context); 

/*****************************************************************************
Terminate function of the lide_c_file_read actor.
*****************************************************************************/
void lide_c_file_read_terminate(lide_c_file_read_context_type *context);

#endif
