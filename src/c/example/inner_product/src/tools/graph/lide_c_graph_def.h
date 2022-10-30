#ifndef _lide_c_graph_def_h
#define _lide_c_graph_def_h

/*******************************************************************************
@ddblock_begin copyright

Copyright (c) 1997-2020
Maryland DSPCAD Research Group, The University of Maryland at College Park 
All rights reserved.

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
/*
The graph ADT context is defined in lide_c_graph_def.h. In the actor design,
pointer to the subgraph context pointer array is the common element in the 
actor ADT context. In the graph design, pointer to the actor context pointer 
array is the common element in the graph ADT context. The circular dependence 
exists since header files of actor ADT and graph ADT need to include each 
other. We then separate the definition of the graph context in this 
lide_c_graph_def.h. Structure details of graph ADT is placed in
lide_c_graph.h. In the actor design, actor header file only needs to include
this header file for actor context definition.*/

struct lide_c_graph_context_struct;
typedef struct lide_c_graph_context_struct lide_c_graph_context_type;

#endif
