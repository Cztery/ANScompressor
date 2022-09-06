
#include "compressor.h"
#include "gtest/gtest.h"
#include "image.h"
#include "ppmlib.h"

using namespace anslib;
using namespace ppmlib;

RawImage openTestImg() {
  PpmImage ppm(CMAKE_SOURCE_DIR
               "/test_images/A2/e50_a-1200-8.ppm");
  return RawImage(ppm.r, ppm.g, ppm.b, ppm.width_, ppm.height_);
}

class EncoderTest : public AnsEncoder, public testing::Test {
 public:
  EncoderTest() : AnsEncoder(openTestImg().dataPlanes_.at(0)) {}
};

TEST_F(EncoderTest, check_data_validity_after_decompression) {
  std::vector<uint8_t> encoded = encodePlane(pixInRawChannel_);
  EXPECT_LT(encoded.size(), openTestImg().dataPlanes_.at(0).size());
  AnsDecoder dec1(hist_.counts, encoded);
  std::vector<AnsSymbol> decoded = dec1.decodePlane();
  EXPECT_EQ(decoded.size(), pixInRawChannel_.size());
  EXPECT_EQ(decoded, pixInRawChannel_);
}

TEST(encode_tests, check_data_validity_static_api) {
  RawImage inImg = openTestImg();
  CompImage outImg;
  RawImage decodedImg;
  AnsEncoder::compressImage(inImg, outImg);
  AnsDecoder::decompressImage(outImg, decodedImg);
  EXPECT_LT(outImg.bytesSizeOfImage(), inImg.bytesSizeOfImage());
  EXPECT_EQ(inImg.bytesSizeOfImage(), decodedImg.bytesSizeOfImage());
  EXPECT_EQ(decodedImg.dataPlanes_, inImg.dataPlanes_);
}