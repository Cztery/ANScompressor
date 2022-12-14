#pragma once
#include <cstdint>
#include <cstddef>
#include <limits>


namespace anslib {
typedef uint8_t AnsSymbolType;
typedef uint32_t AnsState;
typedef uint32_t AnsCountsType;
typedef uint8_t AnsCompType;

const size_t ansMaxSymbol = std::numeric_limits<AnsSymbolType>::max();
const size_t ansCountsArrSize = std::numeric_limits<AnsSymbolType>::max() + 1;
const size_t ansCumulArrSize = std::numeric_limits<AnsSymbolType>::max() + 2;
const size_t ansSymbolTypeSize = sizeof(AnsSymbolType);
const size_t ansCompTypeSize = sizeof(std::numeric_limits<AnsSymbolType>::max());

extern uint16_t PROB_BITS;
extern uint32_t PROB_SCALE;
extern const uint32_t STATE_LOWER_BOUND;
extern const uint32_t ANS_SIGNATURE;
}  // namespace anslib