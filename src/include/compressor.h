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
  std::vector<AnsSymbolType> pixInRawChannel_;
  Histogram<AnsSymbolType> hist_;
  AnsState encodeSym(const AnsSymbolType s, const AnsState x,
                     const SymbolStats &symStats);
  AnsState renormState(const AnsState x, std::vector<uint8_t> &stateBuf,
                       const AnsSymbolType s);
  std::vector<SymbolStats> prepareEncSymStats();
  static void compressPlane(const std::vector<anslib::AnsSymbolType> &inData,
                            std::vector<anslib::AnsCountsType> &symCounts,
                            std::vector<uint8_t> &outData);

 public:
  AnsEncoder(const std::vector<AnsSymbolType> &pixChannel);
  AnsEncoder(const std::vector<AnsSymbolType> &pixChannel,
             std::vector<AnsCountsType> &symCounts);
  static void compressImage(const anslib::RawImage &inImg,
                            anslib::CompImage &outImg);
  std::vector<uint8_t> encodePlane(std::vector<AnsSymbolType> rawPlane);
  std::vector<uint8_t> encodePlane();
};

class AnsDecoder {
 protected:
  std::vector<uint8_t> ansInCompressedChannel_;
  Histogram<AnsSymbolType> hist_;
  size_t rawPlaneSize_;
  std::vector<AnsSymbolType> cum2sym_;
  void decodeSymAndAdvanceState(AnsState &x);
  void countCum2sym();
  static void decompressPlane(
      const std::vector<uint8_t> &inData,
      const std::vector<anslib::AnsCountsType> &sym_counts,
      size_t rawPlaneSize,
      std::vector<anslib::AnsSymbolType> &outData);

 public:
  AnsDecoder(const std::vector<AnsCountsType> &symCounts,
             const std::vector<uint8_t> &ansStateBytes,
             size_t rawPlaneSize);
  ~AnsDecoder(){};
  static void decompressImage(const anslib::CompImage &inImg,
                              anslib::RawImage &outImg);
  std::vector<AnsSymbolType> decodePlane(std::vector<uint8_t> compressedPlane);
  std::vector<AnsSymbolType> decodePlane();
};
}  // namespace anslib