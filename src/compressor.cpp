#include "compressor.h"

namespace anslib {

// AnsProcessor::AnsProcessor(std::vector<AnsSymbol> pixChannel) :
// hist_(pixChannel), pixChannelRaw_(pixChannel) {
// }

// AnsProcessor::~AnsProcessor() {}

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
  AnsState cur_state = (AnsState)compressedPlane.back();       compressedPlane.pop_back();
          cur_state |= (AnsState)compressedPlane.back() << 8;  compressedPlane.pop_back();
          cur_state |= (AnsState)compressedPlane.back() << 16; compressedPlane.pop_back();
          cur_state |= (AnsState)compressedPlane.back() << 24; compressedPlane.pop_back();
          cur_state |= (AnsState)compressedPlane.back() << 32; compressedPlane.pop_back();
          cur_state |= (AnsState)compressedPlane.back() << 40; compressedPlane.pop_back();
          cur_state |= (AnsState)compressedPlane.back() << 48; compressedPlane.pop_back();
          cur_state |= (AnsState)compressedPlane.back() << 56; compressedPlane.pop_back();
  AnsSymbol cur_symbol;
  const uint32_t mask = (1u << PROB_BITS) - 1;
  while(!compressedPlane.empty()) {
    cur_symbol = cum2sym_.at(cur_state & (PROB_SCALE - 1));
    decoded_syms.push_back(cur_symbol);
    cur_state = hist_.counts_norm.at(cur_symbol) * (cur_state >> PROB_BITS) + (cur_state & mask) -
      hist_.cumul_norm.at(cur_symbol);
    if (cur_state < STATE_LOWER_BOUND) {
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

AnsDecoder::AnsDecoder(std::vector<AnsCountsType> symCounts,
                       std::vector<uint8_t> ansStateBytes)
    : ansInCompressedChannel_(ansStateBytes), hist_(symCounts) {
  hist_.norm_freqs();
  countCum2sym();
}

AnsState AnsEncoder::encodeSym(const AnsSymbol s, const AnsState x) {
  AnsState xTemp = x / hist_.counts_norm.at(s) * hist_.total +
                   hist_.cumul_norm.at(s) + (x % hist_.counts_norm.at(s));
  return xTemp;
}

AnsState AnsEncoder::renormState(AnsState x, std::vector<uint8_t> &stateBuf,
                                 const AnsSymbol s) {
  AnsState xMax =
      ((STATE_LOWER_BOUND >> PROB_BITS) << 8) * hist_.counts_norm.at(s);
  while (x >= xMax) {
    stateBuf.push_back(x & 0xff);
    x >>= 8;
  }
  return x;
}

std::vector<uint8_t> AnsEncoder::encodePlane(std::vector<AnsSymbol> plane) {
  std::vector<uint8_t> encoded_states;
  AnsState xTmp = STATE_LOWER_BOUND;
  for (const AnsSymbol s : plane) {
    xTmp = renormState(xTmp, encoded_states, s);
    xTmp = encodeSym(s, xTmp);
  }
  while (xTmp >= 0xff) {
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

}  // namespace anslib