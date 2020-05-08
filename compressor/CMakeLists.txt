set(HEADER_FILES
    include/image_algorithms.hpp
    include/seam.hpp
    include/image.hpp
    
    include/BMP/bmp_color.hpp
    include/BMP/bmp_image.hpp
)

set(SOURCE_FILES
    src/image_algorithms.cpp
    src/image.tpp
    
    src/BMP/bmp_color.cpp
    src/BMP/bmp_image.cpp
)

add_library(libCompressor
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

target_include_directories(libCompressor
    PUBLIC    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> 
        $<INSTALL_INTERFACE:lib/include>
)

target_include_directories(libCompressor
    PUBLIC    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/BMP> 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:lib/include/BMP>
)
