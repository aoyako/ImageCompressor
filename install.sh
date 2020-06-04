#!/bin/bash
mkdir build
cd build && cmake -D DEBUG=OFF .. && make

mkdir $HOME/ImageResizer
mkdir $HOME/ImageResizer/icon
cd ..

cp build/ImageCompressor $HOME/ImageResizer/ImageResizer
cp build/kernel.cl $HOME/ImageResizer/kernel.cl
cp install/icon.png $HOME/ImageResizer/icon/icon.png

sed "s:HOME:$HOME:g" install/Image_Resizer.desktop > Image_Resizer.desktop
cp Image_Resizer.desktop $HOME/.local/share/applications/Image_Resizer.desktop
rm Image_Resizer.desktop

echo "Done!"
