cmake_minimum_required(VERSION 2.8)
project(Inpainting)

set(CMAKE_OSX_ARCHITECTURES "x86_64")

#find_package(PNG)
if(NOT PNG_FOUND)
    find_package(ZLIB) # zlib is needed by libPNG
    if(NOT ZLIB_FOUND)
        add_subdirectory(third_party/zlib-1.2.13)
        set(ZLIB_FOUND TRUE)
    endif()
    include_directories(${ZLIB_INCLUDE_DIRS})

    if(NOT PNG_FOUND)
        set(SKIP_INSTALL_ALL 1)
        add_subdirectory(third_party/libpng-1.6.39)
    endif()
endif()

add_definitions(${PNG_DEFINITIONS})
include_directories(${PNG_INCLUDE_DIRS})

set(SRC
    cmdLine.h
    image.cpp image.h
    io_png.c io_png.h
    node.cpp node.h
    fft.cpp fft.h
    main.cpp)

add_executable(bpInpainting ${SRC})
target_link_libraries(bpInpainting ${PNG_LIBRARIES})

if(UNIX)
    set_target_properties(bpInpainting PROPERTIES
                          COMPILE_FLAGS "-Wall -Wextra ")
endif(UNIX)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
endif(MSVC)
