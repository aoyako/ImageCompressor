#pragma once

#include <memory>
#include <string>

namespace image {

    /**
     * Image class represents interface over image types
     */
    template<typename ImageOperator, typename ColorOperator>
    class Image
    {
    public:
        /**
         * Constructor from filename
         */
        Image(std::string filename);

        /**
         * Constructor from image object
         */
        Image(ImageOperator img);

        /**
         * Copy Constructor
         */
        Image(const Image &img);

        /**
         * Assignment operator
         */
        Image& operator=(const Image &img);

        /**
         * Move Constructor
         */
        Image(Image &&img);

        /**
         * Returns color of pixel with (x, y) coordinates
         */
        ColorOperator pixel(size_t x, size_t y) const;

        /**
         * Sets color of pixel with (x, y) coordinates
         */
        void setPixel(size_t x, size_t y, ColorOperator color);

        /**
         * Saves image, ImageOperator specifies format
         */
        void save(std::string filename) const;

        /**
         * Returns image's width
         */
        int width() const;

        /**
         * Returns image's height
         */
        int height() const;

        /**
         * Returns image object
         */
        ImageOperator& getImage();
    private:
        /**
         * Image object
         */
        ImageOperator image;
    };

}
#include <../src/image.tpp>
