#pragma once

#include <fgla/object_gen.hpp>
#include <fgla/types.hpp>

namespace fgla {

#define FGLA_OBJ_NAME ImageView

/**
 * Represents a view of an `Image`, specifying the format, aspect
 * and subresource range through which the image is accessed
 */
FGLA_OBJ_START

/**
 * The interpreted shape of the data
 */
enum class Mode { D1, D2, D3, D1_ARRAY, D2_ARRAY, CUBE, CUBE_ARRAY };

/**
 * Specifies which aspect(s) of an `Image` an `ImageView` accesses.
 */
struct AspectBits {
  enum Value : uint8_t { COLOR = 1 << 0, DEPTH = 1 << 1, STENCIL = 1 << 2 };
};
using AspectFlags = uint8_t;

/**
 * Stores the settings used to create an `ImageView`
 */
struct Descriptor {
  /**
   * The color format to interpret the `Image` as
   */
  Format format;
  /**
   * The shape to interpret the `Image` as
   */
  Mode mode;
  /**
   * The aspect of the `Image` to access
   */
  AspectFlags aspect_flags;

  uint32_t base_mip_level, num_mip_levels;
  uint32_t base_array_layer, num_array_layers;
};

#define FGLA_OBJ_FUNCTIONS(FN)

FGLA_OBJ_END

#undef FGLA_OBJ_NAME
#undef FGLA_OBJ_FUNCTIONS

} // namespace fgla
