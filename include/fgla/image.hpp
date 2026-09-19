#pragma once

#include <fgla/completion.hpp>
#include <fgla/error.hpp>
#include <fgla/image_subresource_range.hpp>
#include <fgla/image_view.hpp>
#include <fgla/memory.hpp>
#include <fgla/object_gen.hpp>
#include <fgla/types.hpp>

namespace fgla {

#define FGLA_OBJ_NAME Image

/**
 * Represents an image, a multidimensional (up to 3d) block of GPU-resident
 * structured data
 */
FGLA_OBJ_START

using SubresourceRange = _ImageSubresourceRange;
using AspectBits = _ImageSubresourceRange::AspectBits;
using AspectFlags = _ImageSubresourceRange::AspectFlags;

enum class Usage {
  NONE = 0,
  SAMPLED = 1 << 0,
  STORAGE = 1 << 1,
  COLOR_ATTACHMENT = 1 << 2,
  DEPTH_STENCIL_ATTACHMENT = 1 << 3,
  TRANSFER_SRC = 1 << 4,
  TRANSFER_DST = 1 << 5,
};

enum class Dimension {
  D1,
  D2,
  D3,
  D1_ARRAY,
  D2_ARRAY,
};

struct Descriptor {
  Memory::Properties memory_properties;

  Usage usage;

  Dimension dimension;
  Format format;

  Extent3d extent;
  uint32_t array_layers = 1;

  uint32_t mip_levels = 1;
};

#define FGLA_OBJ_FUNCTIONS(FN)                                                 \
  FN(/**                                                                       \
      * Creates an `ImageView` referencing this `Image` with the specified     \
      * settings                                                               \
      *                                                                        \
      * @param descriptor An `ImageView::Descriptor` specifying the data       \
      * format and subresource range of the `Image` to view                    \
      * @returns An `ImageView` with the specified settings or an `Error`      \
      * containing failure information                                         \
      */                                                                       \
     , Result<ImageView>, create_view, (const ImageView::Descriptor &desc),    \
     (desc))                                                                   \
  FN(/**                                                                       \
      * @returns a handle to an arbitrary `Completion` stored in this `Image`  \
      * @note This is a convenience slot to store an arbitrary `Completion`    \
      */                                                                       \
     , Completion &, get_completion, (), ())

FGLA_OBJ_END

constexpr Image::Usage operator|(Image::Usage a, Image::Usage b) noexcept {
  return (Image::Usage)((uint32_t)a | (uint32_t)b);
}
constexpr Image::Usage operator&(Image::Usage a, Image::Usage b) noexcept {
  return (Image::Usage)((uint32_t)a & (uint32_t)b);
}

#undef FGLA_OBJ_NAME
#undef FGLA_OBJ_FUNCTIONS

} // namespace fgla
