#include <fgla/backends/vulkan/device.hpp>
#include <fgla/internal.hpp>
#include <spdlog/spdlog.h>

namespace fgla::backends::vulkan {

DeviceImpl::DeviceImpl(VkDevice device, VkPhysicalDevice physical_device,
                       QueueAllocator::Queues queues)
    : device(device), physical_device(physical_device), queues(std::move(queues)) {
  for (auto &[_, queue] : this->queues) {
    QueueImpl *qi = dynamic_cast<QueueImpl *>(fgla::internal::ImplAccessor::get_impl(queue));

    qi->set_semaphore_pool(&this->semaphore_pool);
  }
}

Queue *DeviceImpl::get_queue(Queue::Type type, uint32_t index) {
  std::pair<Queue::Type, uint32_t> key = {type, index};

  if (this->queues.find(key) == this->queues.end()) {
    return nullptr;
  }

  return &this->queues.at({type, index});
}

DeviceImpl::~DeviceImpl() { vkDestroyDevice(this->device, nullptr); }
} // namespace fgla::backends::vulkan
