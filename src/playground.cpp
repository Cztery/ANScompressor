#include <iostream>

#include "bmplib.h"
#include "image.h"

int main() {
  std::vector<uint8_t> doopa;
  // anslib::shiftDClevel(doopa);
  class AllBmp : public anslib::bmplib::BmpImage {};
  anslib::bmplib::BmpImage inImg;
  inImg.bmpRead(CMAKE_SOURCE_DIR
                "/test_images/PHOTO_CD_KODAK/BMP_IMAGES/IMG0002.bmp");
  anslib::Image img(inImg);
  for (auto &pix : img.dataPlanes_.at(0)) {
    pix *= 0.9;
  }
  for (auto &pix : img.dataPlanes_.at(1)) {
    pix *= 0.2;
  }
  for (auto &pix : img.dataPlanes_.at(2)) {
    pix *= 0.4;
  }
  auto bmpData = img.getPlanesAsBmpData();
  anslib::bmplib::BmpImage outImg(inImg.fileHeader_, inImg.infoHeader_,
                                  bmpData);
  if (outImg.data.size() != inImg.data.size()) {
    for (size_t i = 0; i < inImg.data.size() && i < outImg.data.size(); ++i) {
      if (inImg.data.at(i) != outImg.data.at(i)) {
        std::cout << "so it's here!! : i = " << i << '\n';
      }
    }
    std::cout << "in bmp and out png sizes dont match:\nin = "
              << inImg.data.size() << "\nout = " << outImg.data.size() << '\n';
  }
  outImg.bmpWrite("doopa.bmp");

  if (0) {
    anslib::bmplib::BmpImage myBmp;
    myBmp.bmpRead(CMAKE_SOURCE_DIR "/test/testimg.bmp");

    anslib::Image id(myBmp);

    for (const auto &plane : id.dataPlanes_) {
      for (const auto &px : plane) {
        std::cout << px + 0 << "\t";
      }
      std::cout << '\n';
    }

    std::cout << "current cxx standard : " << __cplusplus << '\n';
  }
  return 1;
}