#include <gtest/gtest.h>

#include <QImage>
#include "../compressor/include/seam.hpp"
#include "../ui/include/image_container.hpp"


TEST(ImageContainer, setImage) {
    ImageContainer cont;
    QImage img("../install/icon.png");
    cont.setImage(img);
    EXPECT_EQ(cont.image, img);
}

TEST(ImageContainer, setBackup) {
    ImageContainer cont;
    QImage img("../install/icon.png");
    cont.setImage(img);
    cont.setBackup();
    cont.image.setPixelColor(0, 0, QColor{1, 1, 1});
    EXPECT_NE(cont.image, img);
    cont.backup();
    EXPECT_EQ(cont.image, img);
}
