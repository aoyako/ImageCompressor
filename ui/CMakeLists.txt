cmake_minimum_required(VERSION 3.5)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets Core REQUIRED)
find_package(OpenCL 2.2 REQUIRED)

set(SOURCE_FILES
  main.cpp
  mainwindow.cpp
  mainwindow.hpp
  mainwindow.ui

  include/image_adapter.hpp
  include/image_container.hpp
  include/saver.hpp
  include/execution.hpp

  src/image_adapter.cpp
  src/image_container.cpp
  src/saver.cpp
  src/execution.cpp
)

add_executable(ImageCompressor ${SOURCE_FILES})

target_link_libraries(ImageCompressor PRIVATE OpenCL::OpenCL)

target_link_libraries(ImageCompressor PRIVATE 
    Qt5::Widgets
    Qt5::Core
)

target_link_libraries(ImageCompressor PRIVATE
    libCompressor
)

target_include_directories(libCompressor
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}include/>
)
