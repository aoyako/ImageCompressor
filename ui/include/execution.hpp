#pragma once

#ifdef __APPLE__
    #include <OpenCL/cl.hpp>
#else
    #include <CL/cl.hpp>
#endif
#include <vector>
#include <seam.hpp>

/**
 * @brief Execution stores all available platforms and selected one
 */
class Execution {
public:
    Execution();

    /**
     * @brief Returns selected device options
     */
    device::Params getParams();

    /**
     * @brief Setup OpenCL code to specified platform
     */
    void initDevices(const cl::Platform &platform);

    /**
     * @brief Switches to GPU calculations
     */
    void switchToGPU();

    /**
     * @brief Switches to CPU calculations
     */
    void switchToCPU();

    /**
     * @brief List of available devices
     */
    std::vector<device::Params> devices;

    /**
     * @brief Selected device (index in "devices")
     */
    size_t device_option = 0;
};
