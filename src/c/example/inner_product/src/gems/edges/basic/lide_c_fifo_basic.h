#ifndef _lide_c_fifo_basic_h
#define _lide_c_fifo_basic_h

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
#include "lide_c_fifo.h"
/*****************************************************************************
Each data item in the FIFO is referred to as a "token". Tokens can have
arbitrary types associated with them. For a given FIFO instance, there
is a fixed token size (number of bytes per token). Tokens have arbitrary
types --- e.g., they can be integers, floating point values (float or
double), characters, or pointers (to any kind of data). This organization
allows for flexibility in storing different kinds of data values, 
and efficiency in storing the data values directly (without being
encapsulated in any sort of higher-level "token" object).
*****************************************************************************/
  
/* A FIFO. */
typedef struct _lide_c_fifo_basic_struct lide_c_fifo_basic_type;   

/* A pointer to a fifo. */
typedef lide_c_fifo_basic_type *lide_c_fifo_basic_pointer;

/*****************************************************************************
A pointer to a "token printing function", which is a function that 
prints the data value encapsulated by a single token to a given file.
This type is normally used just for diagnostic purposes.
*****************************************************************************/
typedef void (*lide_c_fifo_basic_f_token_printing)(FILE *output, void *token);

/* ----------------------------  Public functions --------------------- */

/*****************************************************************************
Display information about the FIFO to the specified file.  The display output
is written to the specified file, which must be open for writing prior to
calling this function.  This function is primarily provided for diagnostic
purposes.
*****************************************************************************/
void lide_c_fifo_basic_display_status(lide_c_fifo_basic_pointer fifo, FILE *output);

/*****************************************************************************
Display the contents of the fifo, one byte at time, with each byte displayed in
hexadecimal format. The bytes that correspond to a given token are printed on
the same line, and successive tokens in the FIFO (starting with the olidest
token) are displayed on successive lines. The display output is written to the
specified file, which must be open for writing prior to calling this
function. This function is primarily for diagnostic purposes.
*****************************************************************************/
void lide_c_fifo_basic_display_contents(lide_c_fifo_basic_pointer fifo, FILE *output);

/*****************************************************************************
Create a new FIFO with the specified capacity (maximum number of tokens that it
can hold), and the specified token_size (the number of bytes used to store a
single token).  If the specified capacity is less than or equal to zero, the
specified token size is less than or equal to zero, or there is not enough
memory to allocate the FIFO, the function fails silently and returns NULL.
*****************************************************************************/
lide_c_fifo_basic_pointer lide_c_fifo_basic_new(int capacity, int token_size,  
                                                int index);

/*****************************************************************************
Return the number of tokens that are currently in the fifo.
*****************************************************************************/
int lide_c_fifo_basic_population(lide_c_fifo_basic_pointer fifo);

/*****************************************************************************
Return the capacity of the fifo.
*****************************************************************************/
int lide_c_fifo_basic_capacity(lide_c_fifo_basic_pointer fifo);

/*****************************************************************************
Return the number of bytes that are used to store a single token in the fifo.
*****************************************************************************/
int lide_c_fifo_basic_token_size(lide_c_fifo_basic_pointer fifo);

/*****************************************************************************
Insert a new token into the buffer. This function copies N bytes, where N is
the fifo token size, starting at the address specified by the data argument.
This data is copied into the next available position in the buffer. If the FIFO
is already full upon entry to this function, the function fails silently
(without modifying the FIFO).
*****************************************************************************/
void lide_c_fifo_basic_write(lide_c_fifo_basic_pointer fifo, void *data);

/*****************************************************************************
Write a block of "size" tokens into the buffer. The "data" argument points to
the beginning of the block of data to be written. For large block sizes,
calling this function is likely to be significantly more efficient than
repeated calls to lide_c_fifo_basic_write. The function fails silently (without
modifying the FIFO) if there is not enough room in the FIFO to hold "size" new
tokens.
*****************************************************************************/
void lide_c_fifo_basic_write_block(lide_c_fifo_basic_pointer fifo, void *data, int size);


/*****************************************************************************
Update the FIFO population as a FIFO write, but do not write the token to the 
buffer. write_advance function will update (move forward) the write pointer by 
one token. Then update the FIFO population. The funtion will fails silently if 
there is no enough empty space in FIFO(without modifying the FIFO). 
*****************************************************************************/
void lide_c_fifo_basic_write_advance(lide_c_fifo_basic_pointer fifo);  



/*****************************************************************************
Read a token from the buffer, and remove the token from the buffer.  It is
assumed that the data argument points to a block of memory that consists of at
least N contiguous bytes, where N is the token size associated with the FIFO.
The function copies N bytes of memory from the current read position in the
FIFO (the olidest token) into the block of memory pointed to by the data
argument. The function fails silently (without modifying the FIFO) if the FIFO
is empty upon entry to this function.
*****************************************************************************/
void lide_c_fifo_basic_read(lide_c_fifo_basic_pointer fifo, void *data); 

/*****************************************************************************
Read a block of "size" tokens from the buffer, and remove these tokens from the
buffer. It is assumed that the "data" argument points to a block of memory that
consists of at least ("size" * N) contiguous bytes, where N is the token size
associated with this FIFO. The function copies ("size" * token_size) bytes of
memory from the current read position in the fifo (the olidest token) into the
block of memory pointed to by the data argument.  For large block sizes,
calling this function is likely to be significantly more efficient than
repeated calls to lide_c_fifo_basic_read.  The function fails silently (without
modifying the FIFO) if the FIFO contains fewer than "size" tokens upon entry to
this function.
*****************************************************************************/
void lide_c_fifo_basic_read_block(lide_c_fifo_basic_pointer fifo, void *data, int size);


/*****************************************************************************
Read a token from the buffer, but do NOT remove it from the buffer.  It is
assumed that the data argument points to a block of memory that consists of at
least N contiguous bytes, where N is the token size associated with the FIFO.
The function copies N bytes of memory from the current read position in the
FIFO (the oldest token) into the block of memory pointed to by the data
argument. The function fails silently (without modifying the FIFO) if the FIFO
is empty upon entry to this function.
*****************************************************************************/
void lide_c_fifo_basic_peek(lide_c_fifo_basic_pointer fifo, void *data);

/*****************************************************************************
Read a block of "size" tokens from the buffer, but do NOT remove it from the 
buffer. It is assumed that the data argument points to a block of memory that 
consists of at least ("size" * N) contiguous bytes, where N is the token size 
associated with the FIFO. The function copies ("size" * N) bytes of memory 
from the current read position in the FIFO (the oldest token) into the block 
of memory pointed to by the data argument. The function fails silently 
(without modifying the FIFO) if the FIFO is empty upon entry to this function.
*****************************************************************************/
void lide_c_fifo_basic_peek_block(lide_c_fifo_basic_pointer fifo, 
    void *data, int size);

/*****************************************************************************
Update the FIFO population as a FIFO read, but do not read the token from the  
buffer. read_advance function will update (move forward) the read pointer by 
one token. Then update the FIFO population. The funtion will fails silently if 
there is no token in FIFO(without modifying the FIFO). 
*****************************************************************************/
void lide_c_fifo_basic_read_advance(lide_c_fifo_basic_pointer fifo); 

/*****************************************************************************
Reset the FIFO so that it contains no tokens --- that is, reset to an
empty FIFO.
*****************************************************************************/
void lide_c_fifo_basic_reset(lide_c_fifo_basic_pointer);

/*****************************************************************************
Deallocate the storage associated with the given fifo.
*****************************************************************************/
void lide_c_fifo_basic_free(lide_c_fifo_basic_pointer fifo);

/*****************************************************************************
Set the token printing function that is associated with this fifo.
*****************************************************************************/
void lide_c_fifo_basic_set_token_printing(lide_c_fifo_basic_pointer fifo,
        lide_c_fifo_basic_f_token_printing f);

/*****************************************************************************
Print to the given file all of the tokens in the fifo using the token printing
function that is associated with the fifo. If no token printing function is
associated with the fifo, then the function does nothing.  A newline is
inserted after printing each token. The tokens are printed sequentially,
starting with the olidest (least-recently written) one.
*****************************************************************************/
void lide_c_fifo_basic_print_tokens(lide_c_fifo_basic_pointer fifo, FILE *output);

/*****************************************************************************
Set index of FIFO in a dataflow graph
*****************************************************************************/
void lide_c_fifo_basic_set_index(lide_c_fifo_basic_pointer fifo, int index);

/*****************************************************************************
Get index of FIFO in a dataflow graph
*****************************************************************************/
int lide_c_fifo_basic_get_index(lide_c_fifo_basic_pointer fifo);
#endif

