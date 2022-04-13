#pragma once

#include <cstdint>
#include <iomanip>
#include <ostream>
#include <vector>

namespace anslib {
namespace bmplib {

const uint16_t BMPmagic = 0x4D42;

#pragma pack(push, 1)

struct BmpInfoHeader {
  uint32_t size{sizeof(BmpInfoHeader)};
  int32_t width{0};
  int32_t height{0};
  uint16_t planes{1};
  uint16_t bitCount{0};
  uint32_t compression{0};  // only BI_RGB (0) supported
  uint32_t dataSize{0};
  int32_t horResolution{0};
  int32_t verResolution{0};
  uint32_t colorPalette{0};
  uint32_t importantColors{0};
};

struct BmpFileHeader {
  uint16_t fileFormat{BMPmagic};
  uint32_t fileSize{0};
  uint16_t reserved1{0};
  uint16_t reserved2{0};
  uint32_t pixelDataOffset{sizeof(BmpFileHeader) + sizeof(BmpInfoHeader)};
};

#pragma pack(pop)

struct BmpImage {
  BmpFileHeader fileHeader_;
  BmpInfoHeader infoHeader_;
  // no color table included;
  std::vector<uint8_t> data;

  BmpImage(){};
  BmpImage(BmpFileHeader fh, BmpInfoHeader ih, std::vector<uint8_t> inData);
  void bmpRead(const char* filename);
  void bmpWrite(const char* filename);
  size_t getPxIndex(uint32_t y, uint32_t x, uint plane) const;
  size_t getNumOfChannels() const;
  uint8_t& getRow(size_t n);
  void bmpPrint(void){};
};

inline std::ostream& operator<<(std::ostream& os, struct BmpFileHeader b) {
  os << "FileHeader:\n"
     << " fileFormat:\n\t" << std::setw(8) << std::right << std::hex
     << b.fileFormat << std::endl
     << " fileSize:\n\t" << std::setw(8) << std::right << std::hex << b.fileSize
     << std::endl
     << " reserved1:\n\t" << std::setw(8) << std::right << std::hex
     << b.reserved1 << std::endl
     << " reserved2:\n\t" << std::setw(8) << std::right << std::hex
     << b.reserved2 << std::endl
     << " pixelDataOffset:\n\t" << std::setw(8) << std::right << std::hex
     << b.pixelDataOffset << std::endl;

  return os;
}

inline std::ostream& operator<<(std::ostream& os, struct BmpInfoHeader b) {
  os << "InfoHeader:\n"
     << " size:\n\t" << std::setw(8) << std::right << std::hex << b.size
     << std::endl
     << " width:\n\t" << std::setw(8) << std::right << std::hex << b.width
     << std::endl
     << " height:\n\t" << std::setw(8) << std::right << std::hex << b.height
     << std::endl
     << " planes:\n\t" << std::setw(8) << std::right << std::hex << b.planes
     << std::endl
     << " bitCount:\n\t" << std::setw(8) << std::right << std::hex << b.bitCount
     << std::endl
     << " compression:\n\t" << std::setw(8) << std::right << std::hex
     << b.compression << std::endl
     << " dataSize:\n\t" << std::setw(8) << std::right << std::hex << b.dataSize
     << std::endl
     << " horResolution:\n\t" << std::setw(8) << std::right << std::hex
     << b.horResolution << std::endl
     << " verResolution:\n\t" << std::setw(8) << std::right << std::hex
     << b.verResolution << std::endl
     << " colorPalette:\n\t" << std::setw(8) << std::right << std::hex
     << b.colorPalette << std::endl
     << " importantColors:\n\t" << std::setw(8) << std::right << std::hex
     << b.importantColors << std::endl;

  return os;
}
inline std::ostream& operator<<(std::ostream& os, struct BmpImage b) {
  os << b.fileHeader_ << b.infoHeader_;
  return os;
}

}  // namespace bmplib
}  // namespace anslib
