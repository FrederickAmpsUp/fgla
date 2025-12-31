#pragma once

#include <cstdint>
#include <fgla/backends/vulkan/adapter.hpp>
#include <fgla/backends/vulkan/queue.hpp>
#include <fgla/device.hpp>
#include <fgla/internal.hpp>
#include <unordered_map>

namespace fgla::backends::vulkan {

struct DeviceImpl : public Device::Impl {
  DeviceImpl(VkDevice device, VkPhysicalDevice physical_device, QueueAllocator::Queues queues);

  virtual Queue *get_queue(Queue::Type type, uint32_t index) override;

  VkDevice get_device() const { return this->device; }
  VkPhysicalDevice get_physical_device() const { return this->physical_device; }
  std::vector<VkSemaphore> &get_semaphore_pool() { return this->semaphore_pool; }

  // lazy-creates command pools
  VkCommandPool get_command_pool(uint32_t queue_family_index);

  virtual ~DeviceImpl() override;

private:
  VkDevice device;
  VkPhysicalDevice physical_device;
  QueueAllocator::Queues queues;

  std::vector<VkSemaphore> semaphore_pool;
  std::unordered_map<uint32_t, VkCommandPool> command_pools;
};
} // namespace fgla::backends::vulkan
