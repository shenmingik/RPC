# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_SOURCE_DIR = /home/ubuntu/code/RPC

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ubuntu/code/RPC/build

# Include any dependencies generated for this target.
include example/callee/CMakeFiles/provider.dir/depend.make

# Include the progress variables for this target.
include example/callee/CMakeFiles/provider.dir/progress.make

# Include the compile flags for this target's objects.
include example/callee/CMakeFiles/provider.dir/flags.make

example/callee/CMakeFiles/provider.dir/UserService.cpp.o: example/callee/CMakeFiles/provider.dir/flags.make
example/callee/CMakeFiles/provider.dir/UserService.cpp.o: ../example/callee/UserService.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ubuntu/code/RPC/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object example/callee/CMakeFiles/provider.dir/UserService.cpp.o"
	cd /home/ubuntu/code/RPC/build/example/callee && /usr/bin/g++-7  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/provider.dir/UserService.cpp.o -c /home/ubuntu/code/RPC/example/callee/UserService.cpp

example/callee/CMakeFiles/provider.dir/UserService.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/provider.dir/UserService.cpp.i"
	cd /home/ubuntu/code/RPC/build/example/callee && /usr/bin/g++-7 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ubuntu/code/RPC/example/callee/UserService.cpp > CMakeFiles/provider.dir/UserService.cpp.i

example/callee/CMakeFiles/provider.dir/UserService.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/provider.dir/UserService.cpp.s"
	cd /home/ubuntu/code/RPC/build/example/callee && /usr/bin/g++-7 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ubuntu/code/RPC/example/callee/UserService.cpp -o CMakeFiles/provider.dir/UserService.cpp.s

example/callee/CMakeFiles/provider.dir/UserService.cpp.o.requires:

.PHONY : example/callee/CMakeFiles/provider.dir/UserService.cpp.o.requires

example/callee/CMakeFiles/provider.dir/UserService.cpp.o.provides: example/callee/CMakeFiles/provider.dir/UserService.cpp.o.requires
	$(MAKE) -f example/callee/CMakeFiles/provider.dir/build.make example/callee/CMakeFiles/provider.dir/UserService.cpp.o.provides.build
.PHONY : example/callee/CMakeFiles/provider.dir/UserService.cpp.o.provides

example/callee/CMakeFiles/provider.dir/UserService.cpp.o.provides.build: example/callee/CMakeFiles/provider.dir/UserService.cpp.o


# Object files for target provider
provider_OBJECTS = \
"CMakeFiles/provider.dir/UserService.cpp.o"

# External object files for target provider
provider_EXTERNAL_OBJECTS =

../bin/provider: example/callee/CMakeFiles/provider.dir/UserService.cpp.o
../bin/provider: example/callee/CMakeFiles/provider.dir/build.make
../bin/provider: example/callee/CMakeFiles/provider.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ubuntu/code/RPC/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../../bin/provider"
	cd /home/ubuntu/code/RPC/build/example/callee && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/provider.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
example/callee/CMakeFiles/provider.dir/build: ../bin/provider

.PHONY : example/callee/CMakeFiles/provider.dir/build

example/callee/CMakeFiles/provider.dir/requires: example/callee/CMakeFiles/provider.dir/UserService.cpp.o.requires

.PHONY : example/callee/CMakeFiles/provider.dir/requires

example/callee/CMakeFiles/provider.dir/clean:
	cd /home/ubuntu/code/RPC/build/example/callee && $(CMAKE_COMMAND) -P CMakeFiles/provider.dir/cmake_clean.cmake
.PHONY : example/callee/CMakeFiles/provider.dir/clean

example/callee/CMakeFiles/provider.dir/depend:
	cd /home/ubuntu/code/RPC/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ubuntu/code/RPC /home/ubuntu/code/RPC/example/callee /home/ubuntu/code/RPC/build /home/ubuntu/code/RPC/build/example/callee /home/ubuntu/code/RPC/build/example/callee/CMakeFiles/provider.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : example/callee/CMakeFiles/provider.dir/depend

