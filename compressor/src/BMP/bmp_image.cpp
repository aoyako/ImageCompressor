#include <string.h>

#include <BMP/bmp_image.hpp>


namespace image {

    BMPImage::BMPImage(size_t width, size_t height, BMPColor *data) {
        /// Default metadata
        memcpy(info, "\x42\x4d\x16\x47\x3f\x00\x00\x00\x00\x00\x36\x00\x00\x00\x28\x00\x00\x00\xac\x01\x00\x00\xce\x02\x00\x00\x01\x00\x18\x00\x00\x00\x00\x00\xe0\x46\x3f\x00\xe9\x24\x00\x00\xe9\x24\x00\x00\x00\x00\x00\x00\x00\x00", 54);
//        auto temp = BMPImage("MAIN.BMP");
//        memcpy(info, temp.info, 54);
        img_width = width;
        img_height = height;
        this->data = data;
    }
    
    BMPImage::BMPImage(std::string filename) {
        std::ifstream file;
        file.open(filename, std::ios::in | std::ios::binary);
        if (file.is_open()) {
            /// Calculate file size
            int last = file.seekg(-1, std::ios_base::end).tellg();
            int file_size = last - file.seekg(0).tellg();
            
            /// Read metadata
            file.read(info, 54);
            
            /// Pull width and height from metadata
            std::memcpy(&img_width, info + 18, sizeof(int));
            std::memcpy(&img_height, info + 22, sizeof(int));
            
            /// Pull begin of pixel data
            int offset;
            std::memcpy(&offset, info + 10, sizeof(int));
            
            /// Calculate offset in each row
            int addition = 0;
            if ((3*img_width + addition) % 4 != 0) {
                addition = 4 - (3*img_width + addition) % 4;
            }
            
            
            /// Read pixel data
            size_t size = img_width*img_height;
            char *color_data = new char[size*3 + addition*img_height];
            file.read(color_data, size*3);
            data = new BMPColor [size];
            
            int nums_of_offset = 0;
            for(int i = 0; i < size; ++i) {
                    /// @note BMP stores pixels as BGR, so convert it to RGB
                    data[i] = BMPColor(color_data[i*3+2 + nums_of_offset*addition], color_data[i*3+1 + nums_of_offset*addition], color_data[i*3 + nums_of_offset*addition]);
                    if ((i+1) % (img_width) == 0) {
                        nums_of_offset++;
                        continue;
                    }
            }            
            delete[] color_data;
            file.close();
        } else {
            throw std::runtime_error("Cannot open BMPImage file");
        }
    }
    
    BMPImage::BMPImage(const BMPImage &img) {
        img_width = img.img_width;
        img_height = img.img_height;
        std::copy(img.info, img.info+54, info);
        data = new BMPColor [img_width*img_height];
        /// TODO: use std::copy
        for (int j = 0; j < img_height; ++j) {
            for (int i = 0; i < img_width; ++i) {
                data[j*img_width + i] = img.data[j*img_width + i];
            }
        }
    }
    
    BMPImage::BMPImage(BMPImage &&img) {
        img_width = img.img_width;
        img_height = img.img_height;
        std::copy(img.info, img.info+54, info);
        if (data)
            delete[] data;
        data = img.data;
        img.data = nullptr;
    }

    BMPImage& BMPImage::operator=(const BMPImage &img) {
        if (data)
            delete[] data;
        img_width = img.img_width;
        img_height = img.img_height;
        data = new BMPColor [img_width*img_height];
        std::copy(img.info, img.info+54, info);
        std::copy(img.data, img.data+img_width*img_height, data);

        return *this;
    }
    
    BMPImage::~BMPImage() {
        if (data) {
            delete[] data;
        }
    }
    
    BMPColor BMPImage::pixel(int x, int y) const {
        return data[y*img_width + x];
    }
    
    void BMPImage::setPixel(int x, int y, BMPColor color) {
        data[y*img_width + x] = color;
    }
    
    void BMPImage::save(std::string filename) const {
        std::ofstream file;
        file.open(filename, std::ios::out | std::ios::binary);
        if (file.is_open()) {
            /// Write metadata
            file.write(info, 18);
            file.write((const char*) &img_width, 4);
            file.write((const char*) &img_height, 4);
            file.write(info+26, 28);
            
            /// Calculate offset
            int addition = 0;
            if ((3*img_width + addition) % 4 != 0) {
                addition = 4 - (3*img_width + addition) % 4;
            }
            
            /// Write pixel data
            for(int i = 0; i < (img_width)*(img_height); ++i) {
                char element;
                if ((i+1) % (img_width) == 0) {
                    element = 0;
                    for (int k = 0; k < addition; ++k) {
                        file.write(&element, 1);
                    }
                }
                
                element = data[i].blue();
                file.write(&element, 1);
                
                element = data[i].green();
                file.write(&element, 1);
                
                element = data[i].red();
                file.write(&element, 1);
            }
            char element = 0;
            for (int k = 0; k < addition; ++k) {
                file.write(&element, 1);
            }
            file.close();
        } else {
            throw std::runtime_error("Cannot open BMPImage file");
        }
    }
    
    size_t BMPImage::width() const {
        return img_width;
    }
    size_t BMPImage::height() const {
        return img_height;
    }
    
    void BMPImage::removeHorisontalPoints(std::vector<size_t> &line) {
        /// ******
//        auto temp = BMPImage(*this);
//        for (auto item : line) {
//            temp.setPixel(item%temp.width(), item/temp.width(), BMPColor(255, 0, 0));
//        }
//        temp.save("SEAM.BMP");
        /// ******
        if (line.empty()) {
            return;
        }
        size_t index = 0;
        int current_point = line.size()-1;
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
    
    void BMPImage::removeVertiacallPoints(std::vector<size_t> &line) {
        if (line.empty()) {
            return;
        }
//        /// ******
//        auto temp = BMPImage(*this);
//        for (auto item : line) {
//            temp.setPixel(item%temp.width(), item/temp.width(), BMPColor(255, 0, 0));
//        }
//        temp.save("SEAM.BMP");
//        /// ******
        size_t index = 0;
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
    
    void BMPImage::updateSide(size_t new_value, DIRECTION direction, std::vector<size_t> &line) {
        if (direction == DIRECTION::HORISONTAL) {
            removeHorisontalPoints(line);
            img_width = new_value;
        } else if (direction == DIRECTION::VERTICAL) {
            removeVertiacallPoints(line);
            img_height = new_value;
        }
    }

}
