SET(source_list
    lide_c_fifo.c
    )

ADD_LIBRARY(lide_c_edges_basic ${source_list})


INSTALL(TARGETS lide_c_edges_basic DESTINATION .)
