#pragma once

#include <fgla/backends/vulkan/image.hpp>

namespace fgla::backends::vulkan::ext::windowing {

struct SwapchainImageImpl : public BaseImageImpl {
  SwapchainImageImpl(VkImage image, VkDevice device, VkExtent3D extent)
      : BaseImageImpl(image, device) {
    this->layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    this->extent = extent;
  }

  virtual ~SwapchainImageImpl() override = default;
};
} // namespace fgla::backends::vulkan::ext::windowing
