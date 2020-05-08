#pragma once 

#include <limits>

#include <bmp_image.hpp>
#include <image_algorithms.hpp>

namespace algorithm {
    
    class SeamFinder {
    private:
        template<typename Img, typename Color>
        std::vector<size_t> createVerticalSeams(const image::Image<Img, Color> &img, int N, size_t *seam_image) {
            std::vector<size_t> line;
            line.reserve(img.width()*N);
            for (int i = 1; i < img.height(); ++i) {
                seam_image[i*img.width() + img.width()-1] = std::numeric_limits<size_t>::max();
                seam_image[i*img.width()] = std::numeric_limits<size_t>::max();
            }
            for (int col = 1; col < img.height(); ++col) {
                for (int row = 1; row < img.width()-1; ++row) {
                    size_t min = std::min(std::min(seam_image[(col-1)*img.width() + row-1], seam_image[(col-1)*img.width() + row]), seam_image[(col-1)*img.width() + row+1]);
                    size_t addition = (img.pixel(row, col).red()+128 <= Params::MIN_BORDER_COLOR) ? 0 : ((img.pixel(row, col).red()+128 <= Params::MAX_BORDER_COLOR) ? Params::MID_ADDITION : Params::MAX_ADDITION);
                    seam_image[col*img.width() + row] = min + img.pixel(row, col).red() + 128 + addition;
                }
            }
            
            for (int num = 0; num < N; ++num) {
                int min_index = 1;
                for (int index = 0; index < img.width()-1; ++index) {
                    if (seam_image[(img.height()-1)*img.width() + index] < seam_image[(img.height()-1)*img.width() + min_index]) {
                        min_index = index;
                    }
                }
                int current_index = min_index;
                for (int i = img.height()-1; i >= 1; --i) {
                    seam_image[current_index + img.width()*i] = std::numeric_limits<size_t>::max();
                    line.push_back(current_index + i*img.width());
                    int left_offset = -1;
                    int right_offset = 1;
                    
                    if ((seam_image[current_index - 1 + img.width()*(i-1)] == std::numeric_limits<size_t>::max()) &&
                        (seam_image[current_index + img.width()*(i-1)] == std::numeric_limits<size_t>::max()) &&
                        (seam_image[current_index + 1 + img.width()*(i-1)] == std::numeric_limits<size_t>::max())) {
                            while ((current_index+left_offset >= 2) && (seam_image[current_index + left_offset + img.width()*(i-1)] == std::numeric_limits<size_t>::max())) {
                                left_offset--;
                            }
                            while ((current_index+right_offset < img.width()-3) && (seam_image[current_index + right_offset + img.width()*(i-1)] == std::numeric_limits<size_t>::max())) {
                                right_offset++;
                            }
                    }
                    if ((seam_image[current_index+left_offset + img.width()*(i-1)] <= seam_image[current_index + img.width()*(i-1)]) &&
                        (seam_image[current_index+left_offset + img.width()*(i-1)] <= seam_image[current_index+right_offset + img.width()*(i-1)])) {
                        current_index += left_offset;
                    } else if ((seam_image[current_index+right_offset + img.width()*(i-1)] <= seam_image[current_index+left_offset + img.width()*(i-1)]) &&
                        (seam_image[current_index+right_offset + img.width()*(i-1)] <= seam_image[current_index + img.width()*(i-1)])) {
                        current_index += right_offset;
                    } else if (seam_image[current_index + img.width()*(i-1)] == std::numeric_limits<size_t>::max()) {
                        current_index += left_offset;
                    }
                }
                line.push_back(static_cast<size_t>(current_index));
                seam_image[current_index] = std::numeric_limits<size_t>::max();
            }
            return line;
        }

        template<typename Img, typename Color>
        std::vector<size_t> createHorisontalSeams(const image::Image<Img, Color> &img, int N, size_t *seam_image) {
            std::vector<size_t> line;
            line.reserve(img.width()*N);
            for (int i = 1; i < img.width(); ++i) {
                seam_image[i] = std::numeric_limits<size_t>::max();
                seam_image[i+(img.height()-1)*img.width()] = std::numeric_limits<size_t>::max();
            }
            for (int col = 1; col < img.width(); ++col) {
                for (int row = 1; row < img.height()-1; ++row) {
                    size_t min = std::min(std::min(seam_image[row*img.width() + col-1], seam_image[(row-1)*img.width() + col-1]), seam_image[(row+1)*img.width() + col-1]);
                    size_t addition = (img.pixel(col, row).red()+128 <= Params::MIN_BORDER_COLOR) ? 0 : ((img.pixel(col, row).red()+128 <= Params::MAX_BORDER_COLOR) ? Params::MID_ADDITION : Params::MAX_ADDITION);
                    seam_image[row*img.width() + col] = min + img.pixel(col, row).red() + 128 + addition;
                }
            }
            
            for (int num = 0; num < N; ++num) {
                int min_index = 1;
                for (int index = 1; index < img.height()-1; ++index) {
                    if (seam_image[(index)*img.width()+img.width()-1] < seam_image[(min_index)*img.width()+img.width()-1]) {
                        min_index = index;
                    }
                }
                int current_index = min_index;
                for (int i = img.width()-1; i >= 1; --i) {
                    seam_image[current_index*img.width() + i] = std::numeric_limits<size_t>::max();
                    line.push_back(current_index*img.width() + i);
                    int top_offset = -1;
                    int bot_offset = 1;
                    
                    if ((seam_image[(current_index+top_offset)*img.width() + i-1] == std::numeric_limits<size_t>::max()) &&
                        (seam_image[(current_index+bot_offset)*img.width() + i-1] == std::numeric_limits<size_t>::max()) &&
                        (seam_image[(current_index)*img.width() + i-1] == std::numeric_limits<size_t>::max())) {
                            while ((current_index+top_offset >= 2) && (seam_image[(current_index+top_offset)*img.width() + i-1] == std::numeric_limits<size_t>::max())) {
                                top_offset--;
                            }
                            while ((current_index+bot_offset < img.height()-3) && (seam_image[(current_index+bot_offset)*img.width() + i-1] == std::numeric_limits<size_t>::max())) {
                                bot_offset++;
                            }
                    }
                    if ((seam_image[(current_index+top_offset)*img.width() + i-1] <= seam_image[current_index*img.width() + i-1]) &&
                        (seam_image[(current_index+top_offset)*img.width() + i-1] <= seam_image[(current_index+bot_offset)*img.width() + i-1])) {
                        current_index += top_offset;
                    } else if ((seam_image[(current_index+bot_offset)*img.width() + i-1] <= seam_image[current_index*img.width() + i-1]) &&
                        (seam_image[(current_index+bot_offset)*img.width() + i-1] <= seam_image[(current_index+top_offset)*img.width() + i-1])) {
                        current_index += bot_offset;
                    } else if (seam_image[(current_index)*img.width() + i-1] == std::numeric_limits<size_t>::max()) {
                        current_index += top_offset;
                    }
                }
                line.push_back(current_index*img.width());
                seam_image[current_index*img.width()] = std::numeric_limits<size_t>::max();
            }
            return line;
        }
    };
    
}