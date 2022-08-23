#include <iostream>
#include <stdexcept>

#include "bmplib.h"
#include "gtest/gtest.h"
#include "image.h"

using namespace anslib;
using namespace bmplib;

TEST(sample_test_case, sample_test) { EXPECT_EQ(1, 1); }

TEST(reading_bitmaps, throw_on_nonexistent) {
  BmpImage myBmp;
  try {
    myBmp.bmpRead("nonexistent.bmp");
    FAIL() << "Expected std::ios_base::failure";
  } catch (std::ios_base::failure const &err) {
    EXPECT_EQ(err.what(), std::string("Could not open bitmap: iostream error"));
  } catch (...) {
    FAIL() << "Expected \"Could not open bitmap\" exception";
  }
}

TEST(reading_bitmaps, reading_file_header) {
  BmpImage myBmp;
  myBmp.bmpRead(CMAKE_SOURCE_DIR "/test/testimg.bmp");
  EXPECT_EQ(myBmp.fileHeader_.fileFormat, BMPmagic);
  // testing.bmp has a fileSize set manually to 0x86
  EXPECT_EQ(myBmp.fileHeader_.fileSize, 0x86);
  EXPECT_EQ(myBmp.fileHeader_.reserved1, 0);
  EXPECT_EQ(myBmp.fileHeader_.reserved2, 0);
  EXPECT_EQ(myBmp.fileHeader_.pixelDataOffset, 0x36);

  EXPECT_EQ(myBmp.infoHeader_.DIBsize, 0x28);
  EXPECT_EQ(myBmp.infoHeader_.width, 0x5);
  EXPECT_EQ(myBmp.infoHeader_.height, 0x5);
  EXPECT_EQ(myBmp.infoHeader_.planes, 0x1);
  EXPECT_EQ(myBmp.infoHeader_.bitCount, 0x18);  // incorrect value, set manually
  EXPECT_EQ(myBmp.infoHeader_.compression, 0x0);  // BI_RGB no compression
  EXPECT_EQ(myBmp.infoHeader_.dataSize, 0x50);
  EXPECT_EQ(myBmp.infoHeader_.horResolution, 0x0EC4);
  EXPECT_EQ(myBmp.infoHeader_.verResolution, 0x0EC4);
  EXPECT_EQ(myBmp.infoHeader_.colorPalette, 0x0);
  EXPECT_EQ(myBmp.infoHeader_.importantColors, 0x0);
}

TEST(reading_bitmaps, check_red_image) {
  BmpImage myBmp;
  myBmp.bmpRead(CMAKE_SOURCE_DIR
                "/test/redTesting.bmp");  // generated by MS Paint
  // EXPECT_TRUE(false) << myBmp;
  EXPECT_EQ(myBmp.infoHeader_.height, 6);
  EXPECT_EQ(myBmp.infoHeader_.width, 6);
  EXPECT_EQ(myBmp.fileHeader_.pixelDataOffset,
            sizeof(myBmp.fileHeader_) + sizeof(myBmp.infoHeader_));
  EXPECT_EQ(myBmp.fileHeader_.fileSize, sizeof(myBmp.fileHeader_) +
                                            sizeof(myBmp.infoHeader_) +
                                            myBmp.infoHeader_.dataSize);
  Image id(myBmp);

  EXPECT_EQ(id.dataPlanes_.at(2).at(1), 255);
  EXPECT_EQ(id.dataPlanes_.at(1).at(1), 0);
  EXPECT_EQ(id.dataPlanes_.at(0).at(1), 0);
}

TEST(reading_bitmaps, creating_ans_image) {
  BmpImage myBmp;
  myBmp.bmpRead(CMAKE_SOURCE_DIR "/test/testimg.bmp");

  Image id(myBmp);
  EXPECT_EQ(id.width_, 0x5);
  EXPECT_EQ(id.height_, 0x5);

  EXPECT_EQ(id.dataPlanes_.at(0).size(), 25);
  EXPECT_EQ(id.dataPlanes_.at(1).size(), 25);
  EXPECT_EQ(id.dataPlanes_.at(2).size(), 25);
  EXPECT_EQ(id.dataPlanes_.size(), 3);

  EXPECT_EQ(id.dataPlanes_.at(0).at(10), 0);
  EXPECT_EQ(id.dataPlanes_.at(1).at(10), 0);  // black pixel
  EXPECT_EQ(id.dataPlanes_.at(2).at(10), 0);
  EXPECT_EQ(id[10], std::make_tuple(0, 0, 0));

  EXPECT_EQ(id.dataPlanes_.at(0).at(11), 255);
  EXPECT_EQ(id.dataPlanes_.at(1).at(11), 0);  // blue pixel
  EXPECT_EQ(id.dataPlanes_.at(2).at(11), 0);
  EXPECT_EQ(id[11], std::make_tuple(255, 0, 0));

  EXPECT_EQ(id.dataPlanes_.at(0).at(24), 0);
  EXPECT_EQ(id.dataPlanes_.at(1).at(24), 255);  // should be green
  EXPECT_EQ(id.dataPlanes_.at(2).at(24), 0);
}