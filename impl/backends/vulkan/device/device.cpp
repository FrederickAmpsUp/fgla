#include <fgla/backends/vulkan/device.hpp>
#include <spdlog/spdlog.h>

namespace fgla::backends::vulkan {

DeviceImpl::DeviceImpl(VkDevice device, VkPhysicalDevice physical_device,
                       VkInstance instance, QueueAllocator::Queues queues,
                       const std::vector<std::filesystem::path> &shader_paths)
    : device(device), physical_device(physical_device), instance(instance),
      queues(std::move(queues)) {
  init_queue();
  init_memory();
  init_shader(shader_paths);
}

DeviceImpl::~DeviceImpl() {
  vkDeviceWaitIdle(this->device);
  this->queues.clear();
  for (auto [family_index, command_pool] : this->command_pools) {
    vkDestroyCommandPool(this->device, command_pool, nullptr);
  }

  vmaDestroyAllocator(this->allocator);

  vkDestroyDevice(this->device, nullptr);
}

} // namespace fgla::backends::vulkan
