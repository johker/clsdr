# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

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
CMAKE_SOURCE_DIR = /home/johker/workspace/plot_htm/examples

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/johker/workspace/plot_htm/examples/build

# Include any dependencies generated for this target.
include CMakeFiles/phtest.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/phtest.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/phtest.dir/flags.make

CMakeFiles/phtest.dir/phtest.cpp.o: CMakeFiles/phtest.dir/flags.make
CMakeFiles/phtest.dir/phtest.cpp.o: ../phtest.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/johker/workspace/plot_htm/examples/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/phtest.dir/phtest.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/phtest.dir/phtest.cpp.o -c /home/johker/workspace/plot_htm/examples/phtest.cpp

CMakeFiles/phtest.dir/phtest.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/phtest.dir/phtest.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/johker/workspace/plot_htm/examples/phtest.cpp > CMakeFiles/phtest.dir/phtest.cpp.i

CMakeFiles/phtest.dir/phtest.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/phtest.dir/phtest.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/johker/workspace/plot_htm/examples/phtest.cpp -o CMakeFiles/phtest.dir/phtest.cpp.s

# Object files for target phtest
phtest_OBJECTS = \
"CMakeFiles/phtest.dir/phtest.cpp.o"

# External object files for target phtest
phtest_EXTERNAL_OBJECTS =

phtest: CMakeFiles/phtest.dir/phtest.cpp.o
phtest: CMakeFiles/phtest.dir/build.make
phtest: /usr/lib/libcurses.so
phtest: CMakeFiles/phtest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/johker/workspace/plot_htm/examples/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable phtest"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/phtest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/phtest.dir/build: phtest

.PHONY : CMakeFiles/phtest.dir/build

CMakeFiles/phtest.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/phtest.dir/cmake_clean.cmake
.PHONY : CMakeFiles/phtest.dir/clean

CMakeFiles/phtest.dir/depend:
	cd /home/johker/workspace/plot_htm/examples/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/johker/workspace/plot_htm/examples /home/johker/workspace/plot_htm/examples /home/johker/workspace/plot_htm/examples/build /home/johker/workspace/plot_htm/examples/build /home/johker/workspace/plot_htm/examples/build/CMakeFiles/phtest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/phtest.dir/depend

