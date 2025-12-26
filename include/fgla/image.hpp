#pragma once

#include <memory>
#include <fgla/internal.hpp>

namespace fgla {

/// Represents an image, a multidimensional (up to 3d) block of GPU-resident structured data
class Image {
public:
  
  /// The backend-defined implementation of the `Image`'s functions
  struct Impl {
  
    virtual ~Impl() = 0;
  };

  /// Creates an `Image` from a raw implentation
  /// This should only be used internally
  static inline Image from_raw(std::unique_ptr<Image::Impl> impl) {
    Image image;
    image.impl = std::move(impl);
    return image;
  }
private:
  friend struct fgla::internal::ImplAccessor;
  std::unique_ptr<Impl> impl;
};

inline Image::Impl::~Impl() = default;

} // namespace fgla
