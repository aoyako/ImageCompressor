#include <execution.hpp>

#include <sstream>

Execution::Execution() {
    std::vector<cl::Platform> all_platforms;
    cl::Platform::get(&all_platforms);

    if (all_platforms.size() == 0) {
        std::cerr<<" No platforms found. Check OpenCL installation!\n";
        exit(1);
    }

    for (auto const &platrform : all_platforms) {
        initDevices(platrform);
    }

    device_option = all_platforms.size()-1;
}

void Execution::initDevices(const cl::Platform &platform) {
    devices.push_back(device::Params{});
    devices.back().platform = platform;

    std::vector<cl::Device> all_devices;
    platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
    if(all_devices.size() == 0){
        std::cerr<<"No devices found for "<<platform.getInfo<CL_PLATFORM_NAME>()<<". Check OpenCL installation!\n";
    }

    devices.back().device = all_devices[0];
    std::cout<<"Found device: "<<devices.back().device.getInfo<CL_DEVICE_NAME>()<<std::endl;

    devices.back().context = cl::Context({devices.back().device});
    cl::Program::Sources sources;

    std::ifstream code(KERNEL_PATH);
    if (!code) {
        std::cout<<"Cannot find \"kernel.cl\" file!"<<std::endl;
    }
    std::string kernel_code;
    {
        std::ostringstream ss;
        ss << code.rdbuf();
        kernel_code = ss.str();
    }

    sources.push_back({kernel_code.c_str(), kernel_code.length()});

    devices.back().program = std::make_shared<cl::Program>(devices.back().context, sources);
    if (devices.back().program->build({devices.back().device}) != CL_SUCCESS) {
        std::cout <<"Error building: "<<  devices.back().program->getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices.back().device) << std::endl;
        exit(1);
    }
    devices.back().queue = std::make_shared<cl::CommandQueue>(devices.back().context, devices.back().device);
}

device::Params Execution::getDeviceParams() {
    return devices[device_option];
}

execution::Params Execution::getExecutionParams() {
    return execution::Params{advanced_alg};
}

void Execution::switchToGPU() {
    device_option = 0;
}

void Execution::switchToCPU() {
    device_option = devices.size()-1;
}

void Execution::setPrettyView(bool value) {
    advanced_alg = value;
}
