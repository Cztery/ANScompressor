// #pragma once
#include <cstdint>

namespace anslib {
typedef uint8_t AnsSymbol;
typedef uint32_t AnsState;
typedef uint32_t AnsCountsType;
extern uint32_t PROB_BITS;
extern uint32_t PROB_SCALE;
extern const uint32_t STATE_LOWER_BOUND;
extern const uint32_t ANS_SIGNATURE;
}  // namespace anslib