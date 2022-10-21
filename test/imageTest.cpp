#include "image.h"
#include "gtest/gtest.h"
#include "bmplib.h"

using namespace anslib;
using namespace bmplib;

RawImage openTestImg() {
  bmplib::BmpImage bmp(CMAKE_SOURCE_DIR "/test_images/PHOTO_CD_KODAK/BMP_IMAGES/IMG0003.bmp");
  RawImage raw(bmp);
  return raw;
}

TEST(chunking_tests, chunking_is_reversible) {
  RawImage referenceImg = openTestImg();
  RawImage mergedImg = openTestImg();
  mergedImg.splitIntoChunks(64);
  EXPECT_NE(mergedImg.dataPlanes_, referenceImg.dataPlanes_);
  mergedImg.mergeImageChunks();
  bmplib::BmpImage bmp(CMAKE_SOURCE_DIR "/test_images/PHOTO_CD_KODAK/BMP_IMAGES/IMG0003.bmp");
  bmplib::BmpImage out(bmp.fileHeader_, bmp.infoHeader_, mergedImg.getPlanesAsBmpData());
  out.bmpWrite("ejoo.bmp");
  EXPECT_EQ(mergedImg.dataPlanes_, referenceImg.dataPlanes_);
}