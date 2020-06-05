cmake_minimum_required(VERSION 3.12)

project(ImageCompressing VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)

set(CMAKE_CXX_FLAGS "-pthread")

add_compile_definitions(USER_PATH="/home/$ENV{USER}")
set(DEBUG ON CACHE BOOL "For local debugging")
if (DEBUG)
    set(CMAKE_BUILD_TYPE Debug)
    add_compile_definitions(KERNEL_PATH="kernel.cl")
    set(CMAKE_CXX_FLAGS_RELEASE "-g3")
else()
    set(CMAKE_BUILD_TYPE Release)
    add_compile_definitions(KERNEL_PATH="/home/$ENV{USER}/ImageResizer/kernel.cl")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3") 
endif()

set(ALGORITHM_DIR "compressor/")
set(UI_DIR "ui/")
set(TESTS_DIR "tests/")
set(MODULE_GTEST "modules/gtest/")

add_subdirectory(${ALGORITHM_DIR})
add_subdirectory(${UI_DIR})
add_subdirectory(${MODULE_GTEST})
add_subdirectory(${TESTS_DIR})


configure_file(${CMAKE_CURRENT_SOURCE_DIR}/compressor/kernel.cl ${CMAKE_BINARY_DIR} COPYONLY)
