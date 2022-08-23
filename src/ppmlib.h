#pragma once

#include <stdint.h>

#include <string>
#include <vector>

namespace anslib {
namespace ppmlib {

struct PpmImage {
  std::string fileMagic;
  size_t width_, height_;
  size_t maxVal_;

  std::vector<uint8_t> r;
  std::vector<uint8_t> g;
  std::vector<uint8_t> b;

  PpmImage(const char* filename);
};

}  // namespace ppmlib
}  // namespace anslib