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
const size_t ansCountsSize = std::numeric_limits<AnsSymbolType>::max() + 1;
const size_t ansCumulSize = std::numeric_limits<AnsSymbolType>::max() + 2;
const size_t ansCompTypeSize = std::numeric_limits<AnsSymbolType>::max();

extern uint32_t PROB_BITS;
extern uint32_t PROB_SCALE;
extern const uint32_t STATE_LOWER_BOUND;
extern const uint32_t ANS_SIGNATURE;
}  // namespace anslib