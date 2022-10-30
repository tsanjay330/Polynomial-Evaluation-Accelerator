#ifndef _lide_c_table_lookup_h
#define _lide_c_table_lookup_h

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
/*******************************************************************************
A table lookup actor. Upon instantiation the actor reads integer
values from a specified text file into an internal table T (integer array).
When the actor fires, it consumes one integer value x, and produces 
the value T[x], which is the xth element of the array. Indexing
of T is assumed to start at 0, we must have 0 <= x < N - 1, where N
is the size of the table. 
*******************************************************************************/
#include "lide_c_actor.h"
#include "lide_c_fifo.h"

/* Actor modes */
#define LIDE_C_TABLE_LOOKUP_MODE_ERROR   0
#define LIDE_C_TABLE_LOOKUP_MODE_READ    1

/*******************************************************************************
TYPE DEFINITIONS
*******************************************************************************/

/* Structure and pointer types associated with table lookup objects. */
struct _lide_c_table_lookup_context_struct;
typedef struct _lide_c_table_lookup_context_struct
        lide_c_table_lookup_context_type;

/*******************************************************************************
INTERFACE FUNCTIONS
*******************************************************************************/

/*******************************************************************************
The given file must be opened for reading before entry to the constructor.
The file must contain at least "size" integers, separated only by white
space. 
*******************************************************************************/
lide_c_table_lookup_context_type *lide_c_table_lookup_new(FILE *file, int size,
        lide_c_fifo_pointer in, lide_c_fifo_pointer out);

bool lide_c_table_lookup_enable(lide_c_table_lookup_context_type *context);

void lide_c_table_lookup_invoke(lide_c_table_lookup_context_type *context); 

void lide_c_table_lookup_terminate(lide_c_table_lookup_context_type *context);

#endif
