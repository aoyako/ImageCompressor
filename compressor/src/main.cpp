#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#include <limits>
#include <vector>
#include <chrono>
#include <future>
#include <algorithm>

constexpr const double PI = 3.14159265358979323846;
constexpr const int MIN_BORDER_COLOR = 120;
constexpr const int MAX_BORDER_COLOR = 200;
constexpr const int MAX_ADDITION = 200;
constexpr const int MID_ADDITION = 100;

class BMPImage
{
public:
    BMPImage(std::string filename) {
        std::ifstream file;
        file.open(filename, std::ios::in | std::ios::binary);
        if (file.is_open()) {
            int last = file.seekg(-1, std::ios_base::end).tellg();
            int file_size = last - file.seekg(0).tellg();
            
            file.read(info, 54);
            
            std::memcpy(&img_width, info + 18, sizeof(int));
            std::memcpy(&img_height, info + 22, sizeof(int));
            
            int offset;
            std::memcpy(&offset, info + 10, sizeof(int));
            
            int addition = 0;
            while ((img_width*3 + addition) % 4 != 0) ++addition;
            
            size_t size = img_width*img_height;
            char *color_data = new char[size*3 + addition*img_height];
            file.read(color_data, size*3);
            data = new BMPColor [size];
            
            int read = 54;
            int nums_of_offset = 0;
            for(int i = 0; i < size; ++i) {
                    data[i] = BMPColor(color_data[i*3+2 + nums_of_offset*addition], color_data[i*3+1 + nums_of_offset*addition], color_data[i*3 + nums_of_offset*addition]);
                    read++;
                    if ((i+1) % (img_width) == 0) {
                        nums_of_offset++;
                        continue;
                    }
            }
            
            std::cout<<"read: "<<read<<std::endl;
            std::cout<<offset<<std::endl;
            
            delete[] color_data;
            file.close();
        } else {
            throw std::runtime_error("Cannot open BMPImage file");
        }
    }
    
    BMPImage(const BMPImage &img) {
        img_width = img.img_width;
        img_height = img.img_height;
        std::copy(img.info, img.info+54, info);
        data = new BMPColor [img_width*img_height];
        for (int j = 0; j < img_height; ++j) {
            for (int i = 0; i < img_width; ++i) {
                data[j*img_width + i] = img.data[j*img_width + i];
            }
        }
        std::cout<<"copy"<<std::endl;
    }
    BMPImage(BMPImage &&img) {
        img_width = img.img_width;
        img_height = img.img_height;
        std::copy(img.info, img.info+54, info);
        if (data)
            delete[] data;
        data = img.data;
        img.data = nullptr;
        std::cout<<"move"<<std::endl;
    }
    
    ~BMPImage() {
        if (data) {
            delete[] data;
        }
    }
    
    BMPColor pixel(int i, int j) const{
        return data[j*img_width + i];
    }
    void setPixel(int i, int j, BMPColor color) {
        data[j*img_width + i] = color;
    }
    
    void save(std::string filename) const {
        std::ofstream file;
        file.open(filename, std::ios::out | std::ios::binary);
        if (file.is_open()) {
            file.write(info, 18);
//             std::cout<<"WIDTH: "<<img_width<<std::endl;
            file.write((const char*) &img_width, 4);
            file.write((const char*) &img_height, 4);
            file.write(info+26, 28);
            
            int addition = 0;
            while ((3*img_width + addition) % 4 != 0) ++addition;
            std::cout<<addition<<std::endl;
            
            int written_bytes = 54;
            
            for(int i = 0; i < (img_width)*(img_height); ++i) {
                char element;
                if ((i+1) % (img_width) == 0) {
                    element = 0;
                    for (int k = 0; k < addition; ++k) {
                        file.write(&element, 1);
                        written_bytes++;
                    }
                }
                
                element = data[i].blue();
                file.write(&element, 1);
                
                element = data[i].green();
                file.write(&element, 1);
                
                element = data[i].red();
                file.write(&element, 1);
                
                written_bytes += 3;
            }
            char element = 0;
            for (int k = 0; k < addition; ++k) {
                file.write(&element, 1);
                written_bytes++;
            }
            file.close();
            std::cout<<"written: "<<written_bytes<<std::endl;
        } else {
            throw std::runtime_error("Cannot open BMPImage file");
        }
    }
    
    int width() const {
        return img_width;
    }
    int height() const {
        return img_height;
    }
    
    void removeHorisontalPoints(std::vector<size_t> &line) {
        size_t index = 0;
        int current_point = line.size()-1;
//         for (int i = 0; i < width()*height(); ++i) {
//             if ((current_point>=0) && (i == line[current_point])) {
//                 current_point--;
//                 continue;
//             }
//                     data[index] = data[i];   
//                     index++;
//         }
        current_point = line.size()-1;
        current_point--;
        int offset = 0;
        
        while (current_point >= 0) {
            std::copy(data+line[current_point+1]+1, data+line[current_point], data+line[current_point+1]-offset);
            offset++;
            current_point--;
        }
        current_point = 0;
        std::copy(data+line[current_point]+1, data+img_height*img_width, data+line[current_point]-offset);
    }
    void removeVertiacallPoints(std::vector<size_t> &line) {
        size_t index = 0;
//         for (size_t col = 0; col < width(); ++col) {
//             size_t index = height()-1;
//             for (int row = height()-1; row >= 0; --row) {
//                 if (std::binary_search(line.begin(), line.end(), row*width() + col)) {
//                     continue;
//                 }
//                 data[index*width() + col] = data[row*width() + col];
//                 index--;
//             }
//         }
         for (size_t col = 0; col < width(); ++col) {
            size_t index = 0;
            for (int row = 0; row < height(); ++row) {
                if (std::binary_search(line.begin(), line.end(), row*width() + col, std::greater<size_t>())) {
                    continue;
                }
                data[index*width() + col] = data[row*width() + col];
                index++;
            }
        }
    }
    
    void updateSide(size_t new_value, int side, std::vector<size_t> &line) {
        if (side == 1) {
            removeHorisontalPoints(line);
            img_width = new_value;
        } else if (side == -1) {
            removeVertiacallPoints(line);
            img_height = new_value;
        }
    }
    
    size_t img_width = 0;
    size_t img_height = 0;
private:
    char info[54];
    BMPColor *data = nullptr;
};

template<typename Img, typename Color>
Image<Img, Color> GrayLevel(const Image<Img, Color> &img) {
    Image<Img, Color> gray_img(img);
    for (int j = 0; j < gray_img.height(); ++j) {
        for (int i = 0; i < gray_img.width(); ++i) {
            char color = 0.299*gray_img.pixel(i, j).red() + 0.587*gray_img.pixel(i, j).green() + 0.114*gray_img.pixel(i, j).blue();
            gray_img.setPixel(i, j, Color(color, color, color));
        }
    }
    return gray_img;
}

template<typename Img, typename Color>
Image<Img, Color> NoiseRemove(const Image<Img, Color> &img) {
    Image<Img, Color> noise_img(img);
    
    int n = 3;
//     int Gaussian[5][5] = {
//         {1, 4, 7, 4, 1},
//         {4, 16, 26, 16, 4},
//         {7, 26, 41, 26, 7},
//         {4, 16, 26, 16, 4},
//         {1, 4, 7, 4, 1},
//     };
    int Gaussian[3][3] = {
        {1, 2, 1},
        {2, 4, 2},
        {1, 2, 1},
    };
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
Image<Img, Color> SobelEdges(const Image<Img, Color> &img) {
    Image<Img, Color> result_image(img);
    
    int n = 3;
    int SobelX[3][3] = {
        {1, 0, -1},
        {2, 0, -2},
        {1, 0, -1},
    };
    int SobelY[3][3] = {
        {1, 2, 1},
        {0, 0, 0},
        {-1, -2, -1},
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
                direction = std::atan(gy/gx)*180/PI;
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
    result_image.save("FAT.BMP");

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
Image<Img, Color> DoubleTreshold(const Image<Img, Color> &img) {
    Image<Img, Color> result_image(img);
    for (int j = 1; j < result_image.height()-1; ++j) {
        for (int i = 1; i < result_image.width()-1; ++i) {
            if (img.pixel(i, j).red() < MIN_BORDER_COLOR) {
                result_image.setPixel(i, j, Color(0, 0, 0));
            } else if ((img.pixel(i, j).red() < MAX_BORDER_COLOR) && (img.pixel(i, j).red() > MIN_BORDER_COLOR)) {
                char color = 0;
                for (int x = -1; x <= 1; ++x) {
                    for (int y = -1; y <= 1; ++y) {
                        if (result_image.pixel(i, j).red() > MAX_BORDER_COLOR) {
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

template<typename Img, typename Color>
std::vector<size_t> createVerticalSeams(const Image<Img, Color> &img, int N, size_t *seam_image) {
    std::vector<size_t> line;
    line.reserve(img.width()*N);
    for (int i = 1; i < img.height(); ++i) {
        seam_image[i*img.width() + img.width()-1] = std::numeric_limits<size_t>::max();
        seam_image[i*img.width()] = std::numeric_limits<size_t>::max();
    }
    for (int col = 1; col < img.height(); ++col) {
        for (int row = 1; row < img.width()-1; ++row) {
            size_t min = std::min(std::min(seam_image[(col-1)*img.width() + row-1], seam_image[(col-1)*img.width() + row]), seam_image[(col-1)*img.width() + row+1]);
            size_t addition = (img.pixel(row, col).red()+128 <= MIN_BORDER_COLOR) ? 0 : ((img.pixel(row, col).red()+128 <= MAX_BORDER_COLOR) ? MID_ADDITION : MAX_ADDITION);
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
        line.push_back(current_index);
        seam_image[current_index] = std::numeric_limits<size_t>::max();
    }
    return std::move(line);
}

template<typename Img, typename Color>
std::vector<size_t> createHorisontalSeams(const Image<Img, Color> &img, int N, size_t *seam_image) {
    std::vector<size_t> line;
    line.reserve(img.width()*N);
    for (int i = 1; i < img.width(); ++i) {
        seam_image[i] = std::numeric_limits<size_t>::max();
        seam_image[i+(img.height()-1)*img.width()] = std::numeric_limits<size_t>::max();
    }
    for (int col = 1; col < img.width(); ++col) {
        for (int row = 1; row < img.height()-1; ++row) {
            size_t min = std::min(std::min(seam_image[row*img.width() + col-1], seam_image[(row-1)*img.width() + col-1]), seam_image[(row+1)*img.width() + col-1]);
            size_t addition = (img.pixel(col, row).red()+128 <= MIN_BORDER_COLOR) ? 0 : ((img.pixel(col, row).red()+128 <= MAX_BORDER_COLOR) ? MID_ADDITION : MAX_ADDITION);
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
    return std::move(line);
}

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
    
//     auto seamed = std::move(createHorisontalSeams(fat, N, seam_image));
//     size_t last = seamed[0];
//     for (size_t point : seamed) {
//         result.setPixel(point%result.width(), point/result.width(), BMPColor(250, 0, 0));
//         if (last == point)
//             std::cout<<point<<std::endl;
//         last = point;
//     }
    
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s\n";

    delete[] seam_image;
    fat.save("FATTER.BMP");
    result.save("GHGHGHG.BMP");
    return 0;
}
