SET(source_list
    lide_c_cluster_computation.c
    lide_c_cluster_isolation.c
    lide_c_cluster_threshold.c
    lide_c_cluster_weight.c
    lide_c_jet_recon.c
    lide_c_overlap_filter.c
    lide_c_particle_id.c
    )

ADD_LIBRARY(lide_c_hep
    ${source_list}
    )


INSTALL(TARGETS lide_c_hep DESTINATION .)
