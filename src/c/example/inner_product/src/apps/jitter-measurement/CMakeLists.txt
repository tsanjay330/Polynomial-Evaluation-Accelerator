SET(source_list
    lide_c_CRE.c
    lide_c_DVL.c
    lide_c_FSM_s.c
    lide_c_Phase.c
    lide_c_RE.c
    lide_c_RRE.c
    lide_c_STR.c
    lide_c_TRT.c
    lide_c_fifoext.c
    lide_c_file_sink_jitter.c
    lide_c_jitter_graph.c
    )

ADD_LIBRARY(lide_c_actors_jitter ${source_list})

INSTALL(TARGETS lide_c_actors_jitter DESTINATION .)
