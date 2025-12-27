#pragma once

#include <fgla/image_view.hpp>
#include <fgla/types.hpp>
#include <spdlog/spdlog.h>
#include <vulkan/vulkan.h>

namespace fgla::backends::vulkan {

#define VK_FORMAT_MAPPING                                                                          \
  CASE(R8_UNORM, VK_FORMAT_R8_UNORM)                                                               \
  CASE(R8_SNORM, VK_FORMAT_R8_SNORM)                                                               \
  CASE(R8_UINT, VK_FORMAT_R8_UINT)                                                                 \
  CASE(R8_SINT, VK_FORMAT_R8_SINT)                                                                 \
                                                                                                   \
  CASE(R8G8_UNORM, VK_FORMAT_R8G8_UNORM)                                                           \
  CASE(R8G8_SNORM, VK_FORMAT_R8G8_SNORM)                                                           \
  CASE(R8G8_UINT, VK_FORMAT_R8G8_UINT)                                                             \
  CASE(R8G8_SINT, VK_FORMAT_R8G8_SINT)                                                             \
                                                                                                   \
  CASE(R8G8B8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM)                                                   \
  CASE(R8G8B8A8_SNORM, VK_FORMAT_R8G8B8A8_SNORM)                                                   \
  CASE(R8G8B8A8_UINT, VK_FORMAT_R8G8B8A8_UINT)                                                     \
  CASE(R8G8B8A8_SINT, VK_FORMAT_R8G8B8A8_SINT)                                                     \
  CASE(R8G8B8A8_SRGB, VK_FORMAT_R8G8B8A8_SRGB)                                                     \
                                                                                                   \
  CASE(B8G8R8A8_UNORM, VK_FORMAT_B8G8R8A8_UNORM)                                                   \
  CASE(B8G8R8A8_SRGB, VK_FORMAT_B8G8R8A8_SRGB)                                                     \
                                                                                                   \
  CASE(R16_UNORM, VK_FORMAT_R16_UNORM)                                                             \
  CASE(R16_SNORM, VK_FORMAT_R16_SNORM)                                                             \
  CASE(R16_UINT, VK_FORMAT_R16_UINT)                                                               \
  CASE(R16_SINT, VK_FORMAT_R16_SINT)                                                               \
  CASE(R16_FLOAT, VK_FORMAT_R16_SFLOAT)                                                            \
                                                                                                   \
  CASE(R16G16_UNORM, VK_FORMAT_R16G16_UNORM)                                                       \
  CASE(R16G16_SNORM, VK_FORMAT_R16G16_SNORM)                                                       \
  CASE(R16G16_UINT, VK_FORMAT_R16G16_UINT)                                                         \
  CASE(R16G16_SINT, VK_FORMAT_R16G16_SINT)                                                         \
  CASE(R16G16_FLOAT, VK_FORMAT_R16G16_SFLOAT)                                                      \
                                                                                                   \
  CASE(R16G16B16A16_UNORM, VK_FORMAT_R16G16B16A16_UNORM)                                           \
  CASE(R16G16B16A16_SNORM, VK_FORMAT_R16G16B16A16_SNORM)                                           \
  CASE(R16G16B16A16_UINT, VK_FORMAT_R16G16B16A16_UINT)                                             \
  CASE(R16G16B16A16_SINT, VK_FORMAT_R16G16B16A16_SINT)                                             \
  CASE(R16G16B16A16_FLOAT, VK_FORMAT_R16G16B16A16_SFLOAT)                                          \
                                                                                                   \
  CASE(R32_UINT, VK_FORMAT_R32_UINT)                                                               \
  CASE(R32_SINT, VK_FORMAT_R32_SINT)                                                               \
  CASE(R32_FLOAT, VK_FORMAT_R32_SFLOAT)                                                            \
                                                                                                   \
  CASE(R32G32_UINT, VK_FORMAT_R32G32_UINT)                                                         \
  CASE(R32G32_SINT, VK_FORMAT_R32G32_SINT)                                                         \
  CASE(R32G32_FLOAT, VK_FORMAT_R32G32_SFLOAT)                                                      \
                                                                                                   \
  CASE(R32G32B32_UINT, VK_FORMAT_R32G32B32_UINT)                                                   \
  CASE(R32G32B32_SINT, VK_FORMAT_R32G32B32_SINT)                                                   \
  CASE(R32G32B32_FLOAT, VK_FORMAT_R32G32B32_SFLOAT)                                                \
                                                                                                   \
  CASE(R32G32B32A32_UINT, VK_FORMAT_R32G32B32A32_UINT)                                             \
  CASE(R32G32B32A32_SINT, VK_FORMAT_R32G32B32A32_SINT)                                             \
  CASE(R32G32B32A32_FLOAT, VK_FORMAT_R32G32B32A32_SFLOAT)                                          \
                                                                                                   \
  CASE(D16_UNORM, VK_FORMAT_D16_UNORM)                                                             \
  CASE(D24_UNORM_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT)                                             \
  CASE(D32_FLOAT, VK_FORMAT_D32_SFLOAT)                                                            \
  CASE(D32_FLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT_S8_UINT)

#define VK_IMAGE_VIEW_TYPE_MAPPING                                                                 \
  CASE(D1, VK_IMAGE_VIEW_TYPE_1D)                                                                  \
  CASE(D2, VK_IMAGE_VIEW_TYPE_2D)                                                                  \
  CASE(D3, VK_IMAGE_VIEW_TYPE_3D)                                                                  \
  CASE(D1_ARRAY, VK_IMAGE_VIEW_TYPE_1D_ARRAY)                                                      \
  CASE(D2_ARRAY, VK_IMAGE_VIEW_TYPE_2D_ARRAY)                                                      \
  CASE(CUBE, VK_IMAGE_VIEW_TYPE_CUBE)                                                              \
  CASE(CUBE_ARRAY, VK_IMAGE_VIEW_TYPE_CUBE_ARRAY)

constexpr VkFormat vulkanize(TextureFormat fmt) {
#define CASE(name, vk)                                                                             \
  case TextureFormat::name:                                                                        \
    return vk;
  switch (fmt) {
    VK_FORMAT_MAPPING
#undef CASE
  default:
    return VK_FORMAT_UNDEFINED;
  }
}

constexpr TextureFormat devulkanize(VkFormat fmt) {
#define CASE(name, vk)                                                                             \
  case vk:                                                                                         \
    return TextureFormat::name;
  switch (fmt) {
    VK_FORMAT_MAPPING
#undef CASE
  default:
    return TextureFormat::UNDEFINED;
  }
}

inline VkImageViewType vulkanize(ImageView::Mode mode) {
  static auto logger = spdlog::get("fgla::backends::vulkan");
#define CASE(name, vk)                                                                             \
  case ImageView::Mode::name:                                                                      \
    return vk;
  switch (mode) {
    VK_IMAGE_VIEW_TYPE_MAPPING
#undef CASE
  default:
    logger->warn("Unsupported ImageView::Mode, defaulting to 2D.");
    return VK_IMAGE_VIEW_TYPE_2D;
  }
}
} // namespace fgla::backends::vulkan
