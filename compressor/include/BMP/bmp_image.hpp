#pragma once

#include <fstream>
#include <string>
#include <memory>
#include <vector>
#include <cstring>
#include <algorithm>

#include <BMP/bmp_color.hpp>

namespace image {
    /**
     * @brief BMPImage class stores metadata and pixel map of image
     */
    class BMPImage
    {
    public:
        /**
         * @brief Describes how pixel will be joined
         */
        enum DIRECTION {
            HORISONTAL,
            VERTICAL
        };

        /**
         * @brief Constructor from pieces
         */
        BMPImage(size_t width, size_t height, BMPColor *data);
        
        /**
         * @brief Constructor from bmp file
         */
        BMPImage(std::string filename);
        
        BMPImage(const BMPImage &img);
        
        BMPImage(BMPImage &&img);

        BMPImage& operator=(const BMPImage &img);
        
        ~BMPImage();
        
        /**
         * @brief Returns color of pixel
         */
        BMPColor pixel(int x, int y) const;
        
        /**
         * @brief Sets color of pixel
         */
        void setPixel(int x, int y, BMPColor color);
        
        /**
         * @brief Saves image in BMP format
         */
        void save(std::string filename) const;
        
        /**
         * @brief Returns width of image
         */
        size_t width() const;
        
        /**
         * @brief Returns height of image
         */
        size_t height() const;
        
        /**
         * @brief Erases horisontal pixels
         * @param line vector of erased points. Used one-line coord system (i.e. (x, y) -> x*width + y)
         */ 
        void removeHorisontalPoints(std::vector<size_t> &line);
        
        /**
         * @brief Erases vertical pixels
         * @param line vector of erased points. Used one-line coord system (i.e. (x, y) -> x*width + y)
         */ 
        void removeVertiacallPoints(std::vector<size_t> &line);
        
        /**
         * @brief Erases pixels from image
         * @param DIRECTION specifies which remove function will be used
         * @param line vector of erased points. Used one-line coord system (i.e. (x, y) -> x*width + y)
         */ 
        void updateSide(size_t new_value, DIRECTION direction, std::vector<size_t> &line);

        /**
         * @brief Pixed data
         */
        BMPColor *data = nullptr;

        /**
         * @brief Image's width
         */
        size_t img_width = 0;

        /**
         * @brief Image's height
         */
        size_t img_height = 0;
    private:
        
        /**
         * @brief Metadata of BMP image
         */ 
        char info[54];
    };

}
