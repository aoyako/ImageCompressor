find_package(OpenCL 2.2 REQUIRED)

set(HEADER_FILES
    include/seam.hpp
    include/image.hpp
    
    include/BMP/bmp_color.hpp
    include/BMP/bmp_image.hpp
)

set(SOURCE_FILES
    src/image.tpp
    src/seam.cpp
    
    src/BMP/bmp_color.cpp
    src/BMP/bmp_image.cpp

    kernel.cl
)

add_library(libCompressor
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

target_link_libraries(libCompressor OpenCL::OpenCL)

target_include_directories(libCompressor
    PUBLIC    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> 
        $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
