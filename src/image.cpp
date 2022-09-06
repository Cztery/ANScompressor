#include "image.h"

#include <iostream>
#include <iterator>

#include "bmplib.h"

namespace anslib {

bool shiftDClevel(std::vector<AnsSymbol> &inData) { return false; }

RawImage::RawImage(){};

RawImage::RawImage(ushort bd, ushort planesNum)
    : bitDepth_(bd), numOfPlanes_(planesNum) {
  ;
}

RawImage::RawImage(const bmplib::BmpImage &img) {
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

RawImage::RawImage(const std::vector<AnsSymbol> p1, const std::vector<AnsSymbol> p2,
             const std::vector<AnsSymbol> p3, size_t wid, size_t hei)
    : width_(wid), height_(hei) {
  dataPlanes_.push_back(p1);
  dataPlanes_.push_back(p2);
  dataPlanes_.push_back(p3);
}

RawImage &RawImage::operator=(const RawImage &r) {
  if (&r != this) {
    bitDepth_ = r.bitDepth_;
    dataPlanes_ = r.dataPlanes_;
    height_ = r.height_;
    width_ = r.width_;
    numOfPlanes_ = r.numOfPlanes_;
  }
  return *this;
}

size_t anslib::RawImage::bytesSizeOfImage() {
  size_t byteSize = 0;
  for (auto plane : dataPlanes_) {
    byteSize += plane.size() * sizeof(decltype(plane.back()));
  }
  return byteSize;
}

size_t anslib::CompImage::bytesSizeOfImage() {
  size_t byteSize = 0;
  for (auto plane : compressedPlanes_) {
    byteSize += plane.plane.size() * sizeof(decltype(plane.plane.back()));
    byteSize += plane.counts.size() * sizeof(decltype(plane.counts.back()));
  }
  return byteSize;
}

std::vector<uint8_t> RawImage::getPlanesAsBmpData() {
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

void RawImage::splitIntoChunks(size_t chunkSize) {
  const size_t chunksXcount = width_/chunkSize;
  const size_t chunksYcount = height_/chunkSize;
  std::vector<std::vector<anslib::AnsSymbol>> chunks;//(height_/chunkSize * width_/chunkSize);
  for (auto &plane : dataPlanes_) {
    for (size_t w = 0; w < chunksXcount; ++w) {
      for (size_t h = 0; h < chunksYcount; ++h) {
        std::vector<anslib::AnsSymbol> chunk(chunkSize*chunkSize);
        for (size_t i = 0; i <= chunkSize*chunkSize; ++i) {
          chunk.push_back(plane.at((i / chunkSize + h * chunkSize) * width_ + i % chunkSize + w * chunkSize));
        }
        chunks.push_back(chunk);
      }
    }
  }
  dataPlanes_ = chunks;
}

CompImage::CompImage() {};

}  // namespace anslib
