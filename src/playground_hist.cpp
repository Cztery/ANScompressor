#include "ansComp.h"
#include "bmplib.h"
#include "histogram.h"

int main() {
  anslib::bmplib::BmpImage inImg;
  inImg.bmpRead(CMAKE_SOURCE_DIR
                "/test_images/PHOTO_CD_KODAK/BMP_IMAGES/IMG0001.bmp");
  inImg.bmpWrite("doopa.bmp");
  std::cout << inImg.fileHeader_ << inImg.infoHeader_;
  anslib::Image img(inImg);

  Histogram<uint8_t> h0(img.dataPlanes.at(0));
  Histogram<uint8_t> h1(img.dataPlanes.at(1));
  Histogram<uint8_t> h2(img.dataPlanes.at(2));

  h0.print(90, false);
  h1.print(90, false);
  h2.print(90, false);
}