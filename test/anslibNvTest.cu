#include "image.cuh"
#include "gtest/gtest.h"
#include "ppmlib.h"

using namespace anslib;
using namespace ppmlib;

RawImage openTestImg() {
  PpmImage ppm(CMAKE_SOURCE_DIR "/test_images/A2/e50_a-1200-8.ppm");
  RawImage raw(ppm.r, ppm.g, ppm.b, ppm.width_, ppm.height_);
  return raw;
}

TEST(chunking_test, check_cuda_chunking_and_joining_validity) {
  RawImage inImg = openTestImg();
  ImageDev imgDev(inImg);
  imgDev.runCompressionPipeline(64);
  ImageDev imgDev2 = imgDev;
  imgDev2.runDecompressionPipeline();
  auto v1 = imgDev2.getPlane(0);
  auto v2 = imgDev2.getPlane(0);

  EXPECT_EQ(v1, v2);
}