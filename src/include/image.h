#pragma once

#include <cstdint>
#include <tuple>
#include <vector>

#include "bmplib.h"
#include "common.h"

namespace anslib {

bool shiftDClevel(std::vector<AnsSymbolType> &inData);

struct RawImage {
  uint16_t width_, height_;
  RawImage();
  RawImage(ushort bd, ushort planesNum);
  RawImage(const bmplib::BmpImage &img);
  RawImage(const std::vector<AnsSymbolType> p1, const std::vector<AnsSymbolType> p2,
           const std::vector<AnsSymbolType> p3, size_t wid, size_t hei);

  size_t bytesSizeOfImage();

  // either RGB, YCoCg or single gray plane
  std::vector<std::vector<AnsSymbolType>> dataPlanes_;
  uint8_t bitDepth_ = 24;
  uint8_t numOfPlanes_ = 0;
  // Image can be partitioned into chunks for further computation - prediction
  // and compression; width of square chunks if image was partitioned;
  size_t chunkWidth_ = 0;
  inline size_t chunksPerPlaneCount() const {
    if (chunkWidth_) {
      return ((width_  / chunkWidth_) *
              (height_ / chunkWidth_));
    }
    return 1;
  }

  std::tuple<AnsSymbolType, AnsSymbolType, AnsSymbolType> operator[](int i) const {
    return std::make_tuple(dataPlanes_.at(0).at(i), dataPlanes_.at(1).at(i),
                           dataPlanes_.at(2).at(i));
  }

  std::vector<uint8_t> getPlanesAsBmpData();
  void splitIntoChunks(size_t chunkSize);
  void mergeImageChunks();
};

struct CompImage {
  uint16_t width_, height_;
  uint8_t numOfPlanes_;
  uint8_t bitDepth_;
  size_t chunkWidth_;
  size_t rawChannelSize_;
  size_t chunksPerPlaneCount() const {
    if (chunkWidth_) {
      return ((width_  / chunkWidth_) *
              (height_ / chunkWidth_));
    }
    return 1;
  }

  CompImage();
  size_t bytesSizeOfImage();

  struct PlaneAndCounts {
    std::vector<uint8_t> plane;
    std::vector<AnsCountsType> counts;
    size_t rawPlaneSize;
  };
  std::vector<PlaneAndCounts> compressedPlanes_;
};
}  // namespace anslib