#pragma once

#include <fgla/color.hpp>
#include <fgla/image_view.hpp>
#include <fgla/internal.hpp>
#include <fgla/render_pipeline.hpp>
#include <memory>
#include <vector>

namespace fgla {

class RenderPass {
public:
  struct LoadOp {
    enum class Op { LOAD, CLEAR, DONT_CARE };
    Op op;
    Color clear_color = {};
    // TODO: clear depth

    static const LoadOp LOAD;
    static LoadOp CLEAR(Color clear_color);
    static const LoadOp DONT_CARE;
  };

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

  /// Represents the settings used to create a `RenderPass`
  struct Descriptor {
    const std::vector<ColorAttachmentDescriptor> &color_attachments;
  };

  inline void bind_pipeline(const RenderPipeline &pipeline) {
    return this->impl->bind_pipeline(pipeline);
  }

  inline void draw(uint32_t vertex_count) {
    return this->impl->draw(vertex_count);
  }

  /// The backend-defined implementation of the `RenderPass`'s functions
  struct Impl {
    virtual void bind_pipeline(const RenderPipeline &) = 0;
    virtual void draw(uint32_t) = 0;

    virtual ~Impl() = 0;
  };

  /// Creates a `RenderPass` from a raw implementation
  /// This should only be used internally
  static inline RenderPass from_raw(std::unique_ptr<Impl> impl) {
    RenderPass render_pass;
    render_pass.impl = std::move(impl);
    return render_pass;
  }

private:
  friend struct fgla::internal::ImplAccessor;
  std::unique_ptr<Impl> impl;
};

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

inline RenderPass::Impl::~Impl() = default;

} // namespace fgla
