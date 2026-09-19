#pragma once

// this file is needed because C++ is a picky little BUM who
// can't handle the thought of 2 modules that reference each other

#include <cstdint>

namespace fgla {

struct _ImageSubresourceRange {
  /**
   * Specifies which aspect(s) of an `Image` to access.
   */
  struct AspectBits {
    enum Value : uint8_t { COLOR = 1 << 0, DEPTH = 1 << 1, STENCIL = 1 << 2 };
  };
  using AspectFlags = uint8_t;

  /**
   * The aspect of the `Image` to access
   */
  AspectFlags aspect_flags;

  uint32_t base_mip_level = 0, num_mip_levels = 1;
  uint32_t base_array_layer = 0, num_array_layers = 1;
};
} // namespace fgla
