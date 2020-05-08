namespace image {
    
    template<typename ImageOperator, typename ColorOperator>
    Image<ImageOperatorm ColorOperator>::Image(std::string filename) : image(filename)
    {
    }
    
    template<typename ImageOperator, typename ColorOperator>
    Image<ImageOperatorm ColorOperator>::Image(ImageOperator img) : image(img)
    {
    }
    
    template<typename ImageOperator, typename ColorOperator>
    Image<ImageOperatorm ColorOperator>::Image(const Image &img) : image(img.image)
    {
    }
    
    template<typename ImageOperator, typename ColorOperator>
    Image<ImageOperatorm ColorOperator>::Image(Image &&img) : image(std::move(img.image))
    {
    }
    
    template<typename ImageOperator, typename ColorOperator>
    ColorOperator Image<ImageOperatorm ColorOperator>::pixel(size_t x, size_t y) const {
        return image.pixel(x, y);
    }
    
    template<typename ImageOperator, typename ColorOperator>
    Image<ImageOperatorm ColorOperator>::void setPixel(size_t x, size_t y, ColorOperator color) {
        image.setPixel(x, y, color);
    }
    
    template<typename ImageOperator, typename ColorOperator>
    void Image<ImageOperatorm ColorOperator>::save(std::string filename) const{
        image.save(filename);
    }
    
    template<typename ImageOperator, typename ColorOperator>
    int Image<ImageOperatorm ColorOperator>::width() const{
        return image.width();
    }
    
    template<typename ImageOperator, typename ColorOperator>
    int Image<ImageOperatorm ColorOperator>::height() const{
        return image.height();
    }
    
    template<typename ImageOperator, typename ColorOperator>
    ImageOperator& Image<ImageOperatorm ColorOperator>::getImage() {
        return image;
    }
    
}
