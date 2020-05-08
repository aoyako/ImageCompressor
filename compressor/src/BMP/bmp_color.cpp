#include <bmp_color.hpp>

namespace image {
    
    BMPColor::BMPColor(unsigned char red, unsigned char green, unsigned char blue) : red_color(red), green_color(green), blue_color(blue)
    {
    }

    BMPColor::BMPColor(const BMPColor &col) : red_color(col.red()), green_color(col.green()), blue_color(col.blue())
    {
    }
    
    unsigned char BMPColor::red() const {
        return red_color;
    }

    unsigned char BMPColor::green() const {
        return green_color;
    }

    unsigned char BMPColor::blue() const {
        return blue_color;
    }
    
}
