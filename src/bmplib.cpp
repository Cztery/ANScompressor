#include "bmplib.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <ostream>
#include <stdexcept>

namespace anslib {
namespace bmplib {

BmpImage::BmpImage(BmpFileHeader fh, BmpInfoHeader ih,
                   const std::vector<uint8_t> &inData)
    : fileHeader_(fh), infoHeader_(ih) {}

void BmpImage::bmpRead(const char *filename) {
  std::ifstream f_img(filename, std::ios::binary);

  if (!f_img.is_open()) {
    throw std::ios_base::failure("Could not open bitmap");
  }

  std::streampos fileSize;

  f_img.seekg(0, std::ios::end);
  fileSize = f_img.tellg();
  f_img.seekg(0, std::ios::beg);

  f_img.read(reinterpret_cast<char *>(&fileHeader_), sizeof(BmpFileHeader));
  f_img.read(reinterpret_cast<char *>(&infoHeader_), sizeof(BmpInfoHeader));
  // f_img.seekg(fileHeader_.pixelDataOffset);

  data.reserve(fileSize - f_img.tellg());

  char byte;
  while (f_img.read(&byte, sizeof(byte))) {
    data.emplace_back(byte);
  }
}

void BmpImage::bmpWrite(const char *filename) {
  std::ofstream f_img(filename, std::ios::binary);

  if (!f_img.is_open()) {
    throw std::ios_base::failure("Could not open bitmap");
  }
  if (f_img.write(reinterpret_cast<char *>(&fileHeader_), sizeof(BmpFileHeader))
          .rdstate() &
      std::ofstream::badbit) {
    throw std::ios_base::failure("Could not write fileHeader to bitmap");
  }
  if (f_img.write(reinterpret_cast<char *>(&infoHeader_), sizeof(BmpInfoHeader))
          .rdstate() &
      std::ofstream::badbit) {
    throw std::ios_base::failure("Could not write infoHeader to bitmap");
  }
  if (f_img
          .write(reinterpret_cast<char *>(data.data()),
                 data.size() * sizeof(char))
          .rdstate() &
      std::ofstream::badbit) {
    throw std::ios_base::failure("Could not write pixel data to bitmap");
  }
}

size_t BmpImage::getPxIndex(uint32_t y, uint32_t x, uint plane) const {
  size_t numOfChannels = getNumOfChannels();
  const size_t rowPadding = infoHeader_.width * numOfChannels % 4
                                ? 4 - (infoHeader_.width * numOfChannels % 4)
                                : 0;
  const size_t rowOffset = (infoHeader_.height - y - 1) *
                           (numOfChannels * infoHeader_.width + rowPadding);

  return rowOffset + x * numOfChannels + plane;
}

size_t BmpImage::getNumOfChannels() const {
  switch (infoHeader_.compression) {
    case 0:
    default:
      return 3;
  }
}
}  // namespace bmplib
}  // namespace anslib
