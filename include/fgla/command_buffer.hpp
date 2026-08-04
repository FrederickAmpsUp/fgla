#pragma once

#include <fgla/error.hpp>
#include <fgla/internal.hpp>
#include <fgla/render_pass.hpp>
#include <memory>

namespace fgla {

/// Represents a command buffer, which is used to record commands before
/// submitting to the GPU
class CommandBuffer {
public:
  inline Result<RenderPass>
  begin_render_pass(const RenderPass::Descriptor &desc) {
    return this->impl->begin_render_pass(desc);
  }

  /// The backend-defined implementation of the `CommandBuffer`'s functions
  struct Impl {
    virtual Result<RenderPass>
    begin_render_pass(const RenderPass::Descriptor &desc) = 0;

    virtual ~Impl() = 0;
  };

  /// Creates a `CommandBuffer` from a raw implementation
  /// This should only be used internally
  static inline CommandBuffer from_raw(std::unique_ptr<Impl> impl) {
    CommandBuffer command_buffer;
    command_buffer.impl = std::move(impl);
    return command_buffer;
  }

private:
  friend struct fgla::internal::ImplAccessor;
  std::unique_ptr<Impl> impl;
};

inline CommandBuffer::Impl::~Impl() = default;
} // namespace fgla
