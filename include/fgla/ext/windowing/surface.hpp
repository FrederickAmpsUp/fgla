#pragma once

#include <fgla/instance.hpp>
#include <fgla/internal.hpp>
#include <memory>

namespace fgla::ext::windowing {

/// Represents a surface, which is used to render to a window
class Surface {
public:
  // TODO: descriptors (?)

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

} // namespace fgla::ext::windowing
