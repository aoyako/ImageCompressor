#pragma once

namespace image {
    
    /**
     * @brief BMPColor class represents color value of BMP image type
     */
    class BMPColor {
    public:
        /**
         * @brief Constructor with RGB palette
         */
        BMPColor(unsigned char red = 0, unsigned char green = 0, unsigned char blue = 0);
        
        BMPColor(const BMPColor &col);

        BMPColor(BMPColor &&col) = default;
        
        BMPColor& operator=(const BMPColor &col) = default;
        
        /**
         * @brief Returns value of red component in RGB palette
         */
        unsigned char red() const;
        
        /**
         * @brief Returns value of green component in RGB palette
         */
        unsigned char green() const;
        
        /**
         * @brief Returns value of blue component in RGB palette
         */
        unsigned char blue() const;
    private:
        /**
         * @brief Value of red component in RGB palette
         */
        unsigned char red_color;
        
        /**
         * @brief Value of red component in RGB palette
         */
        unsigned char green_color;
        
        /**
         * @brief Value of red component in RGB palette
         */
        unsigned char blue_color;
    };
}
