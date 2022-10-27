SET(source_list
    lide_c_multirate_fir.c
    )

ADD_LIBRARY(lide_c_dsp
    ${source_list})


INSTALL(TARGETS lide_c_dsp DESTINATION .)
