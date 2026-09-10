#pragma once

#include <fgla/buffer.hpp>
#include <fgla/error.hpp>
#include <fgla/object_gen.hpp>
#include <fgla/render_pass.hpp>
#include <initializer_list>

namespace fgla {

#define FGLA_OBJ_NAME CommandBuffer
/**
 * Represents a command buffer, which is used to record commands before
 * submitting to the GPU
 */
FGLA_OBJ_START

struct BufferCopy {
  size_t src_offset;
  size_t dst_offset;
  size_t size;
};

#define FGLA_OBJ_FUNCTIONS(FN)                                                 \
  FN(/**                                                                       \
      * Begins a `RenderPass` using the specified descriptor                   \
      * @param desc The `RenderPass::Descriptor` describing the render pass    \
      * @returns The created `RenderPass`, or an `Error` with failure          \
      * information                                                            \
      */                                                                       \
     , Result<RenderPass>, begin_render_pass,                                  \
     (const RenderPass::Descriptor &desc), (desc))                             \
  FN(/**                                                                       \
      */                                                                       \
     , void, copy_buffer,                                                      \
     (const Buffer &src, const Buffer &dst,                                    \
      std::initializer_list<BufferCopy> regions),                              \
     (src, dst, regions))

FGLA_OBJ_END

#undef FGLA_OBJ_NAME
#undef FGLA_OBJ_FUNCTIONS

} // namespace fgla
