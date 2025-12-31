#pragma once

#include <cstdint>
#include <fgla/command_buffer.hpp>
#include <fgla/error.hpp>
#include <fgla/internal.hpp>
#include <memory>

namespace fgla {

/// Represents a logical queue, which is used to submit GPU commands
class Queue {
public:
  /// Type type of a `Queue`, indicating what functions it may perform
  enum class Type { Graphics, Transfer };

  /// Represents the creation of a number of `Queue`s, all with a shared `Type`
  struct Request {
    /// The type of the `Queue`s to create
    Type type;
    /// The number of `Queue`s of this type to create
    uint32_t count;
    /// @brief Any arbitrary data to use during queue creation
    ///
    /// This may be left null in most cases
    void *user_data = nullptr;
  };

  inline Result<CommandBuffer> begin_recording() { return this->impl->begin_recording(); }
  inline void submit(CommandBuffer &&cb) { return this->impl->submit(std::move(cb)); }

  /// The backend-defined implementation of the `Queue`'s functions
  struct Impl {
    virtual Result<CommandBuffer> begin_recording() = 0;
    virtual void submit(CommandBuffer &&cb) = 0;
    virtual ~Impl() = 0;
  };

  /// Creates a `Queue` from a raw implementation
  /// This should only be used internally
  static inline Queue from_raw(std::unique_ptr<Impl> impl) {
    Queue queue;
    queue.impl = std::move(impl);
    return queue;
  }

private:
  friend struct fgla::internal::ImplAccessor;
  std::unique_ptr<Impl> impl;
};

inline Queue::Impl::~Impl() = default;
} // namespace fgla
