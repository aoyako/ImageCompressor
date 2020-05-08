#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#include <limits>
#include <vector>
#include <chrono>
#include <future>
#include <algorithm>


int main(int argc, char *argv[]) {
	std::string name = "CASTLE.BMP";
    int ROWS = 20;
    int COLS = 5;
    if (argc > 1) {
	    name = argv[1];
    }
    if (argc > 2) {
	    ROWS = std::atoi(argv[2])/100;
    }
    Image<BMPImage, BMPColor> img(name);
    auto gray = GrayLevel(img);
    gray.save("GRAY.BMP");
    auto noise = NoiseRemove(gray);
    noise.save("NOISE.BMP");
    auto sobel = SobelEdges(noise);
    sobel.save("SOBEL.BMP");
    auto treshoold = DoubleTreshold(sobel);
    treshoold.save("TRESHOLD.BMP");
    Image<BMPImage, BMPColor> fat("FAT.BMP");
    auto result = img;
    int N = 50;
    
    auto start = std::chrono::high_resolution_clock::now();
    size_t *seam_image = new size_t [img.width()*img.height()];
    for (int i = 0; i < img.width()*img.height(); ++i) {
        seam_image[i] = std::numeric_limits<size_t>::max();
        if (i / img.width() == 0) seam_image[i] = 0;
    }
    
    for (int i = 0; i < ROWS; ++i) {
        auto seamed = std::move(createVerticalSeams(fat, N, seam_image));
        std::sort(seamed.begin(), seamed.end(), std::greater<size_t>());
        result.getImage().updateSide(fat.width()-N, 1, seamed);
        fat.getImage().updateSide(fat.width()-N, 1, seamed);
        std::cout<<"Iteration: "<<i<<std::endl;
    }
    for (int i = 0; i < img.width()*img.height(); ++i) {
        seam_image[i] = std::numeric_limits<size_t>::max();
        if (i % img.width() == 0) seam_image[i] = 0;
    }
    

    for (int i = 0; i < COLS; ++i) {
        auto seamed = std::move(createHorisontalSeams(fat, N, seam_image));
        std::sort(seamed.begin(), seamed.end(), std::greater<size_t>());
        result.getImage().updateSide(fat.height()-N, -1, seamed);
        fat.getImage().updateSide(fat.height()-N, -1, seamed);
        std::cout<<"Iteration: "<<i<<std::endl;
    }
    
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s\n";

    delete[] seam_image;
    fat.save("FATTER.BMP");
    result.save("GHGHGHG.BMP");
    return 0;
}
