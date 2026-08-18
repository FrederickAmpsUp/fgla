#pragma once

#include <fgla/error.hpp>
#include <fgla/object_gen.hpp>
#include <fgla/render_pass.hpp>

namespace fgla {

#define FGLA_OBJ_NAME CommandBuffer
/**
 * Represents a command buffer, which is used to record commands before
 * submitting to the GPU
 */
FGLA_OBJ_START

#define FGLA_OBJ_FUNCTIONS(FN)                                                 \
  FN(/**                                                                       \
      * Begins a `RenderPass` using the specified descriptor                   \
      * @param desc The `RenderPass::Descriptor` describing the render pass    \
      * @returns The created `RenderPass`, or an `Error` with failure          \
      * information                                                            \
      */                                                                       \
     , Result<RenderPass>, begin_render_pass,                                  \
     (const RenderPass::Descriptor &desc), (desc))

FGLA_OBJ_END

#undef FGLA_OBJ_NAME
#undef FGLA_OBJ_FUNCTIONS

} // namespace fgla
