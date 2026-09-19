#pragma once

#include <fgla/image_subresource_range.hpp>
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

  _ImageSubresourceRange subresource_range;
};

#define FGLA_OBJ_FUNCTIONS(FN)

FGLA_OBJ_END

#undef FGLA_OBJ_NAME
#undef FGLA_OBJ_FUNCTIONS

} // namespace fgla
