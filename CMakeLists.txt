cmake_minimum_required(VERSION 3.14)

project(ImageCompressing VERSION 0.1 LANGUAGES CXX)

#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)

set(CMAKE_CXX_FLAGS "-pthread")
set(CMAKE_CXX_FLAGS_RELEASE "-g3") 
# set(CMAKE_CXX_FLAGS_RELEASE "-O3") 

message($ENV{USER})
# add_compile_definitions(KERNEL_PATH="/home/$ENV{USER}/ImageResizer/kernel.cl")
add_compile_definitions(KERNEL_PATH="/home/$ENV{USER}/cpp/compressing/build/kernel.cl")

set(ALGORITHM_DIR "compressor/")
set(UI_DIR "ui/")

add_subdirectory(${ALGORITHM_DIR})
add_subdirectory(${UI_DIR})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/compressor/kernel.cl ${CMAKE_BINARY_DIR} COPYONLY)
