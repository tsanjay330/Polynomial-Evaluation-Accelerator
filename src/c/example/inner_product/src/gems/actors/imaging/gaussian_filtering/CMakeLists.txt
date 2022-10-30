SET(source_list
    lide_c_bmp_file_read_halo.c
    lide_c_bmp_file_write.c
    lide_c_file_source_fcv.c
    lide_c_gfilter.c
    lide_c_invert.c
    lide_c_t.c
    )

ADD_LIBRARY(lide_c_imaging_gaussian_filtering
    ${source_list})


INSTALL(TARGETS lide_c_imaging_gaussian_filtering DESTINATION .)
