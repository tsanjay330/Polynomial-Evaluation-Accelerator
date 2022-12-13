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
    ./src/c/PEA_actor/gems/actors/common
    ./src/c/PEA_actor/gems/actors/basic
    ./src/c/PEA_actor/gems/actors/pea
    ./src/c/PEA_actor/gems/edges/basic
    ./src/c/PEA_actor/tools/runtime
    )

ADD_SUBDIRECTORY(src/c/PEA_actor/gems/actors/basic)
ADD_SUBDIRECTORY(src/c/PEA_actor/gems/actors/pea)
ADD_SUBDIRECTORY(src/c/PEA_actor/gems/edges/basic)
ADD_SUBDIRECTORY(src/c/PEA_actor/tools/runtime)
ADD_SUBDIRECTORY(util)

