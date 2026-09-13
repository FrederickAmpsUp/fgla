#include "fgla/backends/vulkan/memory.hpp"

#include <fgla/backends/vulkan/device.hpp>
#include <vulkan/vulkan_core.h>

namespace fgla::backends::vulkan {

void DeviceImpl::init_memory() {
  VmaAllocatorCreateInfo create_info = {};
  create_info.flags = 0;
  create_info.vulkanApiVersion = VK_API_VERSION_1_3;
  create_info.device = this->device;
  create_info.physicalDevice = this->physical_device;
  create_info.instance = this->instance;

  vmaCreateAllocator(&create_info, &this->allocator);
}

} // namespace fgla::backends::vulkan
