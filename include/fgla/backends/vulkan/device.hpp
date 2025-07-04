#pragma once

#include <fgla/backends/vulkan/adapter.hpp>
#include <fgla/backends/vulkan/queue.hpp>
#include <fgla/device.hpp>

namespace fgla::backends::vulkan {

struct DeviceImpl : public Device::Impl {
  DeviceImpl(AdapterImpl &adapter, const Device::Descriptor &descriptor,
             QueueAllocator &queue_allocator);
  bool is_ok() const;

  virtual Queue *get_queue(Queue::Type type, uint32_t index) override;

  virtual ~DeviceImpl() override;

private:
  VkDevice device;
  QueueAllocator::Queues queues;
};
} // namespace fgla::backends::vulkan
