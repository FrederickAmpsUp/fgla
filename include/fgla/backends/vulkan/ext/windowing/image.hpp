#pragma once

#include <fgla/backends/vulkan/image.hpp>

namespace fgla::backends::vulkan::ext::windowing {

struct SwapchainImageImpl : public BaseImageImpl {
  SwapchainImageImpl(VkImage image, VkDevice device) : BaseImageImpl(image, device) {}

  virtual ~SwapchainImageImpl() override = default;
};
} // namespace fgla::backends::vulkan::ext::windowing
