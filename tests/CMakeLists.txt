project(test)

find_package(OpenCL 2.2 REQUIRED)
find_package(Qt5 COMPONENTS Core Gui REQUIRED)

add_executable(${PROJECT_NAME}
    main.cpp
    test_execution.cpp
    test_image_container.cpp
    test_kernels.cpp
    test_utils.cpp
    ../compressor/src/seam.cpp
    ../ui/src/execution.cpp
    ../ui/src/image_container.cpp
    ../ui/src/image_adapter.cpp
)

target_link_libraries(${PROJECT_NAME} 
    gtest
    libCompressor
    OpenCL::OpenCL
    Qt5::Core
    Qt5::Gui
)
