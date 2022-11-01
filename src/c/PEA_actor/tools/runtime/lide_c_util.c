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
#include "lide_c_util.h"

#include <stdio.h>
#include <stdlib.h>

/* Open a file. Exit with an error message if the file cannot
   be opened with the specified mode ("r", "w", etc. as in
   coventional fopen). Otherwise return the file pointer of
   the opened file.
*/
FILE *lide_c_util_fopen(const char *fname, const char *mode) {
    FILE *fp;

    if ((fp=fopen(fname, mode))==NULL) {
        fprintf(stderr, "could not open file named '%s' with mode '%s'", 
                fname, mode);
        exit(1);
    }
    return fp;
}

void *lide_c_util_malloc(size_t size) {
    void *p;

    if ((p = malloc(size)) != NULL) {
        return(p);
    } else {
        fprintf(stderr, "lide_c_util_malloc error: insufficient memory");
        exit(1);
    } 
    return NULL;  /*NOTREACHED*/
}

bool lide_c_util_guarded_execution(lide_c_actor_context_type *context,
        char *descriptor) {

    if (context->enable(context)) {
        context->invoke(context);
        printf("%s visit complete.\n", descriptor);
        return true;
    } else {
        return false;
    } 
}

void lide_c_util_simple_scheduler(lide_c_actor_context_type *actors[], 
        int actor_count, char *descriptors[]) {

    bool progress = false;
    int i = 0;

    do {
        progress = 0;
        for (i = 0; i < actor_count; i++) {
            progress |= 
                    lide_c_util_guarded_execution(actors[i], descriptors[i]);
        }
    } while (progress);
}

void lide_c_set_actor_mode(lide_c_actor_context_type *context, int mode){
    if(context != NULL){
        context->mode = mode;
    }
    return;
}

int lide_c_get_actor_mode(lide_c_actor_context_type *context){
    if(context != NULL){
        return context->mode;
    }else{
        fprintf(stderr, "actor context does not exist.\n");
        return -1;
    }
}

