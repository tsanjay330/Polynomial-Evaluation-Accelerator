/*******************************************************************************
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
*******************************************************************************/

/* Common elements across context type of fifos. */

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

/* methods in fifo*/
lide_c_fifo_population_ftype get_population;
lide_c_fifo_capacity_ftype get_capacity;
lide_c_fifo_token_size_ftype  get_token_size;
lide_c_fifo_write_ftype write;
lide_c_fifo_read_ftype read;
lide_c_fifo_read_block_ftype read_block;
lide_c_fifo_write_block_ftype write_block;






