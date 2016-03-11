# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.2

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
CMAKE_SOURCE_DIR = /home/weima/MINAMI/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/weima/MINAMI/src

# Include any dependencies generated for this target.
include minami/CMakeFiles/minami.dir/depend.make

# Include the progress variables for this target.
include minami/CMakeFiles/minami.dir/progress.make

# Include the compile flags for this target's objects.
include minami/CMakeFiles/minami.dir/flags.make

minami/CMakeFiles/minami.dir/path.cpp.o: minami/CMakeFiles/minami.dir/flags.make
minami/CMakeFiles/minami.dir/path.cpp.o: minami/path.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/weima/MINAMI/src/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object minami/CMakeFiles/minami.dir/path.cpp.o"
	cd /home/weima/MINAMI/src/minami && g++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/minami.dir/path.cpp.o -c /home/weima/MINAMI/src/minami/path.cpp

minami/CMakeFiles/minami.dir/path.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/minami.dir/path.cpp.i"
	cd /home/weima/MINAMI/src/minami && g++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/weima/MINAMI/src/minami/path.cpp > CMakeFiles/minami.dir/path.cpp.i

minami/CMakeFiles/minami.dir/path.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/minami.dir/path.cpp.s"
	cd /home/weima/MINAMI/src/minami && g++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/weima/MINAMI/src/minami/path.cpp -o CMakeFiles/minami.dir/path.cpp.s

minami/CMakeFiles/minami.dir/path.cpp.o.requires:
.PHONY : minami/CMakeFiles/minami.dir/path.cpp.o.requires

minami/CMakeFiles/minami.dir/path.cpp.o.provides: minami/CMakeFiles/minami.dir/path.cpp.o.requires
	$(MAKE) -f minami/CMakeFiles/minami.dir/build.make minami/CMakeFiles/minami.dir/path.cpp.o.provides.build
.PHONY : minami/CMakeFiles/minami.dir/path.cpp.o.provides

minami/CMakeFiles/minami.dir/path.cpp.o.provides.build: minami/CMakeFiles/minami.dir/path.cpp.o

# Object files for target minami
minami_OBJECTS = \
"CMakeFiles/minami.dir/path.cpp.o"

# External object files for target minami
minami_EXTERNAL_OBJECTS =

minami/libminami.so: minami/CMakeFiles/minami.dir/path.cpp.o
minami/libminami.so: minami/CMakeFiles/minami.dir/build.make
minami/libminami.so: snap-core/libSnap.so
minami/libminami.so: 3rdparty/g3log/libg3log.a
minami/libminami.so: glib-core/libGlib.so
minami/libminami.so: minami/CMakeFiles/minami.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX shared library libminami.so"
	cd /home/weima/MINAMI/src/minami && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/minami.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
minami/CMakeFiles/minami.dir/build: minami/libminami.so
.PHONY : minami/CMakeFiles/minami.dir/build

minami/CMakeFiles/minami.dir/requires: minami/CMakeFiles/minami.dir/path.cpp.o.requires
.PHONY : minami/CMakeFiles/minami.dir/requires

minami/CMakeFiles/minami.dir/clean:
	cd /home/weima/MINAMI/src/minami && $(CMAKE_COMMAND) -P CMakeFiles/minami.dir/cmake_clean.cmake
.PHONY : minami/CMakeFiles/minami.dir/clean

minami/CMakeFiles/minami.dir/depend:
	cd /home/weima/MINAMI/src && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/weima/MINAMI/src /home/weima/MINAMI/src/minami /home/weima/MINAMI/src /home/weima/MINAMI/src/minami /home/weima/MINAMI/src/minami/CMakeFiles/minami.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : minami/CMakeFiles/minami.dir/depend
