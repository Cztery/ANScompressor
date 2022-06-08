#pragma once
#include <cstdint>

namespace anslib {
typedef uint8_t AnsSymbol;
typedef uint64_t AnsState;
typedef uint32_t AnsCountsType;
const uint32_t PROB_BITS = 30;
const uint32_t PROB_SCALE = 1u << PROB_BITS;
const uint32_t STATE_LOWER_BOUND = 1u << PROB_BITS;
}  // namespace anslib