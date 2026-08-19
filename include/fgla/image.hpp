#pragma once

#include <fgla/completion.hpp>
#include <fgla/error.hpp>
#include <fgla/image_view.hpp>
#include <fgla/object_gen.hpp>

namespace fgla {

#define FGLA_OBJ_NAME Image

/**
 * Represents an image, a multidimensional (up to 3d) block of GPU-resident
 * structured data
 */
FGLA_OBJ_START

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

#undef FGLA_OBJ_NAME
#undef FGLA_OBJ_FUNCTIONS

} // namespace fgla
