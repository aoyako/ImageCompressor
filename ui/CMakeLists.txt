cmake_minimum_required(VERSION 3.5)

project(compressor LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

set(SOURCE_FILES
  main.cpp
  mainwindow.cpp
  mainwindow.hpp
  mainwindow.ui
)

add_executable(ImageCompressor ${SOURCE_FILES})

target_link_libraries(ImageCompressor PRIVATE 
    Qt5::Widgets
)

# target_link_libraries(ImageCompressor PRIVATE
#     libCompressor
# )
