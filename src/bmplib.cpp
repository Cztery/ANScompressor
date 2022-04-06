#include "bmplib.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <ostream>
#include <stdexcept>

namespace anslib {
namespace bmplib {

void BmpImage::bmpRead(const char *filename) {
  std::ifstream f_img(filename, std::ios::binary);

  // std::cout << std::filesystem::current_path() << std::endl;
  // std::cout << filename << std::endl;
  if (!f_img.is_open()) {
    throw std::ios_base::failure("Could not open bitmap");
  }

  std::streampos fileSize;

  f_img.seekg(0, std::ios::end);
  fileSize = f_img.tellg();
  f_img.seekg(0, std::ios::beg);

  f_img.read((char *)&fileHeader_, sizeof(BmpFileHeader));
  f_img.read((char *)&infoHeader_, sizeof(BmpInfoHeader));
  // f_img.seekg(fileHeader_.pixelDataOffset);
  data.insert(data.begin(), std::istream_iterator<uint8_t>(f_img),
              std::istream_iterator<uint8_t>());
}

}  // namespace bmplib
}  // namespace anslib
