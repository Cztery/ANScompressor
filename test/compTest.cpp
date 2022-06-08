
#include "compressor.h"
#include "gtest/gtest.h"
#include "image.h"

using namespace anslib;
using namespace bmplib;

Image openTestImg() {
  // BmpImage bmp(CMAKE_SOURCE_DIR "/test/testimg.bmp");
  BmpImage bmp(CMAKE_SOURCE_DIR
               "/test_images/PHOTO_CD_KODAK/BMP_IMAGES/IMG0001.bmp");
  return Image(bmp);
}

class EncoderTest : public AnsEncoder, /*public AnsDecoder,*/ public testing::Test {
 public:
  EncoderTest() : AnsEncoder(openTestImg().dataPlanes.at(0)) {}
};

TEST_F(EncoderTest, dupadupa) {
  std::vector<uint8_t> encoded = encodePlane(pixInRawChannel_);
//   EXPECT_LT(encoded.size(), openTestImg().dataPlanes.at(0).size());
//   EXPECT_EQ(encoded.size(), openTestImg().dataPlanes.at(0).size());
  AnsDecoder dec1(hist_.counts, encoded);
  std::vector<AnsSymbol> decoded = dec1.decodePlane();
  EXPECT_EQ(decoded, pixInRawChannel_);

  // AnsDecoder dec1((AnsProcessor*)this);
  // AnsDecoder dec1(hist_.counts_norm, ansChannelComp_);
  // dec1.processImg();
}