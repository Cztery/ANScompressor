#pragma once
#include <cstdint>
#include <cstddef>
#include <limits>


namespace anslib {
typedef uint8_t AnsSymbol;
typedef uint32_t AnsState;
typedef uint32_t AnsCountsType;

const size_t ansMaxSymbol = std::numeric_limits<AnsSymbol>::max();
const size_t ansCountsSize = std::numeric_limits<AnsSymbol>::max() + 1;
const size_t ansCumulSize = std::numeric_limits<AnsSymbol>::max() + 2;

extern uint32_t PROB_BITS;
extern uint32_t PROB_SCALE;
extern const uint32_t STATE_LOWER_BOUND;
extern const uint32_t ANS_SIGNATURE;
}  // namespace anslib