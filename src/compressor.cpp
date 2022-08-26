#include "compressor.h"

#include <iterator>
namespace anslib {

void AnsDecoder::countCum2sym() {
  cum2sym_.resize(PROB_SCALE + 1);
  for (unsigned short s = 0; s < hist_.cumul_norm.size() - 1; ++s) {
    for (AnsCountsType i = hist_.cumul_norm.at(s);
         i < hist_.cumul_norm.at(s + 1); ++i) {
      cum2sym_.at(i) = s;
    }
  }
}

void AnsDecoder::decodeSymAndAdvanceState(AnsState &x) {
  const uint32_t mask = (1u << PROB_BITS) - 1;
  const uint32_t cumul = x & mask;
  const AnsSymbol sym = cum2sym_.at(cumul);
  x = hist_.counts_norm.at(sym) * (x >> PROB_BITS) + (x & mask) -
      hist_.cumul_norm.at(sym);
  // TODO: normalize?
  if (x < STATE_LOWER_BOUND) {
    std::cout << "normalize now: x=" << x << '\n';
  }
  if (x > (1ul << 30)) {
    std::cout << "x state over 31 bits: x=" << x << '\n';
  }
}

std::vector<AnsSymbol> AnsDecoder::decodePlane(
    std::vector<uint8_t> compressedPlane) {
  std::vector<AnsSymbol> decoded_syms;
  // initialize ans state
  AnsState cur_state = (AnsState)compressedPlane.back() << 24;
  compressedPlane.pop_back();
  cur_state |= (AnsState)compressedPlane.back() << 16;
  compressedPlane.pop_back();
  cur_state |= (AnsState)compressedPlane.back() << 8;
  compressedPlane.pop_back();
  cur_state |= (AnsState)compressedPlane.back();
  compressedPlane.pop_back();
  // cur_state |= (AnsState)compressedPlane.back() << 32;
  // compressedPlane.pop_back(); cur_state |= (AnsState)compressedPlane.back()
  // << 40; compressedPlane.pop_back(); cur_state |=
  // (AnsState)compressedPlane.back() << 48; compressedPlane.pop_back();
  // cur_state |= (AnsState)compressedPlane.back() << 56;
  // compressedPlane.pop_back();
  AnsSymbol cur_symbol;
  const uint32_t mask = PROB_SCALE - 1;
  while (cur_state != ANS_SIGNATURE) {
    cur_symbol = cum2sym_.at(cur_state & mask);
    decoded_syms.push_back(cur_symbol);
    cur_state = hist_.counts_norm.at(cur_symbol) * (cur_state >> PROB_BITS) +
                (cur_state & mask) - hist_.cumul_norm.at(cur_symbol);
    while (cur_state < STATE_LOWER_BOUND && !compressedPlane.empty()) {
      cur_state <<= 8;
      cur_state |= compressedPlane.back();
      compressedPlane.pop_back();
    }
  }
  return decoded_syms;
}

std::vector<AnsSymbol> AnsDecoder::decodePlane() {
  return decodePlane(ansInCompressedChannel_);
}

AnsDecoder::AnsDecoder(const std::vector<AnsCountsType> &symCounts,
                       const std::vector<uint8_t> &ansStateBytes)
    : ansInCompressedChannel_(ansStateBytes), hist_(symCounts) {
  hist_.norm_freqs();
  countCum2sym();
}

AnsState AnsEncoder::encodeSym(const AnsSymbol s, const AnsState x, const SymbolStats& symStats) {
  AnsState q = ((uint64_t) x * symStats.a) >> symStats.shift;
  AnsState xTemp = x + symStats.cumul + q * symStats.r;
  return xTemp;
}

AnsState AnsEncoder::renormState(AnsState x, std::vector<uint8_t> &stateBuf,
                                 const AnsSymbol s) {
  auto needsRenorm = [&]() {
    return ((x >> (sizeof(AnsState) * 8 - PROB_BITS)) >=
            hist_.counts_norm.at(s));
  };
  while (needsRenorm()) {
    stateBuf.push_back(x & 0xff);
    x >>= 8;
  }
  return x;
}

std::vector<uint8_t> AnsEncoder::encodePlane(std::vector<AnsSymbol> plane) {
  std::vector<SymbolStats> enc_stats;
  for (size_t i = 0; i <= std::numeric_limits<AnsSymbol>::max(); ++i) {
    SymbolStats stats;
    if(hist_.counts_norm.at(i)) {
      uint8_t shift = 0;
      while (hist_.counts_norm.at(i) > (1u << shift))
        ++shift;
      stats.shift = shift + 31;
      stats.a = ((1ull << stats.shift) + hist_.counts_norm.at(i) - 1) / (uint64_t)hist_.counts_norm.at(i); // reciprocal of counts; assuming counts is not more than 32 bits
      stats.r = PROB_SCALE - hist_.counts_norm.at(i);             // (M - counts) needed later to calculate new ANS state
      stats.cumul = hist_.cumul_norm.at(i);                       // cumulative counts needed later for new ANS state, here for data locality
      enc_stats.push_back(stats);
    } else {
      enc_stats.push_back({0,0,0});
    }
  }
  std::vector<uint8_t> encoded_states;
  AnsState xTmp = ANS_SIGNATURE;

  for (std::vector<AnsSymbol>::reverse_iterator s = plane.rbegin();
       s != plane.rend(); ++s) {
    xTmp = renormState(xTmp, encoded_states, *s);
    xTmp = encodeSym(*s, xTmp, enc_stats.at(*s));
  }
  while (xTmp) {
    encoded_states.push_back(xTmp & 0xff);
    xTmp >>= 8;
  }
  return encoded_states;
}

std::vector<uint8_t> AnsEncoder::encodePlane() {
  return encodePlane(pixInRawChannel_);
}

AnsEncoder::AnsEncoder(std::vector<AnsSymbol> pixChannel)
    : pixInRawChannel_(pixChannel), hist_(pixChannel) {
  hist_.norm_freqs();
}

AnsEncoder::AnsEncoder(std::vector<AnsSymbol> pixChannel,
                       std::vector<AnsCountsType> &symCounts)
    : pixInRawChannel_(pixChannel), hist_(pixChannel) {
  hist_.norm_freqs();
  symCounts = hist_.counts_norm;
}

}  // namespace anslib