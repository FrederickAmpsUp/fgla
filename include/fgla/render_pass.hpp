#pragma once

#include <fgla/buffer.hpp>
#include <fgla/color.hpp>
#include <fgla/image_view.hpp>
#include <fgla/object_gen.hpp>
#include <fgla/render_pipeline.hpp>
#include <vector>

namespace fgla {

#define FGLA_OBJ_NAME RenderPass

FGLA_OBJ_START

/**
 * Stores the desired behavior for
 * loading data from attachments
 */
struct LoadOp {
  enum class Op { LOAD, CLEAR, DONT_CARE };
  Op op;
  /**
   * The `Color` to clear to when using
   * `LoadOp::CLEAR` on a color attachment
   */
  Color clear_color = {};
  // TODO: clear depth

  static const LoadOp LOAD;
  static LoadOp CLEAR(Color clear_color);
  static const LoadOp DONT_CARE;
};

/**
 * Stores the desired behavior for
 * storing data to attachments
 */
struct StoreOp {
  enum class Op { STORE, DONT_CARE };
  Op op;

  static const StoreOp STORE;
  static const StoreOp DONT_CARE;
};

struct ColorAttachmentDescriptor {
  const ImageView &view;
  const LoadOp &load_op;
  const StoreOp &store_op;
};

/**
 * Stores the settings used to create a `RenderPass`
 */
struct Descriptor {
  const std::vector<ColorAttachmentDescriptor> &color_attachments;
};

struct DrawDescriptor {
  const RenderPipeline &pipeline;

  std::vector<std::reference_wrapper<Buffer>> vertex_buffers;

  uint32_t vertex_count;
  uint32_t first_vertex = 0;

  uint32_t instance_count = 1;
  uint32_t first_instance = 0;
};

#define FGLA_OBJ_FUNCTIONS(FN)                                                 \
  FN(/**                                                                       \
      * Draws the specified number of vertices using the                       \
      * specified `RenderPipeline`                                             \
      *                                                                        \
      * @param desc The `DrawDescriptor` containing                            \
      * settings to use for drawing                                            \
      */                                                                       \
     , void, draw, (const DrawDescriptor &desc), (desc))

FGLA_OBJ_END

#undef FGLA_OBJ_NAME
#undef FGLA_OBJ_FUNCTIONS

inline const RenderPass::LoadOp RenderPass::LoadOp::LOAD = {
    .op = RenderPass::LoadOp::Op::LOAD};
inline RenderPass::LoadOp RenderPass::LoadOp::CLEAR(Color clear_color) {
  return {.op = RenderPass::LoadOp::Op::CLEAR, .clear_color = clear_color};
}
inline const RenderPass::LoadOp RenderPass::LoadOp::DONT_CARE = {
    .op = RenderPass::LoadOp::Op::DONT_CARE};

inline const RenderPass::StoreOp RenderPass::StoreOp::STORE = {
    .op = RenderPass::StoreOp::Op::STORE};
inline const RenderPass::StoreOp RenderPass::StoreOp::DONT_CARE = {
    .op = RenderPass::StoreOp::Op::DONT_CARE};

} // namespace fgla
