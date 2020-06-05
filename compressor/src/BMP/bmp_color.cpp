#include <BMP/bmp_color.hpp>

namespace image {
    
    BMPColor::BMPColor(unsigned char red, unsigned char green, unsigned char blue) : _red(red), _green(green), _blue(blue)
    {
    }

    BMPColor::BMPColor(const BMPColor &col) : _red(col.red()), _green(col.green()), _blue(col.blue())
    {
    }
    
    unsigned char BMPColor::red() const {
        return _red;
    }

    unsigned char BMPColor::green() const {
        return _green;
    }

    unsigned char BMPColor::blue() const {
        return _blue;
    }
    
}
