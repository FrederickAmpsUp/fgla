#pragma once

#include <fgla/backends/vulkan/image.hpp>

namespace fgla::backends::vulkan::ext::windowing {

struct SwapchainImageImpl : public BaseImageImpl {
  SwapchainImageImpl(VkImage image, VkExtent3D extent, VkDevice device)
      : BaseImageImpl(image, extent, VK_IMAGE_ASPECT_COLOR_BIT, device) {
    this->layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  }

  virtual ~SwapchainImageImpl() override = default;
};
} // namespace fgla::backends::vulkan::ext::windowing
