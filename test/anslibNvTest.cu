#include "gtest/gtest.h"
#include "image.cuh"
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
  ImageCompressor comp(inImg);
  const size_t CHUNKWID = 64;
  ImageDev imgDev = comp.compress(CHUNKWID);
  EXPECT_EQ(imgDev.imgInfo.numOfChunksPerPlane_,
            (imgDev.imgInfo.width_ + CHUNKWID - 1) / CHUNKWID *
                (imgDev.imgInfo.height_ + CHUNKWID - 1) / CHUNKWID);
  EXPECT_EQ(imgDev.imgInfo.chunkWidth_, CHUNKWID);
  auto v1 = imgDev.getPlane(0);

  // ImageDev imgDev2 = imgDev;
  ImageDev imgDev2 = comp.decompress();
  auto v2 = imgDev.getPlane(0);

  EXPECT_EQ(v1, v2);
  EXPECT_EQ(imgDev.imgInfo.numOfChunksPerPlane_, 1);
  EXPECT_EQ(imgDev.imgInfo.chunkWidth_, 0);
}