#pragma once

#include <fgla/internal.hpp>
#include <memory>

namespace fgla {

/// Represents a command buffer, which is used to record commands before submitting to the GPU
class CommandBuffer {
public:
  /// The backend-defined implementation of the `CommandBuffer`'s functions
  struct Impl {
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
