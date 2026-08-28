#pragma once

#include <stdint.h>

// Mapping helpers for raw analog readings -> usable values. Pure logic, so
// host-testable like the rest of the suite's non-hardware modules.
namespace mapping {

// Scale an ADC reading into 0..100 given an observed [min,max] range, clamped
// at the ends. A degenerate (min == max) range maps everything to 0 — the
// caller has no usable sweep yet, matching "unscanned" semantics.
static inline uint8_t percent(uint16_t raw, uint16_t min, uint16_t max) {
  if (max <= min) return 0;
  if (raw <= min) return 0;
  if (raw >= max) return 100;
  return (uint8_t)(((uint32_t)(raw - min) * 100u) / (max - min));
}

}  // namespace mapping