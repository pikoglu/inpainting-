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
CMAKE_BINARY_DIR = /Users/felixfourreau/Desktop/projet_vacances/build-stereo-adaptive-weights_1.4-Desktop_Qt_5_13_1_clang_64bit-Debug

# Include any dependencies generated for this target.
include CMakeFiles/tiff2png.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/tiff2png.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/tiff2png.dir/flags.make

CMakeFiles/tiff2png.dir/io_png.c.o: CMakeFiles/tiff2png.dir/flags.make
CMakeFiles/tiff2png.dir/io_png.c.o: /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/io_png.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/felixfourreau/Desktop/projet_vacances/build-stereo-adaptive-weights_1.4-Desktop_Qt_5_13_1_clang_64bit-Debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/tiff2png.dir/io_png.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/tiff2png.dir/io_png.c.o   -c /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/io_png.c

CMakeFiles/tiff2png.dir/io_png.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/tiff2png.dir/io_png.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/io_png.c > CMakeFiles/tiff2png.dir/io_png.c.i

CMakeFiles/tiff2png.dir/io_png.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/tiff2png.dir/io_png.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/io_png.c -o CMakeFiles/tiff2png.dir/io_png.c.s

CMakeFiles/tiff2png.dir/io_tiff.c.o: CMakeFiles/tiff2png.dir/flags.make
CMakeFiles/tiff2png.dir/io_tiff.c.o: /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/io_tiff.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/felixfourreau/Desktop/projet_vacances/build-stereo-adaptive-weights_1.4-Desktop_Qt_5_13_1_clang_64bit-Debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/tiff2png.dir/io_tiff.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/tiff2png.dir/io_tiff.c.o   -c /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/io_tiff.c

CMakeFiles/tiff2png.dir/io_tiff.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/tiff2png.dir/io_tiff.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/io_tiff.c > CMakeFiles/tiff2png.dir/io_tiff.c.i

CMakeFiles/tiff2png.dir/io_tiff.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/tiff2png.dir/io_tiff.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/io_tiff.c -o CMakeFiles/tiff2png.dir/io_tiff.c.s

CMakeFiles/tiff2png.dir/tiff2png.cpp.o: CMakeFiles/tiff2png.dir/flags.make
CMakeFiles/tiff2png.dir/tiff2png.cpp.o: /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/tiff2png.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/felixfourreau/Desktop/projet_vacances/build-stereo-adaptive-weights_1.4-Desktop_Qt_5_13_1_clang_64bit-Debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/tiff2png.dir/tiff2png.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tiff2png.dir/tiff2png.cpp.o -c /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/tiff2png.cpp

CMakeFiles/tiff2png.dir/tiff2png.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tiff2png.dir/tiff2png.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/tiff2png.cpp > CMakeFiles/tiff2png.dir/tiff2png.cpp.i

CMakeFiles/tiff2png.dir/tiff2png.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tiff2png.dir/tiff2png.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4/tiff2png.cpp -o CMakeFiles/tiff2png.dir/tiff2png.cpp.s

# Object files for target tiff2png
tiff2png_OBJECTS = \
"CMakeFiles/tiff2png.dir/io_png.c.o" \
"CMakeFiles/tiff2png.dir/io_tiff.c.o" \
"CMakeFiles/tiff2png.dir/tiff2png.cpp.o"

# External object files for target tiff2png
tiff2png_EXTERNAL_OBJECTS =

tiff2png: CMakeFiles/tiff2png.dir/io_png.c.o
tiff2png: CMakeFiles/tiff2png.dir/io_tiff.c.o
tiff2png: CMakeFiles/tiff2png.dir/tiff2png.cpp.o
tiff2png: CMakeFiles/tiff2png.dir/build.make
tiff2png: third_party/tiff-4.0.10/libtiff/libtiff.a
tiff2png: third_party/jpeg-9e/libjpeg.a
tiff2png: third_party/libpng-1.6.39/libpng16d.a
tiff2png: /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX14.5.sdk/usr/lib/libz.tbd
tiff2png: /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX14.5.sdk/usr/lib/libm.tbd
tiff2png: /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX14.5.sdk/usr/lib/libz.tbd
tiff2png: CMakeFiles/tiff2png.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/felixfourreau/Desktop/projet_vacances/build-stereo-adaptive-weights_1.4-Desktop_Qt_5_13_1_clang_64bit-Debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable tiff2png"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tiff2png.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/tiff2png.dir/build: tiff2png

.PHONY : CMakeFiles/tiff2png.dir/build

CMakeFiles/tiff2png.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/tiff2png.dir/cmake_clean.cmake
.PHONY : CMakeFiles/tiff2png.dir/clean

CMakeFiles/tiff2png.dir/depend:
	cd /Users/felixfourreau/Desktop/projet_vacances/build-stereo-adaptive-weights_1.4-Desktop_Qt_5_13_1_clang_64bit-Debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4 /Users/felixfourreau/Desktop/projet_vacances/stereo-adaptive-weights_1.4 /Users/felixfourreau/Desktop/projet_vacances/build-stereo-adaptive-weights_1.4-Desktop_Qt_5_13_1_clang_64bit-Debug /Users/felixfourreau/Desktop/projet_vacances/build-stereo-adaptive-weights_1.4-Desktop_Qt_5_13_1_clang_64bit-Debug /Users/felixfourreau/Desktop/projet_vacances/build-stereo-adaptive-weights_1.4-Desktop_Qt_5_13_1_clang_64bit-Debug/CMakeFiles/tiff2png.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/tiff2png.dir/depend

