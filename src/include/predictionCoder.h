#pragma once

#include <image.h>

// C B D
// A X

anslib::RawImage predictImg(const anslib::RawImage &inImg) {
  anslib::RawImage outImg(inImg);
  for (auto p : inImg.dataPlanes)
    for (auto y = 0; y < inImg.height_; ++y) {
      for (auto x = 0; x < inImg.width_; ++x) }
}

struct Neighbors {
  uint8_t A;
  uint8_t B;
  uint8_t C;
  uint8_t D;
}

inline uint8_t
getNeighborA(size_t x, size_t y, size_t wid, size_t hei,
             const std::vector<uint8_t> &dataPlane) {
  return dataPlane.at(y * wid + (x - 1));
}

inline uint8_t getNeighborB(size_t x, size_t y, size_t wid, size_t hei,
                            const std::vector<uint8_t> &dataPlane) {
  return dataPlane.at((y - 1) * wid + x);
}

inline uint8_t getNeighborC(size_t x, size_t y, size_t wid, size_t hei,
                            const std::vector<uint8_t> &dataPlane) {
  return dataPlane.at((y - 1) * wid + (x - 1));
}

inline uint8_t getNeighborD(size_t x, size_t y, size_t wid, size_t hei,
                            const std::vector<uint8_t> &dataPlane) {
  return dataPlane.at((y - 1) * wid + (x + 1));
}

uint8_t getPrediction(size_t x, size_t y, size_t wid, size_t hei,
                      const std::vector<uint8_t> &dataPlane) {
  uint8_t prediction;
  if (x == y == 0) {
    prediction = dataPlane.at(y * wid + x);
  }
  if (x > 0 && y == 0) {
    prediction = getNeighborA(x, y, wid, hei, dataPlane);
  }
  if (x == 0 && y > 0) {
    prediction = getNeighborB(x, y, wid, hei, dataPlane);
  }
}
