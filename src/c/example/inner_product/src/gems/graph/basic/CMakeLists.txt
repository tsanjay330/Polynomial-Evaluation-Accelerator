SET(source_list
    lide_c_inner_product_graph.c
    )

ADD_LIBRARY(lide_c_graph_basic ${source_list})
INCLUDE_DIRECTORIES(
$ENV{UXLIDEC}/src/gems/edges/basic
$ENV{UXLIDEC}/src/gems/actors/basic
$ENV{UXLIDEC}/src/tools/runtime
)
TARGET_LINK_LIBRARIES(lide_c_graph_basic

$ENV{LIDEGEN}/liblide_c_actors_basic.a
$ENV{LIDEGEN}/liblide_c_edges_basic.a
$ENV{LIDEGEN}/liblide_c_runtime.a
        )

INSTALL(TARGETS lide_c_graph_basic DESTINATION .)
