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
CMAKE_COMMAND = /Applications/CMake.app/Contents/bin/cmake

# The command to remove a file.
RM = /Applications/CMake.app/Contents/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/build

# Include any dependencies generated for this target.
include CMakeFiles/show_weights.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/show_weights.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/show_weights.dir/flags.make

CMakeFiles/show_weights.dir/image.cpp.o: CMakeFiles/show_weights.dir/flags.make
CMakeFiles/show_weights.dir/image.cpp.o: ../image.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/show_weights.dir/image.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/show_weights.dir/image.cpp.o -c /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/image.cpp

CMakeFiles/show_weights.dir/image.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/show_weights.dir/image.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/image.cpp > CMakeFiles/show_weights.dir/image.cpp.i

CMakeFiles/show_weights.dir/image.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/show_weights.dir/image.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/image.cpp -o CMakeFiles/show_weights.dir/image.cpp.s

CMakeFiles/show_weights.dir/io_png.c.o: CMakeFiles/show_weights.dir/flags.make
CMakeFiles/show_weights.dir/io_png.c.o: ../io_png.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/show_weights.dir/io_png.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/show_weights.dir/io_png.c.o   -c /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/io_png.c

CMakeFiles/show_weights.dir/io_png.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/show_weights.dir/io_png.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/io_png.c > CMakeFiles/show_weights.dir/io_png.c.i

CMakeFiles/show_weights.dir/io_png.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/show_weights.dir/io_png.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/io_png.c -o CMakeFiles/show_weights.dir/io_png.c.s

CMakeFiles/show_weights.dir/io_tiff.c.o: CMakeFiles/show_weights.dir/flags.make
CMakeFiles/show_weights.dir/io_tiff.c.o: ../io_tiff.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/show_weights.dir/io_tiff.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/show_weights.dir/io_tiff.c.o   -c /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/io_tiff.c

CMakeFiles/show_weights.dir/io_tiff.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/show_weights.dir/io_tiff.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/io_tiff.c > CMakeFiles/show_weights.dir/io_tiff.c.i

CMakeFiles/show_weights.dir/io_tiff.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/show_weights.dir/io_tiff.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/io_tiff.c -o CMakeFiles/show_weights.dir/io_tiff.c.s

CMakeFiles/show_weights.dir/show_weights.cpp.o: CMakeFiles/show_weights.dir/flags.make
CMakeFiles/show_weights.dir/show_weights.cpp.o: ../show_weights.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/show_weights.dir/show_weights.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/show_weights.dir/show_weights.cpp.o -c /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/show_weights.cpp

CMakeFiles/show_weights.dir/show_weights.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/show_weights.dir/show_weights.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/show_weights.cpp > CMakeFiles/show_weights.dir/show_weights.cpp.i

CMakeFiles/show_weights.dir/show_weights.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/show_weights.dir/show_weights.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/show_weights.cpp -o CMakeFiles/show_weights.dir/show_weights.cpp.s

# Object files for target show_weights
show_weights_OBJECTS = \
"CMakeFiles/show_weights.dir/image.cpp.o" \
"CMakeFiles/show_weights.dir/io_png.c.o" \
"CMakeFiles/show_weights.dir/io_tiff.c.o" \
"CMakeFiles/show_weights.dir/show_weights.cpp.o"

# External object files for target show_weights
show_weights_EXTERNAL_OBJECTS =

show_weights: CMakeFiles/show_weights.dir/image.cpp.o
show_weights: CMakeFiles/show_weights.dir/io_png.c.o
show_weights: CMakeFiles/show_weights.dir/io_tiff.c.o
show_weights: CMakeFiles/show_weights.dir/show_weights.cpp.o
show_weights: CMakeFiles/show_weights.dir/build.make
show_weights: third_party/tiff-4.0.10/libtiff/libtiff.a
show_weights: third_party/jpeg-9e/libjpeg.a
show_weights: third_party/libpng-1.6.39/libpng16d.a
show_weights: /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX14.5.sdk/usr/lib/libz.tbd
show_weights: /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX14.5.sdk/usr/lib/libm.tbd
show_weights: /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX14.5.sdk/usr/lib/libz.tbd
show_weights: CMakeFiles/show_weights.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable show_weights"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/show_weights.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/show_weights.dir/build: show_weights

.PHONY : CMakeFiles/show_weights.dir/build

CMakeFiles/show_weights.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/show_weights.dir/cmake_clean.cmake
.PHONY : CMakeFiles/show_weights.dir/clean

CMakeFiles/show_weights.dir/depend:
	cd /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4 /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4 /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/build /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/build /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/build/CMakeFiles/show_weights.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/show_weights.dir/depend

