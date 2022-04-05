#include "ansComp.h"
#include "bmplib.h"
#include <iostream>
#include <iterator>

namespace anslib {

bool shiftDClevel(std::vector<uint8_t> &inData) {
    return false;
}

Image::Image(const bmplib::BmpImage &img) {
    switch (img.infoHeader_.compression) {
    case 0: // no compression, R-G-B, three planes
        numOfPlanes_ = 3;
        bitD_ = 24;
        break;
    default:
        throw std::runtime_error("unexpected compression type in bmp image");
        break;
    }

    width_ = img.infoHeader_.width;
    height_ = img.infoHeader_.height;
    
    dataPlanes.resize(numOfPlanes_, std::vector<uint8_t>(width_*height_, 0));

    auto getPx = [this](uint32_t y, uint32_t x) {
        const size_t rowPadding = width_ * numOfPlanes_ % 4 ?
                                  4 - (width_ * numOfPlanes_ % 4) : 0;
        const size_t rowOffset = (height_ - y - 1) * (numOfPlanes_ * width_ + rowPadding);
        
        return rowOffset + x * numOfPlanes_;
    };

    for (auto row = 0; row < height_; ++row) {
        for ( auto x = 0; x < width_; ++x) {
            for ( auto plane = 0; plane != numOfPlanes_; ++plane) {
                const size_t inIndex = getPx(row, x)+plane;
                const size_t outIndex = width_*row+x;
                // std::cout << "in: " << inIndex << " out: " << outIndex << " row: " << row << std::endl;
                dataPlanes.at(plane).at(outIndex) = img.data.at(inIndex);
            }
        }
    }
}

}

