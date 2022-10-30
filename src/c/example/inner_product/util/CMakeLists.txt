CMAKE_MINIMUM_REQUIRED(VERSION 2.8 FATAL_ERROR)

#PROJECT(LIDEPUB C)

SET(CMAKE_EXPORT_COMPILE_COMMANDS on)

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/src/gems/actors/basic
    ${PROJECT_SOURCE_DIR}/src/gems/edges/basic
    ${PROJECT_SOURCE_DIR}/src/gems/actors/common
    ${PROJECT_SOURCE_DIR}/src/tools/runtime
    )

LINK_DIRECTORIES(
        $ENV{LIDEGEN}
        $ENV{LIDEGEN}/c
    )

ADD_EXECUTABLE(objtr_hist_gen_driver1.exe
    objtr_hist_gen_driver1.c
    )

TARGET_LINK_LIBRARIES(objtr_hist_gen_driver1.exe
        ${PROJECT_SOURCE_DIR}/src/gems/actors/basic/liblide_c_actors_basic.a
        ${PROJECT_SOURCE_DIR}/src/gems/edges/basic/liblide_c_edges_basic.a
        ${PROJECT_SOURCE_DIR}/src/tools/runtime/liblide_c_runtime.a )

INSTALL(TARGETS objtr_hist_gen_driver1.exe DESTINATION .)

