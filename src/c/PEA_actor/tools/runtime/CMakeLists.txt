SET(source_list 
    lide_c_util.c
    )

ADD_LIBRARY(lide_c_runtime ${source_list})


INSTALL(TARGETS lide_c_runtime DESTINATION .)
