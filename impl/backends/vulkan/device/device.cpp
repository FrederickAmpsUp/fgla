#include <fgla/backends/vulkan/device.hpp>
#include <fgla/internal.hpp>
#include <spdlog/spdlog.h>

namespace fgla::backends::vulkan {

DeviceImpl::DeviceImpl(VkDevice device, VkPhysicalDevice physical_device,
                       QueueAllocator::Queues queues)
    : device(device), physical_device(physical_device),
      queues(std::move(queues)) {
  init_queue();
  init_shader();
}

DeviceImpl::~DeviceImpl() {
  vkDeviceWaitIdle(this->device);
  this->queues.clear();
  for (auto [family_index, command_pool] : this->command_pools) {
    vkDestroyCommandPool(this->device, command_pool, nullptr);
  }
  vkDestroyDevice(this->device, nullptr);
}

} // namespace fgla::backends::vulkan
