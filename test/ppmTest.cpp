#include <iostream>
#include <stdexcept>

#include "ppmlib.h"
#include "gtest/gtest.h"
#include "image.h"

using namespace anslib;
using namespace ppmlib;

TEST(reading_ppms, check_ppm_image_loads_into_image_obj) {
  struct PpmImage myPpm(CMAKE_SOURCE_DIR "/test_images/A1/im_c_flower-xxl-8");
  EXPECT_EQ(myPpm.fileMagic, "P6");
  EXPECT_EQ(myPpm.height_, 4832);
  EXPECT_EQ(myPpm.width_, 7376);
  EXPECT_EQ(myPpm.maxVal_, 255);
  Image id(myPpm.r, myPpm.g, myPpm.b, myPpm.width_, myPpm.height_);
}
