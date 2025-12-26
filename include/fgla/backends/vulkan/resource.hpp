#pragma once

#include <vulkan/vulkan.h>

namespace fgla::backends::vulkan {

struct Resource {

  VkSemaphore available_trigger;
};
} // namespace fgla::backends::vulkan
