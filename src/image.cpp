#include "image.h"

#include <iostream>
#include <iterator>

#include "bmplib.h"

namespace anslib {

bool shiftDClevel(std::vector<AnsSymbol> &inData) { return false; }

Image::Image(){};

Image::Image(ushort bd, ushort planesNum)
    : bitDepth_(bd), numOfPlanes_(planesNum) {
  ;
}

Image::Image(const bmplib::BmpImage &img) {
  switch (img.infoHeader_.compression) {
    case 0:  // no compression, R-G-B, three planes
      numOfPlanes_ = 3;
      bitDepth_ = 24;
      break;
    default:
      throw std::runtime_error("unexpected compression type in bmp image");
      break;
  }

  width_ = img.infoHeader_.width;
  height_ = img.infoHeader_.height;

  dataPlanes_.resize(numOfPlanes_, std::vector<AnsSymbol>(width_ * height_, 0));

  size_t inIndex, outIndex;
  for (size_t row = 0; row < height_; ++row) {
    for (size_t x = 0; x < width_; ++x) {
      for (auto plane = 0; plane != numOfPlanes_; ++plane) {
        inIndex = img.getPxIndex(row, x, plane);
        outIndex = width_ * row + x;
        dataPlanes_.at(plane).at(outIndex) = img.data.at(inIndex);
      }
    }
  }
}

Image::Image(const std::vector<AnsSymbol> p1, const std::vector<AnsSymbol> p2,
             const std::vector<AnsSymbol> p3, size_t wid, size_t hei)
    : width_(wid), height_(hei) {
  dataPlanes_.push_back(p1);
  dataPlanes_.push_back(p2);
  dataPlanes_.push_back(p3);
}

Image &Image::operator=(const Image &r) {
  if (&r != this) {
    bitDepth_ = r.bitDepth_;
    dataPlanes_ = r.dataPlanes_;
    height_ = r.height_;
    width_ = r.width_;
    numOfPlanes_ = r.numOfPlanes_;
    symCountsIfCompressed_ = r.symCountsIfCompressed_;
    isAnsCompressed_ = r.isAnsCompressed_;
  }
  return *this;
}

std::vector<uint8_t> Image::getPlanesAsBmpData() {
  const size_t rowPadding =
      width_ * numOfPlanes_ % 4 ? 4 - (width_ * numOfPlanes_ % 4) : 0;
  std::vector<uint8_t>
      outData /*(height_ * width_ * numOfPlanes_ + height_ * rowPadding)*/;

  auto addRowPadding = [rowPadding, &outData]() {
    for (size_t i = 0; i < rowPadding; ++i) {
      outData.push_back(0);
    }
  };

  for (size_t row = 0; row < height_; ++row) {
    for (size_t x = 0; x < width_; ++x) {
      for (auto &plane : dataPlanes_) {
        outData.emplace_back(plane.at(width_ * (height_ - row - 1) + x));
      }
    }
    addRowPadding();
  }
  return outData;
}

void Image::GBRtoYCbCr() { ; }

}  // namespace anslib
