#pragma once

#include <cstdint>
#include <vector>

#include "bmplib.h"

namespace anslib {

bool shiftDClevel(std::vector<uint8_t> &inData);

class Image {
 public:
  size_t width_, height_;
  Image(ushort bd, ushort planesNum) : bitD_(bd), numOfPlanes_(planesNum) { ; }
  Image(const bmplib::BmpImage &img);

  // either RGB, YCoCg or single gray plane
  std::vector<std::vector<uint8_t>> dataPlanes;
  ushort bitD_ = 24;
  ushort numOfPlanes_ = 0;

  std::tuple<uint8_t, uint8_t, uint8_t> operator[](int i) const {
    return std::make_tuple(dataPlanes.at(0).at(i), dataPlanes.at(1).at(i),
                           dataPlanes.at(2).at(i));
  }
  // std::tuple<uint8_t, uint8_t, uint8_t> &operator [](int i) {return
  // std::make_tuple(dataPlanes.at(0).at(i), dataPlanes.at(1).at(i),
  // dataPlanes.at(2).at(i));}
};

}  // namespace anslib