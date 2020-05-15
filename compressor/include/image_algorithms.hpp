#pragma once

#include <image.hpp>
#include <cmath>

namespace algorithm {
    
    struct Params {
        Params(size_t min_border_color, size_t max_border_color, size_t max_addition, size_t mid_addition) :
            MIN_BORDER_COLOR(min_border_color),
            MAX_BORDER_COLOR(max_border_color),
            MAX_ADDITION(max_addition),
            MID_ADDITION(mid_addition) {}
        Params() = default;


        constexpr static double PI = 3.14159265358979323846;
        constexpr static size_t DEFAULT_MIN_BORDER_COLOR = 120;
        constexpr static size_t DEFAULT_MAX_BORDER_COLOR = 200;
        constexpr static size_t DEFAULT_MAX_ADDITION = 200;
        constexpr static size_t DEFAULT_MID_ADDITION = 100;

        size_t MIN_BORDER_COLOR = DEFAULT_MIN_BORDER_COLOR;
        size_t MAX_BORDER_COLOR = DEFAULT_MAX_BORDER_COLOR;
        size_t MAX_ADDITION = DEFAULT_MAX_ADDITION;
        size_t MID_ADDITION = DEFAULT_MID_ADDITION;

        bool fat_lines = true;
    };

    class Algorithm {
    public:
        template<typename Img, typename Color>
        static image::Image<Img, Color> GrayLevel(const image::Image<Img, Color> &img, const Params &param) {
            image::Image<Img, Color> gray_img(img);
            for (int j = 0; j < gray_img.height(); ++j) {
                for (int i = 0; i < gray_img.width(); ++i) {
//                    char color = 0.299*gray_img.pixel(i, j).red() + 0.587*gray_img.pixel(i, j).green() + 0.114*gray_img.pixel(i, j).blue();
                    char color = 0.33*gray_img.pixel(i, j).red() + 0.33*gray_img.pixel(i, j).green() + 0.34*gray_img.pixel(i, j).blue();
                    gray_img.setPixel(i, j, Color(color, color, color));
                }
            }
            return gray_img;
        }
        
        template<typename Img, typename Color>
        static image::Image<Img, Color> NoiseRemove(const image::Image<Img, Color> &img, const Params &param) {
            image::Image<Img, Color> noise_img(img);
            
            int n = 5;
             int Gaussian[5][5] = {
                 {1, 4, 7, 4, 1},
                 {4, 16, 26, 16, 4},
                 {7, 26, 41, 26, 7},
                 {4, 16, 26, 16, 4},
                 {1, 4, 7, 4, 1},
             };
//            int Gaussian[3][3] = {
//                {1, 2, 1},
//                {2, 4, 2},
//                {1, 2, 1},
//            };
            int sum = 0;
            
            for (int j = n/2; j < noise_img.height()-n/2; ++j) {
                for (int i = n/2; i < noise_img.width()-n/2; ++i) {
                    sum = 0;
                    int color = 0;
                    for (int x = -n/2; x <= n/2; ++x) {
                        for (int y = -n/2; y <= n/2; ++y) {
                        sum += Gaussian[n/2 + x][n/2 + y];
                        color += Gaussian[n/2 + x][n/2 + y]*(img.pixel(i-x, j-y).red()+128);
                        }
                    }
                    color /= sum;
                    color -= 128;
                    noise_img.setPixel(i, j, Color(color, color, color));
                }
            }
            return noise_img;
        }
        
        template<typename Img, typename Color>
        static image::Image<Img, Color> SobelEdges(const image::Image<Img, Color> &img, const Params &param) {
            image::Image<Img, Color> result_image(img);
            
            int n = 3;
            int SobelX[3][3] = {
                {3, 0, -3},
                {10, 0, -10},
                {3, 0, -3},
            };
            int SobelY[3][3] = {
                {3, 10, 3},
                {0, 0, 0},
                {-3, -10, -3},
            };
            
            struct info {
                int magnitude = 0;
                char direction = 0;
            };
            info *sobel_image = new info [img.width()*img.height()];
            
            int max = 0;
            
            for (int j = n/2; j < result_image.height()-n/2; ++j) {
                for (int i = n/2; i < result_image.width()-n/2; ++i) {
                    
                    int gx = 0;
                    int gy = 0;
                    for (int y = -n/2; y <= n/2; ++y) {
                        for (int x = -n/2; x <= n/2; ++x) {
                            gx += (result_image.pixel(i+x, j+y).red()+128)*SobelX[n/2+x][n/2+y];
                            gy += (result_image.pixel(i+x, j+y).red()+128)*SobelY[n/2+x][n/2+y];
                        }
                    }
                    sobel_image[j*img.width() + i].magnitude = std::sqrt(gx*gx + gy*gy);
                    if (sobel_image[j*img.width() + i].magnitude > max) {
                        max = sobel_image[j*img.width() + i].magnitude;
                    }
                    double direction;
                    if (gx == 0) {
                        if (gy < 0) {
                            direction = -89.99999;
                        } else {
                            direction = 89.99999;
                        }
                    } else {
                        direction = std::atan(gy/gx)*180/Params::PI;
                    }
                    direction += 90;
                    if ((direction <= 22.5 )|| (direction > 157.5)) {
                        sobel_image[j*img.width() + i].direction = 0;
                    } else if (direction <= 67.5) {
                        sobel_image[j*img.width() + i].direction = 1;
                    } else if (direction <= 112.5) {
                        sobel_image[j*img.width() + i].direction = 2;
                    } else if (direction <= 157.5) {
                        sobel_image[j*img.width() + i].direction = 3;
                    }            
                }
            }
            
            for (int j = 1; j < result_image.height()-1; ++j) {
                for (int i = 1; i < result_image.width()-1; ++i) {
                    sobel_image[j*img.width() + i].magnitude = 255*sobel_image[j*img.width() + i].magnitude/max;
                    char magnitude = sobel_image[j*img.width() + i].magnitude;
                    result_image.setPixel(i, j, Color(magnitude, magnitude, magnitude));
                }
            }

            if (param.fat_lines) {
                delete[] sobel_image;
                return result_image;
            }
//            result_image.save("FAT.BMP");

            for (int j = 1; j < result_image.height()-1; ++j) {
                for (int i = 1; i < result_image.width()-1; ++i) {
                    char direction = sobel_image[j*img.width() + i].direction;
                    if (direction == 0) {
                        if ((sobel_image[j*img.width() + i].magnitude < sobel_image[j*img.width() + i + 1].magnitude) ||
                            (sobel_image[j*img.width() + i].magnitude < sobel_image[j*img.width() + i - 1].magnitude))
                        {
                            sobel_image[j*img.width() + i].magnitude = 0;
                        }
                    } else if (direction == 1) {
                        if ((sobel_image[j*img.width() + i].magnitude < sobel_image[(j+1)*img.width() + i + 1].magnitude) ||
                            (sobel_image[j*img.width() + i].magnitude < sobel_image[(j-1)*img.width() + i - 1].magnitude))
                        {
                            sobel_image[j*img.width() + i].magnitude = 0;
                        }
                    } else if (direction == 2) {
                        if ((sobel_image[j*img.width() + i].magnitude < sobel_image[(j+1)*img.width() + i].magnitude) ||
                            (sobel_image[j*img.width() + i].magnitude < sobel_image[(j-1)*img.width() + i].magnitude))
                        {
                            sobel_image[j*img.width() + i].magnitude = 0;
                        }
                    } else if (direction == 3) {
                        if ((sobel_image[j*img.width() + i].magnitude < sobel_image[(j-1)*img.width() + i + 1].magnitude) ||
                            (sobel_image[j*img.width() + i].magnitude < sobel_image[(j+1)*img.width() + i - 1].magnitude))
                        {
                            sobel_image[j*img.width() + i].magnitude = 0;
                        }
                    }
                    int magnitude = sobel_image[j*img.width() + i].magnitude;
                    result_image.setPixel(i, j, Color(magnitude, magnitude, magnitude));
                }
            }
            
            delete[] sobel_image;
            return result_image;
        }
        
        template<typename Img, typename Color>
        static image::Image<Img, Color> DoubleTreshold(const image::Image<Img, Color> &img, const Params &param) {
            image::Image<Img, Color> result_image(img);
            for (int j = 1; j < result_image.height()-1; ++j) {
                for (int i = 1; i < result_image.width()-1; ++i) {
                    if (img.pixel(i, j).red() < param.MIN_BORDER_COLOR) {
                        result_image.setPixel(i, j, Color(0, 0, 0));
                    } else if ((img.pixel(i, j).red() < param.MAX_BORDER_COLOR) && (img.pixel(i, j).red() > param.MIN_BORDER_COLOR)) {
                        char color = 0;
                        for (int x = -1; x <= 1; ++x) {
                            for (int y = -1; y <= 1; ++y) {
                                if (result_image.pixel(i, j).red() > param.MAX_BORDER_COLOR) {
                                    color = img.pixel(i, j).red();
                                    break;
                                }
                            }
                        }
                        result_image.setPixel(i, j, Color(color, color, color));
                    }
                }
            }
            return result_image;
        }

        Params param;
    };
    
    
}
