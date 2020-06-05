#include <image.hpp>

namespace image {
    
    template<typename ImageOperator, typename ColorOperator>
    Image<ImageOperator, ColorOperator>::Image(std::string filename) : _image(filename)
    {
    }
    
    template<typename ImageOperator, typename ColorOperator>
    Image<ImageOperator, ColorOperator>::Image(ImageOperator img) : _image(img)
    {
    }
    
    template<typename ImageOperator, typename ColorOperator>
    Image<ImageOperator, ColorOperator>::Image(const Image &img) : _image(img.image)
    {
    }
    
    template<typename ImageOperator, typename ColorOperator>
    Image<ImageOperator, ColorOperator>::Image(Image &&img) : _image(std::move(img._image))
    {
    }

    template<typename ImageOperator, typename ColorOperator>
    Image<ImageOperator, ColorOperator>& Image<ImageOperator, ColorOperator>::operator=(const Image &img)
    {
            _image = img.image;
            return *this;
    }

    template<typename ImageOperator, typename ColorOperator>
    ColorOperator Image<ImageOperator, ColorOperator>::pixel(size_t x, size_t y) const {
        return _image.pixel(x, y);
    }
    
    template<typename ImageOperator, typename ColorOperator>
    void Image<ImageOperator, ColorOperator>::setPixel(size_t x, size_t y, ColorOperator color) {
        _image.setPixel(x, y, color);
    }
    
    template<typename ImageOperator, typename ColorOperator>
    void Image<ImageOperator, ColorOperator>::save(std::string filename) const {
        _image.save(filename);
    }
    
    template<typename ImageOperator, typename ColorOperator>
    int Image<ImageOperator, ColorOperator>::width() const{
        return _image.width();
    }
    
    template<typename ImageOperator, typename ColorOperator>
    int Image<ImageOperator, ColorOperator>::height() const{
        return _image.height();
    }
    
    template<typename ImageOperator, typename ColorOperator>
    ImageOperator& Image<ImageOperator, ColorOperator>::getImage() {
        return _image;
    }
    
}
