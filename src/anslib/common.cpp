#include "common.h"

#include <cstdint>
namespace anslib {
uint32_t PROB_BITS = 14u;
uint32_t PROB_SCALE = 1u << PROB_BITS;
const uint32_t STATE_LOWER_BOUND = 1u << 24;
const uint32_t ANS_SIGNATURE = 0x13;
}  // namespace anslib