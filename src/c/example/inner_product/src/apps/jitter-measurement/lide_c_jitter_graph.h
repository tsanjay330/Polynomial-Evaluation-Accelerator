#ifndef _lide_c_jitter_graph_h
#define _lide_c_jitter_graph_h

/******************************************************************************
 * Header file for the jitter graph context type.
 * Created and based on the lide_c_simple_graph.h used in the taulide demo.
******************************************************************************/

#include "lide_c_fifoext.h"
#include "lide_c_file_source_double.h"
#include "lide_c_file_sink_jitter.h"
#include "lide_c_FSM_s.h"
#include "lide_c_DVL.h"
#include "lide_c_STR.h"
#include "lide_c_TRT.h"
#include "lide_c_RE.h"
#include "lide_c_RRE.h"
#include "lide_c_CRE.h"
#include "lide_c_Phase.h"
#include "lide_c_util.h"

#define MILLION         1000000
#define BUFFER_CAPACITY 100000
#define BUFFER_FLOAT    512
#define BUFFER_POINTER  512

/* An enumeration of the actors in the graph */
#define ACTOR_XSOURCE   0
#define ACTOR_DVL       1
#define ACTOR_STR       2
#define ACTOR_FSM_s     3
#define ACTOR_TRT       4
#define ACTOR_RE        5
#define ACTOR_RRE       6
#define ACTOR_CRE       7
#define ACTOR_PHASE     8
#define ACTOR_SINK      9

/* The total number of actors and fifos in the graph */
#define ACTOR_COUNT     10
#define FIFO_COUNT      24

/* The total number of parameter files for the graph */
#define PARA_COUNT      1
/*Ws, Op, Th*/

/*******************************************************************************
 * STRUCTURE DEFINITIONS
*******************************************************************************/
struct _lide_c_jitter_graph_context_struct;
typedef struct _lide_c_jitter_graph_context_struct
	lide_c_jitter_graph_context_type;

/*******************************************************************************
 * FUNCTION DEFINITIONS
*******************************************************************************/

lide_c_jitter_graph_context_type *lide_c_jitter_graph_new(
		char *in, char *out, char *param);

void lide_c_jitter_scheduler(lide_c_jitter_graph_context_type *context);

void lide_c_jitter_graph_terminate(
		lide_c_jitter_graph_context_type *context);

#endif
