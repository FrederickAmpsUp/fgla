#pragma once

#include <cstdint>

namespace fgla {

/// Represents a 2d extent, with width and height
struct Extent2d {
  union {
    /// The width or X dimension of the extent
    ///
    /// Alias of `width`
    uint32_t x;
    /// The width or X dimension of the extent
    ///
    /// Alias of `x`
    uint32_t width;
  };
  union {
    /// The height or Y dimension of the extent
    ///
    /// Alias of `height`
    uint32_t y;
    /// The height or Y dimension of the extent
    ///
    /// Alias of `y`
    uint32_t height;
  };
};
} // namespace fgla
