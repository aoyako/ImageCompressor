#pragma once

namespace image {
    
/**
 * BMPColor class represents color value of BMP image type
 */
class BMPColor {
public:
    /**
     * Constructor with RGB palette
     */
    BMPColor(unsigned char red = 0, unsigned char green = 0, unsigned char blue = 0);
    
    /**
     * Copy constructor
     */
    BMPColor(const BMPColor &col);
    
    /**
     * Move constructor
     */
    BMPColor(BMPColor &&col) = default;
    
    /**
     * Assignment operator
     */
    BMPColor& operator=(const BMPColor &col) = default;
    
    /**
     * Returns value of red component in RGB palette
     */
    unsigned char red() const;
    
    /**
     * Returns value of green component in RGB palette
     */
    unsigned char green() const;
    
    /**
     * Returns value of blue component in RGB palette
     */
    unsigned char blue() const;
private:
    /**
     * Value of red component in RGB palette
     */
    unsigned char red_color;
    
    /**
     * Value of red component in RGB palette
     */
    unsigned char green_color;
    
    /**
     * Value of red component in RGB palette
     */
    unsigned char blue_color;
};
}
