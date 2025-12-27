#pragma once

#include <cstdint>
#include <fgla/internal.hpp>
#include <fgla/types.hpp>
#include <memory>

namespace fgla {

/// Represents a view into an image.
class ImageView {
public:
  /// Determines the interpreted shape of the image data.
  enum class Mode { D1, D2, D3, D1_ARRAY, D2_ARRAY, CUBE, CUBE_ARRAY };

  struct Descriptor {
    TextureFormat format;
    ImageView::Mode mode;
    // TODO: aspects (color/depth/stencil)
    uint32_t base_mip_level, num_mip_levels;
    uint32_t base_array_layer, num_array_layers;
  };

  /// The backend-defined implementation of the `ImageView`'s functions
  struct Impl {

    virtual ~Impl() = 0;
  };

  /// Creates an `ImageView` from a raw implementation
  /// This should only be used internally
  static inline ImageView from_raw(std::unique_ptr<ImageView::Impl> impl) {
    ImageView image_view;
    image_view.impl = std::move(impl);
    return image_view;
  }

private:
  friend struct fgla::internal::ImplAccessor;
  std::unique_ptr<Impl> impl;
};

inline ImageView::Impl::~Impl() = default;

} // namespace fgla
