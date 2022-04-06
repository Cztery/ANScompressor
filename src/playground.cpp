#include <iostream>

#include "ansComp.h"
#include "bmplib.h"

int main() {
  std::vector<uint8_t> doopa;
  // anslib::shiftDClevel(doopa);

  anslib::bmplib::BmpImage myBmp;
  myBmp.bmpRead(CMAKE_SOURCE_DIR "/test/testimg.bmp");

  anslib::Image id(myBmp);

  for (auto plane : id.dataPlanes) {
    for (auto px : plane) {
      std::cout << px + 0 << "\t";
    }
    std::cout << std::endl;
  }

  return 1;
}