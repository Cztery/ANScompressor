#pragma once

#include <cstdint>
#include <vector>

#include "bmplib.h"
#include "common.h"

namespace anslib {

bool shiftDClevel(std::vector<AnsSymbol> &inData);

class RawImage {
 public:
  uint16_t width_, height_;
  RawImage();
  RawImage(ushort bd, ushort planesNum);
  RawImage(const bmplib::BmpImage &img);
  RawImage(const std::vector<AnsSymbol> p1, const std::vector<AnsSymbol> p2,
        const std::vector<AnsSymbol> p3, size_t wid, size_t hei);

  size_t bytesSizeOfImage();
  RawImage &operator=(const RawImage &r);

  // either RGB, YCoCg or single gray plane
  std::vector<std::vector<AnsSymbol>> dataPlanes_;
  uint8_t bitDepth_ = 24;
  uint8_t numOfPlanes_ = 0;
  // Image can be partitioned into chunks for further computation - prediction and compression;
  // width of square chunks if image was partitioned; 
  uint8_t chunkWidth_ = 0;
  inline size_t chunksPerPlaneCount() {
    if (chunkWidth_) {
      return ((width_ + chunkWidth_ - 1) / chunkWidth_ * (height_ + chunkWidth_ - 1) / chunkWidth_);
    }
    return 0;
  }

  std::tuple<AnsSymbol, AnsSymbol, AnsSymbol> operator[](int i) const {
    return std::make_tuple(dataPlanes_.at(0).at(i), dataPlanes_.at(1).at(i),
                           dataPlanes_.at(2).at(i));
  }

  std::vector<uint8_t> getPlanesAsBmpData();
  void splitIntoChunks(size_t chunkSize);
};


class CompImage {
 public:
  uint16_t width_, height_;
  uint8_t numOfPlanes_;
  uint8_t bitDepth_;
  uint8_t chunkWidth_;

  CompImage();
  size_t bytesSizeOfImage();

  struct PlaneAndCounts {
    std::vector<uint8_t> plane;
    std::vector<AnsCountsType> counts;
  };
  std::vector<PlaneAndCounts> compressedPlanes_;
};
}  // namespace anslib