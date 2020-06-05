#include <gtest/gtest.h>

#include "../compressor/include/seam.hpp"

TEST(NextPowerOfTwo, Siple) {
    EXPECT_EQ(algorithm::utils::nextPowerOf2(0), 1);
    EXPECT_EQ(algorithm::utils::nextPowerOf2(1), 1);
    EXPECT_EQ(algorithm::utils::nextPowerOf2(2), 2);
    EXPECT_EQ(algorithm::utils::nextPowerOf2(3), 4);
    EXPECT_EQ(algorithm::utils::nextPowerOf2(4), 4);
    EXPECT_EQ(algorithm::utils::nextPowerOf2(5), 8);
}

TEST(NextPowerOfTwo, Medium) {
    EXPECT_EQ(algorithm::utils::nextPowerOf2(120), 128);
    EXPECT_EQ(algorithm::utils::nextPowerOf2(240), 256);
    EXPECT_EQ(algorithm::utils::nextPowerOf2(500), 512);
    EXPECT_EQ(algorithm::utils::nextPowerOf2(600), 1024);
    EXPECT_EQ(algorithm::utils::nextPowerOf2(800), 1024);
}

TEST(NextPowerOfTwo, Hard) {
    EXPECT_EQ(algorithm::utils::nextPowerOf2(1024), 1024);
    EXPECT_EQ(algorithm::utils::nextPowerOf2(1025), 2048);
    EXPECT_EQ(algorithm::utils::nextPowerOf2(1100), 2048);
    EXPECT_EQ(algorithm::utils::nextPowerOf2(2049), 4096);
    EXPECT_EQ(algorithm::utils::nextPowerOf2(3000), 4096);
}
