cmake_minimum_required(VERSION 3.5)

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

  include/image_adapter.hpp
  src/image_adapter.cpp
)

add_executable(ImageCompressor ${SOURCE_FILES})

target_link_libraries(ImageCompressor PRIVATE 
    Qt5::Widgets
)

target_link_libraries(ImageCompressor PRIVATE
    libCompressor
)

target_include_directories(libCompressor
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}include/>
)
