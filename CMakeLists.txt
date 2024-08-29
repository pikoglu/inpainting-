cmake_minimum_required(VERSION 3.13)
project(Inpainting)

set(CMAKE_OSX_ARCHITECTURES "x86_64")

find_package(OpenMP)
find_package(PNG)

if(NOT PNG_FOUND)
    find_package(ZLIB) # zlib is needed by libPNG
    if(NOT ZLIB_FOUND)
        set(ZLIB_BUILD_EXAMPLES OFF)
        add_subdirectory(third_party/zlib-1.3.1)
        set(ZLIB_FOUND TRUE)
        set(PNG_BUILD_ZLIB ON)
        set_target_properties(${ZLIB_LIBRARIES} PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${ZLIB_INCLUDE_DIRS}")
        add_library(ZLIB::ZLIB ALIAS "${ZLIB_LIBRARIES}")
    endif()

    if(NOT PNG_FOUND)
        set(PNG_SHARED OFF)
        set(PNG_TESTS OFF)
        set(PNG_TOOLS OFF)
        set(SKIP_INSTALL_ALL 1)
        add_subdirectory(third_party/libpng-1.6.43)
        set_target_properties(${PNG_LIBRARIES} PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${PNG_INCLUDE_DIRS}")
        add_library(PNG::PNG ALIAS "${PNG_LIBRARIES}")
    endif()
endif()

set(SRC
    cmdLine.h
    image.cpp image.h
    io_png.c io_png.h
    node.cpp node.h
    fft.cpp fft.h
    main.cpp)

add_executable(bpInpainting ${SRC})
target_link_libraries(bpInpainting PRIVATE PNG::PNG)

if(OpenMP_CXX_FOUND)
    target_link_libraries(bpInpainting PUBLIC OpenMP::OpenMP_CXX)
    target_compile_options(bpInpainting PRIVATE ${OpenMP_CXX_FLAGS})
endif()

if(UNIX)
    set_target_properties(bpInpainting PROPERTIES
                          COMPILE_FLAGS "-Wall -Wextra ")
endif(UNIX)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
endif(MSVC)
