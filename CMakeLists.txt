cmake_minimum_required(VERSION 3.14)

project(ImageCompressing VERSION 0.1 LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS ON)

# set(CMAKE_CXX_FLAGS "-pthread")
# set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_RELEASE "-g3")

set(ALGORITHM_DIR "compressor/")
set(UI_DIR "ui/")

add_subdirectory(${ALGORITHM_DIR})
add_subdirectory(${UI_DIR})
