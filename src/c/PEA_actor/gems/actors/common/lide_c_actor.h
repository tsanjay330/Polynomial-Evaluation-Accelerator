#ifndef _lide_c_actor_h
#define _lide_c_actor_h

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
#include "lide_c_basic.h"

struct lide_c_actor_context_struct;

/*****************************************************************************
A pointer to a "lide_c_actor_invoke_function", which is a function that invokes
an actor with a given context.
*****************************************************************************/
typedef void (*lide_c_actor_invoke_function_type) 
        (struct lide_c_actor_context_struct *context);

/*****************************************************************************
A pointer to a "lide_c_actor_enable_function", which is a function that enables
an actor with a given context.
*****************************************************************************/
typedef bool (*lide_c_actor_enable_function_type)
        (struct lide_c_actor_context_struct *context);
/* Use of the dlcutil C Boolean datatype is disabled.
Instead, we use the stdbool library for C code, and the bool
datatype for C++ code. This comment and the commented out
type definition can be removed once code is stable
after migrating to the updated C/C++ Boolean usage formats. */

typedef struct lide_c_actor_context_struct {
#include "lide_c_actor_context_type_common.h"
} lide_c_actor_context_type;

#endif
