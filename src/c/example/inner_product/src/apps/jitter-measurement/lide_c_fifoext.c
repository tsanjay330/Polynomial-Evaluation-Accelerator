/****************************************************************************
@ddblock_begin copyright

Copyright (c) 1997-2017
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
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lide_c_fifoext.h"

struct _lide_c_fifo_struct {
    /* The number of tokens that the buffer can hold. */
    int capacity;

    /* Pointer to the first token placeholider in the buffer. */
    void *buffer_start;

    /* Pointer to the last token placeholider in the buffer. This member
       is here for convenience (it can also be derived from the 
       buffer start, capacity, and token_size). 
    */
    void *buffer_end;

    /* The number of tokens that are currently in the FIFO. */
    int population;

    /* Pointer to the next token placeholider to be read from. */
    void *read_pointer;

    /* Pointer to the next token placeholider to be written into. */
    void *write_pointer;

    /* The number of bytes in a single token. */
    int token_size;

    /* The token printing function (if any) associated with this fifo. */
    lide_c_fifo_f_token_printing print; 

}; 


/*****************************************************************************
Private functions.
*****************************************************************************/


/*****************************************************************************
Print diagnostic information relating to internal pointer values associated
with a given FIFO.
*****************************************************************************/
void _lide_c_fifo_diagnostics(lide_c_fifo_pointer fifo, FILE *output) {
    fprintf(output, "RP = %p, WP = %p\n", fifo->read_pointer, 
            fifo->write_pointer);
    fprintf(output, "START = %p, END = %p\n", fifo->buffer_start, 
            fifo->buffer_end);
}
 
/*****************************************************************************
Public functions.
*****************************************************************************/

void lide_c_fifo_display_contents(lide_c_fifo_pointer fifo, FILE *output) {
    char *p = NULL;
    int i = 0;
    int j = 0;
    char value = '\0';

    fprintf(output, "----- fifo contents: -----------\n");

    p = fifo->read_pointer;
    for (i = 0; i < fifo->population; i++) {
        for (j = 0; j < fifo->token_size; j++) {
            if (j != 0) {
                fprintf(output, " ");
            }
            value = p[j];

            /* In case of issues with sign extension, extract just the last 
               byte of the value before printing. 
            */
            fprintf(output, "%02x", value & 0xff);
        }
        fprintf(output, "\n");
        if (p == fifo->buffer_end) {
            p = fifo->buffer_start;
        } else {
            p += fifo->token_size;
        }
    }
    fprintf(output, "--------------------------------\n");
}

void lide_c_fifo_print_tokens(lide_c_fifo_pointer fifo, FILE *output) {
    char *p = NULL;
    int i = 0;

    if (fifo->print == NULL) {
        return;
    }
    p = fifo->read_pointer;
    for (i = 0; i < fifo->population; i++) {
        fifo->print(output, p);
        fprintf(output, "\n");
        if (p == fifo->buffer_end) {
            p = fifo->buffer_start;
        } else {
            p += fifo->token_size;
        }
    }
}


/*****************************************************************************
Print an error message to stderr and exit. If the specified fifo
is non-NULL, also print diagnostic information about the fifo to
stderr.
*****************************************************************************/
void _lide_c_fifo_error(lide_c_fifo_pointer fifo, char *message) {
    if (fifo != NULL) {
        lide_c_fifo_display_status(fifo, stderr);
    }
    fprintf(stderr, "%s\n", message);
    exit(1);
}

/*****************************************************************************
Public functions.
*****************************************************************************/

void lide_c_fifo_display_status(lide_c_fifo_pointer fifo, FILE *output) {
    fprintf(output, "----- fifo status: -----------\n");
    fprintf(output, "population: %d\n", fifo->population);
    fprintf(output, "capacity: %d\n", fifo->capacity);
    fprintf(output, "token size: %d\n", fifo->token_size);
    fprintf(output, "write index: %d\n", 
            (int)(((char*)fifo->write_pointer - (char*)fifo->buffer_start) / 
                    fifo->token_size));
    fprintf(output, "read index: %d\n", 
            (int)(((char*)fifo->read_pointer - (char*)fifo->buffer_start) / 
                    fifo->token_size));
    fprintf(output, "------------------------------\n");
}

lide_c_fifo_pointer lide_c_fifo_new(int capacity, int token_size) {
    lide_c_fifo_pointer fifo = NULL;

    /*************************************************************************
    The use of generic pointers in this implementation requires 
    that characters have unit size. This should be the case, but
    in case it is not, we abort. It would be better to move this
    check into some sort of platform-level configuration for the ADT.
    A point related to this one is that involving certain typecasts
    to (char*) in the code --- these casts are needed to avoid (probably benign
    in our case) compiler warnings when performing arithmetic on generic 
    pointers.
    *************************************************************************/
 
    if (sizeof(char) != 1) {
        _lide_c_fifo_error(NULL, "incompatible target platform");
    }

    if (capacity < 1) {
        return NULL;
    }
    if (token_size <= 0) {
        return NULL;
    }
    fifo = malloc(sizeof(lide_c_fifo_type));
    if (fifo == NULL) {
        return NULL;
    }
    fifo->capacity = capacity;
    fifo->token_size = token_size;
    fifo->buffer_start = NULL;
    fifo->buffer_start = malloc((size_t)(capacity * token_size));
    if (fifo->buffer_start == NULL) {
        free(fifo);
        return NULL;
    }
    fifo->buffer_end = ((char*)fifo->buffer_start) + 
            ((capacity - 1) * token_size);
    lide_c_fifo_reset(fifo);
    fifo->print = NULL;
    return fifo;
}

int lide_c_fifo_population(lide_c_fifo_pointer fifo) {
    return fifo->population;
}

int lide_c_fifo_capacity(lide_c_fifo_pointer fifo) {
    return fifo->capacity;
}

int lide_c_fifo_token_size(lide_c_fifo_pointer fifo) {
    return fifo->token_size;
}

void lide_c_fifo_write(lide_c_fifo_pointer fifo, void *data) {
    /* Make sure there is room for the new data. */
    if (fifo->population >= fifo->capacity) {
        return;
    }

    /* The following assumes that sizeof(char) = 1. */
    memcpy(fifo->write_pointer, data, fifo->token_size);

    /* Advance the write pointer, using a circular buffering convention. */
    if (fifo->write_pointer == fifo->buffer_end) {
        fifo->write_pointer = fifo->buffer_start;
    } else {
        fifo->write_pointer = ((char*)fifo->write_pointer) + fifo->token_size;
    }

    /* Update the token count. */
    fifo->population++; 
}

void lide_c_fifo_read(lide_c_fifo_pointer fifo, void *data) {
    /* Make sure the fifo is not empty. */
    if (fifo->population == 0) {
        return;
    }

    /* The following assumes that sizeof(char) = 1. */
    memcpy(data, fifo->read_pointer, fifo->token_size);

    /* Advance the read pointer, using a circular buffering convention. */
    if (fifo->read_pointer == fifo->buffer_end) {
        fifo->read_pointer = fifo->buffer_start;
    } else {
        fifo->read_pointer = ((char*)fifo->read_pointer) + fifo->token_size;
    }

    /* Update the token count. */
    fifo->population--; 
}

void lide_c_fifo_write_block(lide_c_fifo_pointer fifo, void *data, int size) {
    /* Room (number of token spaces) "in front of the write pointer" within 
       the buffer. 
    */
    int room_in_front = 0;

    /* Breakdown in bytes of the amounts of data to write before and after,
       respectively, the write pointer wraps around the end of the buffer.
    */
    int part1_size = 0;
    int part2_size = 0;

    /* A cache of the FIFO token size. */
    int token_size = 0;

    /* Make sure there is room for the new data. */
    if ((fifo->population + size) > fifo->capacity) {
        return;
    }

    token_size = fifo->token_size;
    room_in_front = ((((char*) (fifo->buffer_end)) - 
            ((char*)(fifo->write_pointer))) / token_size) + 1;

    if (room_in_front >= size) {
        part1_size = size * token_size;
        part2_size = 0;
    } else {
        part1_size = room_in_front * token_size;
        part2_size = (size - room_in_front) * token_size; 
    }
    
    /* The following assumes that sizeof(char) = 1. */
    memcpy(fifo->write_pointer, data, part1_size);
    if (part2_size > 0) {
        data = ((char*)data) + part1_size;
        memcpy(fifo->buffer_start, data, part2_size);
        fifo->write_pointer = ((char*)fifo->buffer_start) + part2_size;
    } else {
        if (size == room_in_front) {
            fifo->write_pointer = fifo->buffer_start;
        } else {
            fifo->write_pointer = ((char*)fifo->write_pointer) + part1_size;
        }
    }

    /* Update the token count. */
    fifo->population += size; 
}

void lide_c_fifo_read_block(lide_c_fifo_pointer fifo, void *data, int size) {
    /* Room (number of token spaces) "in front of the read pointer" within 
       the buffer. 
    */
    int room_in_front = 0;

    /* Breakdown in bytes of the amounts of data to read before and after,
       respectively, the read pointer wraps around the end of the buffer.
    */
    int part1_size = 0;
    int part2_size = 0;

    /* A cache of the FIFO token size. */
    int token_size = 0;

    /* Make sure there are enough tokens in the FIFO. */
    if (fifo->population < size) {
		printf("lide_c_fifo_read_block\n");
        return;
    }

    token_size = fifo->token_size;
    room_in_front = ((((char*) (fifo->buffer_end)) - 
            ((char*)(fifo->read_pointer))) / token_size) + 1;

    if (room_in_front >= size) {
        part1_size = size * token_size;
        part2_size = 0;
    } else {
        part1_size = room_in_front * token_size;
        part2_size = (size - room_in_front) * token_size; 
    }

    /* The following assumes that sizeof(char) = 1. */
    memcpy(data, fifo->read_pointer, part1_size);
    if (part2_size > 0) {
        data = ((char*)data) + part1_size;
        memcpy(data, fifo->buffer_start, part2_size);
        fifo->read_pointer = ((char*)fifo->buffer_start) + part2_size;
    } else {
        if (size == room_in_front) {
            fifo->read_pointer = fifo->buffer_start;
        } else {
            fifo->read_pointer = ((char*)fifo->read_pointer) + part1_size;
        }
    }

    /* Update the token count. */
    fifo->population -= size; 
}




void *lide_c_fifo_peek(lide_c_fifo_pointer fifo, int i){
	void* pkptr = NULL;
	/* Check whether the fifo is empty*/
	if (fifo->population >= 1){
		pkptr  = fifo->read_pointer;
	}else{
		fprintf(stderr, "Warning: "); 
		fprintf(stderr, "requesting %d tokens on CPU memory, ", 1); 
		fprintf(stderr, "but only %d tokens are present\n", 
				fifo->population); 
		pkptr = NULL;
	}	
	return pkptr;
}


void *lide_c_fifo_peek_block(lide_c_fifo_pointer fifo, int size){
	void* pkptr = NULL;
	/* Check whether the fifo has loaded enough data*/
	if (fifo->population >= size){
		pkptr  = fifo->read_pointer;
	}else{
		fprintf(stderr, "Warning: "); 
		fprintf(stderr, "requesting %d tokens on CPU memory, ", size); 
		fprintf(stderr, "but only %d tokens are present\n", 
				fifo->population); 
		pkptr = NULL;
	}
	return pkptr;
}

void lide_c_fifo_advance(lide_c_fifo_pointer fifo){
	/* Make sure the fifo is not empty. */
    if (fifo->population == 0) {
        return;
    }
    /* Advance the read pointer, using a circular buffering convention. */
    if (fifo->read_pointer == fifo->buffer_end) {
        fifo->read_pointer = fifo->buffer_start;
    } else {
        fifo->read_pointer = ((char*)fifo->read_pointer) + fifo->token_size;
    }
	return;
}

void lide_c_fifo_advance_block(lide_c_fifo_pointer fifo, int size){
 /* Room (number of token spaces) "in front of the read pointer" within 
       the buffer. 
    */
    int room_in_front = 0;

    /* Breakdown in bytes of the amounts of data to read before and after,
       respectively, the read pointer wraps around the end of the buffer.
    */
    int part1_size = 0;
    int part2_size = 0;

    /* A cache of the FIFO token size. */
    int token_size = 0;

    /* Make sure there are enough tokens in the FIFO. */
    if (fifo->population < size) {
        return;
    }

    token_size = fifo->token_size;
    room_in_front = ((((char*) (fifo->buffer_end)) - 
            ((char*)(fifo->read_pointer))) / token_size) + 1;

    if (room_in_front >= size) {
        part1_size = size * token_size;
        part2_size = 0;
    } else {
        part1_size = room_in_front * token_size;
        part2_size = (size - room_in_front) * token_size; 
    }

    /* The following assumes that sizeof(char) = 1. */
    if (part2_size > 0) {
        fifo->read_pointer = ((char*)fifo->buffer_start) + part2_size;
    } else {
        if (size == room_in_front) {
            fifo->read_pointer = fifo->buffer_start;
        } else {
            fifo->read_pointer = ((char*)fifo->read_pointer) + part1_size;
        }
    }

}

void lide_c_fifo_consume(lide_c_fifo_pointer fifo){
	/* Update the token count. */
    fifo->population--; 
	return;
} 

void lide_c_fifo_consume_block(lide_c_fifo_pointer fifo, int size){
    /* Update the token count. */
    fifo->population -= size; 
	return;
}




void lide_c_fifo_free(lide_c_fifo_pointer fifo) {
    if (fifo == NULL) {
        return;
    }
    if (fifo->buffer_start != NULL) {
        free(fifo->buffer_start);
    }
    free(fifo);
}

void lide_c_fifo_reset(lide_c_fifo_pointer fifo) {
    fifo->write_pointer = fifo->buffer_start;
    fifo->read_pointer = fifo->buffer_start;
    fifo->population = 0;
}

void lide_c_fifo_set_token_printing(lide_c_fifo_pointer fifo,
        lide_c_fifo_f_token_printing f) {
    fifo->print = f;
}
