SET(source_list
    lide_c_hist_gen.c
    )

ADD_LIBRARY(lide_c_objtr
    ${source_list}
    )

INSTALL(TARGETS lide_c_objtr DESTINATION .)
