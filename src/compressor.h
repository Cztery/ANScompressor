#pragma once

#include "common.h"
#include "histogram.h"
#include "image.h"

namespace anslib {

struct SymbolStats {
  uint64_t a;  // reciprocal of counts; assuming counts is not more than 32 bits
  AnsState r;  // (M - counts) needed later to calculate new ANS state
  AnsState cumul;  // cumulative counts needed later for new ANS state, here for
                   // data locality
  uint8_t shift;
};

class AnsEncoder {
 protected:
  std::vector<AnsSymbol> pixInRawChannel_;
  Histogram<AnsSymbol> hist_;
  AnsState encodeSym(const AnsSymbol s, const AnsState x,
                     const SymbolStats &symStats);
  AnsState renormState(const AnsState x, std::vector<uint8_t> &stateBuf,
                       const AnsSymbol s);
  std::vector<SymbolStats> prepareEncSymStats();
  static void compressPlane(const std::vector<anslib::AnsSymbol> &inData,
                            std::vector<anslib::AnsCountsType> &symCounts,
                            std::vector<uint8_t> &outData);

 public:
  AnsEncoder(const std::vector<AnsSymbol> &pixChannel);
  AnsEncoder(const std::vector<AnsSymbol> &pixChannel,
             std::vector<AnsCountsType> &symCounts);
  static void compressImage(const anslib::RawImage &inImg,
                            anslib::CompImage &outImg);
  std::vector<uint8_t> encodePlane(std::vector<AnsSymbol> rawPlane);
  std::vector<uint8_t> encodePlane();
};

class AnsDecoder {
 protected:
  std::vector<uint8_t> ansInCompressedChannel_;
  Histogram<AnsSymbol> hist_;
  std::vector<AnsSymbol> cum2sym_;
  void decodeSymAndAdvanceState(AnsState &x);
  void countCum2sym();
  static void decompressPlane(
      const std::vector<uint8_t> &inData,
      const std::vector<anslib::AnsCountsType> &sym_counts,
      std::vector<anslib::AnsSymbol> &outData);

 public:
  AnsDecoder(const std::vector<AnsCountsType> &symCounts,
             const std::vector<uint8_t> &ansStateBytes);
  ~AnsDecoder(){};
  static void decompressImage(const anslib::CompImage &inImg,
                              anslib::RawImage &outImg);
  std::vector<AnsSymbol> decodePlane(std::vector<uint8_t> compressedPlane);
  std::vector<AnsSymbol> decodePlane();
};
}  // namespace anslib