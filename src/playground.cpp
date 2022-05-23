#include <iostream>

#include "ansComp.h"
#include "bmplib.h"

int main() {
  std::vector<uint8_t> doopa;
  // anslib::shiftDClevel(doopa);

  anslib::bmplib::BmpImage myBmp;
  myBmp.bmpRead(CMAKE_SOURCE_DIR "/test/testimg.bmp");

  anslib::Image id(myBmp);

  for (const auto &plane : id.dataPlanes) {
    for (const auto &px : plane) {
      std::cout << px + 0 << "\t";
    }
    std::cout << '\n';
  }

  std::cout << "current cxx standard : " << __cplusplus << '\n';

  return 1;
}