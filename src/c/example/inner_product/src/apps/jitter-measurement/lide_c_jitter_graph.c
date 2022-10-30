/*******************************************************************************
 * Making the graph file as input for the OSSA
 * This is all based on the test script for Jitter v0
 * Original driver file in ossa/software/jitter-meas-app/test-router_jitter/
 *              test-router_jitter_v0/util/lide_c_jitter_driver
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lide_c_basic.h"
#include "lide_c_actor.h"
#include "lide_c_jitter_graph.h"
#define MILLION 1000000


#define BUFFER_CAPACITY 100000
#define BUFFER_FLOAT    512
#define BUFFER_POINTER  512

/* An enumeration of the actors in this application. */
#define ACTOR_XSOURCE 0
#define ACTOR_DVL   1
#define ACTOR_STR   2
#define ACTOR_FSM_s 3
#define ACTOR_TRT   4
#define ACTOR_RE    5
#define ACTOR_RRE   6
#define ACTOR_CRE   7
#define ACTOR_PHASE 8
#define ACTOR_SINK  9

/* The total number of actors in the application. */
#define ACTOR_COUNT 10

/* The total number of parameter files in the application. */
#define PARA_COUNT 1 


/*Defining the graph structure*/
struct _lide_c_jitter_graph_context_struct{
#include "lide_c_graph_context_type_common.h"
    char *in_file;/*input file*/
    FILE *out_file;/*output file*/
    int Ws;
    int Th;
    float Op;
    int length;
};

/*******************************************************************************
 * FUNCTION IMPLEMENTATION
*******************************************************************************/

lide_c_jitter_graph_context_type *lide_c_jitter_graph_new(
        char *in, char *out, char *param){

    /*Making the context of the graph*/
    lide_c_jitter_graph_context_type *context = NULL;
    context = lide_c_util_malloc(sizeof(lide_c_jitter_graph_context_type));

    context->actor_count = ACTOR_COUNT;
    context->fifo_count = FIFO_COUNT;

    context->actors = (lide_c_actor_context_type **)lide_c_util_malloc(
            ACTOR_COUNT * sizeof(lide_c_actor_context_type *));
    context->fifos = (lide_c_fifo_pointer *)lide_c_util_malloc(
            FIFO_COUNT * sizeof(lide_c_fifo_pointer));
    context->descriptors = (char **)lide_c_util_malloc(
            ACTOR_COUNT * sizeof(char *));
    int i;
    for(i = 0; i < context->actor_count; i++){
        context->descriptors[i] = (char *)lide_c_util_malloc(
                7 * sizeof(char));/*xsource is longest description*/
    }

    /*Adding the descriptions*/
    strcpy(context->descriptors[ACTOR_XSOURCE],"xsource");
    strcpy(context->descriptors[ACTOR_DVL],"dvl");
    strcpy(context->descriptors[ACTOR_STR],"str");
    strcpy(context->descriptors[ACTOR_FSM_s],"fsm");
    strcpy(context->descriptors[ACTOR_TRT],"trt");
    strcpy(context->descriptors[ACTOR_RE],"re");
    strcpy(context->descriptors[ACTOR_RRE],"rre");
    strcpy(context->descriptors[ACTOR_CRE],"cre");
    strcpy(context->descriptors[ACTOR_PHASE],"phase");
    strcpy(context->descriptors[ACTOR_SINK],"sink");

    /*Creating the FIFO connections for the graph*/
    lide_c_fifo_pointer fifo1 = NULL, fifo20 = NULL, fifo21 = NULL;
    lide_c_fifo_pointer fifo30 = NULL, fifo31 = NULL, fifo32 = NULL;
    lide_c_fifo_pointer fifo40 = NULL, fifo41 = NULL, fifo42 = NULL;
    lide_c_fifo_pointer fifo50 = NULL, fifo51 = NULL, fifo52 = NULL;
    lide_c_fifo_pointer fifo60 = NULL, fifo61 = NULL, fifo62 = NULL;
    lide_c_fifo_pointer fifo70 = NULL, fifo71 = NULL, fifo72 = NULL;
    lide_c_fifo_pointer fifo80 = NULL, fifo81 = NULL, fifo82 = NULL;
    lide_c_fifo_pointer fifo90 = NULL, fifo91 = NULL, fifo92 = NULL;

    int token_size = 0;
    int j = 0; 
    int k = 0;
    int Ws = 0;
    int Th = 0;
    float Op = 0.0;
    int arg_count = 3;
    int NUM = 0;
    int length = 0;
    int Residue = 0;
    double value;

    /*Handling the parameter input*/
    /*Question: The exit commands are used in the test driver,
     * What should be used instead for the context creation case?
     * Should a VOID pointer be used as output or something else?*/
    FILE *fp = NULL;
    fp = fopen(param, "r");
    if (!fp) {
        fprintf(stderr, "Parameter txt file could not be opened");
        exit(1);
    }
    if (!fscanf(fp,"%d",&(context->length))){
        fprintf(stderr, "Cannot read Window size from Parameter.txt\n");
        exit(1);
    }           
    if (!fscanf(fp,"%d",&(context->Ws))){
        fprintf(stderr, "Cannot read Window size from Parameter.txt\n");
        exit(1);
    }
    if (!fscanf(fp,"%f",&(context->Op))){
        fprintf(stderr, "Cannot read overlap of the window from Parameter.txt\n");
        exit(1);
    }
    if (!fscanf(fp,"%d",&(context->Th))){
        fprintf(stderr, "Cannot read threshold from Parameter.txt\n");
        exit(1);
    }
    fclose(fp);
    Ws = context->Ws;
    Th = context->Th;
    Op = context->Op;
    length = context->length;
    NUM = (int)((length-Ws)/Ws/(1-Op))+1;
    Residue = length-NUM*Ws;
    if(Residue != 0)
        NUM++;

    /*FILE *in_file = lide_c_util_fopen(in, "r");*/
    context->in_file = in;
    FILE *out_file = lide_c_util_fopen(out, "w");
    context->out_file = out_file;
    /* Create the buffers. */
    i = 0;
    token_size = sizeof(double);
    fifo1 = lide_c_fifo_new(BUFFER_CAPACITY, token_size);
    context->fifos[i++] = fifo1;
    token_size = sizeof(double);
    fifo20 = lide_c_fifo_new(BUFFER_FLOAT, token_size);
    context->fifos[i++] = fifo20;
    token_size = sizeof(double *);
    fifo21 = lide_c_fifo_new(BUFFER_POINTER, token_size);
    context->fifos[i++] = fifo21;

    token_size = sizeof(double);
    fifo30 = lide_c_fifo_new(BUFFER_FLOAT, token_size);
    context->fifos[i++] = fifo30;
    token_size = sizeof(double *);
    fifo31 = lide_c_fifo_new(BUFFER_POINTER, token_size);
    context->fifos[i++] = fifo31;
    token_size = sizeof(int *);
    fifo32 = lide_c_fifo_new(BUFFER_POINTER, token_size);
    context->fifos[i++] = fifo32;

    token_size = sizeof(double);
    fifo40 = lide_c_fifo_new(BUFFER_FLOAT, token_size);
    context->fifos[i++] = fifo40;
    token_size = sizeof(double *);
    fifo41 = lide_c_fifo_new(BUFFER_POINTER, token_size);
    context->fifos[i++] = fifo41;
    token_size = sizeof(int *);
    fifo42 = lide_c_fifo_new(BUFFER_POINTER, token_size);
    context->fifos[i++] = fifo42;

    token_size = sizeof(double);
    fifo50 = lide_c_fifo_new(BUFFER_FLOAT, token_size);
    context->fifos[i++] = fifo50;
    token_size = sizeof(double *);
    fifo51 = lide_c_fifo_new(BUFFER_POINTER, token_size);
    context->fifos[i++] = fifo51;
    token_size = sizeof(int *);
    fifo52 = lide_c_fifo_new(BUFFER_POINTER, token_size);
    context->fifos[i++] = fifo52;

    token_size = sizeof(double);
    fifo60 = lide_c_fifo_new(BUFFER_FLOAT, token_size);
    context->fifos[i++] = fifo60;
    token_size = sizeof(double *);
    fifo61 = lide_c_fifo_new(BUFFER_POINTER, token_size);
    context->fifos[i++] = fifo61;
    token_size = sizeof(int *);
    fifo62 = lide_c_fifo_new(BUFFER_POINTER, token_size);
    context->fifos[i++] = fifo62;

    token_size = sizeof(double);
    fifo70 = lide_c_fifo_new(BUFFER_FLOAT, token_size);
    context->fifos[i++] = fifo70;
    token_size = sizeof(double *);
    fifo71 = lide_c_fifo_new(BUFFER_POINTER, token_size);
    context->fifos[i++] = fifo71;
    token_size = sizeof(int *);
    fifo72 = lide_c_fifo_new(BUFFER_POINTER, token_size);
    context->fifos[i++] = fifo72;

    token_size = sizeof(double);
    fifo80 = lide_c_fifo_new(BUFFER_FLOAT, token_size);
    context->fifos[i++] = fifo80;
    token_size = sizeof(double *);
    fifo81 = lide_c_fifo_new(BUFFER_POINTER, token_size);
    context->fifos[i++] = fifo81;
    token_size = sizeof(int *);
    fifo82 = lide_c_fifo_new(BUFFER_POINTER, token_size);
    context->fifos[i++] = fifo82;

    token_size = sizeof(double);
    fifo90 = lide_c_fifo_new(BUFFER_FLOAT, token_size);
    context->fifos[i++] = fifo90;
    token_size = sizeof(double *);
    fifo91 = lide_c_fifo_new(BUFFER_POINTER, token_size);
    context->fifos[i++] = fifo91;
    token_size = sizeof(int *);
    fifo92 = lide_c_fifo_new(BUFFER_POINTER, token_size);
    context->fifos[i++] = fifo92;

    /* Create and connect the actors. */  
    k = 0;
    context->actors[ACTOR_XSOURCE] = (lide_c_actor_context_type*)
        (lide_c_file_source_double_new(in, fifo1));

    context->actors[ACTOR_DVL] = (lide_c_actor_context_type*)
        (lide_c_DVL_new(Ws, Op, Th,fifo1, fifo20, fifo21));

    context->actors[ACTOR_STR] = (lide_c_actor_context_type*)
        (lide_c_STR_new(Ws,fifo20,fifo21, fifo30, fifo31,fifo32));

    context->actors[ACTOR_FSM_s] = (lide_c_actor_context_type*)
        (lide_c_FSM_s_new(Ws, fifo30, fifo31,fifo32, fifo40, fifo41, fifo42));

    context->actors[ACTOR_TRT] = (lide_c_actor_context_type*)
        (lide_c_TRT_new(Ws, fifo40, fifo41,fifo42, fifo50, fifo51, fifo52));

    context->actors[ACTOR_RE] = (lide_c_actor_context_type*)
        (lide_c_RE_new(Ws, fifo50, fifo51,fifo52, fifo60, fifo61, fifo62));

    context->actors[ACTOR_RRE] = (lide_c_actor_context_type*)
        (lide_c_RRE_new(Ws, fifo60, fifo61,fifo62, fifo70, fifo71, fifo72));

    context->actors[ACTOR_CRE] = (lide_c_actor_context_type*)
        (lide_c_CRE_new(Ws, k,fifo70, fifo71,fifo72, fifo80, fifo81, fifo82));

    context->actors[ACTOR_PHASE] = (lide_c_actor_context_type*)
        (lide_c_Phase_new(Ws, NUM,fifo80, fifo81,fifo82, fifo90, fifo91, fifo92));

    context->actors[ACTOR_SINK] = (lide_c_actor_context_type *)
        (lide_c_file_sink_jitter_new(out_file, fifo90));


    return context; 
 }

void lide_c_jitter_scheduler(lide_c_jitter_graph_context_type *context){
        int i;
        int j; 
        int k = 0;
        int Ws = 0;
        int Th = 0;
        float Op = 0.0;
        int arg_count = 3;
        int NUM = 0;
        int length = 0;
        int Residue = 0;
        double value = 0.0;
    Ws = context->Ws;
    Th = context->Th;
    Op = context->Op;
    length = context->length;
    FILE *in = NULL;
    fflush(stdout);
    in = lide_c_util_fopen(context->in_file, "r");
    printf("length: %d, Ws: %d, Op: %f", length, Ws, Op);
    NUM = (int)((length-Ws)/Ws/(1-Op))+1;
    Residue = length-NUM*Ws;
    if(Residue != 0)
        NUM++;

        for (k = 0; k<NUM; k++){
        i = 0;
        if(k == 0){
            for (j = 0; j < Ws-(int)(Ws*(1-Op)); j++)
                lide_c_util_guarded_execution(context->actors[i], context->descriptors[i]);
        }
        i = 0;
        if(Residue !=0 && k == NUM-1){
            for (j = 0; j < Residue; j++){
                lide_c_util_guarded_execution(context->actors[i], context->descriptors[i]);
            }
            for (j = Residue; j<Ws; j++)
                lide_c_fifo_write(context->fifos[0], &value);
        }else{
            for (j = 0; j < (int)(Ws*(1-Op)); j++){
                    lide_c_util_guarded_execution(context->actors[i], context->descriptors[i]);
            }
        }

        for(i=1;i<10;i++){
            lide_c_util_guarded_execution(context->actors[i], context->descriptors[i]);
        }       
    }
    fclose(in);


}

void lide_c_jitter_graph_terminate(
        lide_c_jitter_graph_context_type *context){
    /*terminate all fifo connections*/
    fclose(context->out_file);
    int i;
    for (i = 0; i < ACTOR_COUNT; i++){
        lide_c_fifo_free(context->fifos[i]);
    }

    /*terminate all actors*/
    lide_c_file_source_double_terminate(
            (lide_c_file_source_double_context_type *)(context->actors[ACTOR_XSOURCE]));
    
    lide_c_DVL_terminate(
            (lide_c_DVL_context_type *)(context->actors[ACTOR_DVL]));
                
    lide_c_STR_terminate(
            (lide_c_STR_context_type *)(context->actors[ACTOR_STR]));
                
    lide_c_FSM_s_terminate(
            (lide_c_FSM_s_context_type *)(context->actors[ACTOR_FSM_s]));
                    
    lide_c_TRT_terminate(
            (lide_c_TRT_context_type *)(context->actors[ACTOR_TRT]));
                        
    lide_c_RE_terminate(
            (lide_c_RE_context_type *)(context->actors[ACTOR_RE]));
    lide_c_RRE_terminate(
            (lide_c_RRE_context_type *)(context->actors[ACTOR_RRE]));
                                    
    lide_c_CRE_terminate(
            (lide_c_CRE_context_type *)(context->actors[ACTOR_CRE]));
    lide_c_Phase_terminate(
            (lide_c_Phase_context_type *)(context->actors[ACTOR_PHASE]));

    lide_c_file_sink_jitter_terminate(
            (lide_c_file_sink_jitter_context_type *)(context->actors[ACTOR_SINK]));

    /*Free the last memories*/
    free(context->fifos);
    free(context->actors);
    free(context->descriptors);
    free(context);
}
