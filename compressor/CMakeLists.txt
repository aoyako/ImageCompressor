set(HEADER_FILES
    include/algorithm.hpp
    include/image.hpp
    
    include/BMP/bmp_color.hpp
    include/BMP/bmp_image.hpp
)

set(SOURCE_FILES
    src/algorithm.cpp
    src/image.cpp
    
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
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/BMP> 
        $<INSTALL_INTERFACE:lib/include>
        $<INSTALL_INTERFACE:lib/include/BMP>
)
