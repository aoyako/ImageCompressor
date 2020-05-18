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
     * BMPImage class stores metadata and pixel map of image
     */
    class BMPImage
    {
    public:
        /**
         * Describes how pixel will be joined
         */
        enum DIRECTION {
            HORISONTAL,
            VERTICAL
        };

        /**
         * Constructor from pieces
         */
        BMPImage(size_t width, size_t height, BMPColor *data);
        
        /**
         * Constructor from bmp file
         */
        BMPImage(std::string filename);
        
        /**
         * Copy constructor
         */
        BMPImage(const BMPImage &img);
        
        /**
         * Move constructor
         */
        BMPImage(BMPImage &&img);

        /**
         * Assignment operator
         */
        BMPImage& operator=(const BMPImage &img);
        
        ~BMPImage();
        
        /**
         * Returns color of pixel
         */
        BMPColor pixel(int x, int y) const;
        
        /**
         * Sets color of pixel
         */
        void setPixel(int x, int y, BMPColor color);
        
        /**
         * Saves image in BMP format
         */
        void save(std::string filename) const;
        
        /**
         * Returns width of image
         */
        size_t width() const;
        
        /**
         * Returns height of image
         */
        size_t height() const;
        
        /**
         * Erases horisontal pixels
         * @param line vector of erased points. Used one-line coord system (i.e. (x, y) -> x*width + y)
         */ 
        void removeHorisontalPoints(std::vector<size_t> &line);
        
        /**
         * Erases vertical pixels
         * @param line vector of erased points. Used one-line coord system (i.e. (x, y) -> x*width + y)
         */ 
        void removeVertiacallPoints(std::vector<size_t> &line);
        
        /**
         * Erases pixels from image
         * @param DIRECTION specifies which remove function will be used
         * @param line vector of erased points. Used one-line coord system (i.e. (x, y) -> x*width + y)
         */ 
        void updateSide(size_t new_value, DIRECTION direction, std::vector<size_t> &line);

        /**
         * Pixed data
         */
        BMPColor *data = nullptr;

        /**
         * Image's width
         */
        size_t img_width = 0;

        /**
         * Image's height
         */
        size_t img_height = 0;
    private:
        
        /**
         * Metadata of BMP image
         */ 
        char info[54];
    };

}
