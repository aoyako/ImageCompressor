#include <image.hpp>

namespace image {
    
    template<typename ImageOperator, typename ColorOperator>
    Image<ImageOperator, ColorOperator>::Image(std::string filename) : image(filename)
    {
    }
    
    template<typename ImageOperator, typename ColorOperator>
    Image<ImageOperator, ColorOperator>::Image(ImageOperator img) : image(img)
    {
    }
    
    template<typename ImageOperator, typename ColorOperator>
    Image<ImageOperator, ColorOperator>::Image(const Image &img) : image(img.image)
    {
    }
    
    template<typename ImageOperator, typename ColorOperator>
    Image<ImageOperator, ColorOperator>::Image(Image &&img) : image(std::move(img.image))
    {
    }
    
    template<typename ImageOperator, typename ColorOperator>
    ColorOperator Image<ImageOperator, ColorOperator>::pixel(size_t x, size_t y) const {
        return image.pixel(x, y);
    }
    
    template<typename ImageOperator, typename ColorOperator>
    void Image<ImageOperator, ColorOperator>::setPixel(size_t x, size_t y, ColorOperator color) {
        image.setPixel(x, y, color);
    }
    
    template<typename ImageOperator, typename ColorOperator>
    void Image<ImageOperator, ColorOperator>::save(std::string filename) const {
        image.save(filename);
    }
    
    template<typename ImageOperator, typename ColorOperator>
    int Image<ImageOperator, ColorOperator>::width() const{
        return image.width();
    }
    
    template<typename ImageOperator, typename ColorOperator>
    int Image<ImageOperator, ColorOperator>::height() const{
        return image.height();
    }
    
    template<typename ImageOperator, typename ColorOperator>
    ImageOperator& Image<ImageOperator, ColorOperator>::getImage() {
        return image;
    }
    
}
