SET(source_list
    lide_c_file_read.c
    lide_c_file_write.c
    )

ADD_LIBRARY(lide_c_communication_common
    ${source_list})

INSTALL(TARGETS lide_c_communication_common DESTINATION .)
