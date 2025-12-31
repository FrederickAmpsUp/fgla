#include <fgla/backends/vulkan/device.hpp>
#include <fgla/internal.hpp>
#include <spdlog/spdlog.h>

namespace fgla::backends::vulkan {

DeviceImpl::DeviceImpl(VkDevice device, VkPhysicalDevice physical_device,
                       QueueAllocator::Queues queues)
    : device(device), physical_device(physical_device), queues(std::move(queues)) {
  for (auto &[_, queue] : this->queues) {
    QueueImpl *qi = dynamic_cast<QueueImpl *>(fgla::internal::ImplAccessor::get_impl(queue));

    qi->init(&this->semaphore_pool, this->get_command_pool(qi->get_family_index()));
  }
}

Queue *DeviceImpl::get_queue(Queue::Type type, uint32_t index) {
  std::pair<Queue::Type, uint32_t> key = {type, index};

  if (this->queues.find(key) == this->queues.end()) {
    return nullptr;
  }

  return &this->queues.at({type, index});
}

// TODO: make this thread safe
VkCommandPool DeviceImpl::get_command_pool(uint32_t queue_family_index) {
  static auto logger = spdlog::get("fgla::backends::vulkan");

  auto it = this->command_pools.find(queue_family_index);
  if (it != this->command_pools.end()) return it->second;

  VkCommandPoolCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  create_info.flags =
      VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

  create_info.queueFamilyIndex = queue_family_index;

  VkCommandPool pool;
  VkResult res = vkCreateCommandPool(this->device, &create_info, nullptr, &pool);
  if (res != VK_SUCCESS) {
    logger->error("Failed to create Vulkan command pool.");
    return VK_NULL_HANDLE;
  }

  logger->info("Created Vulkan command pool.");

  this->command_pools.insert({queue_family_index, pool});

  return pool;
}

DeviceImpl::~DeviceImpl() {
  for (auto [family_index, command_pool] : this->command_pools) {
    vkDestroyCommandPool(this->device, command_pool, nullptr);
  }
  vkDestroyDevice(this->device, nullptr);
}

} // namespace fgla::backends::vulkan
