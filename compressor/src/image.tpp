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
    ColorOperator Image<ImageOperatorm ColorOperator>::pixel(size_t i, size_t j) const {
        return image.pixel(i, j);
    }
    
    template<typename ImageOperator, typename ColorOperator>
    Image<ImageOperatorm ColorOperator>::void setPixel(size_t i, size_t j, ColorOperator color) {
        image.setPixel(i, j, color);
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
