#pragma once

#include <fgla/backends/vulkan/adapter.hpp>
#include <fgla/backends/vulkan/queue.hpp>
#include <fgla/device.hpp>
#include <fgla/internal.hpp>

namespace fgla::backends::vulkan {

struct DeviceImpl : public Device::Impl {
  DeviceImpl(VkDevice device, VkPhysicalDevice physical_device, QueueAllocator::Queues queues);

  virtual Queue *get_queue(Queue::Type type, uint32_t index) override;

  VkDevice get_device() const { return this->device; }
  VkPhysicalDevice get_physical_device() const { return this->physical_device; }
  std::vector<VkSemaphore> &get_semaphore_pool() { return this->semaphore_pool; }

  virtual ~DeviceImpl() override;

private:
  VkDevice device;
  VkPhysicalDevice physical_device;
  QueueAllocator::Queues queues;

  std::vector<VkSemaphore> semaphore_pool;
};
} // namespace fgla::backends::vulkan
