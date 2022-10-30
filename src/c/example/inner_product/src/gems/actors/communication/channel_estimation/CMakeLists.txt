SET(source_list
    lide_c_auto_correlation_computation.c
    lide_c_coefficient_computation.c
    lide_c_cross_correlation_computation.c
    lide_c_matrix_inversion.c
    lide_c_noise_adder.c
    lide_c_response_computation.c
    )

ADD_LIBRARY(lide_c_communication_channel_estimation
    ${source_list}
    )

INSTALL(TARGETS lide_c_communication_channel_estimation DESTINATION .)
