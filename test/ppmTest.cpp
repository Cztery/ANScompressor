#include <iostream>
#include <stdexcept>

#include "gtest/gtest.h"
#include "image.h"
#include "ppmlib.h"

using namespace anslib;
using namespace ppmlib;

TEST(reading_ppms, check_ppm_image_loads_into_image_obj) {
  struct PpmImage myPpm(CMAKE_SOURCE_DIR
                        "/test_images/A1/im_c_flower-xxl-8.ppm");
  EXPECT_EQ(myPpm.fileMagic, "P6");
  EXPECT_EQ(myPpm.height_, 4832);
  EXPECT_EQ(myPpm.width_, 7376);
  EXPECT_EQ(myPpm.maxVal_, 255);
  RawImage id(myPpm.r, myPpm.g, myPpm.b, myPpm.width_, myPpm.height_);
}
