# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /homes/e/r/erinq20/home/enee408c/project/team1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /homes/e/r/erinq20/home/enee408c/project/team1

# Include any dependencies generated for this target.
include src/c/PEA_actor/gems/edges/basic/CMakeFiles/lide_c_edges_basic.dir/depend.make

# Include the progress variables for this target.
include src/c/PEA_actor/gems/edges/basic/CMakeFiles/lide_c_edges_basic.dir/progress.make

# Include the compile flags for this target's objects.
include src/c/PEA_actor/gems/edges/basic/CMakeFiles/lide_c_edges_basic.dir/flags.make

src/c/PEA_actor/gems/edges/basic/CMakeFiles/lide_c_edges_basic.dir/lide_c_fifo.c.o: src/c/PEA_actor/gems/edges/basic/CMakeFiles/lide_c_edges_basic.dir/flags.make
src/c/PEA_actor/gems/edges/basic/CMakeFiles/lide_c_edges_basic.dir/lide_c_fifo.c.o: src/c/PEA_actor/gems/edges/basic/lide_c_fifo.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/homes/e/r/erinq20/home/enee408c/project/team1/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object src/c/PEA_actor/gems/edges/basic/CMakeFiles/lide_c_edges_basic.dir/lide_c_fifo.c.o"
	cd /homes/e/r/erinq20/home/enee408c/project/team1/src/c/PEA_actor/gems/edges/basic && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/lide_c_edges_basic.dir/lide_c_fifo.c.o   -c /homes/e/r/erinq20/home/enee408c/project/team1/src/c/PEA_actor/gems/edges/basic/lide_c_fifo.c

src/c/PEA_actor/gems/edges/basic/CMakeFiles/lide_c_edges_basic.dir/lide_c_fifo.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/lide_c_edges_basic.dir/lide_c_fifo.c.i"
	cd /homes/e/r/erinq20/home/enee408c/project/team1/src/c/PEA_actor/gems/edges/basic && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /homes/e/r/erinq20/home/enee408c/project/team1/src/c/PEA_actor/gems/edges/basic/lide_c_fifo.c > CMakeFiles/lide_c_edges_basic.dir/lide_c_fifo.c.i

src/c/PEA_actor/gems/edges/basic/CMakeFiles/lide_c_edges_basic.dir/lide_c_fifo.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/lide_c_edges_basic.dir/lide_c_fifo.c.s"
	cd /homes/e/r/erinq20/home/enee408c/project/team1/src/c/PEA_actor/gems/edges/basic && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /homes/e/r/erinq20/home/enee408c/project/team1/src/c/PEA_actor/gems/edges/basic/lide_c_fifo.c -o CMakeFiles/lide_c_edges_basic.dir/lide_c_fifo.c.s

# Object files for target lide_c_edges_basic
lide_c_edges_basic_OBJECTS = \
"CMakeFiles/lide_c_edges_basic.dir/lide_c_fifo.c.o"

# External object files for target lide_c_edges_basic
lide_c_edges_basic_EXTERNAL_OBJECTS =

src/c/PEA_actor/gems/edges/basic/liblide_c_edges_basic.a: src/c/PEA_actor/gems/edges/basic/CMakeFiles/lide_c_edges_basic.dir/lide_c_fifo.c.o
src/c/PEA_actor/gems/edges/basic/liblide_c_edges_basic.a: src/c/PEA_actor/gems/edges/basic/CMakeFiles/lide_c_edges_basic.dir/build.make
src/c/PEA_actor/gems/edges/basic/liblide_c_edges_basic.a: src/c/PEA_actor/gems/edges/basic/CMakeFiles/lide_c_edges_basic.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/homes/e/r/erinq20/home/enee408c/project/team1/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library liblide_c_edges_basic.a"
	cd /homes/e/r/erinq20/home/enee408c/project/team1/src/c/PEA_actor/gems/edges/basic && $(CMAKE_COMMAND) -P CMakeFiles/lide_c_edges_basic.dir/cmake_clean_target.cmake
	cd /homes/e/r/erinq20/home/enee408c/project/team1/src/c/PEA_actor/gems/edges/basic && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/lide_c_edges_basic.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/c/PEA_actor/gems/edges/basic/CMakeFiles/lide_c_edges_basic.dir/build: src/c/PEA_actor/gems/edges/basic/liblide_c_edges_basic.a

.PHONY : src/c/PEA_actor/gems/edges/basic/CMakeFiles/lide_c_edges_basic.dir/build

src/c/PEA_actor/gems/edges/basic/CMakeFiles/lide_c_edges_basic.dir/clean:
	cd /homes/e/r/erinq20/home/enee408c/project/team1/src/c/PEA_actor/gems/edges/basic && $(CMAKE_COMMAND) -P CMakeFiles/lide_c_edges_basic.dir/cmake_clean.cmake
.PHONY : src/c/PEA_actor/gems/edges/basic/CMakeFiles/lide_c_edges_basic.dir/clean

src/c/PEA_actor/gems/edges/basic/CMakeFiles/lide_c_edges_basic.dir/depend:
	cd /homes/e/r/erinq20/home/enee408c/project/team1 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /homes/e/r/erinq20/home/enee408c/project/team1 /homes/e/r/erinq20/home/enee408c/project/team1/src/c/PEA_actor/gems/edges/basic /homes/e/r/erinq20/home/enee408c/project/team1 /homes/e/r/erinq20/home/enee408c/project/team1/src/c/PEA_actor/gems/edges/basic /homes/e/r/erinq20/home/enee408c/project/team1/src/c/PEA_actor/gems/edges/basic/CMakeFiles/lide_c_edges_basic.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/c/PEA_actor/gems/edges/basic/CMakeFiles/lide_c_edges_basic.dir/depend
