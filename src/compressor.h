#pragma once

#include "common.h"
#include "histogram.h"
#include "image.h"

namespace anslib {

struct SymbolStats {
  uint64_t a;     // reciprocal of counts; assuming counts is not more than 32 bits
  AnsState r;     // (M - counts) needed later to calculate new ANS state
  AnsState cumul; // cumulative counts needed later for new ANS state, here for data locality
  uint8_t shift;
};

class AnsEncoder {
 protected:
  std::vector<AnsSymbol> pixInRawChannel_;
  Histogram<AnsSymbol> hist_;
  AnsState encodeSym(const AnsSymbol s, const AnsState x, const SymbolStats& symStats);
  AnsState renormState(const AnsState x, std::vector<uint8_t> &stateBuf,
                       const AnsSymbol s);

 public:
  AnsEncoder(std::vector<AnsSymbol> pixChannel);
  AnsEncoder(std::vector<AnsSymbol> pixChannel,
             std::vector<AnsCountsType> &symCounts);

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

 public:
  AnsDecoder(const std::vector<AnsCountsType> &symCounts,
             const std::vector<uint8_t> &ansStateBytes);
  ~AnsDecoder(){};
  std::vector<AnsSymbol> decodePlane(std::vector<uint8_t> compressedPlane);
  std::vector<AnsSymbol> decodePlane();
};
}  // namespace anslib