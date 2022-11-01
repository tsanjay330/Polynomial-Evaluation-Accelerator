CMAKE_MINIMUM_REQUIRED(VERSION 2.8 FATAL_ERROR)

#PROJECT(LIDEPUB C)

SET(CMAKE_EXPORT_COMPILE_COMMANDS on)

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/src/c/PEA_actor/gems/actors/basic
    ${PROJECT_SOURCE_DIR}/src/c/PEA_actor/gems/edges/basic
    ${PROJECT_SOURCE_DIR}/src/c/PEA_actor/gems/actors/common
    ${PROJECT_SOURCE_DIR}/src/c/PEA_actor/gems/actors/pea
    ${PROJECT_SOURCE_DIR}/src/c/PEA_actor/tools/runtime
    )

LINK_DIRECTORIES(
        $ENV{LIDEGEN}
        $ENV{LIDEGEN}/c
    )

ADD_EXECUTABLE(lide_c_pea_driver.exe
    lide_c_pea_driver.c
    )

TARGET_LINK_LIBRARIES(lide_c_pea_driver.exe
        ${PROJECT_SOURCE_DIR}/src/c/PEA_actor/gems/actors/basic/liblide_c_actors_basic.a
        ${PROJECT_SOURCE_DIR}/src/c/PEA_actor/gems/edges/basic/liblide_c_edges_basic.a
        ${PROJECT_SOURCE_DIR}/src/c/PEA_actor/tools/runtime/liblide_c_runtime.a 
        ${PROJECT_SOURCE_DIR}/src/c/PEA_actor/gems/actors/pea/liblide_c_pea.a 
		-lm
		)

INSTALL(TARGETS lide_c_pea_driver.exe DESTINATION .)

