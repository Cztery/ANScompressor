#pragma once

#include <cstdint>
#include <vector>

#include "bmplib.h"
#include "common.h"

namespace anslib {

bool shiftDClevel(std::vector<AnsSymbol> &inData);

class Image {
 public:
  size_t width_, height_;
  Image(ushort bd, ushort planesNum) : bitDepth_(bd), numOfPlanes_(planesNum) { ; }
  Image(const bmplib::BmpImage &img);

  // either RGB, YCoCg or single gray plane
  std::vector<std::vector<AnsSymbol>> dataPlanes;
  std::vector<std::vector<AnsCountsType>> symCountsIfCompressed;
  ushort bitDepth_ = 24;
  ushort numOfPlanes_ = 0;
  bool isAnsCompressed = false;

  std::tuple<AnsSymbol, AnsSymbol, AnsSymbol> operator[](int i) const {
    return std::make_tuple(dataPlanes.at(0).at(i), dataPlanes.at(1).at(i),
                           dataPlanes.at(2).at(i));
  }

  std::vector<uint8_t> getPlanesAsBmpData();

  void GBRtoYCbCr();
};

class CompImage : public Image {
  CompImage(ushort bd, ushort planesNum) : Image(bd, planesNum) {}
  CompImage(const bmplib::BmpImage &img) : Image(img) {}
};

}  // namespace anslib