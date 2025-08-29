#pragma once

#include <cstdint>
#include <magic_enum/magic_enum.hpp>

namespace fgla {

/// Represents a texture "format", the mapping of the texture's raw data to colors
class TextureFormat {
public:
  enum Value {
    R8_UNORM,
    R8_SNORM,
    R8_UINT,
    R8_SINT,

    R8G8_UNORM,
    R8G8_SNORM,
    R8G8_UINT,
    R8G8_SINT,

    R8G8B8A8_UNORM,
    R8G8B8A8_SNORM,
    R8G8B8A8_UINT,
    R8G8B8A8_SINT,
    R8G8B8A8_SRGB,

    B8G8R8A8_UNORM,
    B8G8R8A8_SRGB,

    R16_UNORM,
    R16_SNORM,
    R16_UINT,
    R16_SINT,
    R16_FLOAT,

    R16G16_UNORM,
    R16G16_SNORM,
    R16G16_UINT,
    R16G16_SINT,
    R16G16_FLOAT,

    R16G16B16A16_UNORM,
    R16G16B16A16_SNORM,
    R16G16B16A16_UINT,
    R16G16B16A16_SINT,
    R16G16B16A16_FLOAT,

    R32_UINT,
    R32_SINT,
    R32_FLOAT,

    R32G32_UINT,
    R32G32_SINT,
    R32G32_FLOAT,

    R32G32B32_UINT,
    R32G32B32_SINT,
    R32G32B32_FLOAT,

    R32G32B32A32_UINT,
    R32G32B32A32_SINT,
    R32G32B32A32_FLOAT,

    D16_UNORM,
    D24_UNORM_S8_UINT,
    D32_FLOAT,
    D32_FLOAT_S8_UINT,

    UNDEFINED
  };

  TextureFormat() = default;
  constexpr TextureFormat(Value v) : v(v) {}

  constexpr operator Value() const { return v; }
  explicit operator bool() const = delete;

  constexpr bool operator==(TextureFormat other) const { return this->v == other.v; }
  constexpr bool operator==(Value other) const { return this->v == other; }
  constexpr bool operator!=(TextureFormat other) const { return this->v != other.v; }
  constexpr bool operator!=(Value other) const { return this->v != other; }

  /// @brief Checks if the `TextureFormat` uses sRGB encoding
  bool is_srgb() const { // maintain me!
    switch (this->v) {
    case R8G8B8A8_SRGB:
    case B8G8R8A8_SRGB:
      return true;
    default:
      return false;
    }
  }

  std::string to_string() const { return std::string(magic_enum::enum_name(this->v)); }

private:
  Value v;
};

struct TextureFormatSupportDetails {
  bool sampleable = false;
  bool render_target = false;
  bool storage = false;
  bool linear_filter = false;
};

/// Represents a 2d extent, with width and height
struct Extent2d {
  union {
    /// The width or X dimension of the extent
    ///
    /// Alias of `width`
    uint32_t x;
    /// The width or X dimension of the extent
    ///
    /// Alias of `x`
    uint32_t width;
  };
  union {
    /// The height or Y dimension of the extent
    ///
    /// Alias of `height`
    uint32_t y;
    /// The height or Y dimension of the extent
    ///
    /// Alias of `y`
    uint32_t height;
  };
};
} // namespace fgla

namespace std {
template <> struct hash<fgla::TextureFormat> {
  std::size_t operator()(fgla::TextureFormat fmt) const noexcept {
    return std::hash<int>()(static_cast<int>(fmt));
  }
};
} // namespace std
