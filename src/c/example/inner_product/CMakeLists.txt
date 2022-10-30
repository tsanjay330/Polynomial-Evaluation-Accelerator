CMAKE_MINIMUM_REQUIRED(VERSION 2.8 FATAL_ERROR)

PROJECT(project)

INCLUDE(CheckIncludeFile)
INCLUDE(CheckIncludeFileCXX)
INCLUDE(CheckIncludeFiles)
#ADD_COMPILE_OPTIONS(-fPIE)
SET(CMAKE_EXPORT_COMPILE_COMMANDS on)
SET(CMAKE_C_STANDARD 11)

FIND_PACKAGE(OpenCL QUIET)

# Language C
INCLUDE_DIRECTORIES(
    ./src/gems/actors/common
    ./src/gems/actors/basic
    #./src/gems/actors/communication/channel_estimation
    #./src/gems/actors/communication/common
    #${PROJECT_SOURCE_DIR}/lang/c/src/gems/actors/dsp
    #${PROJECT_SOURCE_DIR}/lang/c/src/gems/actors/hep
    #${PROJECT_SOURCE_DIR}/lang/c/src/gems/actors/imaging/gaussian_filtering
    ./src/gems/edges/basic
    ./src/gems/graph/common
    ./src/gems/graph/basic
    ./src/tools/runtime
    #${PROJECT_SOURCE_DIR}/lang/c/src/apps/jitter-measurement
    )

ADD_SUBDIRECTORY(src/gems/actors/basic)
#ADD_SUBDIRECTORY(src/gems/actors/communication/channel_estimation)
#ADD_SUBDIRECTORY(src/gems/actors/communication/common)
#ADD_SUBDIRECTORY(lang/c/src/gems/actors/dsp)
#ADD_SUBDIRECTORY(lang/c/src/gems/actors/hep)
#ADD_SUBDIRECTORY(lang/c/src/gems/actors/imaging/gaussian_filtering)
ADD_SUBDIRECTORY(src/gems/edges/basic)
ADD_SUBDIRECTORY(src/gems/graph/basic)
ADD_SUBDIRECTORY(src/tools/runtime)
ADD_SUBDIRECTORY(util)
#ADD_SUBDIRECTORY(lang/c/src/apps/jitter-measurement)

# Language OCL


# code in the test directory should be compiled in each test directory.
#IF(EXISTS ${PROJECT_SOURCE_DIR}/test)
    #ADD_SUBDIRECTORY(test)
#    message(STATUS "test dir found")
#ENDIF(EXISTS ${PROJECT_SOURCE_DIR}/test)
