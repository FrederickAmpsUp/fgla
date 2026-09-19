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

VkSharingMode DeviceImpl::get_resource_sharing_mode() const {
  return this->used_queue_family_indices.size() > 1 ? VK_SHARING_MODE_CONCURRENT
                                                    : VK_SHARING_MODE_EXCLUSIVE;
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
