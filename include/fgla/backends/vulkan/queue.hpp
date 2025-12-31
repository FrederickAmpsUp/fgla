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

  Queues get_queues(VkDevice device);
  inline const std::vector<VkDeviceQueueCreateInfo> &get_queue_create_infos() {
    return this->create_infos;
  }

private:
  QueueMapping queue_mapping;
  std::vector<VkDeviceQueueCreateInfo> create_infos;
};

struct QueueImpl : public fgla::Queue::Impl {
  QueueImpl(VkQueue queue, uint32_t family_index) : queue(queue), family_index(family_index) {}

  inline uint32_t get_family_index() const { return this->family_index; }

  // just called once by DeviceImpl right after initialization
  inline void init(std::vector<VkSemaphore> *semaphore_pool, VkCommandPool command_pool) {
    this->semaphore_pool = semaphore_pool;
    this->command_pool = command_pool;
  }

private:
  VkQueue queue;
  uint32_t family_index;

  std::vector<VkSemaphore> *semaphore_pool = nullptr;
  VkCommandPool command_pool = VK_NULL_HANDLE;
};

} // namespace fgla::backends::vulkan
