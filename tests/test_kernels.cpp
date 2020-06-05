#include <gtest/gtest.h>

#include <QImage>
#include "../compressor/include/seam.hpp"
#include "../ui/include/image_adapter.hpp"
#include "../ui/include/execution.hpp"

TEST(Kernels, LoadKernel) {
    Execution exc;
    QImage img("../install/icon.png");
    image::Image<image::BMPImage, image::BMPColor> result = ImageWrapper(&img);

    algorithm::Algorithm::resizeBMPImage(result, 0, 0, exc.getDeviceParams(), exc.getExecutionParams());
}
