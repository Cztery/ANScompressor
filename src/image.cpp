#include "image.h"

#include <iostream>
#include <iterator>

#include "bmplib.h"

namespace anslib {

bool shiftDClevel(std::vector<AnsSymbol> &inData) { return false; }

Image::Image(const bmplib::BmpImage &img) {
  switch (img.infoHeader_.compression) {
    case 0:  // no compression, R-G-B, three planes
      numOfPlanes_ = 3;
      bitD_ = 24;
      break;
    default:
      throw std::runtime_error("unexpected compression type in bmp image");
      break;
  }

  width_ = img.infoHeader_.width;
  height_ = img.infoHeader_.height;

  dataPlanes.resize(numOfPlanes_, std::vector<AnsSymbol>(width_ * height_, 0));

  size_t inIndex, outIndex;
  for (size_t row = 0; row < height_; ++row) {
    for (size_t x = 0; x < width_; ++x) {
      for (auto plane = 0; plane != numOfPlanes_; ++plane) {
        inIndex = img.getPxIndex(row, x, plane);
        outIndex = width_ * row + x;
        dataPlanes.at(plane).at(outIndex) = img.data.at(inIndex);
      }
    }
  }
}

  

std::vector<uint8_t> Image::getPlanesAsBmpData() {
  const size_t rowPadding = width_ * numOfPlanes_ % 4
                                    ? 4 - (width_ * numOfPlanes_ % 4)
                                    : 0;
  std::vector<uint8_t> outData/*(height_ * width_ * numOfPlanes_ + height_ * rowPadding)*/;

  auto addRowPadding = [rowPadding, &outData](){
    for (size_t i = 0; i < rowPadding; ++i) {
            outData.push_back(0);
    }
  };

  for (size_t row = 0; row < height_; ++row) {
    for (size_t x = 0; x < width_; ++x) {
      for (auto &plane : dataPlanes) {
        outData.emplace_back(plane.at(width_ * (height_ - row - 1) + x));
      }
    }
    addRowPadding();
  }
  return outData;
}
void Image::GBRtoYCbCr() { ; }

}  // namespace anslib
