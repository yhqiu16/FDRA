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
CMAKE_SOURCE_DIR = /home/tallanlong/fdra/app-compiler

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/tallanlong/fdra/app-compiler/build

# Include any dependencies generated for this target.
include llvm-pass/CMakeFiles/CDFGPass.dir/depend.make

# Include the progress variables for this target.
include llvm-pass/CMakeFiles/CDFGPass.dir/progress.make

# Include the compile flags for this target's objects.
include llvm-pass/CMakeFiles/CDFGPass.dir/flags.make

llvm-pass/CMakeFiles/CDFGPass.dir/src/CDFGPass.cpp.o: llvm-pass/CMakeFiles/CDFGPass.dir/flags.make
llvm-pass/CMakeFiles/CDFGPass.dir/src/CDFGPass.cpp.o: ../llvm-pass/src/CDFGPass.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tallanlong/fdra/app-compiler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object llvm-pass/CMakeFiles/CDFGPass.dir/src/CDFGPass.cpp.o"
	cd /home/tallanlong/fdra/app-compiler/build/llvm-pass && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/CDFGPass.dir/src/CDFGPass.cpp.o -c /home/tallanlong/fdra/app-compiler/llvm-pass/src/CDFGPass.cpp

llvm-pass/CMakeFiles/CDFGPass.dir/src/CDFGPass.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/CDFGPass.dir/src/CDFGPass.cpp.i"
	cd /home/tallanlong/fdra/app-compiler/build/llvm-pass && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tallanlong/fdra/app-compiler/llvm-pass/src/CDFGPass.cpp > CMakeFiles/CDFGPass.dir/src/CDFGPass.cpp.i

llvm-pass/CMakeFiles/CDFGPass.dir/src/CDFGPass.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/CDFGPass.dir/src/CDFGPass.cpp.s"
	cd /home/tallanlong/fdra/app-compiler/build/llvm-pass && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tallanlong/fdra/app-compiler/llvm-pass/src/CDFGPass.cpp -o CMakeFiles/CDFGPass.dir/src/CDFGPass.cpp.s

llvm-pass/CMakeFiles/CDFGPass.dir/src/llvm_cdfg.cpp.o: llvm-pass/CMakeFiles/CDFGPass.dir/flags.make
llvm-pass/CMakeFiles/CDFGPass.dir/src/llvm_cdfg.cpp.o: ../llvm-pass/src/llvm_cdfg.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tallanlong/fdra/app-compiler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object llvm-pass/CMakeFiles/CDFGPass.dir/src/llvm_cdfg.cpp.o"
	cd /home/tallanlong/fdra/app-compiler/build/llvm-pass && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/CDFGPass.dir/src/llvm_cdfg.cpp.o -c /home/tallanlong/fdra/app-compiler/llvm-pass/src/llvm_cdfg.cpp

llvm-pass/CMakeFiles/CDFGPass.dir/src/llvm_cdfg.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/CDFGPass.dir/src/llvm_cdfg.cpp.i"
	cd /home/tallanlong/fdra/app-compiler/build/llvm-pass && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tallanlong/fdra/app-compiler/llvm-pass/src/llvm_cdfg.cpp > CMakeFiles/CDFGPass.dir/src/llvm_cdfg.cpp.i

llvm-pass/CMakeFiles/CDFGPass.dir/src/llvm_cdfg.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/CDFGPass.dir/src/llvm_cdfg.cpp.s"
	cd /home/tallanlong/fdra/app-compiler/build/llvm-pass && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tallanlong/fdra/app-compiler/llvm-pass/src/llvm_cdfg.cpp -o CMakeFiles/CDFGPass.dir/src/llvm_cdfg.cpp.s

llvm-pass/CMakeFiles/CDFGPass.dir/src/llvm_cdfg_edge.cpp.o: llvm-pass/CMakeFiles/CDFGPass.dir/flags.make
llvm-pass/CMakeFiles/CDFGPass.dir/src/llvm_cdfg_edge.cpp.o: ../llvm-pass/src/llvm_cdfg_edge.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tallanlong/fdra/app-compiler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object llvm-pass/CMakeFiles/CDFGPass.dir/src/llvm_cdfg_edge.cpp.o"
	cd /home/tallanlong/fdra/app-compiler/build/llvm-pass && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/CDFGPass.dir/src/llvm_cdfg_edge.cpp.o -c /home/tallanlong/fdra/app-compiler/llvm-pass/src/llvm_cdfg_edge.cpp

llvm-pass/CMakeFiles/CDFGPass.dir/src/llvm_cdfg_edge.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/CDFGPass.dir/src/llvm_cdfg_edge.cpp.i"
	cd /home/tallanlong/fdra/app-compiler/build/llvm-pass && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tallanlong/fdra/app-compiler/llvm-pass/src/llvm_cdfg_edge.cpp > CMakeFiles/CDFGPass.dir/src/llvm_cdfg_edge.cpp.i

llvm-pass/CMakeFiles/CDFGPass.dir/src/llvm_cdfg_edge.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/CDFGPass.dir/src/llvm_cdfg_edge.cpp.s"
	cd /home/tallanlong/fdra/app-compiler/build/llvm-pass && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tallanlong/fdra/app-compiler/llvm-pass/src/llvm_cdfg_edge.cpp -o CMakeFiles/CDFGPass.dir/src/llvm_cdfg_edge.cpp.s

llvm-pass/CMakeFiles/CDFGPass.dir/src/llvm_cdfg_node.cpp.o: llvm-pass/CMakeFiles/CDFGPass.dir/flags.make
llvm-pass/CMakeFiles/CDFGPass.dir/src/llvm_cdfg_node.cpp.o: ../llvm-pass/src/llvm_cdfg_node.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/tallanlong/fdra/app-compiler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object llvm-pass/CMakeFiles/CDFGPass.dir/src/llvm_cdfg_node.cpp.o"
	cd /home/tallanlong/fdra/app-compiler/build/llvm-pass && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/CDFGPass.dir/src/llvm_cdfg_node.cpp.o -c /home/tallanlong/fdra/app-compiler/llvm-pass/src/llvm_cdfg_node.cpp

llvm-pass/CMakeFiles/CDFGPass.dir/src/llvm_cdfg_node.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/CDFGPass.dir/src/llvm_cdfg_node.cpp.i"
	cd /home/tallanlong/fdra/app-compiler/build/llvm-pass && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/tallanlong/fdra/app-compiler/llvm-pass/src/llvm_cdfg_node.cpp > CMakeFiles/CDFGPass.dir/src/llvm_cdfg_node.cpp.i

llvm-pass/CMakeFiles/CDFGPass.dir/src/llvm_cdfg_node.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/CDFGPass.dir/src/llvm_cdfg_node.cpp.s"
	cd /home/tallanlong/fdra/app-compiler/build/llvm-pass && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/tallanlong/fdra/app-compiler/llvm-pass/src/llvm_cdfg_node.cpp -o CMakeFiles/CDFGPass.dir/src/llvm_cdfg_node.cpp.s

# Object files for target CDFGPass
CDFGPass_OBJECTS = \
"CMakeFiles/CDFGPass.dir/src/CDFGPass.cpp.o" \
"CMakeFiles/CDFGPass.dir/src/llvm_cdfg.cpp.o" \
"CMakeFiles/CDFGPass.dir/src/llvm_cdfg_edge.cpp.o" \
"CMakeFiles/CDFGPass.dir/src/llvm_cdfg_node.cpp.o"

# External object files for target CDFGPass
CDFGPass_EXTERNAL_OBJECTS =

llvm-pass/libCDFGPass.so: llvm-pass/CMakeFiles/CDFGPass.dir/src/CDFGPass.cpp.o
llvm-pass/libCDFGPass.so: llvm-pass/CMakeFiles/CDFGPass.dir/src/llvm_cdfg.cpp.o
llvm-pass/libCDFGPass.so: llvm-pass/CMakeFiles/CDFGPass.dir/src/llvm_cdfg_edge.cpp.o
llvm-pass/libCDFGPass.so: llvm-pass/CMakeFiles/CDFGPass.dir/src/llvm_cdfg_node.cpp.o
llvm-pass/libCDFGPass.so: llvm-pass/CMakeFiles/CDFGPass.dir/build.make
llvm-pass/libCDFGPass.so: llvm-pass/CMakeFiles/CDFGPass.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/tallanlong/fdra/app-compiler/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX shared module libCDFGPass.so"
	cd /home/tallanlong/fdra/app-compiler/build/llvm-pass && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/CDFGPass.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
llvm-pass/CMakeFiles/CDFGPass.dir/build: llvm-pass/libCDFGPass.so

.PHONY : llvm-pass/CMakeFiles/CDFGPass.dir/build

llvm-pass/CMakeFiles/CDFGPass.dir/clean:
	cd /home/tallanlong/fdra/app-compiler/build/llvm-pass && $(CMAKE_COMMAND) -P CMakeFiles/CDFGPass.dir/cmake_clean.cmake
.PHONY : llvm-pass/CMakeFiles/CDFGPass.dir/clean

llvm-pass/CMakeFiles/CDFGPass.dir/depend:
	cd /home/tallanlong/fdra/app-compiler/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/tallanlong/fdra/app-compiler /home/tallanlong/fdra/app-compiler/llvm-pass /home/tallanlong/fdra/app-compiler/build /home/tallanlong/fdra/app-compiler/build/llvm-pass /home/tallanlong/fdra/app-compiler/build/llvm-pass/CMakeFiles/CDFGPass.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : llvm-pass/CMakeFiles/CDFGPass.dir/depend

