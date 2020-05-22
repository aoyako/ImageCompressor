#pragma once

#include <memory>
#include <string>

namespace image {

    /**
     * @brief Image class represents interface over image types
     */
    template<typename ImageOperator, typename ColorOperator>
    class Image
    {
    public:
        /**
         * @brief Constructor from filename
         */
        Image(std::string filename);

        /**
         * @brief Constructor from image object
         */
        Image(ImageOperator img);

        Image(const Image &img);

        Image& operator=(const Image &img);

        Image(Image &&img);

        /**
         * @brief Returns color of pixel with (x, y) coordinates
         */
        ColorOperator pixel(size_t x, size_t y) const;

        /**
         * @brief Sets color of pixel with (x, y) coordinates
         */
        void setPixel(size_t x, size_t y, ColorOperator color);

        /**
         * @brief Saves image, ImageOperator specifies format
         */
        void save(std::string filename) const;

        /**
         * @brief Returns image's width
         */
        int width() const;

        /**
         * @brief Returns image's height
         */
        int height() const;

        /**
         * @brief Returns image object
         */
        ImageOperator& getImage();
    private:
        /**
         * @brief Image object
         */
        ImageOperator image;
    };

}
#include <../src/image.tpp>
