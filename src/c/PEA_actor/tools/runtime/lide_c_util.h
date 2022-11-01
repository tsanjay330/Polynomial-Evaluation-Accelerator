#ifndef _lide_c_util_h
#define _lide_c_util_h

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

#include "lide_c_basic.h"
#include "lide_c_actor.h"

/* Open a file. Exit with an error message if the file cannot
   be opened with the specified mode ("r", "w", etc. as in
   coventional fopen). Otherwise return the file pointer of
   the opened file.
*/
FILE *lide_c_util_fopen(const char *fname, const char *mode);

/* Wrapper for malloc (memory allocation) that prints an error message exits 
if the requested amount of memory is not available.
*/
void *lide_c_util_malloc(size_t size);

/*******************************************************************************
This is a simple scheduler that keeps executing actors in a graph
until a complete traversal of the graph (all of the actors) completes
without any progress (i.e., with all actors remaining at the same step
in which they started, without any progress to subsequent steps).
The "actors" argument gives an array of the actors that makes up
the graph; the "actors_count" argument gives the number of actors;
and the "descriptors" argument is an array of strings such that the
ith string gives a diagnostic descriptor for the ith actor.
*******************************************************************************/

/*******************************************************************************
This is an CFDF canonical scheduler.
*******************************************************************************/
void lide_c_util_simple_scheduler(lide_c_actor_context_type *actors[], 
        int actor_count, char *descriptors[]);

/*******************************************************************************
If the given actor is enabled, then invoke it once, and return TRUE.
Otherwise, return FALSE.
*******************************************************************************/
bool lide_c_util_guarded_execution(lide_c_actor_context_type *context,
        char *descriptor);
 
/*******************************************************************************
Set mode for LIDE-C actors.
PLEASE BE VERY CAREFUL FOR USING THIS METHOD. IT MAY HAVE NEGATIVE AFFECT ON 
ACTOR'S FUNCTIONALITY IF CHANGING THE MODE MANUALLY.
*******************************************************************************/ 
void lide_c_set_actor_mode(lide_c_actor_context_type *context, int mode);

/*******************************************************************************
Get mode for LIDE-C actors.
PLEASE BE VERY CAREFUL FOR USING THIS METHOD. IT MAY HAVE NEGATIVE AFFECT ON 
ACTOR'S FUNCTIONALITY IF CHANGING THE MODE MANUALLY.
*******************************************************************************/ 
int lide_c_get_actor_mode(lide_c_actor_context_type *context);

#endif
