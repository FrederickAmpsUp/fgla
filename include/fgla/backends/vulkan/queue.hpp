#pragma once

#include <fgla/backends/vulkan/command_buffer.hpp>
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

  virtual Result<CommandBuffer> begin_recording() override;
  virtual void submit(CommandBuffer &&cb) override;

  inline uint32_t get_family_index() const { return this->family_index; }

  // just called once by DeviceImpl right after initialization
  inline void init(VkDevice device, std::vector<VkSemaphore> *semaphore_pool,
                   VkCommandPool command_pool) {
    this->device = device;
    this->semaphore_pool = semaphore_pool;
    this->command_pool = command_pool;
  }

  virtual ~QueueImpl() override;

private:
  Result<CommandBuffer> init_cb(VkCommandBuffer command_buffer, VkFence fence);

  VkQueue queue;
  uint32_t family_index;

  VkDevice device;
  std::vector<VkSemaphore> *semaphore_pool = nullptr;
  VkCommandPool command_pool = VK_NULL_HANDLE;

  std::unordered_map<VkCommandBuffer, VkFence> command_buffer_pool = {};
};

} // namespace fgla::backends::vulkan
