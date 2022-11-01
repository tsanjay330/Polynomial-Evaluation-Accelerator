SET(source_list
    lide_c_pea.c
    )

ADD_LIBRARY(lide_c_pea
    ${source_list}
    )

INSTALL(TARGETS lide_c_pea DESTINATION .)
