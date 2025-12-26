#pragma once

#include <fgla/backends/vulkan/image.hpp>

namespace fgla::backends::vulkan::ext::windowing {

struct SwapchainImageImpl : public BaseImageImpl {
  SwapchainImageImpl(VkImage image) : BaseImageImpl(image) {}

  virtual ~SwapchainImageImpl() override = default;
};
}
