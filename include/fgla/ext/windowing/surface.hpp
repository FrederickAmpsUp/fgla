#pragma once

#include <fgla/instance.hpp>
#include <fgla/internal.hpp>
#include <memory>

namespace fgla::ext::windowing {

/// Represents a surface, which is used to render to a window
class Surface {
public:
  struct Configuration {};

  /// The backend-defined implementation of the `Surface`'s functions
  struct Impl {
    virtual ~Impl() = 0;
  };

  /// Creates a `Surface` from a raw implementation
  /// This should only be used internally
  static inline Surface from_raw(std::shared_ptr<Impl> impl) {
    Surface surface;
    surface.impl = std::move(impl);
    return surface;
  }

private:
  friend struct fgla::internal::ImplAccessor;
  std::shared_ptr<Impl> impl;
};

inline Surface::Impl::~Impl() = default;

struct QueueTypeExt {
  static constexpr Queue::Type Present =
      Queue::Type(16); // a better way to do this is likely a good idea
};
struct PresentQueueOptions {
  const Surface &surface;
  const Adapter &adapter;
};
} // namespace fgla::ext::windowing
