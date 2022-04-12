#include "ansComp.h"
#include "bmplib.h"
#include "histogram.h"

int main() {
  //   {
  //     std::vector<uint8_t> vec8;
  //     std::vector<uint16_t> vec16;

  //     srand(time(NULL));

  //     for (size_t a = 0; a < 8000; ++a) {
  //         vec8.push_back(rand() % std::numeric_limits<uint8_t>::max());
  //     }
  //     for (size_t a = 0; a < 1000; ++a) {
  //         vec16.push_back(rand() % std::numeric_limits<uint16_t>::max());
  //     }

  //     Histogram<uint8_t> h1(vec8);
  //     Histogram<uint16_t> h2(vec16);

  //     h1.print<uint8_t>();

  //     std::cout << std::endl;

  //     h2.print<uint16_t>();
  //   }

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