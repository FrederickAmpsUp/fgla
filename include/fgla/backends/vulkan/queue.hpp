#pragma once

#include <fgla/queue.hpp>
#include <fgla/util.hpp>
#include <unordered_map>
#include <vulkan/vulkan.h>

namespace fgla::backends::vulkan {

struct DeviceImpl;

struct QueueAllocator {
  using QueueMapping = std::unordered_map<std::pair<Queue::Type, uint32_t>,
                                          std::pair<uint32_t, uint32_t>, util::PairHash>;
  using Queues = std::unordered_map<std::pair<Queue::Type, uint32_t>, Queue, util::PairHash>;

  static std::pair<std::vector<VkDeviceQueueCreateInfo>, QueueAllocator::QueueMapping>
  big_brain_allocator_algorithm(const std::initializer_list<Queue::Request> &requests,
                                std::vector<VkQueueFamilyProperties> families);

  QueueAllocator(const std::initializer_list<Queue::Request> &requests,
                 VkPhysicalDevice physical_device);

  Queues get_queues(DeviceImpl &device);
  inline const std::vector<VkDeviceQueueCreateInfo> &get_queue_create_infos() {
    return this->create_infos;
  }

private:
  QueueMapping queue_mapping;
  std::vector<VkDeviceQueueCreateInfo> create_infos;
};

struct QueueImpl : public fgla::Queue::Impl {
  QueueImpl(VkQueue queue, std::vector<VkSemaphore> &semaphore_pool)
      : queue(queue), semaphore_pool(semaphore_pool) {}

private:
  VkQueue queue;

  std::vector<VkSemaphore> &semaphore_pool;
};

} // namespace fgla::backends::vulkan
